
/*--------------------------------------------------------------------*/
/*--- begin                                    host_riscv64_defs.c ---*/
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

#include "host_riscv64_defs.h"
#include "main_util.h"

/*------------------------------------------------------------*/
/*--- Registers                                            ---*/
/*------------------------------------------------------------*/

UInt ppHRegRISCV64(HReg reg)
{
   static const HChar* names[32] = {
      "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
      "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
      "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

   /* Be generic for all virtual regs. */
   if (hregIsVirtual(reg))
      return ppHReg(reg);

   /* Be specific for real regs. */
   switch (hregClass(reg)) {
   case HRcInt64: {
      UInt r = hregEncoding(reg);
      vassert(r >= 0 && r <= 31);
      return vex_printf("%s", names[r]);
   }
   default:
      vpanic("ppHRegRISCV64");
   }
}

static inline UInt iregEnc(HReg r)
{
   vassert(hregClass(r) == HRcInt64);
   vassert(!hregIsVirtual(r));

   UInt n = hregEncoding(r);
   vassert(n > 0 && n <= 31);
   return n;
}

/*------------------------------------------------------------*/
/*--- Memory address expressions (amodes)                  ---*/
/*------------------------------------------------------------*/

RISCV64AMode* RISCV64AMode_RI12(HReg reg, Int soff12)
{
   RISCV64AMode* am          = LibVEX_Alloc_inline(sizeof(RISCV64AMode));
   am->tag                   = RISCV64am_RI12;
   am->RISCV64am.RI12.reg    = reg;
   am->RISCV64am.RI12.soff12 = soff12;
   vassert(soff12 >= -2048 && soff12 <= 2047);
   return am;
}

static void ppRISCV64AMode(RISCV64AMode* am)
{
   switch (am->tag) {
   case RISCV64am_RI12:
      vex_printf("%d(", am->RISCV64am.RI12.soff12);
      ppHRegRISCV64(am->RISCV64am.RI12.reg);
      vex_printf(")");
      break;
   default:
      vpanic("ppRISCV64AMode");
   }
}

static void addRegUsage_RISCV64AMode(HRegUsage* u, RISCV64AMode* am)
{
   switch (am->tag) {
   case RISCV64am_RI12:
      addHRegUse(u, HRmRead, am->RISCV64am.RI12.reg);
      return;
   default:
      vpanic("addRegUsage_RISCV64Amode");
   }
}

static void mapRegs_RISCV64AMode(HRegRemap* m, RISCV64AMode* am)
{
   switch (am->tag) {
   case RISCV64am_RI12:
      am->RISCV64am.RI12.reg = lookupHRegRemap(m, am->RISCV64am.RI12.reg);
      return;
   default:
      vpanic("mapRegs_RISCV64Amode");
   }
}

/*------------------------------------------------------------*/
/*--- Instructions                                         ---*/
/*------------------------------------------------------------*/

RISCV64Instr* RISCV64Instr_LI(HReg dst, ULong imm64)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_LI;
   i->RISCV64in.LI.dst   = dst;
   i->RISCV64in.LI.imm64 = imm64;
   return i;
}

RISCV64Instr* RISCV64Instr_LD(HReg dst, RISCV64AMode* amode)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_LD;
   i->RISCV64in.LD.dst   = dst;
   i->RISCV64in.LD.amode = amode;
   return i;
}

RISCV64Instr* RISCV64Instr_LW(HReg dst, RISCV64AMode* amode)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_LW;
   i->RISCV64in.LW.dst   = dst;
   i->RISCV64in.LW.amode = amode;
   return i;
}

RISCV64Instr* RISCV64Instr_LH(HReg dst, RISCV64AMode* amode)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_LH;
   i->RISCV64in.LH.dst   = dst;
   i->RISCV64in.LH.amode = amode;
   return i;
}

RISCV64Instr* RISCV64Instr_LB(HReg dst, RISCV64AMode* amode)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_LB;
   i->RISCV64in.LB.dst   = dst;
   i->RISCV64in.LB.amode = amode;
   return i;
}

RISCV64Instr* RISCV64Instr_SD(HReg src, RISCV64AMode* amode)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_SD;
   i->RISCV64in.SD.src   = src;
   i->RISCV64in.SD.amode = amode;
   return i;
}

RISCV64Instr* RISCV64Instr_SW(HReg src, RISCV64AMode* amode)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_SW;
   i->RISCV64in.SW.src   = src;
   i->RISCV64in.SW.amode = amode;
   return i;
}

RISCV64Instr* RISCV64Instr_SH(HReg src, RISCV64AMode* amode)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_SH;
   i->RISCV64in.SH.src   = src;
   i->RISCV64in.SH.amode = amode;
   return i;
}

RISCV64Instr* RISCV64Instr_SB(HReg src, RISCV64AMode* amode)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_SB;
   i->RISCV64in.SB.src   = src;
   i->RISCV64in.SB.amode = amode;
   return i;
}

void ppRISCV64Instr(const RISCV64Instr* i, Bool mode64)
{
   vassert(mode64 == True);
   switch (i->tag) {
   case RISCV64in_LI:
      vex_printf("li      ");
      ppHRegRISCV64(i->RISCV64in.LI.dst);
      vex_printf(", 0x%llx", i->RISCV64in.LI.imm64);
      return;
   case RISCV64in_LD:
      vex_printf("ld      ");
      ppHRegRISCV64(i->RISCV64in.LD.dst);
      vex_printf(", ");
      ppRISCV64AMode(i->RISCV64in.LD.amode);
      return;
   case RISCV64in_LW:
      vex_printf("lw      ");
      ppHRegRISCV64(i->RISCV64in.LW.dst);
      vex_printf(", ");
      ppRISCV64AMode(i->RISCV64in.LW.amode);
      return;
   case RISCV64in_LH:
      vex_printf("lh      ");
      ppHRegRISCV64(i->RISCV64in.LH.dst);
      vex_printf(", ");
      ppRISCV64AMode(i->RISCV64in.LH.amode);
      return;
   case RISCV64in_LB:
      vex_printf("lb      ");
      ppHRegRISCV64(i->RISCV64in.LB.dst);
      vex_printf(", ");
      ppRISCV64AMode(i->RISCV64in.LB.amode);
      return;
   case RISCV64in_SD:
      vex_printf("sd      ");
      ppHRegRISCV64(i->RISCV64in.SD.src);
      vex_printf(", ");
      ppRISCV64AMode(i->RISCV64in.SD.amode);
      return;
   case RISCV64in_SW:
      vex_printf("sw      ");
      ppHRegRISCV64(i->RISCV64in.SW.src);
      vex_printf(", ");
      ppRISCV64AMode(i->RISCV64in.SW.amode);
      return;
   case RISCV64in_SH:
      vex_printf("sh      ");
      ppHRegRISCV64(i->RISCV64in.SH.src);
      vex_printf(", ");
      ppRISCV64AMode(i->RISCV64in.SH.amode);
      return;
   case RISCV64in_SB:
      vex_printf("sb      ");
      ppHRegRISCV64(i->RISCV64in.SB.src);
      vex_printf(", ");
      ppRISCV64AMode(i->RISCV64in.SB.amode);
      return;
   default:
      vpanic("ppRISCV64Instr");
   }
}

/*------------------------------------------------------------*/
/*--- Helpers for register allocation                      ---*/
/*------------------------------------------------------------*/

/* Initialise and return the "register universe", i.e. a list of all hardware
   registers. Called once. */
const RRegUniverse* getRRegUniverse_RISCV64(void)
{
   static RRegUniverse all_regs;
   static Bool         initialised = False;
   RRegUniverse*       ru          = &all_regs;

   if (LIKELY(initialised))
      return ru;

   RRegUniverse__init(ru);

   /* Add the registers that are available to the register allocator. */
   /* TODO */
   ru->allocable_start[HRcInt64] = ru->size;
   ru->regs[ru->size++]          = hregRISCV64_x18();
   ru->regs[ru->size++]          = hregRISCV64_x19();
   ru->regs[ru->size++]          = hregRISCV64_x20();
   ru->regs[ru->size++]          = hregRISCV64_x21();
   ru->regs[ru->size++]          = hregRISCV64_x22();
   ru->regs[ru->size++]          = hregRISCV64_x23();
   ru->regs[ru->size++]          = hregRISCV64_x24();
   ru->regs[ru->size++]          = hregRISCV64_x25();
   ru->regs[ru->size++]          = hregRISCV64_x26();
   ru->regs[ru->size++]          = hregRISCV64_x27();
   ru->allocable_end[HRcInt64]   = ru->size - 1;
   ru->allocable                 = ru->size;

   /* Add the registers that are not available for allocation. */
   /* TODO */
   ru->regs[ru->size++] = hregRISCV64_x8();

   initialised = True;

   RRegUniverse__check_is_sane(ru);
   return ru;
}

/* Tell the register allocator how the given instruction uses the registers it
   refers to. */
void getRegUsage_RISCV64Instr(HRegUsage* u, const RISCV64Instr* i, Bool mode64)
{
   vassert(mode64 == True);

   initHRegUsage(u);
   switch (i->tag) {
   case RISCV64in_LI:
      addHRegUse(u, HRmWrite, i->RISCV64in.LI.dst);
      return;
   case RISCV64in_LD:
      addRegUsage_RISCV64AMode(u, i->RISCV64in.LD.amode);
      addHRegUse(u, HRmWrite, i->RISCV64in.LD.dst);
      return;
   case RISCV64in_LW:
      addRegUsage_RISCV64AMode(u, i->RISCV64in.LW.amode);
      addHRegUse(u, HRmWrite, i->RISCV64in.LW.dst);
      return;
   case RISCV64in_LH:
      addRegUsage_RISCV64AMode(u, i->RISCV64in.LH.amode);
      addHRegUse(u, HRmWrite, i->RISCV64in.LH.dst);
      return;
   case RISCV64in_LB:
      addRegUsage_RISCV64AMode(u, i->RISCV64in.LB.amode);
      addHRegUse(u, HRmWrite, i->RISCV64in.LB.dst);
      return;
   case RISCV64in_SD:
      addRegUsage_RISCV64AMode(u, i->RISCV64in.SD.amode);
      addHRegUse(u, HRmRead, i->RISCV64in.SD.src);
      return;
   case RISCV64in_SW:
      addRegUsage_RISCV64AMode(u, i->RISCV64in.SW.amode);
      addHRegUse(u, HRmRead, i->RISCV64in.SW.src);
      return;
   case RISCV64in_SH:
      addRegUsage_RISCV64AMode(u, i->RISCV64in.SH.amode);
      addHRegUse(u, HRmRead, i->RISCV64in.SH.src);
      return;
   case RISCV64in_SB:
      addRegUsage_RISCV64AMode(u, i->RISCV64in.SB.amode);
      addHRegUse(u, HRmRead, i->RISCV64in.SB.src);
      return;
   default:
      ppRISCV64Instr(i, mode64);
      vpanic("getRegUsage_RISCV64Instr");
   }
}

/* Map the registers of the given instruction. */
void mapRegs_RISCV64Instr(HRegRemap* m, RISCV64Instr* i, Bool mode64)
{
   vassert(mode64 == True);

   switch (i->tag) {
   case RISCV64in_LI:
      i->RISCV64in.LI.dst = lookupHRegRemap(m, i->RISCV64in.LI.dst);
      return;
   case RISCV64in_LD:
      i->RISCV64in.LD.dst = lookupHRegRemap(m, i->RISCV64in.LD.dst);
      mapRegs_RISCV64AMode(m, i->RISCV64in.LD.amode);
      return;
   case RISCV64in_LW:
      i->RISCV64in.LW.dst = lookupHRegRemap(m, i->RISCV64in.LW.dst);
      mapRegs_RISCV64AMode(m, i->RISCV64in.LW.amode);
      return;
   case RISCV64in_LH:
      i->RISCV64in.LH.dst = lookupHRegRemap(m, i->RISCV64in.LH.dst);
      mapRegs_RISCV64AMode(m, i->RISCV64in.LH.amode);
      return;
   case RISCV64in_LB:
      i->RISCV64in.LB.dst = lookupHRegRemap(m, i->RISCV64in.LB.dst);
      mapRegs_RISCV64AMode(m, i->RISCV64in.LB.amode);
      return;
   case RISCV64in_SD:
      i->RISCV64in.SD.src = lookupHRegRemap(m, i->RISCV64in.SD.src);
      mapRegs_RISCV64AMode(m, i->RISCV64in.SD.amode);
      return;
   case RISCV64in_SW:
      i->RISCV64in.SW.src = lookupHRegRemap(m, i->RISCV64in.SW.src);
      mapRegs_RISCV64AMode(m, i->RISCV64in.SW.amode);
      return;
   case RISCV64in_SH:
      i->RISCV64in.SH.src = lookupHRegRemap(m, i->RISCV64in.SH.src);
      mapRegs_RISCV64AMode(m, i->RISCV64in.SH.amode);
      return;
   case RISCV64in_SB:
      i->RISCV64in.SB.src = lookupHRegRemap(m, i->RISCV64in.SB.src);
      mapRegs_RISCV64AMode(m, i->RISCV64in.SB.amode);
      return;
   default:
      ppRISCV64Instr(i, mode64);
      vpanic("mapRegs_RISCV64Instr");
   }
}

/* Generate riscv64 spill/reload instructions under the direction of the
   register allocator. Note it's critical these don't write the condition
   codes. */
void genSpill_RISCV64(/*OUT*/ HInstr** i1,
                      /*OUT*/ HInstr** i2,
                      HReg             rreg,
                      Int              offsetB,
                      Bool             mode64)
{
   vassert(offsetB >= 0);
   vassert(!hregIsVirtual(rreg));
   vassert(mode64 == True);

   HRegClass rclass = hregClass(rreg);
   switch (rclass) {
#if 0
   case HRcInt64:
      vassert(0 == (offsetB & 7));
      offsetB >>= 3;
      vassert(offsetB < 4096);
      *i1 = RISCV64Instr_LdSt64(
         False /*!isLoad*/,
         rreg,
         RISCV64AMode_RI12(hregRISCV64_X21(), offsetB, 8));
      return;
#endif
   default:
      ppHRegClass(rclass);
      vpanic("genSpill_RISCV64: unimplemented regclass");
   }
}

void genReload_RISCV64(/*OUT*/ HInstr** i1,
                       /*OUT*/ HInstr** i2,
                       HReg             rreg,
                       Int              offsetB,
                       Bool             mode64)
{
   vassert(offsetB >= 0);
   vassert(!hregIsVirtual(rreg));
   vassert(mode64 == True);

   HRegClass rclass = hregClass(rreg);
   switch (rclass) {
#if 0
   case HRcInt64:
      vassert(0 == (offsetB & 7));
      offsetB >>= 3;
      vassert(offsetB < 4096);
      *i1 = RISCV64Instr_LdSt64(
         True /*isLoad*/,
         rreg,
         RISCV64AMode_RI12(hregRISCV64_X21(), offsetB, 8));
      return;
#endif
   default:
      ppHRegClass(rclass);
      vpanic("genReload_RISCV64: unimplemented regclass");
   }
}

RISCV64Instr* genMove_RISCV64(HReg from, HReg to, Bool mode64)
{
   switch (hregClass(from)) {
#if 0
   case HRcInt64:
      return RISCV64Instr_MovI(to, from);
#endif
   default:
      ppHRegClass(hregClass(from));
      vpanic("genMove_RISCV64: unimplemented regclass");
   }
}

/*------------------------------------------------------------*/
/*--- Functions to emit a sequence of bytes                ---*/
/*------------------------------------------------------------*/

static inline UChar* emit16(UChar* p, UShort val)
{
   *p++ = (val >> 0) & 0xff;
   *p++ = (val >> 8) & 0xff;
   return p;
}

static inline UChar* emit32(UChar* p, UInt val)
{
   *p++ = (val >> 0) & 0xff;
   *p++ = (val >> 8) & 0xff;
   *p++ = (val >> 16) & 0xff;
   *p++ = (val >> 24) & 0xff;
   return p;
}

/*------------------------------------------------------------*/
/*--- Functions to emit various instruction formats        ---*/
/*------------------------------------------------------------*/

/* Emit an I-type instruction. */
static UChar*
emit_I(UChar* p, UInt opcode, UInt rd, UInt funct3, UInt rs1, UInt imm11_0)
{
   vassert(opcode >> 7 == 0);
   vassert(rd >> 5 == 0);
   vassert(funct3 >> 3 == 0);
   vassert(rs1 >> 5 == 0);
   vassert(imm11_0 >> 12 == 0);

   UInt the_insn = 0;

   the_insn |= opcode << 0;
   the_insn |= rd << 7;
   the_insn |= funct3 << 12;
   the_insn |= rs1 << 15;
   the_insn |= imm11_0 << 20;

   return emit32(p, the_insn);
}

/* Emit an S-type instruction. */
static UChar* emit_S(UChar* p,
                     UInt   opcode,
                     UInt   imm4_0,
                     UInt   funct3,
                     UInt   rs1,
                     UInt   rs2,
                     UInt   imm11_5)
{
   vassert(opcode >> 7 == 0);
   vassert(imm4_0 >> 5 == 0);
   vassert(funct3 >> 3 == 0);
   vassert(rs1 >> 5 == 0);
   vassert(rs2 >> 5 == 0);
   vassert(imm11_5 >> 7 == 0);

   UInt the_insn = 0;

   the_insn |= opcode << 0;
   the_insn |= imm4_0 << 7;
   the_insn |= funct3 << 12;
   the_insn |= rs1 << 15;
   the_insn |= rs2 << 20;
   the_insn |= imm11_5 << 25;

   return emit32(p, the_insn);
}

/* Emit a U-type instruction. */
static UChar* emit_U(UChar* p, UInt opcode, UInt rd, UInt imm31_12)
{
   vassert(opcode >> 7 == 0);
   vassert(rd >> 5 == 0);
   vassert(imm31_12 >> 20 == 0);

   UInt the_insn = 0;

   the_insn |= opcode << 0;
   the_insn |= rd << 7;
   the_insn |= imm31_12 << 12;

   return emit32(p, the_insn);
}

/* Emit a CI-type instruction. */
static UChar*
emit_CI(UChar* p, UInt opcode, UInt imm4_0, UInt rd, UInt imm5, UInt funct3)
{
   vassert(opcode >> 2 == 0);
   vassert(imm4_0 >> 5 == 0);
   vassert(rd >> 5 == 0);
   vassert(imm5 >> 1 == 0);
   vassert(funct3 >> 3 == 0);

   UShort the_insn = 0;

   the_insn |= opcode << 0;
   the_insn |= imm4_0 << 2;
   the_insn |= rd << 7;
   the_insn |= imm5 << 12;
   the_insn |= funct3 << 13;

   return emit16(p, the_insn);
}

/*------------------------------------------------------------*/
/*--- Code generation                                      ---*/
/*------------------------------------------------------------*/

/* Get an immediate into a register, using only that register. */
static UChar* imm64_to_ireg(UChar* p, UInt dst, ULong imm64)
{
   vassert(dst > 0 && dst <= 31);

   Long simm64 = imm64;

   if (simm64 >= -32 && simm64 <= 31) {
      /* c.li dst, simm64[5:0] */
      return emit_CI(p, 0b01, imm64 & 0x1f, dst, (imm64 >> 5) & 0x1, 0b010);
   }

   /* TODO Add implementation with addi only and c.lui+addi. */

   if (simm64 >= -2147483648 && simm64 <= 2147483647) {
      /* lui dst, simm64[31:12]+simm64[11] */
      p = emit_U(p, 0b0110111, dst, ((imm64 + 0x800) >> 12) & 0xfffff);
      if ((imm64 & 0xfff) == 0)
         return p;
      /* addi dst, dst, simm64[11:0] */
      return emit_I(p, 0b0010011, dst, 0b000, dst, imm64 & 0xfff);
   }

   /* TODO Implement support for >32-bit constants. */
   vpanic("imm64_to_ireg");
   return p;
}

/* Generate a load to dst, using the given amode for the address. */
static UChar* do_load(UChar* p, UInt dst, const RISCV64AMode* am, UInt szB)
{
   vassert(dst > 0 && dst <= 31);

   if (am->tag == RISCV64am_RI12) {
      UInt base = iregEnc(am->RISCV64am.RI12.reg);
      vassert(base > 0 && base <= 31);
      Int soff12 = am->RISCV64am.RI12.soff12;
      if (szB == 8) {
         /* ld dst, soff12(base) */
         return emit_I(p, 0b0000011, dst, 0b011, base, soff12);
      }
      if (szB == 4) {
         /* lw dst, soff12(base) */
         return emit_I(p, 0b0000011, dst, 0b010, base, soff12);
      }
      if (szB == 2) {
         /* lh dst, soff12(base) */
         return emit_I(p, 0b0000011, dst, 0b001, base, soff12);
      }
      if (szB == 1) {
         /* lb dst, soff12(base) */
         return emit_I(p, 0b0000011, dst, 0b000, base, soff12);
      }
   }
   vpanic("do_load");
}

/* Generate a store from src, using the given amode for the address. */
static UChar* do_store(UChar* p, UInt src, const RISCV64AMode* am, UInt szB)
{
   vassert(src > 0 && src <= 31);

   if (am->tag == RISCV64am_RI12) {
      UInt base = iregEnc(am->RISCV64am.RI12.reg);
      vassert(base > 0 && base <= 31);
      Int  soff12  = am->RISCV64am.RI12.soff12;
      UInt imm4_0  = soff12 & 0x1f;
      UInt imm11_5 = (soff12 >> 5) & 0x7f;
      if (szB == 8) {
         /* sd src, soff12(base) */
         return emit_S(p, 0b0100011, imm4_0, 0b011, base, src, imm11_5);
      }
      if (szB == 4) {
         /* sw src, soff12(base) */
         return emit_S(p, 0b0100011, imm4_0, 0b010, base, src, imm11_5);
      }
      if (szB == 2) {
         /* sh src, soff12(base) */
         return emit_S(p, 0b0100011, imm4_0, 0b001, base, src, imm11_5);
      }
      if (szB == 1) {
         /* sb src, soff12(base) */
         return emit_S(p, 0b0100011, imm4_0, 0b000, base, src, imm11_5);
      }
   }
   vpanic("do_store");
}

/* Emit an instruction into buf and return the number of bytes used. Note that
   buf is not the insn's final place, and therefore it is imperative to emit
   position-independent code. If the emitted instruction was a profiler inc, set
   *is_profInc to True, else leave it unchanged. */
Int emit_RISCV64Instr(/*MB_MOD*/ Bool*    is_profInc,
                      UChar*              buf,
                      Int                 nbuf,
                      const RISCV64Instr* i,
                      Bool                mode64,
                      VexEndness          endness_host,
                      const void*         disp_cp_chain_me_to_slowEP,
                      const void*         disp_cp_chain_me_to_fastEP,
                      const void*         disp_cp_xindir,
                      const void*         disp_cp_xassisted)
{
   vassert(nbuf >= 32);
   vassert(mode64 == True);
   vassert(((HWord)buf & 1) == 0);

   UChar* p = &buf[0];

   switch (i->tag) {
   case RISCV64in_LI:
      p = imm64_to_ireg(p, iregEnc(i->RISCV64in.LI.dst), i->RISCV64in.LI.imm64);
      goto done;
   case RISCV64in_LD:
      p = do_load(p, iregEnc(i->RISCV64in.LD.dst), i->RISCV64in.LD.amode, 8);
      goto done;
   case RISCV64in_LW:
      p = do_load(p, iregEnc(i->RISCV64in.LW.dst), i->RISCV64in.LW.amode, 4);
      goto done;
   case RISCV64in_LH:
      p = do_load(p, iregEnc(i->RISCV64in.LH.dst), i->RISCV64in.LH.amode, 2);
      goto done;
   case RISCV64in_LB:
      p = do_load(p, iregEnc(i->RISCV64in.LB.dst), i->RISCV64in.LB.amode, 1);
      goto done;
   case RISCV64in_SD:
      p = do_store(p, iregEnc(i->RISCV64in.SD.src), i->RISCV64in.SD.amode, 8);
      goto done;
   case RISCV64in_SW:
      p = do_store(p, iregEnc(i->RISCV64in.SW.src), i->RISCV64in.SW.amode, 4);
      goto done;
   case RISCV64in_SH:
      p = do_store(p, iregEnc(i->RISCV64in.SH.src), i->RISCV64in.SH.amode, 2);
      goto done;
   case RISCV64in_SB:
      p = do_store(p, iregEnc(i->RISCV64in.SB.src), i->RISCV64in.SB.amode, 1);
      goto done;
   default:
      goto bad;
   }

bad:
   ppRISCV64Instr(i, mode64);
   vpanic("emit_RISCV64Instr");
   /*NOTREACHED*/

done:
   vassert(p - &buf[0] <= 32);
   return p - &buf[0];
}

/* Return the number of bytes emitted for an RISCV64_INSN_EVCHECK. See
   riscv64_insn_evcheck_emit(). */
Int evCheckSzB_RISCV64(void) { return 8; }

/* NB: what goes on here has to be very closely coordinated with the emitInstr
   case for XDirect, above. */
VexInvalRange chainXDirect_RISCV64(VexEndness  endness_host,
                                   void*       place_to_chain,
                                   const void* disp_cp_chain_me_EXPECTED,
                                   const void* place_to_jump_to)
{
   vassert(endness_host == VexEndnessLE);

   vpanic("chainXDirect_RISCV64");

   VexInvalRange vir = {(HWord)place_to_chain, 0};
   return vir;
}

/* NB: what goes on here has to be very closely coordinated with the emitInstr
   case for XDirect, above. */
VexInvalRange unchainXDirect_RISCV64(VexEndness  endness_host,
                                     void*       place_to_unchain,
                                     const void* place_to_jump_to_EXPECTED,
                                     const void* disp_cp_chain_me)
{
   vassert(endness_host == VexEndnessLE);

   vpanic("unchainXDirect_RISCV64");

   VexInvalRange vir = {(HWord)place_to_unchain, 0};
   return vir;
}

/* Patch the counter address into a profile inc point, as previously created by
   the Ain_ProfInc case for emit_RISCV64Instr(). */
VexInvalRange patchProfInc_RISCV64(VexEndness   endness_host,
                                   void*        place_to_patch,
                                   const ULong* location_of_counter)
{
   vassert(endness_host == VexEndnessLE);

   vpanic("patchProfInc_RISCV64");

   VexInvalRange vir = {(HWord)place_to_patch, 0};
   return vir;
}

/*--------------------------------------------------------------------*/
/*--- end                                      host_riscv64_defs.c ---*/
/*--------------------------------------------------------------------*/
