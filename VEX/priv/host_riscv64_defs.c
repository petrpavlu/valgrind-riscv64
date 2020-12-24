
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

RISCV64Instr* RISCV64Instr_LD(HReg dst, HReg base, Int soff12)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_LD;
   i->RISCV64in.LD.dst    = dst;
   i->RISCV64in.LD.base   = base;
   i->RISCV64in.LD.soff12 = soff12;
   return i;
}

RISCV64Instr* RISCV64Instr_LW(HReg dst, HReg base, Int soff12)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_LW;
   i->RISCV64in.LW.dst    = dst;
   i->RISCV64in.LW.base   = base;
   i->RISCV64in.LW.soff12 = soff12;
   return i;
}

RISCV64Instr* RISCV64Instr_LH(HReg dst, HReg base, Int soff12)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_LH;
   i->RISCV64in.LH.dst    = dst;
   i->RISCV64in.LH.base   = base;
   i->RISCV64in.LH.soff12 = soff12;
   return i;
}

RISCV64Instr* RISCV64Instr_LB(HReg dst, HReg base, Int soff12)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_LB;
   i->RISCV64in.LB.dst    = dst;
   i->RISCV64in.LB.base   = base;
   i->RISCV64in.LB.soff12 = soff12;
   return i;
}

RISCV64Instr* RISCV64Instr_SD(HReg src, HReg base, Int soff12)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_SD;
   i->RISCV64in.SD.src    = src;
   i->RISCV64in.SD.base   = base;
   i->RISCV64in.SD.soff12 = soff12;
   return i;
}

RISCV64Instr* RISCV64Instr_SW(HReg src, HReg base, Int soff12)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_SW;
   i->RISCV64in.SW.src    = src;
   i->RISCV64in.SW.base   = base;
   i->RISCV64in.SW.soff12 = soff12;
   return i;
}

RISCV64Instr* RISCV64Instr_SH(HReg src, HReg base, Int soff12)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_SH;
   i->RISCV64in.SH.src    = src;
   i->RISCV64in.SH.base   = base;
   i->RISCV64in.SH.soff12 = soff12;
   return i;
}

RISCV64Instr* RISCV64Instr_SB(HReg src, HReg base, Int soff12)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_SB;
   i->RISCV64in.SB.src    = src;
   i->RISCV64in.SB.base   = base;
   i->RISCV64in.SB.soff12 = soff12;
   return i;
}

RISCV64Instr* RISCV64Instr_XDirect(
   Addr64 dstGA, HReg base, Int soff12, HReg cond, Bool toFastEP)
{
   RISCV64Instr* i               = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                        = RISCV64in_XDirect;
   i->RISCV64in.XDirect.dstGA    = dstGA;
   i->RISCV64in.XDirect.base     = base;
   i->RISCV64in.XDirect.soff12   = soff12;
   i->RISCV64in.XDirect.cond     = cond;
   i->RISCV64in.XDirect.toFastEP = toFastEP;
   return i;
}

RISCV64Instr* RISCV64Instr_XIndir(HReg dstGA, HReg base, Int soff12, HReg cond)
{
   RISCV64Instr* i            = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                     = RISCV64in_XIndir;
   i->RISCV64in.XIndir.dstGA  = dstGA;
   i->RISCV64in.XIndir.base   = base;
   i->RISCV64in.XIndir.soff12 = soff12;
   i->RISCV64in.XIndir.cond   = cond;
   return i;
}

RISCV64Instr* RISCV64Instr_XAssisted(
   HReg dstGA, HReg base, Int soff12, HReg cond, IRJumpKind jk)
{
   RISCV64Instr* i               = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                        = RISCV64in_XAssisted;
   i->RISCV64in.XAssisted.dstGA  = dstGA;
   i->RISCV64in.XAssisted.base   = base;
   i->RISCV64in.XAssisted.soff12 = soff12;
   i->RISCV64in.XAssisted.cond   = cond;
   i->RISCV64in.XAssisted.jk     = jk;
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
      vex_printf(", %d(", i->RISCV64in.LD.soff12);
      ppHRegRISCV64(i->RISCV64in.LD.base);
      vex_printf(")");
      return;
   case RISCV64in_LW:
      vex_printf("lw      ");
      ppHRegRISCV64(i->RISCV64in.LW.dst);
      vex_printf(", %d(", i->RISCV64in.LW.soff12);
      ppHRegRISCV64(i->RISCV64in.LW.base);
      vex_printf(")");
      return;
   case RISCV64in_LH:
      vex_printf("lh      ");
      ppHRegRISCV64(i->RISCV64in.LH.dst);
      vex_printf(", %d(", i->RISCV64in.LH.soff12);
      ppHRegRISCV64(i->RISCV64in.LH.base);
      vex_printf(")");
      return;
   case RISCV64in_LB:
      vex_printf("lb      ");
      ppHRegRISCV64(i->RISCV64in.LB.dst);
      vex_printf(", %d(", i->RISCV64in.LB.soff12);
      ppHRegRISCV64(i->RISCV64in.LB.base);
      vex_printf(")");
      return;
   case RISCV64in_SD:
      vex_printf("sd      ");
      ppHRegRISCV64(i->RISCV64in.SD.src);
      vex_printf(", %d(", i->RISCV64in.SD.soff12);
      ppHRegRISCV64(i->RISCV64in.SD.base);
      vex_printf(")");
      return;
   case RISCV64in_SW:
      vex_printf("sw      ");
      ppHRegRISCV64(i->RISCV64in.SW.src);
      vex_printf(", %d(", i->RISCV64in.SW.soff12);
      ppHRegRISCV64(i->RISCV64in.SW.base);
      vex_printf(")");
      return;
   case RISCV64in_SH:
      vex_printf("sh      ");
      ppHRegRISCV64(i->RISCV64in.SH.src);
      vex_printf(", %d(", i->RISCV64in.SH.soff12);
      ppHRegRISCV64(i->RISCV64in.SH.base);
      vex_printf(")");
      return;
   case RISCV64in_SB:
      vex_printf("sb      ");
      ppHRegRISCV64(i->RISCV64in.SB.src);
      vex_printf(", %d(", i->RISCV64in.SB.soff12);
      ppHRegRISCV64(i->RISCV64in.SB.base);
      vex_printf(")");
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
      addHRegUse(u, HRmWrite, i->RISCV64in.LD.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.LD.base);
      return;
   case RISCV64in_LW:
      addHRegUse(u, HRmWrite, i->RISCV64in.LW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.LW.base);
      return;
   case RISCV64in_LH:
      addHRegUse(u, HRmWrite, i->RISCV64in.LH.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.LH.base);
      return;
   case RISCV64in_LB:
      addHRegUse(u, HRmWrite, i->RISCV64in.LB.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.LB.base);
      return;
   case RISCV64in_SD:
      addHRegUse(u, HRmRead, i->RISCV64in.SD.src);
      addHRegUse(u, HRmRead, i->RISCV64in.SD.base);
      return;
   case RISCV64in_SW:
      addHRegUse(u, HRmRead, i->RISCV64in.SW.src);
      addHRegUse(u, HRmRead, i->RISCV64in.SW.base);
      return;
   case RISCV64in_SH:
      addHRegUse(u, HRmRead, i->RISCV64in.SH.src);
      addHRegUse(u, HRmRead, i->RISCV64in.SH.base);
      return;
   case RISCV64in_SB:
      addHRegUse(u, HRmRead, i->RISCV64in.SB.src);
      addHRegUse(u, HRmRead, i->RISCV64in.SB.base);
      return;
   default:
      ppRISCV64Instr(i, mode64);
      vpanic("getRegUsage_RISCV64Instr");
   }
}

/* Local helper. */
static void mapReg(HRegRemap* m, HReg* r) { *r = lookupHRegRemap(m, *r); }

/* Map the registers of the given instruction. */
void mapRegs_RISCV64Instr(HRegRemap* m, RISCV64Instr* i, Bool mode64)
{
   vassert(mode64 == True);

   switch (i->tag) {
   case RISCV64in_LI:
      i->RISCV64in.LI.dst = lookupHRegRemap(m, i->RISCV64in.LI.dst);
      return;
   case RISCV64in_LD:
      mapReg(m, &i->RISCV64in.LD.dst);
      mapReg(m, &i->RISCV64in.LD.base);
      return;
   case RISCV64in_LW:
      mapReg(m, &i->RISCV64in.LW.dst);
      mapReg(m, &i->RISCV64in.LW.base);
      return;
   case RISCV64in_LH:
      mapReg(m, &i->RISCV64in.LH.dst);
      mapReg(m, &i->RISCV64in.LH.base);
      return;
   case RISCV64in_LB:
      mapReg(m, &i->RISCV64in.LB.dst);
      mapReg(m, &i->RISCV64in.LB.base);
      return;
   case RISCV64in_SD:
      mapReg(m, &i->RISCV64in.SD.src);
      mapReg(m, &i->RISCV64in.SD.base);
      return;
   case RISCV64in_SW:
      mapReg(m, &i->RISCV64in.SW.src);
      mapReg(m, &i->RISCV64in.SW.base);
      return;
   case RISCV64in_SH:
      mapReg(m, &i->RISCV64in.SH.src);
      mapReg(m, &i->RISCV64in.SH.base);
      return;
   case RISCV64in_SB:
      mapReg(m, &i->RISCV64in.SB.src);
      mapReg(m, &i->RISCV64in.SB.base);
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
static UChar*
emit_S(UChar* p, UInt opcode, UInt imm11_0, UInt funct3, UInt rs1, UInt rs2)
{
   vassert(opcode >> 7 == 0);
   vassert(imm11_0 >> 12 == 0);
   vassert(funct3 >> 3 == 0);
   vassert(rs1 >> 5 == 0);
   vassert(rs2 >> 5 == 0);

   UInt imm4_0  = (imm11_0 >> 0) & 0x1f;
   UInt imm11_5 = (imm11_0 >> 5) & 0x7f;

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
static UChar* emit_CI(UChar* p, UInt opcode, UInt imm5_0, UInt rd, UInt funct3)
{
   vassert(opcode >> 2 == 0);
   vassert(imm5_0 >> 6 == 0);
   vassert(rd >> 5 == 0);
   vassert(funct3 >> 3 == 0);

   UInt imm4_0 = (imm5_0 >> 0) & 0x1f;
   UInt imm5   = (imm5_0 >> 5) & 0x1;

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
      return emit_CI(p, 0b01, imm64 & 0x3f, dst, 0b010);
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
   case RISCV64in_LD: {
      /* ld dst, soff12(base) */
      UInt dst    = iregEnc(i->RISCV64in.LD.dst);
      UInt base   = iregEnc(i->RISCV64in.LD.base);
      Int  soff12 = i->RISCV64in.LD.soff12;
      vassert(soff12 >= -2048 && soff12 < 2048);
      UInt imm11_0 = soff12 & 0xfff;

      p = emit_I(p, 0b0000011, dst, 0b011, base, imm11_0);
      goto done;
   }
   case RISCV64in_LW: {
      /* lw dst, soff12(base) */
      UInt dst    = iregEnc(i->RISCV64in.LW.dst);
      UInt base   = iregEnc(i->RISCV64in.LW.base);
      Int  soff12 = i->RISCV64in.LW.soff12;
      vassert(soff12 >= -2048 && soff12 < 2048);
      UInt imm11_0 = soff12 & 0xfff;

      p = emit_I(p, 0b0000011, dst, 0b010, base, imm11_0);
      goto done;
   }
   case RISCV64in_LH: {
      /* lh dst, soff12(base) */
      UInt dst    = iregEnc(i->RISCV64in.LH.dst);
      UInt base   = iregEnc(i->RISCV64in.LH.base);
      Int  soff12 = i->RISCV64in.LH.soff12;
      vassert(soff12 >= -2048 && soff12 < 2048);
      UInt imm11_0 = soff12 & 0xfff;

      p = emit_I(p, 0b0000011, dst, 0b001, base, imm11_0);
      goto done;
   }
   case RISCV64in_LB: {
      /* lb dst, soff12(base) */
      UInt dst    = iregEnc(i->RISCV64in.LB.dst);
      UInt base   = iregEnc(i->RISCV64in.LB.base);
      Int  soff12 = i->RISCV64in.LB.soff12;
      vassert(soff12 >= -2048 && soff12 < 2048);
      UInt imm11_0 = soff12 & 0xfff;

      p = emit_I(p, 0b0000011, dst, 0b000, base, imm11_0);
      goto done;
   }
   case RISCV64in_SD: {
      /* sd src, soff12(base) */
      UInt src    = iregEnc(i->RISCV64in.SD.src);
      UInt base   = iregEnc(i->RISCV64in.SD.base);
      Int  soff12 = i->RISCV64in.SD.soff12;
      vassert(soff12 >= -2048 && soff12 < 2048);
      UInt imm11_0 = soff12 & 0xfff;

      p = emit_S(p, 0b0100011, imm11_0, 0b011, base, src);
      goto done;
   }
   case RISCV64in_SW: {
      /* sw src, soff12(base) */
      UInt src    = iregEnc(i->RISCV64in.SW.src);
      UInt base   = iregEnc(i->RISCV64in.SW.base);
      Int  soff12 = i->RISCV64in.SW.soff12;
      vassert(soff12 >= -2048 && soff12 < 2048);
      UInt imm11_0 = soff12 & 0xfff;

      p = emit_S(p, 0b0100011, imm11_0, 0b010, base, src);
      goto done;
   }
   case RISCV64in_SH: {
      /* sh src, soff12(base) */
      UInt src    = iregEnc(i->RISCV64in.SH.src);
      UInt base   = iregEnc(i->RISCV64in.SH.base);
      Int  soff12 = i->RISCV64in.SH.soff12;
      vassert(soff12 >= -2048 && soff12 < 2048);
      UInt imm11_0 = soff12 & 0xfff;

      p = emit_S(p, 0b0100011, imm11_0, 0b001, base, src);
      goto done;
   }
   case RISCV64in_SB: {
      /* sb src, soff12(base) */
      UInt src    = iregEnc(i->RISCV64in.SB.src);
      UInt base   = iregEnc(i->RISCV64in.SB.base);
      Int  soff12 = i->RISCV64in.SB.soff12;
      vassert(soff12 >= -2048 && soff12 < 2048);
      UInt imm11_0 = soff12 & 0xfff;

      p = emit_S(p, 0b0100011, imm11_0, 0b000, base, src);
      goto done;
   }
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
