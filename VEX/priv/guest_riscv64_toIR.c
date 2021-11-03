
/*--------------------------------------------------------------------*/
/*--- begin                                   guest_riscv64_toIR.c ---*/
/*--------------------------------------------------------------------*/

/*
   This file is part of Valgrind, a dynamic binary instrumentation
   framework.

   Copyright (C) 2020-2021 Petr Pavlu
      setup@dagobah.cz

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.

   The GNU General Public License is contained in the file COPYING.
*/

/* Translates riscv64 code to IR. */

/* "Special" instructions.

   This instruction decoder can decode four special instructions which mean
   nothing natively (are no-ops as far as regs/mem are concerned) but have
   meaning for supporting Valgrind. A special instruction is flagged by
   a 16-byte preamble:

      00365613 00d65613 03365613 03d65613
      (srli a2, a2, 3;   srli a2, a2, 13
       srli a2, a2, 51;  srli a2, a2, 61)

   Following that, one of the following 4 are allowed (standard interpretation
   in parentheses):

      00a56533 (or a0, a0, a0)   a3 = client_request ( a4 )
      00b5e5b3 (or a1, a1, a1)   a3 = guest_NRADDR
      00c66633 (or a2, a2, a2)   branch-and-link-to-noredir t0
      00d6e6b3 (or a3, a3, a3)   IR injection

   Any other bytes following the 16-byte preamble are illegal and constitute
   a failure in instruction decoding. This all assumes that the preamble will
   never occur except in specific code fragments designed for Valgrind to catch.
*/

#include "libvex_guest_riscv64.h"

#include "guest_riscv64_defs.h"
#include "main_globals.h"
#include "main_util.h"

/*------------------------------------------------------------*/
/*--- Debugging output                                     ---*/
/*------------------------------------------------------------*/

#define DIP(format, args...)                                                   \
   do {                                                                        \
      if (vex_traceflags & VEX_TRACE_FE)                                       \
         vex_printf(format, ##args);                                           \
   } while (0)

#define DIS(buf, format, args...)                                              \
   do {                                                                        \
      if (vex_traceflags & VEX_TRACE_FE)                                       \
         vex_sprintf(buf, format, ##args);                                     \
   } while (0)

/*------------------------------------------------------------*/
/*--- Helper bits and pieces for deconstructing the        ---*/
/*--- riscv64 insn stream.                                 ---*/
/*------------------------------------------------------------*/

/* Do a little-endian load of a 32-bit word, regardless of the endianness of the
   underlying host. */
static inline UInt getUIntLittleEndianly(const UChar* p)
{
   UInt w = 0;
   w      = (w << 8) | p[3];
   w      = (w << 8) | p[2];
   w      = (w << 8) | p[1];
   w      = (w << 8) | p[0];
   return w;
}

/* Do read of an instruction, which can be 16-bit (compressed) or 32-bit in
   size. */
static inline UInt getInsn(const UChar* p)
{
   Bool is_compressed = (p[0] & 0x3) != 0x3;
   UInt w             = 0;
   if (!is_compressed) {
      w = (w << 8) | p[3];
      w = (w << 8) | p[2];
   }
   w = (w << 8) | p[1];
   w = (w << 8) | p[0];
   return w;
}

/* Produce _uint[_bMax:_bMin]. */
#define SLICE_UInt(_uint, _bMax, _bMin)                                        \
   ((((UInt)(_uint)) >> (_bMin)) &                                             \
    (UInt)((1ULL << ((_bMax) - (_bMin) + 1)) - 1ULL))

/*------------------------------------------------------------*/
/*--- Helpers for constructing IR.                         ---*/
/*------------------------------------------------------------*/

/* Create an expression to produce a 64-bit constant. */
static IRExpr* mkU64(ULong i) { return IRExpr_Const(IRConst_U64(i)); }

/* Create an expression to produce a 32-bit constant. */
static IRExpr* mkU32(UInt i) { return IRExpr_Const(IRConst_U32(i)); }

/* Create an expression to produce an 8-bit constant. */
static IRExpr* mkU8(UInt i)
{
   vassert(i < 256);
   return IRExpr_Const(IRConst_U8((UChar)i));
}

/* Create an expression to read a temporary. */
static IRExpr* mkexpr(IRTemp tmp) { return IRExpr_RdTmp(tmp); }

/* Create an unary-operation expression. */
static IRExpr* unop(IROp op, IRExpr* a) { return IRExpr_Unop(op, a); }

/* Create a binary-operation expression. */
static IRExpr* binop(IROp op, IRExpr* a1, IRExpr* a2)
{
   return IRExpr_Binop(op, a1, a2);
}

/* Create an expression to load a value from memory (in the little-endian
   order). */
static IRExpr* loadLE(IRType ty, IRExpr* addr)
{
   return IRExpr_Load(Iend_LE, ty, addr);
}

/* Add a statement to the list held by irsb. */
static void stmt(/*MOD*/ IRSB* irsb, IRStmt* st) { addStmtToIRSB(irsb, st); }

/* Add a statement to assign a value to a temporary. */
static void assign(/*MOD*/ IRSB* irsb, IRTemp dst, IRExpr* e)
{
   stmt(irsb, IRStmt_WrTmp(dst, e));
}

/* Generate a statement to store a value in memory (in the little-endian
   order). */
static void storeLE(/*MOD*/ IRSB* irsb, IRExpr* addr, IRExpr* data)
{
   stmt(irsb, IRStmt_Store(Iend_LE, addr, data));
}

/* Generate a new temporary of the given type. */
static IRTemp newTemp(/*MOD*/ IRSB* irsb, IRType ty)
{
   vassert(isPlausibleIRType(ty));
   return newIRTemp(irsb->tyenv, ty);
}

/* Sign-extend a 32/64-bit integer expression to 64 bits. */
static IRExpr* widenSto64(IRType srcTy, IRExpr* e)
{
   switch (srcTy) {
   case Ity_I64:
      return e;
   case Ity_I32:
      return unop(Iop_32Sto64, e);
   default:
      vpanic("widenSto64(riscv64)");
   }
}

/* Narrow a 64-bit integer expression to 32/64 bits. */
static IRExpr* narrowFrom64(IRType dstTy, IRExpr* e)
{
   switch (dstTy) {
   case Ity_I64:
      return e;
   case Ity_I32:
      return unop(Iop_64to32, e);
   default:
      vpanic("narrowFrom64(riscv64)");
   }
}

/*------------------------------------------------------------*/
/*--- Offsets of various parts of the riscv64 guest state  ---*/
/*------------------------------------------------------------*/

#define OFFB_X0  offsetof(VexGuestRISCV64State, guest_x0)
#define OFFB_X1  offsetof(VexGuestRISCV64State, guest_x1)
#define OFFB_X2  offsetof(VexGuestRISCV64State, guest_x2)
#define OFFB_X3  offsetof(VexGuestRISCV64State, guest_x3)
#define OFFB_X4  offsetof(VexGuestRISCV64State, guest_x4)
#define OFFB_X5  offsetof(VexGuestRISCV64State, guest_x5)
#define OFFB_X6  offsetof(VexGuestRISCV64State, guest_x6)
#define OFFB_X7  offsetof(VexGuestRISCV64State, guest_x7)
#define OFFB_X8  offsetof(VexGuestRISCV64State, guest_x8)
#define OFFB_X9  offsetof(VexGuestRISCV64State, guest_x9)
#define OFFB_X10 offsetof(VexGuestRISCV64State, guest_x10)
#define OFFB_X11 offsetof(VexGuestRISCV64State, guest_x11)
#define OFFB_X12 offsetof(VexGuestRISCV64State, guest_x12)
#define OFFB_X13 offsetof(VexGuestRISCV64State, guest_x13)
#define OFFB_X14 offsetof(VexGuestRISCV64State, guest_x14)
#define OFFB_X15 offsetof(VexGuestRISCV64State, guest_x15)
#define OFFB_X16 offsetof(VexGuestRISCV64State, guest_x16)
#define OFFB_X17 offsetof(VexGuestRISCV64State, guest_x17)
#define OFFB_X18 offsetof(VexGuestRISCV64State, guest_x18)
#define OFFB_X19 offsetof(VexGuestRISCV64State, guest_x19)
#define OFFB_X20 offsetof(VexGuestRISCV64State, guest_x20)
#define OFFB_X21 offsetof(VexGuestRISCV64State, guest_x21)
#define OFFB_X22 offsetof(VexGuestRISCV64State, guest_x22)
#define OFFB_X23 offsetof(VexGuestRISCV64State, guest_x23)
#define OFFB_X24 offsetof(VexGuestRISCV64State, guest_x24)
#define OFFB_X25 offsetof(VexGuestRISCV64State, guest_x25)
#define OFFB_X26 offsetof(VexGuestRISCV64State, guest_x26)
#define OFFB_X27 offsetof(VexGuestRISCV64State, guest_x27)
#define OFFB_X28 offsetof(VexGuestRISCV64State, guest_x28)
#define OFFB_X29 offsetof(VexGuestRISCV64State, guest_x29)
#define OFFB_X30 offsetof(VexGuestRISCV64State, guest_x30)
#define OFFB_X31 offsetof(VexGuestRISCV64State, guest_x31)
#define OFFB_PC  offsetof(VexGuestRISCV64State, guest_pc)

#define OFFB_EMNOTE        offsetof(VexGuestRISCV64State, guest_EMNOTE)
#define OFFB_CMSTART       offsetof(VexGuestRISCV64State, guest_CMSTART)
#define OFFB_CMLEN         offsetof(VexGuestRISCV64State, guest_CMLEN)
#define OFFB_NRADDR        offsetof(VexGuestRISCV64State, guest_NRADDR)

#define OFFB_LLSC_SIZE offsetof(VexGuestRISCV64State, guest_LLSC_SIZE)
#define OFFB_LLSC_ADDR offsetof(VexGuestRISCV64State, guest_LLSC_ADDR)
#define OFFB_LLSC_DATA offsetof(VexGuestRISCV64State, guest_LLSC_DATA)

/*------------------------------------------------------------*/
/*--- Integer registers                                    ---*/
/*------------------------------------------------------------*/

static Int offsetIReg64(UInt iregNo)
{
   switch (iregNo) {
   case 0:
      return OFFB_X0;
   case 1:
      return OFFB_X1;
   case 2:
      return OFFB_X2;
   case 3:
      return OFFB_X3;
   case 4:
      return OFFB_X4;
   case 5:
      return OFFB_X5;
   case 6:
      return OFFB_X6;
   case 7:
      return OFFB_X7;
   case 8:
      return OFFB_X8;
   case 9:
      return OFFB_X9;
   case 10:
      return OFFB_X10;
   case 11:
      return OFFB_X11;
   case 12:
      return OFFB_X12;
   case 13:
      return OFFB_X13;
   case 14:
      return OFFB_X14;
   case 15:
      return OFFB_X15;
   case 16:
      return OFFB_X16;
   case 17:
      return OFFB_X17;
   case 18:
      return OFFB_X18;
   case 19:
      return OFFB_X19;
   case 20:
      return OFFB_X20;
   case 21:
      return OFFB_X21;
   case 22:
      return OFFB_X22;
   case 23:
      return OFFB_X23;
   case 24:
      return OFFB_X24;
   case 25:
      return OFFB_X25;
   case 26:
      return OFFB_X26;
   case 27:
      return OFFB_X27;
   case 28:
      return OFFB_X28;
   case 29:
      return OFFB_X29;
   case 30:
      return OFFB_X30;
   case 31:
      return OFFB_X31;
   default:
      vassert(0);
   }
}

/* Obtain ABI name of a register. */
static const HChar* nameIReg64(UInt iregNo)
{
   vassert(iregNo < 32);
   static const HChar* names[32] = {
      "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
      "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
      "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
   return names[iregNo];
}

/* Read a 64-bit value from a guest integer register. */
static IRExpr* getIReg64(UInt iregNo)
{
   vassert(iregNo < 32);
   return IRExpr_Get(offsetIReg64(iregNo), Ity_I64);
}

/* Write a 64-bit value into a guest integer register. */
static void putIReg64(/*OUT*/ IRSB* irsb, UInt iregNo, /*IN*/ IRExpr* e)
{
   vassert(iregNo > 0 && iregNo < 32);
   vassert(typeOfIRExpr(irsb->tyenv, e) == Ity_I64);
   stmt(irsb, IRStmt_Put(offsetIReg64(iregNo), e));
}

/* Read a 32-bit value from a guest integer register. */
static IRExpr* getIReg32(UInt iregNo)
{
   vassert(iregNo < 32);
   return unop(Iop_64to32, IRExpr_Get(offsetIReg64(iregNo), Ity_I64));
}

/* Write a 32-bit value into a guest integer register. */
static void putIReg32(/*OUT*/ IRSB* irsb, UInt iregNo, /*IN*/ IRExpr* e)
{
   vassert(iregNo > 0 && iregNo < 32);
   vassert(typeOfIRExpr(irsb->tyenv, e) == Ity_I32);
   stmt(irsb, IRStmt_Put(offsetIReg64(iregNo), unop(Iop_32Sto64, e)));
}

/* Write an address into the guest pc. */
static void putPC(/*OUT*/ IRSB* irsb, /*IN*/ IRExpr* e)
{
   vassert(typeOfIRExpr(irsb->tyenv, e) == Ity_I64);
   stmt(irsb, IRStmt_Put(OFFB_PC, e));
}

/*------------------------------------------------------------*/
/*--- Name helpers                                         ---*/
/*------------------------------------------------------------*/

/* Obtain an acquire/release atomic-instruction suffix. */
static const HChar* nameAqRlSuffix(UInt aqrl)
{
   switch (aqrl) {
   case 0b00:
      return "";
   case 0b01:
      return ".rl";
   case 0b10:
      return ".aq";
   case 0b11:
      return ".aqrl";
   default:
      vpanic("nameAqRlSuffix(riscv64)");
   }
}

/*------------------------------------------------------------*/
/*--- Disassemble a single instruction                     ---*/
/*------------------------------------------------------------*/

static Bool dis_RISCV64_compressed(/*MB_OUT*/ DisResult* dres,
                                   /*OUT*/ IRSB*         irsb,
                                   UInt                  insn,
                                   Addr                  guest_pc_curr_instr,
                                   Bool                  sigill_diag)
{
#define INSN(_bMax, _bMin) SLICE_UInt(insn, (_bMax), (_bMin))
   vassert(INSN(1, 0) == 0b00 || INSN(1, 0) == 0b01 || INSN(1, 0) == 0b10);

   /* ---- RV64C compressed instruction set, quadrant 0 ----- */

   /* ------------- c.addi4spn rd, nzuimm[9:2] -------------- */
   if (INSN(1, 0) == 0b00 && INSN(15, 13) == 0b000) {
      UInt rd = INSN(4, 2) + 8;
      UInt nzuimm9_2 =
         INSN(10, 7) << 4 | INSN(12, 11) << 2 | INSN(5, 5) << 1 | INSN(6, 6);
      if (nzuimm9_2 == 0) {
         /* Invalid C.ADDI4SPN, fall through. */
      } else {
         ULong uimm = nzuimm9_2 << 2;
         putIReg64(irsb, rd,
                   binop(Iop_Add64, getIReg64(2 /*x2/sp*/), mkU64(uimm)));
         DIP("c.addi4spn %s, %llu\n", nameIReg64(rd), uimm);
         return True;
      }
   }

   /* -------------- c.fld rd, uimm[7:3](rs1) --------------- */
   if (INSN(1, 0) == 0b00 && INSN(15, 13) == 0b001) {
      /* TODO Implement. */
#if 0
      UInt  rd      = INSN(4, 2) + 8;
      UInt  rs1     = INSN(9, 7) + 8;
      UInt  uimm7_3 = INSN(6, 5) << 3 | INSN(12, 10);
      ULong uimm    = uimm7_3 << 3;
      putIReg64(irsb, rd,
                loadLE(Ity_I64, binop(Iop_Add64, getIReg64(rs1), mkU64(uimm))));
      DIP("c.ld %s, %llu(%s)\n", nameIReg64(rd), uimm, nameIReg64(rs1));
#endif
      return True;
   }

   /* --------------- c.lw rd, uimm[6:2](rs1) --------------- */
   if (INSN(1, 0) == 0b00 && INSN(15, 13) == 0b010) {
      UInt  rd      = INSN(4, 2) + 8;
      UInt  rs1     = INSN(9, 7) + 8;
      UInt  uimm6_2 = INSN(5, 5) << 4 | INSN(12, 10) << 1 | INSN(6, 6);
      ULong uimm    = uimm6_2 << 2;
      putIReg64(
         irsb, rd,
         unop(Iop_32Sto64,
              loadLE(Ity_I32, binop(Iop_Add64, getIReg64(rs1), mkU64(uimm)))));
      DIP("c.lw %s, %llu(%s)\n", nameIReg64(rd), uimm, nameIReg64(rs1));
      return True;
   }

   /* --------------- c.ld rd, uimm[7:3](rs1) --------------- */
   if (INSN(1, 0) == 0b00 && INSN(15, 13) == 0b011) {
      UInt  rd      = INSN(4, 2) + 8;
      UInt  rs1     = INSN(9, 7) + 8;
      UInt  uimm7_3 = INSN(6, 5) << 3 | INSN(12, 10);
      ULong uimm    = uimm7_3 << 3;
      putIReg64(irsb, rd,
                loadLE(Ity_I64, binop(Iop_Add64, getIReg64(rs1), mkU64(uimm))));
      DIP("c.ld %s, %llu(%s)\n", nameIReg64(rd), uimm, nameIReg64(rs1));
      return True;
   }

   /* -------------- c.fsd rs2, uimm[7:3](rs1) -------------- */
   if (INSN(1, 0) == 0b00 && INSN(15, 13) == 0b101) {
      /* TODO Implement. */
#if 0
      UInt  rs1     = INSN(9, 7) + 8;
      UInt  rs2     = INSN(4, 2) + 8;
      UInt  uimm7_3 = INSN(6, 5) << 3 | INSN(12, 10);
      ULong uimm    = uimm7_3 << 3;
      storeLE(irsb, binop(Iop_Add64, getIReg64(rs1), mkU64(uimm)),
              getIReg64(rs2));
      DIP("c.fsd %s, %llu(%s)\n", nameIReg64(rs2), uimm, nameIReg64(rs1));
#endif
      return True;
   }

   /* -------------- c.sw rs2, uimm[6:2](rs1) --------------- */
   if (INSN(1, 0) == 0b00 && INSN(15, 13) == 0b110) {
      UInt  rs1     = INSN(9, 7) + 8;
      UInt  rs2     = INSN(4, 2) + 8;
      UInt  uimm6_2 = INSN(5, 5) << 4 | INSN(12, 10) << 1 | INSN(6, 6);
      ULong uimm    = uimm6_2 << 2;
      storeLE(irsb, binop(Iop_Add64, getIReg64(rs1), mkU64(uimm)),
              unop(Iop_64to32, getIReg64(rs2)));
      DIP("c.sw %s, %llu(%s)\n", nameIReg64(rs2), uimm, nameIReg64(rs1));
      return True;
   }

   /* -------------- c.sd rs2, uimm[7:3](rs1) --------------- */
   if (INSN(1, 0) == 0b00 && INSN(15, 13) == 0b111) {
      UInt  rs1     = INSN(9, 7) + 8;
      UInt  rs2     = INSN(4, 2) + 8;
      UInt  uimm7_3 = INSN(6, 5) << 3 | INSN(12, 10);
      ULong uimm    = uimm7_3 << 3;
      storeLE(irsb, binop(Iop_Add64, getIReg64(rs1), mkU64(uimm)),
              getIReg64(rs2));
      DIP("c.sd %s, %llu(%s)\n", nameIReg64(rs2), uimm, nameIReg64(rs1));
      return True;
   }

   /* ---- RV64C compressed instruction set, quadrant 1 ----- */

   /* ------------------------ c.nop ------------------------ */
   if (INSN(15, 0) == 0b0000000000000001) {
      DIP("c.nop\n");
      return True;
   }

   /* -------------- c.addi rd_rs1, nzimm[5:0] -------------- */
   if (INSN(1, 0) == 0b01 && INSN(15, 13) == 0b000) {
      UInt rd_rs1   = INSN(11, 7);
      UInt nzimm5_0 = INSN(12, 12) << 5 | INSN(6, 2);
      if (rd_rs1 == 0 || nzimm5_0 == 0) {
         /* Invalid C.ADDI, fall through. */
      } else {
         ULong simm = vex_sx_to_64(nzimm5_0, 6);
         putIReg64(irsb, rd_rs1,
                   binop(Iop_Add64, getIReg64(rd_rs1), mkU64(simm)));
         DIP("c.addi %s, %lld\n", nameIReg64(rd_rs1), (Long)simm);
         return True;
      }
   }

   /* -------------- c.addiw rd_rs1, imm[5:0] --------------- */
   if (INSN(1, 0) == 0b01 && INSN(15, 13) == 0b001) {
      UInt rd_rs1 = INSN(11, 7);
      UInt imm5_0 = INSN(12, 12) << 5 | INSN(6, 2);
      if (rd_rs1 == 0) {
         /* Invalid C.ADDIW, fall through. */
      } else {
         UInt simm = (UInt)vex_sx_to_64(imm5_0, 6);
         putIReg32(irsb, rd_rs1,
                   binop(Iop_Add32, getIReg32(rd_rs1), mkU32(simm)));
         DIP("c.addiw %s, %d\n", nameIReg64(rd_rs1), (Int)simm);
         return True;
      }
   }

   /* ------------------ c.li rd, imm[5:0] ------------------ */
   if (INSN(1, 0) == 0b01 && INSN(15, 13) == 0b010) {
      UInt rd     = INSN(11, 7);
      UInt imm5_0 = INSN(12, 12) << 5 | INSN(6, 2);
      if (rd == 0) {
         /* Invalid C.LI, fall through. */
      } else {
         ULong simm = vex_sx_to_64(imm5_0, 6);
         putIReg64(irsb, rd, mkU64(simm));
         DIP("c.li %s, %lld\n", nameIReg64(rd), (Long)simm);
         return True;
      }
   }

   /* ---------------- c.addi16sp nzimm[9:4] ---------------- */
   if (INSN(1, 0) == 0b01 && INSN(15, 13) == 0b011) {
      UInt rd_rs1   = INSN(11, 7);
      UInt nzimm9_4 = INSN(12, 12) << 5 | INSN(4, 3) << 3 | INSN(5, 5) << 2 |
                      INSN(2, 2) << 1 | INSN(6, 6);
      if (rd_rs1 != 2 || nzimm9_4 == 0) {
         /* Invalid C.ADDI16SP, fall through. */
      } else {
         ULong simm = vex_sx_to_64(nzimm9_4 << 4, 10);
         putIReg64(irsb, rd_rs1,
                   binop(Iop_Add64, getIReg64(rd_rs1), mkU64(simm)));
         DIP("c.addi16sp %lld\n", (Long)simm);
         return True;
      }
   }

   /* --------------- c.lui rd, nzimm[17:12] ---------------- */
   if (INSN(1, 0) == 0b01 && INSN(15, 13) == 0b011) {
      UInt rd         = INSN(11, 7);
      UInt nzimm17_12 = INSN(12, 12) << 5 | INSN(6, 2);
      if (rd == 0 || rd == 2 || nzimm17_12 == 0) {
         /* Invalid C.LUI, fall through. */
      } else {
         putIReg64(irsb, rd, mkU64(vex_sx_to_64(nzimm17_12 << 12, 18)));
         DIP("c.lui %s, 0x%x\n", nameIReg64(rd), nzimm17_12);
         return True;
      }
   }

   /* ---------- c.{srli,srai} rd_rs1, nzuimm[5:0] ---------- */
   if (INSN(1, 0) == 0b01 && INSN(11, 11) == 0b0 && INSN(15, 13) == 0b100) {
      Bool is_log    = INSN(10, 10) == 0b0;
      UInt rd_rs1    = INSN(9, 7) + 8;
      UInt nzuimm5_0 = INSN(12, 12) << 5 | INSN(6, 2);
      if (nzuimm5_0 == 0) {
         /* Invalid C.{SRLI,SRAI}, fall through. */
      } else {
         putIReg64(irsb, rd_rs1,
                   binop(is_log ? Iop_Shr64 : Iop_Sar64, getIReg64(rd_rs1),
                         mkU8(nzuimm5_0)));
         DIP("c.%s %s, %u\n", is_log ? "srli" : "srai", nameIReg64(rd_rs1),
             nzuimm5_0);
         return True;
      }
   }

   /* --------------- c.andi rd_rs1, imm[5:0] --------------- */
   if (INSN(1, 0) == 0b01 && INSN(11, 10) == 0b10 && INSN(15, 13) == 0b100) {
      UInt rd_rs1 = INSN(9, 7) + 8;
      UInt imm5_0 = INSN(12, 12) << 5 | INSN(6, 2);
      if (rd_rs1 == 0) {
         /* Invalid C.ANDI, fall through. */
      } else {
         ULong simm = vex_sx_to_64(imm5_0, 6);
         putIReg64(irsb, rd_rs1,
                   binop(Iop_And64, getIReg64(rd_rs1), mkU64(simm)));
         DIP("c.andi %s, 0x%llx\n", nameIReg64(rd_rs1), simm);
         return True;
      }
   }

   /* ----------- c.{sub,xor,or,and} rd_rs1, rs2 ----------- */
   if (INSN(1, 0) == 0b01 && INSN(15, 10) == 0b100011) {
      UInt         funct2 = INSN(6, 5);
      UInt         rd_rs1 = INSN(9, 7) + 8;
      UInt         rs2    = INSN(4, 2) + 8;
      const HChar* name;
      IROp         op;
      switch (funct2) {
      case 0b00:
         name = "sub";
         op   = Iop_Sub64;
         break;
      case 0b01:
         name = "xor";
         op   = Iop_Xor64;
         break;
      case 0b10:
         name = "or";
         op   = Iop_Or64;
         break;
      case 0b11:
         name = "and";
         op   = Iop_And64;
         break;
      default:
         vassert(0);
      }
      putIReg64(irsb, rd_rs1, binop(op, getIReg64(rd_rs1), getIReg64(rs2)));
      DIP("c.%s %s, %s\n", name, nameIReg64(rd_rs1), nameIReg64(rs2));
      return True;
   }

   /* -------------- c.{subw,addw} rd_rs1, rs2 -------------- */
   if (INSN(1, 0) == 0b01 && INSN(6, 6) == 0b0 && INSN(15, 10) == 0b100111) {
      Bool is_sub = INSN(5, 5) == 0b0;
      UInt rd_rs1 = INSN(9, 7) + 8;
      UInt rs2    = INSN(4, 2) + 8;
      putIReg32(irsb, rd_rs1,
                binop(is_sub ? Iop_Sub32 : Iop_Add32, getIReg32(rd_rs1),
                      getIReg32(rs2)));
      DIP("c.%s %s, %s\n", is_sub ? "subw" : "addw", nameIReg64(rd_rs1),
          nameIReg64(rs2));
      return True;
   }

   /* -------------------- c.j imm[11:1] -------------------- */
   if (INSN(1, 0) == 0b01 && INSN(15, 13) == 0b101) {
      UInt imm11_1 = INSN(12, 12) << 10 | INSN(8, 8) << 9 | INSN(10, 9) << 7 |
                     INSN(6, 6) << 6 | INSN(7, 7) << 5 | INSN(2, 2) << 4 |
                     INSN(11, 11) << 3 | INSN(5, 3);
      ULong simm   = vex_sx_to_64(imm11_1 << 1, 12);
      ULong dst_pc = guest_pc_curr_instr + simm;
      putPC(irsb, mkU64(dst_pc));
      dres->whatNext    = Dis_StopHere;
      dres->jk_StopHere = Ijk_Boring;
      DIP("c.j 0x%llx\n", dst_pc);
      return True;
   }

   /* ------------- c.{beqz,bnez} rs1, imm[8:1] ------------- */
   if (INSN(1, 0) == 0b01 && INSN(15, 14) == 0b11) {
      Bool is_eq  = INSN(13, 13) == 0b0;
      UInt rs1    = INSN(9, 7) + 8;
      UInt imm8_1 = INSN(12, 12) << 7 | INSN(6, 5) << 5 | INSN(2, 2) << 4 |
                    INSN(11, 10) << 2 | INSN(4, 3);
      ULong simm   = vex_sx_to_64(imm8_1 << 1, 9);
      ULong dst_pc = guest_pc_curr_instr + simm;
      stmt(irsb, IRStmt_Exit(binop(is_eq ? Iop_CmpEQ64 : Iop_CmpNE64,
                                   getIReg64(rs1), mkU64(0)),
                             Ijk_Boring, IRConst_U64(dst_pc), OFFB_PC));
      putPC(irsb, mkU64(guest_pc_curr_instr + 2));
      dres->whatNext    = Dis_StopHere;
      dres->jk_StopHere = Ijk_Boring;
      DIP("c.%s %s, 0x%llx\n", is_eq ? "beqz" : "bnez", nameIReg64(rs1),
          dst_pc);
      return True;
   }

   /* ---- RV64C compressed instruction set, quadrant 2 ----- */

   /* ------------- c.slli rd_rs1, nzuimm[5:0] -------------- */
   if (INSN(1, 0) == 0b10 && INSN(15, 13) == 0b000) {
      UInt rd_rs1    = INSN(11, 7);
      UInt nzuimm5_0 = INSN(12, 12) << 5 | INSN(6, 2);
      if (rd_rs1 == 0 || nzuimm5_0 == 0) {
         /* Invalid C.SLLI, fall through. */
      } else {
         putIReg64(irsb, rd_rs1,
                   binop(Iop_Shl64, getIReg64(rd_rs1), mkU8(nzuimm5_0)));
         DIP("c.slli %s, %u\n", nameIReg64(rd_rs1), nzuimm5_0);
         return True;
      }
   }

   /* -------------- c.fldsp rd, uimm[8:3](x2) -------------- */
   if (INSN(1, 0) == 0b10 && INSN(15, 13) == 0b001) {
      /* TODO Implement. */
#if 0
      UInt rd      = INSN(11, 7);
      UInt rs1     = 2; /* base=x2/sp */
      UInt uimm8_3 = INSN(4, 2) << 3 | INSN(12, 12) << 2 | INSN(6, 5);
      if (rd == 0) {
         /* Invalid C.LDSP, fall through. */
      } else {
         ULong uimm = uimm8_3 << 3;
         putIReg64(
            irsb, rd,
            loadLE(Ity_I64, binop(Iop_Add64, getIReg64(rs1), mkU64(uimm))));
         DIP("c.ldsp %s, %llu(%s)\n", nameIReg64(rd), uimm, nameIReg64(rs1));
         return True;
      }
#endif
      return True;
   }

   /* -------------- c.lwsp rd, uimm[7:2](x2) --------------- */
   if (INSN(1, 0) == 0b10 && INSN(15, 13) == 0b010) {
      UInt rd      = INSN(11, 7);
      UInt rs1     = 2; /* base=x2/sp */
      UInt uimm7_2 = INSN(3, 2) << 4 | INSN(12, 12) << 3 | INSN(6, 4);
      if (rd == 0) {
         /* Invalid C.LWSP, fall through. */
      } else {
         ULong uimm = uimm7_2 << 2;
         putIReg64(irsb, rd,
                   unop(Iop_32Sto64,
                        loadLE(Ity_I32,
                               binop(Iop_Add64, getIReg64(rs1), mkU64(uimm)))));
         DIP("c.lwsp %s, %llu(%s)\n", nameIReg64(rd), uimm, nameIReg64(rs1));
         return True;
      }
   }

   /* -------------- c.ldsp rd, uimm[8:3](x2) --------------- */
   if (INSN(1, 0) == 0b10 && INSN(15, 13) == 0b011) {
      UInt rd      = INSN(11, 7);
      UInt rs1     = 2; /* base=x2/sp */
      UInt uimm8_3 = INSN(4, 2) << 3 | INSN(12, 12) << 2 | INSN(6, 5);
      if (rd == 0) {
         /* Invalid C.LDSP, fall through. */
      } else {
         ULong uimm = uimm8_3 << 3;
         putIReg64(
            irsb, rd,
            loadLE(Ity_I64, binop(Iop_Add64, getIReg64(rs1), mkU64(uimm))));
         DIP("c.ldsp %s, %llu(%s)\n", nameIReg64(rd), uimm, nameIReg64(rs1));
         return True;
      }
   }

   /* ---------------------- c.jr rs1 ----------------------- */
   if (INSN(1, 0) == 0b10 && INSN(15, 12) == 0b1000) {
      UInt rs1 = INSN(11, 7);
      UInt rs2 = INSN(6, 2);
      if (rs1 == 0 || rs2 != 0) {
         /* Invalid C.JR, fall through. */
      } else {
         putPC(irsb, getIReg64(rs1));
         dres->whatNext = Dis_StopHere;
         if (rs1 == 1 /*x1/ra*/) {
            dres->jk_StopHere = Ijk_Ret;
            DIP("c.ret\n");
         } else {
            dres->jk_StopHere = Ijk_Boring;
            DIP("c.jr %s\n", nameIReg64(rs1));
         }
         return True;
      }
   }

   /* -------------------- c.mv rd, rs2 --------------------- */
   if (INSN(1, 0) == 0b10 && INSN(15, 12) == 0b1000) {
      UInt rd  = INSN(11, 7);
      UInt rs2 = INSN(6, 2);
      if (rd == 0 || rs2 == 0) {
         /* Invalid C.MV, fall through. */
      } else {
         putIReg64(irsb, rd, getIReg64(rs2));
         DIP("c.mv %s, %s\n", nameIReg64(rd), nameIReg64(rs2));
         return True;
      }
   }

   /* --------------------- c.jalr rs1 ---------------------- */
   if (INSN(1, 0) == 0b10 && INSN(15, 12) == 0b1001) {
      UInt rs1 = INSN(11, 7);
      UInt rs2 = INSN(6, 2);
      if (rs1 == 0 || rs2 != 0) {
         /* Invalid C.JALR, fall through. */
      } else {
         putIReg64(irsb, 1 /*x1/ra*/, mkU64(guest_pc_curr_instr + 2));
         putPC(irsb, getIReg64(rs1));
         dres->whatNext    = Dis_StopHere;
         dres->jk_StopHere = Ijk_Call;
         DIP("c.jalr %s\n", nameIReg64(rs1));
         return True;
      }
   }

   /* ------------------ c.add rd_rs1, rs2 ------------------ */
   if (INSN(1, 0) == 0b10 && INSN(15, 12) == 0b1001) {
      UInt rd_rs1 = INSN(11, 7);
      UInt rs2    = INSN(6, 2);
      if (rd_rs1 == 0 || rs2 == 0) {
         /* Invalid C.ADD, fall through. */
      } else {
         putIReg64(irsb, rd_rs1,
                   binop(Iop_Add64, getIReg64(rd_rs1), getIReg64(rs2)));
         DIP("c.add %s, %s\n", nameIReg64(rd_rs1), nameIReg64(rs2));
         return True;
      }
   }

   /* ------------- c.fsdsp rs2, uimm[8:3](x2) -------------- */
   if (INSN(1, 0) == 0b10 && INSN(15, 13) == 0b101) {
      /* TODO Implement. */
#if 0
      UInt  rs1     = 2; /* base=x2/sp */
      UInt  rs2     = INSN(6, 2);
      UInt  uimm8_3 = INSN(9, 7) << 3 | INSN(12, 10);
      ULong uimm    = uimm8_3 << 3;
      storeLE(irsb, binop(Iop_Add64, getIReg64(rs1), mkU64(uimm)),
              getIReg64(rs2));
      DIP("c.sdsp %s, %llu(%s)\n", nameIReg64(rs2), uimm, nameIReg64(rs1));
#endif
      return True;
   }

   /* -------------- c.swsp rs2, uimm[7:2](x2) -------------- */
   if (INSN(1, 0) == 0b10 && INSN(15, 13) == 0b110) {
      UInt  rs1     = 2; /* base=x2/sp */
      UInt  rs2     = INSN(6, 2);
      UInt  uimm7_2 = INSN(8, 7) << 4 | INSN(12, 9);
      ULong uimm    = uimm7_2 << 2;
      storeLE(irsb, binop(Iop_Add64, getIReg64(rs1), mkU64(uimm)),
              unop(Iop_64to32, getIReg64(rs2)));
      DIP("c.swsp %s, %llu(%s)\n", nameIReg64(rs2), uimm, nameIReg64(rs1));
      return True;
   }

   /* -------------- c.sdsp rs2, uimm[8:3](x2) -------------- */
   if (INSN(1, 0) == 0b10 && INSN(15, 13) == 0b111) {
      UInt  rs1     = 2; /* base=x2/sp */
      UInt  rs2     = INSN(6, 2);
      UInt  uimm8_3 = INSN(9, 7) << 3 | INSN(12, 10);
      ULong uimm    = uimm8_3 << 3;
      storeLE(irsb, binop(Iop_Add64, getIReg64(rs1), mkU64(uimm)),
              getIReg64(rs2));
      DIP("c.sdsp %s, %llu(%s)\n", nameIReg64(rs2), uimm, nameIReg64(rs1));
      return True;
   }

   if (sigill_diag)
      vex_printf("RISCV64 front end: compressed\n");
   return False;
#undef INSN
}

static Bool dis_RISCV64_standard(/*MB_OUT*/ DisResult* dres,
                                 /*OUT*/ IRSB*         irsb,
                                 UInt                  insn,
                                 Addr                  guest_pc_curr_instr,
                                 const VexAbiInfo*     abiinfo,
                                 Bool                  sigill_diag)
{
#define INSN(_bMax, _bMin) SLICE_UInt(insn, (_bMax), (_bMin))
   vassert(INSN(1, 0) == 0b11);

   /* ------------- RV64I base instruction set -------------- */

   /* ----------------- lui rd, imm[31:12] ------------------ */
   if (INSN(6, 0) == 0b0110111) {
      UInt rd       = INSN(11, 7);
      UInt imm31_12 = INSN(31, 12);
      if (rd == 0) {
         /* Invalid LUI, fall through. */
      } else {
         putIReg64(irsb, rd, mkU64(vex_sx_to_64(imm31_12 << 12, 32)));
         DIP("lui %s, 0x%x\n", nameIReg64(rd), imm31_12);
         return True;
      }
   }

   /* ---------------- auipc rd, imm[31:12] ----------------- */
   if (INSN(6, 0) == 0b0010111) {
      UInt rd       = INSN(11, 7);
      UInt imm31_12 = INSN(31, 12);
      if (rd == 0) {
         /* Invalid AUIPC, fall through. */
      } else {
         putIReg64(
            irsb, rd,
            mkU64(guest_pc_curr_instr + vex_sx_to_64(imm31_12 << 12, 32)));
         DIP("auipc %s, 0x%x\n", nameIReg64(rd), imm31_12);
         return True;
      }
   }

   /* ------------------ jal rd, imm[20:1] ------------------ */
   if (INSN(6, 0) == 0b1101111) {
      UInt rd      = INSN(11, 7);
      UInt imm20_1 = INSN(31, 31) << 19 | INSN(19, 12) << 11 |
                     INSN(20, 20) << 10 | INSN(30, 21);
      ULong simm   = vex_sx_to_64(imm20_1 << 1, 21);
      ULong dst_pc = guest_pc_curr_instr + simm;
      if (rd != 0)
         putIReg64(irsb, rd, mkU64(guest_pc_curr_instr + 4));
      putPC(irsb, mkU64(dst_pc));
      dres->whatNext = Dis_StopHere;
      if (rd != 0) {
         dres->jk_StopHere = Ijk_Call;
         DIP("jal %s, 0x%llx\n", nameIReg64(rd), dst_pc);
      } else {
         dres->jk_StopHere = Ijk_Boring;
         DIP("j 0x%llx\n", dst_pc);
      }
      return True;
   }

   /* --------------- jalr rd, imm[11:0](rs1) --------------- */
   if (INSN(6, 0) == 0b1100111 && INSN(14, 12) == 0b000) {
      UInt   rd      = INSN(11, 7);
      UInt   rs1     = INSN(19, 15);
      UInt   imm11_0 = INSN(31, 20);
      ULong  simm    = vex_sx_to_64(imm11_0, 12);
      IRTemp dst_pc  = newTemp(irsb, Ity_I64);
      assign(irsb, dst_pc, binop(Iop_Add64, getIReg64(rs1), mkU64(simm)));
      if (rd != 0)
         putIReg64(irsb, rd, mkU64(guest_pc_curr_instr + 4));
      putPC(irsb, mkexpr(dst_pc));
      dres->whatNext = Dis_StopHere;
      if (rd == 0) {
         if (rs1 == 1 /*x1/ra*/ && simm == 0) {
            dres->jk_StopHere = Ijk_Ret;
            DIP("ret\n");
         } else {
            dres->jk_StopHere = Ijk_Boring;
            DIP("jr %lld(%s)\n", (Long)simm, nameIReg64(rs1));
         }
      } else {
         dres->jk_StopHere = Ijk_Call;
         DIP("jalr %s, %lld(%s)\n", nameIReg64(rd), (Long)simm,
             nameIReg64(rs1));
      }
      return True;
   }

   /* ------------ {beq,bne} rs1, rs2, imm[12:1] ------------ */
   /* ------------ {blt,bge} rs1, rs2, imm[12:1] ------------ */
   /* ----------- {bltu,bgeu} rs1, rs2, imm[12:1] ----------- */
   if (INSN(6, 0) == 0b1100011) {
      UInt funct3  = INSN(14, 12);
      UInt rs1     = INSN(19, 15);
      UInt rs2     = INSN(24, 20);
      UInt imm12_1 = INSN(31, 31) << 11 | INSN(7, 7) << 10 | INSN(30, 25) << 4 |
                     INSN(11, 8);
      if (funct3 == 0b010 || funct3 == 0b011) {
         /* Invalid B<x>, fall through. */
      } else {
         ULong        simm   = vex_sx_to_64(imm12_1 << 1, 13);
         ULong        dst_pc = guest_pc_curr_instr + simm;
         const HChar* name;
         IRExpr*      cond;
         switch (funct3) {
         case 0b000:
            name = "beq";
            cond = binop(Iop_CmpEQ64, getIReg64(rs1), getIReg64(rs2));
            break;
         case 0b001:
            name = "bne";
            cond = binop(Iop_CmpNE64, getIReg64(rs1), getIReg64(rs2));
            break;
         case 0b100:
            name = "blt";
            cond = binop(Iop_CmpLT64S, getIReg64(rs1), getIReg64(rs2));
            break;
         case 0b101:
            name = "bge";
            cond = binop(Iop_CmpLE64S, getIReg64(rs2), getIReg64(rs1));
            break;
         case 0b110:
            name = "bltu";
            cond = binop(Iop_CmpLT64U, getIReg64(rs1), getIReg64(rs2));
            break;
         case 0b111:
            name = "bgeu";
            cond = binop(Iop_CmpLE64U, getIReg64(rs2), getIReg64(rs1));
            break;
         default:
            vassert(0);
         }
         stmt(irsb,
              IRStmt_Exit(cond, Ijk_Boring, IRConst_U64(dst_pc), OFFB_PC));
         putPC(irsb, mkU64(guest_pc_curr_instr + 4));
         dres->whatNext    = Dis_StopHere;
         dres->jk_StopHere = Ijk_Boring;
         DIP("%s %s, %s, 0x%llx\n", name, nameIReg64(rs1), nameIReg64(rs2),
             dst_pc);
         return True;
      }
   }

   /* ---------- {lb,lh,lw,ld} rd, imm[11:0](rs1) ----------- */
   /* ---------- {lbu,lhu,lwu} rd, imm[11:0](rs1) ----------- */
   if (INSN(6, 0) == 0b0000011) {
      UInt funct3  = INSN(14, 12);
      UInt rd      = INSN(11, 7);
      UInt rs1     = INSN(19, 15);
      UInt imm11_0 = INSN(31, 20);
      if (rd == 0 || funct3 == 0b111) {
         /* Invalid L<x>, fall through. */
      } else {
         ULong        simm = vex_sx_to_64(imm11_0, 12);
         IRExpr*      ea   = binop(Iop_Add64, getIReg64(rs1), mkU64(simm));
         const HChar* name;
         IRExpr*      expr;
         switch (funct3) {
         case 0b000:
            name = "lb";
            expr = unop(Iop_8Sto64, loadLE(Ity_I8, ea));
            break;
         case 0b001:
            name = "lh";
            expr = unop(Iop_16Sto64, loadLE(Ity_I16, ea));
            break;
         case 0b010:
            name = "lw";
            expr = unop(Iop_32Sto64, loadLE(Ity_I32, ea));
            break;
         case 0b011:
            name = "ld";
            expr = loadLE(Ity_I64, ea);
            break;
         case 0b100:
            name = "lbu";
            expr = unop(Iop_8Uto64, loadLE(Ity_I8, ea));
            break;
         case 0b101:
            name = "lhu";
            expr = unop(Iop_16Uto64, loadLE(Ity_I16, ea));
            break;
         case 0b110:
            name = "lwu";
            expr = unop(Iop_32Uto64, loadLE(Ity_I32, ea));
            break;
         default:
            vassert(0);
         }
         putIReg64(irsb, rd, expr);
         DIP("%s %s, %lld(%s)\n", name, nameIReg64(rd), (Long)simm,
             nameIReg64(rs1));
         return True;
      }
   }

   /* ---------- {sb,sh,sw,sd} rs2, imm[11:0](rs1) ---------- */
   if (INSN(6, 0) == 0b0100011) {
      UInt funct3  = INSN(14, 12);
      UInt rs1     = INSN(19, 15);
      UInt rs2     = INSN(24, 20);
      UInt imm11_0 = INSN(31, 25) << 5 | INSN(11, 7);
      if (funct3 == 0b100 || funct3 == 0b101 || funct3 == 0b110 ||
          funct3 == 0b111) {
         /* Invalid S<x>, fall through. */
      } else {
         ULong        simm = vex_sx_to_64(imm11_0, 12);
         IRExpr*      ea   = binop(Iop_Add64, getIReg64(rs1), mkU64(simm));
         const HChar* name;
         IRExpr*      expr;
         switch (funct3) {
         case 0b000:
            name = "sb";
            expr = unop(Iop_64to8, getIReg64(rs2));
            break;
         case 0b001:
            name = "sh";
            expr = unop(Iop_64to16, getIReg64(rs2));
            break;
         case 0b010:
            name = "sw";
            expr = unop(Iop_64to32, getIReg64(rs2));
            break;
         case 0b011:
            name = "sd";
            expr = getIReg64(rs2);
            break;
         default:
            vassert(0);
         }
         storeLE(irsb, ea, expr);
         DIP("%s %s, %lld(%s)\n", name, nameIReg64(rs2), (Long)simm,
             nameIReg64(rs1));
         return True;
      }
   }

   /* -------- {addi,slti,sltiu} rd, rs1, imm[11:0] --------- */
   /* --------- {xori,ori,andi} rd, rs1, imm[11:0] ---------- */
   if (INSN(6, 0) == 0b0010011) {
      UInt funct3  = INSN(14, 12);
      UInt rd      = INSN(11, 7);
      UInt rs1     = INSN(19, 15);
      UInt imm11_0 = INSN(31, 20);
      if (rd == 0 || funct3 == 0b001 || funct3 == 0b101) {
         /* Invalid <x>I, fall through. */
      } else {
         ULong        simm = vex_sx_to_64(imm11_0, 12);
         const HChar* name;
         IRExpr*      expr;
         switch (funct3) {
         case 0b000:
            name = "addi";
            expr = binop(Iop_Add64, getIReg64(rs1), mkU64(simm));
            break;
         case 0b010:
            name = "slti";
            expr = unop(Iop_1Uto64,
                        binop(Iop_CmpLT64S, getIReg64(rs1), mkU64(simm)));
            break;
         case 0b011:
            /* Note that the comparison itself is unsigned but the immediate is
               sign-extended. */
            name = "sltiu";
            expr = unop(Iop_1Uto64,
                        binop(Iop_CmpLT64U, getIReg64(rs1), mkU64(simm)));
            break;
         case 0b100:
            name = "xori";
            expr = binop(Iop_Xor64, getIReg64(rs1), mkU64(simm));
            break;
         case 0b110:
            name = "ori";
            expr = binop(Iop_Or64, getIReg64(rs1), mkU64(simm));
            break;
         case 0b111:
            name = "andi";
            expr = binop(Iop_And64, getIReg64(rs1), mkU64(simm));
            break;
         default:
            vassert(0);
         }
         putIReg64(irsb, rd, expr);
         DIP("%s %s, %s, %lld\n", name, nameIReg64(rd), nameIReg64(rs1),
             (Long)simm);
         return True;
      }
   }

   /* --------------- slli rd, rs1, uimm[5:0] --------------- */
   if (INSN(6, 0) == 0b0010011 && INSN(14, 12) == 0b001 &&
       INSN(31, 26) == 0b000000) {
      UInt rd      = INSN(11, 7);
      UInt rs1     = INSN(19, 15);
      UInt uimm5_0 = INSN(25, 20);
      if (rd == 0) {
         /* Invalid SLLI, fall through. */
      } else {
         putIReg64(irsb, rd, binop(Iop_Shl64, getIReg64(rs1), mkU8(uimm5_0)));
         DIP("slli %s, %s, %u\n", nameIReg64(rd), nameIReg64(rs1), uimm5_0);
         return True;
      }
   }

   /* ----------- {srli,srai} rd, rs1, uimm[5:0] ----------=- */
   if (INSN(6, 0) == 0b0010011 && INSN(14, 12) == 0b101 &&
       INSN(29, 26) == 0b0000 && INSN(31, 31) == 0b0) {
      Bool is_log  = INSN(30, 30) == 0b0;
      UInt rd      = INSN(11, 7);
      UInt rs1     = INSN(19, 15);
      UInt uimm5_0 = INSN(25, 20);
      if (rd == 0) {
         /* Invalid {SRLI,SRAI}, fall through. */
      } else {
         putIReg64(irsb, rd,
                   binop(is_log ? Iop_Shr64 : Iop_Sar64, getIReg64(rs1),
                         mkU8(uimm5_0)));
         DIP("%s %s, %s, %u\n", is_log ? "srli" : "srai", nameIReg64(rd),
             nameIReg64(rs1), uimm5_0);
         return True;
      }
   }

   /* --------------- {add,sub} rd, rs1, rs2 ---------------- */
   /* ------------- {sll,srl,sra} rd, rs1, rs2 -------------- */
   /* --------------- {slt,sltu} rd, rs1, rs2 --------------- */
   /* -------------- {xor,or,and} rd, rs1, rs2 -------------- */
   if (INSN(6, 0) == 0b0110011 && INSN(29, 25) == 0b00000 &&
       INSN(31, 31) == 0b0) {
      UInt funct3  = INSN(14, 12);
      Bool is_base = INSN(30, 30) == 0b0;
      UInt rd      = INSN(11, 7);
      UInt rs1     = INSN(19, 15);
      UInt rs2     = INSN(24, 20);
      if (rd == 0 || (!is_base && funct3 != 0b000 && funct3 != 0b101)) {
         /* Invalid <x>, fall through. */
      } else {
         const HChar* name;
         IRExpr*      expr;
         switch (funct3) {
         case 0b000:
            name = is_base ? "add" : "sub";
            expr = binop(is_base ? Iop_Add64 : Iop_Sub64, getIReg64(rs1),
                         getIReg64(rs2));
            break;
         case 0b001:
            name = "sll";
            expr = binop(Iop_Shl64, getIReg64(rs1),
                         unop(Iop_64to8, getIReg64(rs2)));
            break;
         case 0b010:
            name = "slt";
            expr = unop(Iop_1Uto64,
                        binop(Iop_CmpLT64S, getIReg64(rs1), getIReg64(rs2)));
            break;
         case 0b011:
            name = "sltu";
            expr = unop(Iop_1Uto64,
                        binop(Iop_CmpLT64U, getIReg64(rs1), getIReg64(rs2)));
            break;
         case 0b100:
            name = "xor";
            expr = binop(Iop_Xor64, getIReg64(rs1), getIReg64(rs2));
            break;
         case 0b101:
            name = is_base ? "srl" : "sra";
            expr = binop(is_base ? Iop_Shr64 : Iop_Sar64, getIReg64(rs1),
                         unop(Iop_64to8, getIReg64(rs2)));
            break;
         case 0b110:
            name = "or";
            expr = binop(Iop_Or64, getIReg64(rs1), getIReg64(rs2));
            break;
         case 0b111:
            name = "and";
            expr = binop(Iop_And64, getIReg64(rs1), getIReg64(rs2));
            break;
         default:
            vassert(0);
         }
         putIReg64(irsb, rd, expr);
         DIP("%s %s, %s, %s\n", name, nameIReg64(rd), nameIReg64(rs1),
             nameIReg64(rs2));
         return True;
      }
   }

   /* ------------------------ fence ------------------------ */
   if (INSN(19, 0) == 0b00000000000000001111 && INSN(31, 28) == 0b0000) {
      UInt succ = INSN(23, 20);
      UInt pred = INSN(27, 24);
      stmt(irsb, IRStmt_MBE(Imbe_Fence));
      if (pred == 0b1111 && succ == 0b1111)
         DIP("fence\n");
      else
         DIP("fence %s%s%s%s,%s%s%s%s\n", (pred & 0x8) ? "i" : "",
             (pred & 0x4) ? "o" : "", (pred & 0x2) ? "r" : "",
             (pred & 0x1) ? "w" : "", (succ & 0x8) ? "i" : "",
             (succ & 0x4) ? "o" : "", (succ & 0x2) ? "r" : "",
             (succ & 0x1) ? "w" : "");
      return True;
   }

   /* ------------------------ ecall ------------------------ */
   if (INSN(31, 0) == 0b00000000000000000000000001110011) {
      putPC(irsb, mkU64(guest_pc_curr_instr + 4));
      dres->whatNext    = Dis_StopHere;
      dres->jk_StopHere = Ijk_Sys_syscall;
      DIP("ecall\n");
      return True;
   }

   /* -------------- addiw rd, rs1, imm[11:0] --------------- */
   if (INSN(6, 0) == 0b0011011 && INSN(14, 12) == 0b000) {
      UInt rd      = INSN(11, 7);
      UInt rs1     = INSN(19, 15);
      UInt imm11_0 = INSN(31, 20);
      if (rd == 0) {
         /* Invalid ADDIW, fall through. */
      } else {
         UInt simm = (UInt)vex_sx_to_64(imm11_0, 12);
         putIReg32(irsb, rd, binop(Iop_Add32, getIReg32(rs1), mkU32(simm)));
         DIP("addiw %s, %s, %d\n", nameIReg64(rd), nameIReg64(rs1), (Int)simm);
         return True;
      }
   }

   /* -------------- slliw rd, rs1, uimm[4:0] --------------- */
   if (INSN(6, 0) == 0b0011011 && INSN(14, 12) == 0b001 &&
       INSN(31, 25) == 0b0000000) {
      UInt rd      = INSN(11, 7);
      UInt rs1     = INSN(19, 15);
      UInt uimm4_0 = INSN(24, 20);
      if (rd == 0) {
         /* Invalid SLLIW, fall through. */
      } else {
         putIReg32(irsb, rd, binop(Iop_Shl32, getIReg32(rs1), mkU8(uimm4_0)));
         DIP("slliw %s, %s, %u\n", nameIReg64(rd), nameIReg64(rs1), uimm4_0);
         return True;
      }
   }

   /* ---------- {srliw,sraiw} rd, rs1, uimm[4:0] ----------- */
   if (INSN(6, 0) == 0b0011011 && INSN(14, 12) == 0b101 &&
       INSN(29, 25) == 0b00000 && INSN(31, 31) == 0b0) {
      Bool is_log  = INSN(30, 30) == 0b0;
      UInt rd      = INSN(11, 7);
      UInt rs1     = INSN(19, 15);
      UInt uimm4_0 = INSN(24, 20);
      if (rd == 0) {
         /* Invalid {SRLIW,SRAIW}, fall through. */
      } else {
         putIReg32(irsb, rd,
                   binop(is_log ? Iop_Shr32 : Iop_Sar32, getIReg32(rs1),
                         mkU8(uimm4_0)));
         DIP("%s %s, %s, %u\n", is_log ? "srliw" : "sraiw", nameIReg64(rd),
             nameIReg64(rs1), uimm4_0);
         return True;
      }
   }

   /* -------------- {addw,subw} rd, rs1, rs2 --------------- */
   if (INSN(6, 0) == 0b0111011 && INSN(14, 12) == 0b000 &&
       INSN(29, 25) == 0b00000 && INSN(31, 31) == 0b0) {
      Bool is_add = INSN(30, 30) == 0b0;
      UInt rd     = INSN(11, 7);
      UInt rs1    = INSN(19, 15);
      UInt rs2    = INSN(24, 20);
      if (rd == 0) {
         /* Invalid {ADDW,SUBW}, fall through. */
      } else {
         putIReg32(irsb, rd,
                   binop(is_add ? Iop_Add32 : Iop_Sub32, getIReg32(rs1),
                         getIReg32(rs2)));
         DIP("%s %s, %s, %s\n", is_add ? "addw" : "subw", nameIReg64(rd),
             nameIReg64(rs1), nameIReg64(rs2));
         return True;
      }
   }

   /* ------------------ sllw rd, rs1, rs2 ------------------ */
   if (INSN(6, 0) == 0b0111011 && INSN(14, 12) == 0b001 &&
       INSN(31, 25) == 0b0000000) {
      UInt rd  = INSN(11, 7);
      UInt rs1 = INSN(19, 15);
      UInt rs2 = INSN(24, 20);
      if (rd == 0) {
         /* Invalid SLLW, fall through. */
      } else {
         putIReg32(
            irsb, rd,
            binop(Iop_Shl32, getIReg32(rs1), unop(Iop_64to8, getIReg64(rs2))));
         DIP("sllw %s, %s, %s\n", nameIReg64(rd), nameIReg64(rs1),
             nameIReg64(rs2));
         return True;
      }
   }

   /* -------------- {srlw,sraw} rd, rs1, rs2 --------------- */
   if (INSN(6, 0) == 0b0111011 && INSN(14, 12) == 0b101 &&
       INSN(29, 25) == 0b00000 && INSN(31, 31) == 0b0) {
      Bool is_log = INSN(30, 30) == 0b0;
      UInt rd     = INSN(11, 7);
      UInt rs1    = INSN(19, 15);
      UInt rs2    = INSN(24, 20);
      if (rd == 0) {
         /* Invalid {SRLW,SRAW}, fall through. */
      } else {
         putIReg32(irsb, rd,
                   binop(is_log ? Iop_Shr32 : Iop_Sar32, getIReg32(rs1),
                         unop(Iop_64to8, getIReg64(rs2))));
         DIP("%s %s, %s, %s\n", is_log ? "srlw" : "sraw", nameIReg64(rd),
             nameIReg64(rs1), nameIReg64(rs2));
         return True;
      }
   }

   /* -------------- RV64M standard extension --------------- */

   /* -------- {mul,mulh,mulhsu,mulhu} rd, rs1, rs2 --------- */
   /* --------------- {div,divu} rd, rs1, rs2 --------------- */
   /* --------------- {rem,remu} rd, rs1, rs2 --------------- */
   if (INSN(6, 0) == 0b0110011 && INSN(31, 25) == 0b0000001) {
      UInt rd     = INSN(11, 7);
      UInt funct3 = INSN(14, 12);
      UInt rs1    = INSN(19, 15);
      UInt rs2    = INSN(24, 20);
      /* TODO Handle mulhsu. */
      if (rd == 0 || funct3 == 0b010) {
         /* Invalid {MUL,DIV,REM}<x>, fall through. */
      } else {
         const HChar* name;
         IRExpr*      expr;
         switch (funct3) {
         case 0b000:
            name = "mul";
            expr = binop(Iop_Mul64, getIReg64(rs1), getIReg64(rs2));
            break;
         case 0b001:
            name = "mulh";
            expr = unop(Iop_128HIto64,
                        binop(Iop_MullS64, getIReg64(rs1), getIReg64(rs2)));
            break;
         case 0b010:
            name = "mulhsu";
            /* TODO */
            vassert(0);
            break;
         case 0b011:
            name = "mulhu";
            expr = unop(Iop_128HIto64,
                        binop(Iop_MullU64, getIReg64(rs1), getIReg64(rs2)));
            break;
         case 0b100:
            name = "div";
            expr = binop(Iop_DivS64, getIReg64(rs1), getIReg64(rs2));
            break;
         case 0b101:
            name = "divu";
            expr = binop(Iop_DivU64, getIReg64(rs1), getIReg64(rs2));
            break;
         case 0b110:
            name = "rem";
            expr = unop(Iop_128HIto64, binop(Iop_DivModS64to64, getIReg64(rs1),
                                             getIReg64(rs2)));
            break;
         case 0b111:
            name = "remu";
            expr = unop(Iop_128HIto64, binop(Iop_DivModU64to64, getIReg64(rs1),
                                             getIReg64(rs2)));
            break;
         default:
            vassert(0);
         }
         putIReg64(irsb, rd, expr);
         DIP("%s %s, %s, %s\n", name, nameIReg64(rd), nameIReg64(rs1),
             nameIReg64(rs2));
         return True;
      }
   }

   /* ------------------ mulw rd, rs1, rs2 ------------------ */
   /* -------------- {divw,divuw} rd, rs1, rs2 -------------- */
   /* -------------- {remw,remuw} rd, rs1, rs2 -------------- */
   if (INSN(6, 0) == 0b0111011 && INSN(31, 25) == 0b0000001) {
      UInt rd     = INSN(11, 7);
      UInt funct3 = INSN(14, 12);
      UInt rs1    = INSN(19, 15);
      UInt rs2    = INSN(24, 20);
      if (rd == 0 || (funct3 == 0b001 || funct3 == 0b010 || funct3 == 0b011)) {
         /* Invalid {MUL,DIV,REM}<x>W, fall through. */
      } else {
         const HChar* name;
         IRExpr*      expr;
         switch (funct3) {
         case 0b000:
            name = "mulw";
            expr = binop(Iop_Mul32, getIReg32(rs1), getIReg32(rs2));
            break;
         case 0b100:
            name = "divw";
            expr = binop(Iop_DivS32, getIReg32(rs1), getIReg32(rs2));
            break;
         case 0b101:
            name = "divuw";
            expr = binop(Iop_DivU32, getIReg32(rs1), getIReg32(rs2));
            break;
         case 0b110:
            name = "remw";
            expr = unop(Iop_64HIto32, binop(Iop_DivModS32to32, getIReg32(rs1),
                                            getIReg32(rs2)));
            break;
         case 0b111:
            name = "remuw";
            expr = unop(Iop_64HIto32, binop(Iop_DivModU32to32, getIReg32(rs1),
                                            getIReg32(rs2)));
            break;
         default:
            vassert(0);
         }
         putIReg32(irsb, rd, expr);
         DIP("%s %s, %s, %s\n", name, nameIReg64(rd), nameIReg64(rs1),
             nameIReg64(rs2));
         return True;
      }
   }

   /* -------------- RV64D standard extension --------------- */

   /* --------------- flw rd, imm[11:0](rs1) ---------------- */
   if (INSN(6, 0) == 0b0000111 && INSN(14, 12) == 0b010) {
      /* TODO Implement. */
      return True;
   }

   /* --------------- fsw rs2, imm[11:0](rs1) --------------- */
   if (INSN(6, 0) == 0b0100111 && INSN(14, 12) == 0b010) {
      /* TODO Implement. */
      return True;
   }

   /* --------------- fld rd, imm[11:0](rs1) ---------------- */
   if (INSN(6, 0) == 0b0000111 && INSN(14, 12) == 0b011) {
      /* TODO Implement. */
#if 0
      UInt rd      = INSN(11, 7);
      UInt rs1     = INSN(19, 15);
      UInt imm11_0 = INSN(31, 20);
      if (rd == 0) {
         /* Invalid LD, fall through. */
      } else {
         ULong simm = vex_sx_to_64(imm11_0, 12);
         putIReg64(
            irsb, rd,
            loadLE(Ity_I64, binop(Iop_Add64, getIReg64(rs1), mkU64(simm))));
         DIP("ld %s, %lld(%s)\n", nameIReg64(rd), (Long)simm, nameIReg64(rs1));
         return True;
      }
#endif
      return True;
   }

   /* --------------- fsd rs2, imm[11:0](rs1) --------------- */
   if (INSN(6, 0) == 0b0100111 && INSN(14, 12) == 0b011) {
      /* TODO Implement. */
#if 0
      UInt rs1     = INSN(19, 15);
      UInt rs2     = INSN(24, 20);
      UInt imm11_0 = INSN(31, 25) << 5 | INSN(11, 7);
      /* Note: All FSD encodings are valid. */
      ULong simm = vex_sx_to_64(imm11_0, 12);
      storeLE(irsb, binop(Iop_Add64, getIReg64(rs1), mkU64(simm)),
              getIReg64(rs2));
      DIP("sd %s, %lld(%s)\n", nameIReg64(rs2), (Long)simm, nameIReg64(rs1));
#endif
      return True;
   }

   /* -------------- RV64A standard extension --------------- */

   /* ----------------- lr.{w,d} rd, (rs1) ------------------ */
   if (INSN(6, 0) == 0b0101111 && INSN(14, 13) == 0b01 &&
       INSN(31, 27) == 0b00010) {
      UInt rd    = INSN(11, 7);
      Bool is_32 = INSN(12, 12) == 0b0;
      UInt rs1   = INSN(19, 15);
      UInt rs2   = INSN(24, 20);
      UInt aqrl  = INSN(26, 25);
      if (rd == 0 || rs2 != 0) {
         /* Invalid LR.<x>, fall through. */
      } else {
         if (aqrl & 0x1)
            stmt(irsb, IRStmt_MBE(Imbe_Fence));

         IRType ty = is_32 ? Ity_I32 : Ity_I64;
         if (abiinfo->guest__use_fallback_LLSC) {
            /* Get address of the load. */
            IRTemp ea = newTemp(irsb, Ity_I64);
            assign(irsb, ea, getIReg64(rs1));

            /* Load the value. */
            IRTemp res = newTemp(irsb, Ity_I64);
            assign(irsb, res, widenSto64(ty, loadLE(ty, mkexpr(ea))));

            /* Set up the LLSC fallback data. */
            stmt(irsb, IRStmt_Put(OFFB_LLSC_DATA, mkexpr(res)));
            stmt(irsb, IRStmt_Put(OFFB_LLSC_ADDR, mkexpr(ea)));
            stmt(irsb, IRStmt_Put(OFFB_LLSC_SIZE, mkU64(4)));

            /* Write the result to the destination register. */
            putIReg64(irsb, rd, mkexpr(res));
         } else {
            /* TODO Rework the non-fallback mode by recognizing common LR+SC
               sequences and simulating them as one. */
            IRTemp res = newTemp(irsb, ty);
            stmt(irsb, IRStmt_LLSC(Iend_LE, res, getIReg64(rs1), NULL /*LL*/));
            putIReg64(irsb, rd, widenSto64(ty, mkexpr(res)));
         }

         if (aqrl & 0x2)
            stmt(irsb, IRStmt_MBE(Imbe_Fence));

         DIP("lr.%s%s %s, (%s)%s\n", is_32 ? "w" : "d", nameAqRlSuffix(aqrl),
             nameIReg64(rd), nameIReg64(rs1),
             abiinfo->guest__use_fallback_LLSC ? " (fallback implementation)"
                                               : "");
         return True;
      }
   }

   /* --------------- sc.{w,d} rd, rs2, (rs1) --------------- */
   if (INSN(6, 0) == 0b0101111 && INSN(14, 13) == 0b01 &&
       INSN(31, 27) == 0b00011) {
      UInt rd    = INSN(11, 7);
      Bool is_32 = INSN(12, 12) == 0b0;
      UInt rs1   = INSN(19, 15);
      UInt rs2   = INSN(24, 20);
      UInt aqrl  = INSN(26, 25);
      if (rd == 0) {
         /* Invalid SC.<x>, fall through. */
      } else {
         if (aqrl & 0x1)
            stmt(irsb, IRStmt_MBE(Imbe_Fence));

         IRType ty = is_32 ? Ity_I32 : Ity_I64;
         if (abiinfo->guest__use_fallback_LLSC) {
            /* Get address of the load. */
            IRTemp ea = newTemp(irsb, Ity_I64);
            assign(irsb, ea, getIReg64(rs1));

            /* Get the continuation address. */
            IRConst* nia = IRConst_U64(guest_pc_curr_instr + 4);

            /* Mark the SC initially as failed. */
            putIReg64(irsb, rd, mkU64(1));

            /* Set that no transaction is in progress. */
            IRTemp size = newTemp(irsb, Ity_I64);
            assign(irsb, size, IRExpr_Get(OFFB_LLSC_SIZE, Ity_I64));
            stmt(irsb,
                 IRStmt_Put(OFFB_LLSC_SIZE, mkU64(0) /* "no transaction" */));

            /* Fail if no or wrong-size transaction. */
            stmt(irsb, IRStmt_Exit(binop(Iop_CmpNE64, mkexpr(size), mkU64(4)),
                                   Ijk_Boring, nia, OFFB_PC));

            /* Fail if the address doesn't match the LL address. */
            stmt(irsb, IRStmt_Exit(binop(Iop_CmpNE64, mkexpr(ea),
                                         IRExpr_Get(OFFB_LLSC_ADDR, Ity_I64)),
                                   Ijk_Boring, nia, OFFB_PC));

            /* Fail if the data doesn't match the LL data. */
            IRTemp data = newTemp(irsb, Ity_I64);
            assign(irsb, data, IRExpr_Get(OFFB_LLSC_DATA, Ity_I64));
            stmt(irsb, IRStmt_Exit(binop(Iop_CmpNE64,
                                         widenSto64(ty, loadLE(ty, mkexpr(ea))),
                                         mkexpr(data)),
                                   Ijk_Boring, nia, OFFB_PC));

            /* Try to CAS the new value in. */
            IRTemp old  = newTemp(irsb, ty);
            IRTemp expd = newTemp(irsb, ty);
            assign(irsb, expd, narrowFrom64(ty, mkexpr(data)));
            stmt(irsb, IRStmt_CAS(mkIRCAS(
                          /*oldHi*/ IRTemp_INVALID, old, Iend_LE, mkexpr(ea),
                          /*expdHi*/ NULL, mkexpr(expd),
                          /*dataHi*/ NULL, narrowFrom64(ty, getIReg64(rs2)))));

            /* Fail if the CAS failed (old != expd). */
            stmt(irsb, IRStmt_Exit(binop(is_32 ? Iop_CmpNE32 : Iop_CmpNE64,
                                         mkexpr(old), mkexpr(expd)),
                                   Ijk_Boring, nia, OFFB_PC));

            /* Otherwise mark the operation as successful. */
            putIReg64(irsb, rd, mkU64(0));
         } else {
            IRTemp res = newTemp(irsb, Ity_I1);
            stmt(irsb, IRStmt_LLSC(Iend_LE, res, getIReg64(rs1),
                                   narrowFrom64(ty, getIReg64(rs2))));
            /* IR semantics: res is 1 if store succeeds, 0 if it fails. Need to
               set rd to 1 on failure, 0 on success. */
            putIReg64(
               irsb, rd,
               binop(Iop_Xor64, unop(Iop_1Uto64, mkexpr(res)), mkU64(1)));
         }

         if (aqrl & 0x2)
            stmt(irsb, IRStmt_MBE(Imbe_Fence));

         DIP("sc.%s%s %s, %s, (%s)%s\n", is_32 ? "w" : "d",
             nameAqRlSuffix(aqrl), nameIReg64(rd), nameIReg64(rs2),
             nameIReg64(rs1),
             abiinfo->guest__use_fallback_LLSC ? " (fallback implementation)"
                                               : "");
         return True;
      }
   }

   /* --------- amo{swap,add}.{w,d} rd, rs2, (rs1) ---------- */
   /* -------- amo{xor,and,or}.{w,d} rd, rs2, (rs1) --------- */
   /* ---------- amo{min,max}.{w,d} rd, rs2, (rs1) ---------- */
   /* --------- amo{minu,maxu}.{w,d} rd, rs2, (rs1) --------- */
   if (INSN(6, 0) == 0b0101111 && INSN(14, 13) == 0b01) {
      UInt rd     = INSN(11, 7);
      Bool is_32  = INSN(12, 12) == 0b0;
      UInt rs1    = INSN(19, 15);
      UInt rs2    = INSN(24, 20);
      UInt aqrl   = INSN(26, 25);
      UInt funct5 = INSN(31, 27);
      if ((funct5 & 0b00010) || funct5 == 0b00101 || funct5 == 0b01001 ||
          funct5 == 0b01101 || funct5 == 0b10001 || funct5 == 0b10101 ||
          funct5 == 0b11001 || funct5 == 0b11101) {
         /* Invalid AMO<x>, fall through. */
      } else {
         if (aqrl & 0x1)
            stmt(irsb, IRStmt_MBE(Imbe_Fence));

         IRTemp addr = newTemp(irsb, Ity_I64);
         assign(irsb, addr, getIReg64(rs1));

         IRType ty   = is_32 ? Ity_I32 : Ity_I64;
         IRTemp orig = newTemp(irsb, ty);
         assign(irsb, orig, loadLE(ty, mkexpr(addr)));
         IRExpr* lhs = mkexpr(orig);
         IRExpr* rhs = narrowFrom64(ty, getIReg64(rs2));

         /* Perform the operation. */
         const HChar* name;
         IRExpr*      res;
         switch (funct5) {
         case 0b00001:
            name = "amoswap";
            res  = rhs;
            break;
         case 0b00000:
            name = "amoadd";
            res  = binop(is_32 ? Iop_Add32 : Iop_Add64, lhs, rhs);
            break;
         case 0b00100:
            name = "amoxor";
            res  = binop(is_32 ? Iop_Xor32 : Iop_Xor64, lhs, rhs);
            break;
         case 0b01100:
            name = "amoand";
            res  = binop(is_32 ? Iop_And32 : Iop_And64, lhs, rhs);
            break;
         case 0b01000:
            name = "amoor";
            res  = binop(is_32 ? Iop_Or32 : Iop_Or64, lhs, rhs);
            break;
         case 0b10000:
            name = "amomin";
            res  = IRExpr_ITE(
               binop(is_32 ? Iop_CmpLT32S : Iop_CmpLT64S, lhs, rhs), lhs, rhs);
            break;
         case 0b10100:
            name = "amomax";
            res  = IRExpr_ITE(
               binop(is_32 ? Iop_CmpLT32S : Iop_CmpLT64S, lhs, rhs), rhs, lhs);
            break;
         case 0b11000:
            name = "amominu";
            res  = IRExpr_ITE(
               binop(is_32 ? Iop_CmpLT32U : Iop_CmpLT64U, lhs, rhs), lhs, rhs);
            break;
         case 0b11100:
            name = "amomaxu";
            res  = IRExpr_ITE(
               binop(is_32 ? Iop_CmpLT32U : Iop_CmpLT64U, lhs, rhs), rhs, lhs);
            break;
         default:
            vassert(0);
         }

         /* Store the result back if the original value remains unchanged in
            memory. */
         IRTemp old = newTemp(irsb, ty);
         stmt(irsb, IRStmt_CAS(mkIRCAS(/*oldHi*/ IRTemp_INVALID, old, Iend_LE,
                                       mkexpr(addr),
                                       /*expdHi*/ NULL, mkexpr(orig),
                                       /*dataHi*/ NULL, res)));

         if (aqrl & 0x2)
            stmt(irsb, IRStmt_MBE(Imbe_Fence));

         /* Retry if the CAS failed (i.e. when old != orig). */
         stmt(irsb, IRStmt_Exit(binop(is_32 ? Iop_CasCmpNE32 : Iop_CasCmpNE64,
                                      mkexpr(old), mkexpr(orig)),
                                Ijk_Boring, IRConst_U64(guest_pc_curr_instr),
                                OFFB_PC));
         /* Otherwise we succeeded. */
         if (rd != 0)
            putIReg64(irsb, rd, widenSto64(ty, mkexpr(old)));

         DIP("%s.%s%s %s, %s, (%s)\n", name, is_32 ? "w" : "d",
             nameAqRlSuffix(aqrl), nameIReg64(rd), nameIReg64(rs2),
             nameIReg64(rs1));
         return True;
      }
   }

   if (sigill_diag)
      vex_printf("RISCV64 front end: standard\n");
   return False;
#undef INSN
}

/* Disassemble a single riscv64 instruction into IR. Returns True iff the
   instruction was decoded, in which case *dres will be set accordingly, or
   False, in which case *dres should be ignored by the caller. */
static Bool disInstr_RISCV64_WRK(/*MB_OUT*/ DisResult* dres,
                                 /*OUT*/ IRSB*         irsb,
                                 const UChar*          guest_instr,
                                 Addr                  guest_pc_curr_instr,
                                 const VexArchInfo*    archinfo,
                                 const VexAbiInfo*     abiinfo,
                                 Bool                  sigill_diag)
{
   /* A macro to fish bits out of 'insn'. */
#define INSN(_bMax, _bMin) SLICE_UInt(insn, (_bMax), (_bMin))

   /* Set result defaults. */
   dres->whatNext    = Dis_Continue;
   dres->len         = 0;
   dres->jk_StopHere = Ijk_INVALID;
   dres->hint        = Dis_HintNone;

   /* Read the instruction word. */
   UInt insn = getInsn(guest_instr);

   if (0)
      vex_printf("insn: 0x%x\n", insn);

   DIP("\t(riscv64) 0x%llx:  ", (ULong)guest_pc_curr_instr);

   vassert((guest_pc_curr_instr & 1) == 0);

   /* Spot "Special" instructions (see comment at top of file). */
   {
      const UChar* code = guest_instr;
      /* Spot the 16-byte preamble:
            00365613   srli a2, a2, 3
            00d65613   srli a2, a2, 13
            03365613   srli a2, a2, 51
            03d65613   srli a2, a2, 61
      */
      UInt word1 = 0x00365613;
      UInt word2 = 0x00d65613;
      UInt word3 = 0x03365613;
      UInt word4 = 0x03d65613;
      if (getUIntLittleEndianly(code + 0) == word1 &&
          getUIntLittleEndianly(code + 4) == word2 &&
          getUIntLittleEndianly(code + 8) == word3 &&
          getUIntLittleEndianly(code + 12) == word4) {
         /* Got a "Special" instruction preamble. Which one is it? */
         dres->len = 20;
         UInt which = getUIntLittleEndianly(code + 16);
         if (which == 0x00a56533 /* or a0, a0, a0 */) {
            /* a3 = client_request ( a4 ) */
            DIP("a3 = client_request ( a4 )\n");
            putPC(irsb, mkU64(guest_pc_curr_instr + 20));
            dres->jk_StopHere = Ijk_ClientReq;
            dres->whatNext    = Dis_StopHere;
            return True;
         } else if (which == 0x00b5e5b3 /* or a1, a1, a1 */) {
            /* a3 = guest_NRADDR */
            DIP("a3 = guest_NRADDR\n");
            putIReg64(irsb, 13 /*x13/a3*/, IRExpr_Get(OFFB_NRADDR, Ity_I64));
            return True;
         } else if (which == 0x00c66633 /* or a2, a2, a2 */) {
            /* branch-and-link-to-noredir t0 */
            DIP("branch-and-link-to-noredir t0\n");
            putIReg64(irsb, 1 /*x1/ra*/, mkU64(guest_pc_curr_instr + 20));
            putPC(irsb, getIReg64(5 /*x5/t0*/));
            dres->jk_StopHere = Ijk_NoRedir;
            dres->whatNext    = Dis_StopHere;
            return True;
         } else if (which == 0x00d6e6b3 /* or a3, a3, a3 */) {
            /* IR injection */
            DIP("IR injection\n");
            vex_inject_ir(irsb, Iend_LE);
            /* Invalidate the current insn. The reason is that the IRop we're
               injecting here can change. In which case the translation has to
               be redone. For ease of handling, we simply invalidate all the
               time. */
            stmt(irsb, IRStmt_Put(OFFB_CMSTART, mkU64(guest_pc_curr_instr)));
            stmt(irsb, IRStmt_Put(OFFB_CMLEN, mkU64(20)));
            putPC(irsb, mkU64(guest_pc_curr_instr + 20));
            dres->whatNext    = Dis_StopHere;
            dres->jk_StopHere = Ijk_InvalICache;
            return True;
         }
         /* We don't know what it is. */
         return False;
      }
   }

   /* Main riscv64 instruction decoder starts here. */
   Bool ok = False;
   UInt inst_size;

   /* Parse insn[1:0] to determine whether the instruction is 16-bit
      (compressed) or 32-bit. */
   switch (INSN(1, 0)) {
   case 0b00:
   case 0b01:
   case 0b10:
      dres->len = inst_size = 2;
      ok = dis_RISCV64_compressed(dres, irsb, insn, guest_pc_curr_instr,
                                  sigill_diag);
      break;

   case 0b11:
      dres->len = inst_size = 4;
      ok = dis_RISCV64_standard(dres, irsb, insn, guest_pc_curr_instr, abiinfo,
                                sigill_diag);
      break;

   default:
      vassert(0); /* Can't happen. */
   }

   /* If the next-level down decoders failed, make sure dres didn't get
      changed. */
   if (!ok) {
      vassert(dres->whatNext == Dis_Continue);
      vassert(dres->len == inst_size);
      vassert(dres->jk_StopHere == Ijk_INVALID);
   }

   return ok;
#undef INSN
}

/*------------------------------------------------------------*/
/*--- Top-level fn                                         ---*/
/*------------------------------------------------------------*/

/* Disassemble a single instruction into IR. The instruction is located in host
   memory at &guest_code[delta]. */
DisResult disInstr_RISCV64(IRSB*              irsb,
                           const UChar*       guest_code,
                           Long               delta,
                           Addr               guest_IP,
                           VexArch            guest_arch,
                           const VexArchInfo* archinfo,
                           const VexAbiInfo*  abiinfo,
                           VexEndness         host_endness,
                           Bool               sigill_diag)
{
   DisResult dres;
   vex_bzero(&dres, sizeof(dres));

   vassert(guest_arch == VexArchRISCV64);

   /* Try to decode. */
   Bool ok = disInstr_RISCV64_WRK(&dres, irsb, &guest_code[delta], guest_IP,
                                  archinfo, abiinfo, sigill_diag);
   if (ok) {
      /* All decode successes end up here. */
      vassert(dres.len == 2 || dres.len == 4 || dres.len == 20);
      switch (dres.whatNext) {
      case Dis_Continue:
         putPC(irsb, mkU64(guest_IP + dres.len));
         break;
      case Dis_StopHere:
         break;
      default:
         vassert(0);
      }
      DIP("\n");
   } else {
      /* All decode failures end up here. */
      if (sigill_diag) {
         Int   i, j;
         UChar buf[64];
         UInt  insn = getInsn(&guest_code[delta]);
         vex_bzero(buf, sizeof(buf));
         for (i = j = 0; i < 32; i++) {
            if (i > 0) {
               if ((i & 7) == 0)
                  buf[j++] = ' ';
               else if ((i & 3) == 0)
                  buf[j++] = '\'';
            }
            buf[j++] = (insn & (1 << (31 - i))) ? '1' : '0';
         }
         vex_printf("disInstr(riscv64): unhandled instruction 0x%08x\n", insn);
         vex_printf("disInstr(riscv64): %s\n", buf);
      }

      /* Tell the dispatcher that this insn cannot be decoded, and so has not
         been executed, and (is currently) the next to be executed. The pc
         register should be up-to-date since it is made so at the start of each
         insn, but nevertheless be paranoid and update it again right now. */
      putPC(irsb, mkU64(guest_IP));
      dres.len         = 0;
      dres.whatNext    = Dis_StopHere;
      dres.jk_StopHere = Ijk_NoDecode;
   }
   return dres;
}

/*--------------------------------------------------------------------*/
/*--- end                                     guest_riscv64_toIR.c ---*/
/*--------------------------------------------------------------------*/
