
/*--------------------------------------------------------------------*/
/*--- begin                                    host_riscv64_defs.h ---*/
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

   Neither the names of the U.S. Department of Energy nor the
   University of California nor the names of its contributors may be
   used to endorse or promote products derived from this software
   without prior written permission.
*/

#ifndef __VEX_HOST_RISCV64_DEFS_H
#define __VEX_HOST_RISCV64_DEFS_H

#include "libvex.h"
#include "libvex_basictypes.h"

#include "host_generic_regs.h"

/*------------------------------------------------------------*/
/*--- Registers                                            ---*/
/*------------------------------------------------------------*/

#define ST_IN static inline
ST_IN HReg hregRISCV64_x18(void) { return mkHReg(False, HRcInt64, 18, 0); }
ST_IN HReg hregRISCV64_x19(void) { return mkHReg(False, HRcInt64, 19, 1); }
ST_IN HReg hregRISCV64_x20(void) { return mkHReg(False, HRcInt64, 20, 2); }
ST_IN HReg hregRISCV64_x21(void) { return mkHReg(False, HRcInt64, 21, 3); }
ST_IN HReg hregRISCV64_x22(void) { return mkHReg(False, HRcInt64, 22, 4); }
ST_IN HReg hregRISCV64_x23(void) { return mkHReg(False, HRcInt64, 23, 5); }
ST_IN HReg hregRISCV64_x24(void) { return mkHReg(False, HRcInt64, 24, 6); }
ST_IN HReg hregRISCV64_x25(void) { return mkHReg(False, HRcInt64, 25, 7); }
ST_IN HReg hregRISCV64_x26(void) { return mkHReg(False, HRcInt64, 26, 8); }
ST_IN HReg hregRISCV64_x27(void) { return mkHReg(False, HRcInt64, 27, 9); }

ST_IN HReg hregRISCV64_x0(void) { return mkHReg(False, HRcInt64, 0, 10); }
ST_IN HReg hregRISCV64_x8(void) { return mkHReg(False, HRcInt64, 8, 11); }
#undef ST_IN

/*------------------------------------------------------------*/
/*--- Instructions                                         ---*/
/*------------------------------------------------------------*/

/* The kind of instructions. */
typedef enum {
   RISCV64in_LI = 0x52640000, /* Load immediate pseudoinstruction. */
   RISCV64in_MV,              /* Copy one register to another. */
   RISCV64in_ADD,             /* Addition of two registers. */
   RISCV64in_ADDIW,           /* 32-bit addition of a register and a sx-12-bit
                                 immediate. */
   RISCV64in_SUB,             /* Subtraction of one register from another. */
   RISCV64in_XOR,             /* Bitwise logical XOR of two registers. */
   RISCV64in_OR,              /* Bitwise logical OR of two registers. */
   RISCV64in_AND,             /* Bitwise logical AND of two registers. */
   RISCV64in_SLL,             /* Logical left shift on a register. */
   RISCV64in_SRL,             /* Logical right shift on a register. */
   RISCV64in_SRA,             /* Arithmetic right shift on a register. */
   RISCV64in_SLLI,            /* Logical left shift on a register by a 6-bit
                                 immediate. */
   RISCV64in_SRLI,            /* Logical right shift on a register by a 6-bit
                                 immediate. */
   RISCV64in_SLLW,            /* 32-bit logical left shift on a register. */
   RISCV64in_SRAW,            /* 32-bit arithmetic right shift on a register. */
   RISCV64in_SLT,             /* Signed comparison of two registers. */
   RISCV64in_SLTU,            /* Unsigned comparison of two registers. */
   RISCV64in_SLTIU,           /* Unsigned comparison of a register and
                                 a sx-12-bit immediate. */
   RISCV64in_MUL,             /* Multiplication of two registers, producing the
                                 lower 64 bits. */
   RISCV64in_DIVU,            /* Unsigned division of one register by
                                 another. */
   RISCV64in_REMU,            /* Remainder from unsigned division of one
                                 register by another. */
   RISCV64in_LD,              /* 64-bit load. */
   RISCV64in_LW,              /* sx-32-to-64-bit load. */
   RISCV64in_LH,              /* sx-16-to-64-bit load. */
   RISCV64in_LB,              /* sx-8-to-64-bit load. */
   RISCV64in_SD,              /* 64-bit store. */
   RISCV64in_SW,              /* 32-bit store. */
   RISCV64in_SH,              /* 16-bit store. */
   RISCV64in_SB,              /* 8-bit store. */
   RISCV64in_FENCE,           /* Device I/O and memory fence. */
   RISCV64in_XDirect,         /* Direct transfer to guest address. */
   RISCV64in_XIndir,          /* Indirect transfer to guest address. */
   RISCV64in_XAssisted,       /* Assisted transfer to guest address. */
} RISCV64InstrTag;

typedef struct {
   RISCV64InstrTag tag;
   union {
      /* Load immediate pseudoinstruction. */
      struct {
         HReg  dst;
         ULong imm64;
      } LI;
      /* Copy one register to another. */
      struct {
         HReg dst;
         HReg src;
      } MV;
      /* Addition of two registers. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } ADD;
      /* 32-bit addition of a register and a sx-12-bit immediate. */
      struct {
         HReg dst;
         HReg src;
         Int  simm12;
      } ADDIW;
      /* Subtraction of one register from another. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } SUB;
      /* Bitwise logical XOR of two registers. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } XOR;
      /* Bitwise logical OR of two registers. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } OR;
      /* Bitwise logical AND of two registers. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } AND;
      /* Logical left shift on a register. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } SLL;
      /* Logical right shift on a register. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } SRL;
      /* Arithmetic right shift on a register. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } SRA;
      /* Logical left shift on a register by a 6-bit immediate. */
      struct {
         HReg dst;
         HReg src;
         UInt uimm6;
      } SLLI;
      /* Logical right shift on a register by a 6-bit immediate. */
      struct {
         HReg dst;
         HReg src;
         UInt uimm6;
      } SRLI;
      /* 32-bit logical left shift on a register. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } SLLW;
      /* 32-bit arithmetic right shift on a register. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } SRAW;
      /* Signed comparison of two registers. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } SLT;
      /* Unsigned comparison of two registers. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } SLTU;
      /* Unsigned comparison of a register and a sx-12-bit immediate. */
      struct {
         HReg dst;
         HReg src;
         Int  simm12;
      } SLTIU;
      /* Multiplication of two registers, producing the lower 64 bits. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } MUL;
      /* Unsigned division of one register by another. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } DIVU;
      /* Remainder from unsigned division of one register by another. */
      struct {
         HReg dst;
         HReg src1;
         HReg src2;
      } REMU;
      /* 64-bit load. */
      struct {
         HReg dst;
         HReg base;
         Int  soff12; /* -2048 .. +2047 */
      } LD;
      /* sx-32-to-64-bit load. */
      struct {
         HReg dst;
         HReg base;
         Int  soff12; /* -2048 .. +2047 */
      } LW;
      /* sx-16-to-64-bit load. */
      struct {
         HReg dst;
         HReg base;
         Int  soff12; /* -2048 .. +2047 */
      } LH;
      /* sx-8-to-64-bit load. */
      struct {
         HReg dst;
         HReg base;
         Int  soff12; /* -2048 .. +2047 */
      } LB;
      /* 64-bit store. */
      struct {
         HReg src;
         HReg base;
         Int  soff12; /* -2048 .. +2047 */
      } SD;
      /* 32-bit store. */
      struct {
         HReg src;
         HReg base;
         Int  soff12; /* -2048 .. +2047 */
      } SW;
      /* 16-bit store. */
      struct {
         HReg src;
         HReg base;
         Int  soff12; /* -2048 .. +2047 */
      } SH;
      /* 8-bit store. */
      struct {
         HReg src;
         HReg base;
         Int  soff12; /* -2048 .. +2047 */
      } SB;
      /* Device I/O and memory fence. */
      struct {
      } FENCE;
      /* Update the guest pc value, then exit requesting to chain to it. May be
         conditional. */
      struct {
         Addr64 dstGA;    /* Next guest address. */
         HReg   base;     /* Base to access the guest state. */
         Int    soff12;   /* Offset from the base register to access pc. */
         HReg   cond;     /* Condition, can be INVALID_HREG for "always". */
         Bool   toFastEP; /* Chain to the slow or fast point? */
      } XDirect;
      /* Boring transfer to a guest address not known at JIT time. Not
         chainable. May be conditional. */
      struct {
         HReg dstGA;  /* Next guest address. */
         HReg base;   /* Base to access the guest state. */
         Int  soff12; /* Offset from the base register to access pc. */
         HReg cond;   /* Condition, can be INVALID_REG for "always". */
      } XIndir;
      /* Assisted transfer to a guest address, most general case. Not chainable.
         May be conditional. */
      struct {
         HReg       dstGA;  /* Next guest address. */
         HReg       base;   /* Base to access the guest state. */
         Int        soff12; /* Offset from the base register to access pc. */
         HReg       cond;   /* Condition, can be INVALID_REG for "always". */
         IRJumpKind jk;
      } XAssisted;
   } RISCV64in;
} RISCV64Instr;

RISCV64Instr* RISCV64Instr_LI(HReg dst, ULong imm64);
RISCV64Instr* RISCV64Instr_MV(HReg dst, HReg src);
RISCV64Instr* RISCV64Instr_ADD(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_ADDIW(HReg dst, HReg src, Int simm12);
RISCV64Instr* RISCV64Instr_SUB(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_XOR(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_OR(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_AND(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_SLL(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_SRL(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_SRA(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_SLLI(HReg dst, HReg src, UInt uimm6);
RISCV64Instr* RISCV64Instr_SRLI(HReg dst, HReg src, UInt uimm6);
RISCV64Instr* RISCV64Instr_SLLW(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_SRAW(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_SLT(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_SLTU(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_SLTIU(HReg dst, HReg src, Int simm12);
RISCV64Instr* RISCV64Instr_MUL(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_DIVU(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_REMU(HReg dst, HReg src1, HReg src2);
RISCV64Instr* RISCV64Instr_LD(HReg dst, HReg base, Int soff12);
RISCV64Instr* RISCV64Instr_LW(HReg dst, HReg base, Int soff12);
RISCV64Instr* RISCV64Instr_LH(HReg dst, HReg base, Int soff12);
RISCV64Instr* RISCV64Instr_LB(HReg dst, HReg base, Int soff12);
RISCV64Instr* RISCV64Instr_SD(HReg src, HReg base, Int soff12);
RISCV64Instr* RISCV64Instr_SW(HReg src, HReg base, Int soff12);
RISCV64Instr* RISCV64Instr_SH(HReg src, HReg base, Int soff12);
RISCV64Instr* RISCV64Instr_SB(HReg src, HReg base, Int soff12);
RISCV64Instr* RISCV64Instr_FENCE(void);
RISCV64Instr* RISCV64Instr_XDirect(
   Addr64 dstGA, HReg base, Int soff12, HReg cond, Bool toFastEP);
RISCV64Instr* RISCV64Instr_XIndir(HReg dstGA, HReg base, Int soff12, HReg cond);
RISCV64Instr* RISCV64Instr_XAssisted(
   HReg dstGA, HReg base, Int soff12, HReg cond, IRJumpKind jk);

/*------------------------------------------------------------*/
/* --- Interface exposed to VEX                           --- */
/*------------------------------------------------------------*/

UInt ppHRegRISCV64(HReg reg);

void ppRISCV64Instr(const RISCV64Instr* i, Bool mode64);

const RRegUniverse* getRRegUniverse_RISCV64(void);

/* Some functions that insulate the register allocator from details of the
   underlying instruction set. */
void getRegUsage_RISCV64Instr(HRegUsage* u, const RISCV64Instr* i, Bool mode64);
void mapRegs_RISCV64Instr(HRegRemap* m, RISCV64Instr* i, Bool mode64);

void genSpill_RISCV64(
   /*OUT*/ HInstr** i1, /*OUT*/ HInstr** i2, HReg rreg, Int offset, Bool);
void genReload_RISCV64(
   /*OUT*/ HInstr** i1, /*OUT*/ HInstr** i2, HReg rreg, Int offset, Bool);
RISCV64Instr* genMove_RISCV64(HReg from, HReg to, Bool);

Int emit_RISCV64Instr(/*MB_MOD*/ Bool*    is_profInc,
                      UChar*              buf,
                      Int                 nbuf,
                      const RISCV64Instr* i,
                      Bool                mode64,
                      VexEndness          endness_host,
                      const void*         disp_cp_chain_me_to_slowEP,
                      const void*         disp_cp_chain_me_to_fastEP,
                      const void*         disp_cp_xindir,
                      const void*         disp_cp_xassisted);

/* Return the number of bytes of code needed for an event check. */
Int evCheckSzB_RISCV64(void);

/* Perform a chaining and unchaining of an XDirect jump. */
VexInvalRange chainXDirect_RISCV64(VexEndness  endness_host,
                                   void*       place_to_chain,
                                   const void* disp_cp_chain_me_EXPECTED,
                                   const void* place_to_jump_to);

VexInvalRange unchainXDirect_RISCV64(VexEndness  endness_host,
                                     void*       place_to_unchain,
                                     const void* place_to_jump_to_EXPECTED,
                                     const void* disp_cp_chain_me);

/* Patch the counter location into an existing ProfInc point. */
VexInvalRange patchProfInc_RISCV64(VexEndness   endness_host,
                                   void*        place_to_patch,
                                   const ULong* location_of_counter);

HInstrArray* iselSB_RISCV64(const IRSB*        bb,
                            VexArch            arch_host,
                            const VexArchInfo* archinfo_host,
                            const VexAbiInfo*  vbi,
                            Int                offs_Host_EvC_Counter,
                            Int                offs_Host_EvC_FailAddr,
                            Bool               chainingAllowed,
                            Bool               addProfInc,
                            Addr               max_ga);

#endif /* ndef __VEX_HOST_RISCV64_DEFS_H */

/*--------------------------------------------------------------------*/
/*--- end                                      host_riscv64_defs.h ---*/
/*--------------------------------------------------------------------*/
