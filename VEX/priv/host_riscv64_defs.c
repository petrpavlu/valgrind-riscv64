
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

#include "libvex_trc_values.h"

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
      vassert(r < 32);
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
   vassert(n < 32);
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

RISCV64Instr* RISCV64Instr_MV(HReg dst, HReg src)
{
   RISCV64Instr* i     = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag              = RISCV64in_MV;
   i->RISCV64in.MV.dst = dst;
   i->RISCV64in.MV.src = src;
   return i;
}

RISCV64Instr* RISCV64Instr_ADD(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_ADD;
   i->RISCV64in.ADD.dst  = dst;
   i->RISCV64in.ADD.src1 = src1;
   i->RISCV64in.ADD.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_SUB(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_SUB;
   i->RISCV64in.SUB.dst  = dst;
   i->RISCV64in.SUB.src1 = src1;
   i->RISCV64in.SUB.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_SLTU(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_SLTU;
   i->RISCV64in.SLTU.dst  = dst;
   i->RISCV64in.SLTU.src1 = src1;
   i->RISCV64in.SLTU.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_SLTIU(HReg dst, HReg src, Int simm12)
{
   RISCV64Instr* i           = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                    = RISCV64in_SLTIU;
   i->RISCV64in.SLTIU.dst    = dst;
   i->RISCV64in.SLTIU.src    = src;
   i->RISCV64in.SLTIU.simm12 = simm12;
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
   case RISCV64in_MV:
      vex_printf("mv      ");
      ppHRegRISCV64(i->RISCV64in.MV.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.MV.src);
      return;
   case RISCV64in_ADD:
      vex_printf("add     ");
      ppHRegRISCV64(i->RISCV64in.ADD.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.ADD.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.ADD.src2);
      return;
   case RISCV64in_SUB:
      vex_printf("sub     ");
      ppHRegRISCV64(i->RISCV64in.SUB.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SUB.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SUB.src2);
      return;
   case RISCV64in_SLTU:
      vex_printf("sltu    ");
      ppHRegRISCV64(i->RISCV64in.SLTU.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SLTU.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SLTU.src2);
      return;
   case RISCV64in_SLTIU:
      vex_printf("sltiu   ");
      ppHRegRISCV64(i->RISCV64in.SLTIU.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SLTIU.src);
      vex_printf(", %d", i->RISCV64in.SLTIU.simm12);
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
   case RISCV64in_XDirect:
      vex_printf("(xDirect) ");
      if (!hregIsInvalid(i->RISCV64in.XDirect.cond)) {
         vex_printf("beq ");
         ppHRegRISCV64(i->RISCV64in.XDirect.cond);
         vex_printf(", zero, 1f; ");
      }
      vex_printf("li t0, 0x%llx; ", i->RISCV64in.XDirect.dstGA);
      vex_printf("sd t0, %d(", i->RISCV64in.XDirect.soff12);
      ppHRegRISCV64(i->RISCV64in.XDirect.base);
      vex_printf("); li t0, <%s>; ", i->RISCV64in.XDirect.toFastEP
                                        ? "disp_cp_chain_me_to_fastEP"
                                        : "disp_cp_chain_me_to_slowEP");
      vex_printf("jalr x1, 0(t0); 1:");
      return;
   case RISCV64in_XIndir:
      vex_printf("(xIndir) ");
      if (!hregIsInvalid(i->RISCV64in.XIndir.cond)) {
         vex_printf("beq ");
         ppHRegRISCV64(i->RISCV64in.XIndir.cond);
         vex_printf(", zero, 1f; ");
      }
      vex_printf("sd ");
      ppHRegRISCV64(i->RISCV64in.XIndir.dstGA);
      vex_printf(", %d(", i->RISCV64in.XIndir.soff12);
      ppHRegRISCV64(i->RISCV64in.XIndir.base);
      vex_printf("); li t0, <disp_cp_xindir>; ");
      vex_printf("jr 0(t0); 1:");
      return;
   case RISCV64in_XAssisted:
      vex_printf("(xAssisted) ");
      if (!hregIsInvalid(i->RISCV64in.XAssisted.cond)) {
         vex_printf("beq ");
         ppHRegRISCV64(i->RISCV64in.XAssisted.cond);
         vex_printf(", zero, 1f; ");
      }
      vex_printf("sd ");
      ppHRegRISCV64(i->RISCV64in.XAssisted.dstGA);
      vex_printf(", %d(", i->RISCV64in.XAssisted.soff12);
      ppHRegRISCV64(i->RISCV64in.XAssisted.base);
      vex_printf("); mv s0, $IRJumpKind_to_TRCVAL(%d)",
                 (Int)i->RISCV64in.XAssisted.jk);
      vex_printf("; li t0, <disp_cp_xassisted>; ");
      vex_printf("jr t0(0); 1:");
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
   ru->regs[ru->size++] = hregRISCV64_x0();
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
   case RISCV64in_MV:
      addHRegUse(u, HRmWrite, i->RISCV64in.MV.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.MV.src);
      return;
   case RISCV64in_ADD:
      addHRegUse(u, HRmWrite, i->RISCV64in.ADD.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.ADD.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.ADD.src2);
      return;
   case RISCV64in_SUB:
      addHRegUse(u, HRmWrite, i->RISCV64in.SUB.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SUB.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.SUB.src2);
      return;
   case RISCV64in_SLTU:
      addHRegUse(u, HRmWrite, i->RISCV64in.SLTU.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SLTU.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.SLTU.src2);
      return;
   case RISCV64in_SLTIU:
      addHRegUse(u, HRmWrite, i->RISCV64in.SLTIU.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SLTIU.src);
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
   /* XDirect/XIndir/XAssisted are also a bit subtle. They conditionally exit
      the block. Hence we only need to list (1) the registers that they read,
      and (2) the registers that they write in the case where the block is not
      exited. (2) is empty, hence only (1) is relevant here. */
   case RISCV64in_XDirect:
      addHRegUse(u, HRmRead, i->RISCV64in.XDirect.base);
      if (!hregIsInvalid(i->RISCV64in.XDirect.cond))
         addHRegUse(u, HRmRead, i->RISCV64in.XDirect.cond);
      return;
   case RISCV64in_XIndir:
      addHRegUse(u, HRmRead, i->RISCV64in.XIndir.dstGA);
      addHRegUse(u, HRmRead, i->RISCV64in.XIndir.base);
      if (!hregIsInvalid(i->RISCV64in.XIndir.cond))
         addHRegUse(u, HRmRead, i->RISCV64in.XIndir.cond);
      return;
   case RISCV64in_XAssisted:
      addHRegUse(u, HRmRead, i->RISCV64in.XAssisted.dstGA);
      addHRegUse(u, HRmRead, i->RISCV64in.XAssisted.base);
      if (!hregIsInvalid(i->RISCV64in.XAssisted.cond))
         addHRegUse(u, HRmRead, i->RISCV64in.XAssisted.cond);
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
      mapReg(m, &i->RISCV64in.LI.dst);
      return;
   case RISCV64in_MV:
      mapReg(m, &i->RISCV64in.MV.dst);
      mapReg(m, &i->RISCV64in.MV.src);
      return;
   case RISCV64in_ADD:
      mapReg(m, &i->RISCV64in.ADD.dst);
      mapReg(m, &i->RISCV64in.ADD.src1);
      mapReg(m, &i->RISCV64in.ADD.src2);
      return;
   case RISCV64in_SUB:
      mapReg(m, &i->RISCV64in.SUB.dst);
      mapReg(m, &i->RISCV64in.SUB.src1);
      mapReg(m, &i->RISCV64in.SUB.src2);
      return;
   case RISCV64in_SLTU:
      mapReg(m, &i->RISCV64in.SLTU.dst);
      mapReg(m, &i->RISCV64in.SLTU.src1);
      mapReg(m, &i->RISCV64in.SLTU.src2);
      return;
   case RISCV64in_SLTIU:
      mapReg(m, &i->RISCV64in.SLTIU.dst);
      mapReg(m, &i->RISCV64in.SLTIU.src);
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
   case RISCV64in_XDirect:
      mapReg(m, &i->RISCV64in.XDirect.base);
      if (!hregIsInvalid(i->RISCV64in.XDirect.cond))
         mapReg(m, &i->RISCV64in.XDirect.cond);
      return;
   case RISCV64in_XIndir:
      mapReg(m, &i->RISCV64in.XIndir.dstGA);
      mapReg(m, &i->RISCV64in.XIndir.base);
      if (!hregIsInvalid(i->RISCV64in.XIndir.cond))
         mapReg(m, &i->RISCV64in.XIndir.cond);
      return;
   case RISCV64in_XAssisted:
      mapReg(m, &i->RISCV64in.XAssisted.dstGA);
      mapReg(m, &i->RISCV64in.XAssisted.base);
      if (!hregIsInvalid(i->RISCV64in.XAssisted.cond))
         mapReg(m, &i->RISCV64in.XAssisted.cond);
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

/* Emit an R-type instruction. */
static UChar* emit_R(
   UChar* p, UInt opcode, UInt rd, UInt funct3, UInt rs1, UInt rs2, UInt funct7)
{
   vassert(opcode >> 7 == 0);
   vassert(rd >> 5 == 0);
   vassert(funct3 >> 3 == 0);
   vassert(rs1 >> 5 == 0);
   vassert(rs2 >> 5 == 0);
   vassert(funct7 >> 7 == 0);

   UInt the_insn = 0;

   the_insn |= opcode << 0;
   the_insn |= rd << 7;
   the_insn |= funct3 << 12;
   the_insn |= rs1 << 15;
   the_insn |= rs2 << 20;
   the_insn |= funct7 << 25;

   return emit32(p, the_insn);
}

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

/* Emit a B-type instruction. */
static UChar*
emit_B(UChar* p, UInt opcode, UInt imm12_1, UInt funct3, UInt rs1, UInt rs2)
{
   vassert(opcode >> 7 == 0);
   vassert(imm12_1 >> 12 == 0);
   vassert(funct3 >> 3 == 0);
   vassert(rs1 >> 5 == 0);
   vassert(rs2 >> 5 == 0);

   UInt imm11   = (imm12_1 >> 10) & 0x1;
   UInt imm4_1  = (imm12_1 >> 0) & 0xf;
   UInt imm10_5 = (imm12_1 >> 4) & 0x3f;
   UInt imm12   = (imm12_1 >> 11) & 0x1;

   UInt the_insn = 0;

   the_insn |= opcode << 0;
   the_insn |= imm11 << 7;
   the_insn |= imm4_1 << 8;
   the_insn |= funct3 << 12;
   the_insn |= rs1 << 15;
   the_insn |= rs2 << 20;
   the_insn |= imm10_5 << 25;
   the_insn |= imm12 << 31;

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

/* Emit a CR-type instruction. */
static UChar* emit_CR(UChar* p, UInt opcode, UInt rs2, UInt rd, UInt funct4)
{
   vassert(opcode >> 2 == 0);
   vassert(rs2 >> 5 == 0);
   vassert(rd >> 5 == 0);
   vassert(funct4 >> 4 == 0);

   UShort the_insn = 0;

   the_insn |= opcode << 0;
   the_insn |= rs2 << 2;
   the_insn |= rd << 7;
   the_insn |= funct4 << 12;

   return emit16(p, the_insn);
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
      /* addiw dst, dst, simm64[11:0] */
      return emit_I(p, 0b0011011, dst, 0b000, dst, imm64 & 0xfff);
   }

   /* TODO Implement support for >32-bit constants. */
   vpanic("imm64_to_ireg");
   return p;
}

/* Get a 48-bit address into a register, using only that register, and
   generating a constant number of instructions with 18 bytes in size,
   regardless of the value of the address. This is used when generating
   sections of code that need to be patched later, so as to guarantee a
   specific size.

   Notice that this function is designed to support target systems that use the
   Sv39 or Sv48 virtual-memory system. The input address is checked to be in
   the Sv48 format, that is bits [63:48] must be all equal to bit 47.
   Utilizing the fact that the address is only 48-bits in size allows to save 2
   instructions compared to materializing a full 64-bit address.

   TODO Review if generating instead 'c.ld dst, 1f; c.j 2f; .align 3;
   1: .quad imm; 2:' is possible and would be better.
   */
static UChar* addr48_to_ireg_EXACTLY_18B(UChar* p, UInt dst, ULong imm48)
{
   vassert(imm48 >> 47 == 0 || imm48 >> 47 == 0x1ffff);

   ULong rem = imm48;
   ULong imm47_28, imm27_16, imm15_4, imm3_0;
   imm3_0   = rem & 0xf;
   rem      = ((rem >> 3) + 1) >> 1;
   imm15_4  = rem & 0xfff;
   rem      = ((rem >> 11) + 1) >> 1;
   imm27_16 = rem & 0xfff;
   rem      = ((rem >> 11) + 1) >> 1;
   imm47_28 = rem & 0xfffff;

   /* lui dst, imm47_28 */
   p = emit_U(p, 0b0110111, dst, imm47_28);
   /* addiw dst, dst, imm27_16 */
   p = emit_I(p, 0b0011011, dst, 0b000, dst, imm27_16);
   /* c.slli dst, 12 */
   p = emit_CI(p, 0b10, 12, dst, 0b000);
   /* addi dst, dst, imm15_4 */
   p = emit_I(p, 0b0010011, dst, 0b000, dst, imm15_4);
   /* c.slli dst, 4 */
   p = emit_CI(p, 0b10, 4, dst, 0b000);
   if (imm3_0 != 0) {
      /* c.addi dst, imm3_0 */
      p = emit_CI(p, 0b01, imm3_0 | (imm3_0 >> 3 != 0 ? 0x30 : 0), dst, 0b000);
   } else {
      /* c.nop */
      p = emit_CI(p, 0b01, 0, 0, 0b000);
   }

   return p;
}

/* Check whether p points at an instruction sequence cooked up by
   addr48_to_ireg_EXACTLY_18B(). */
static Bool is_addr48_to_ireg_EXACTLY_18B(UChar* p, UInt dst, ULong imm48)
{
   UChar  tmp[18];
   UChar* q;

   q = addr48_to_ireg_EXACTLY_18B(&tmp[0], dst, imm48);
   if (q - &tmp[0] != 18)
      return False;

   q = &tmp[0];
   for (UInt i = 0; i < 18; i++) {
      if (*p != *q)
         return False;
      p++;
      q++;
   }
   return True;
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
   case RISCV64in_MV: {
      /* c.mv dst, src */
      UInt dst = iregEnc(i->RISCV64in.MV.dst);
      UInt src = iregEnc(i->RISCV64in.MV.src);

      p = emit_CR(p, 0b10, src, dst, 0b1000);
      goto done;
   }
   case RISCV64in_ADD: {
      /* add dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.ADD.dst);
      UInt src1 = iregEnc(i->RISCV64in.ADD.src1);
      UInt src2 = iregEnc(i->RISCV64in.ADD.src2);

      p = emit_R(p, 0b0110011, dst, 0b000, src1, src2, 0b0000000);
      goto done;
   }
   case RISCV64in_SUB: {
      /* sub dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.SUB.dst);
      UInt src1 = iregEnc(i->RISCV64in.SUB.src1);
      UInt src2 = iregEnc(i->RISCV64in.SUB.src2);

      p = emit_R(p, 0b0110011, dst, 0b000, src1, src2, 0b0100000);
      goto done;
   }
   case RISCV64in_SLTU: {
      /* sltu dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.SLTU.dst);
      UInt src1 = iregEnc(i->RISCV64in.SLTU.src1);
      UInt src2 = iregEnc(i->RISCV64in.SLTU.src2);

      p = emit_R(p, 0b0110011, dst, 0b011, src1, src2, 0b0000000);
      goto done;
   }
   case RISCV64in_SLTIU: {
      /* sltiu dst, src, simm12 */
      UInt dst    = iregEnc(i->RISCV64in.SLTIU.dst);
      UInt src    = iregEnc(i->RISCV64in.SLTIU.src);
      Int  simm12 = i->RISCV64in.SLTIU.simm12;
      vassert(simm12 >= -2048 && simm12 < 2048);
      UInt imm11_0 = simm12 & 0xfff;

      p = emit_I(p, 0b0010011, dst, 0b011, src, imm11_0);
      goto done;
   }
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

   case RISCV64in_XDirect: {
      /* NB: what goes on here has to be very closely coordinated with the
         chainXDirect_RISCV64() and unchainXDirect_RISCV64() below. */
      /* We're generating chain-me requests here, so we need to be sure this is
         actually allowed -- no-redir translations can't use chain-me's.
         Hence: */
      vassert(disp_cp_chain_me_to_slowEP != NULL);
      vassert(disp_cp_chain_me_to_fastEP != NULL);

      /* First off, if this is conditional, create a conditional jump over the
         rest of it. Or at least, leave a space for it that we will shortly fill
         in. */
      UChar* ptmp = NULL;
      if (!hregIsInvalid(i->RISCV64in.XDirect.cond)) {
         ptmp = p;
         p += 4;
      }

      /* Update the guest pc. */
      {
         /* li t0, dstGA */
         p = imm64_to_ireg(p, 5 /*x5/t0*/, i->RISCV64in.XDirect.dstGA);

         /* sd t0, soff12(base) */
         UInt base   = iregEnc(i->RISCV64in.XDirect.base);
         Int  soff12 = i->RISCV64in.XDirect.soff12;
         vassert(soff12 >= -2048 && soff12 < 2048);
         UInt imm11_0 = soff12 & 0xfff;

         p = emit_S(p, 0b0100011, imm11_0, 0b011, base, 5 /*x5/t0*/);
      }

      /* --- FIRST PATCHABLE BYTE follows --- */
      /* VG_(disp_cp_chain_me_to_{slowEP,fastEP}) (where we're calling to) backs
         up the return address, so as to find the address of the first patchable
         byte. So: don't change the number of instructions (3) below. */
      /* li t0, VG_(disp_cp_chain_me_to_{slowEP,fastEP}) */
      const void* disp_cp_chain_me = i->RISCV64in.XDirect.toFastEP
                                        ? disp_cp_chain_me_to_fastEP
                                        : disp_cp_chain_me_to_slowEP;
      p = addr48_to_ireg_EXACTLY_18B(p, 5 /*x5/t0*/,
                                     (ULong)(Addr)disp_cp_chain_me);

      /* c.jalr t0(0) */
      p = emit_CR(p, 0b10, 0 /*x0/zero*/, 5 /*x5/t0*/, 0b1001);
      /* --- END of PATCHABLE BYTES --- */

      /* Fix up the conditional jump, if there was one. */
      if (!hregIsInvalid(i->RISCV64in.XDirect.cond)) {
         /* beq cond, zero, delta */
         UInt cond  = iregEnc(i->RISCV64in.XDirect.cond);
         UInt delta = p - ptmp;
         /* delta_min = 4 (beq) + 2 (c.li) + 4 (sd) + 18 (addr48) + 2 (c.jalr)
                      = 30 */
         vassert(delta >= 30 && delta < 4096 && (delta & 1) == 0);
         UInt imm12_1 = (delta >> 1) & 0x7ff;

         emit_B(ptmp, 0b1100011, imm12_1, 0b000, cond, 0 /*x0/zero*/);
      }

      goto done;
   }

   case RISCV64in_XIndir: {
      /* We're generating transfers that could lead indirectly to a chain-me, so
         we need to be sure this is actually allowed -- no-redir translations
         are not allowed to reach normal translations without going through the
         scheduler. That means no XDirects or XIndirs out from no-redir
         translations. Hence: */
      vassert(disp_cp_xindir != NULL);

      /* First off, if this is conditional, create a conditional jump over the
         rest of it. Or at least, leave a space for it that we will shortly fill
         in. */
      UChar* ptmp = NULL;
      if (!hregIsInvalid(i->RISCV64in.XIndir.cond)) {
         ptmp = p;
         p += 4;
      }

      /* Update the guest pc. */
      {
         /* sd r-dstGA, soff12(base) */
         UInt src    = iregEnc(i->RISCV64in.XIndir.dstGA);
         UInt base   = iregEnc(i->RISCV64in.XIndir.base);
         Int  soff12 = i->RISCV64in.XIndir.soff12;
         vassert(soff12 >= -2048 && soff12 < 2048);
         UInt imm11_0 = soff12 & 0xfff;

         p = emit_S(p, 0b0100011, imm11_0, 0b011, base, src);
      }

      /* li t0, VG_(disp_cp_xindir) */
      p = imm64_to_ireg(p, 5 /*x5/t0*/, (ULong)(Addr)disp_cp_xindir);

      /* c.jr t0(0) */
      p = emit_CR(p, 0b10, 0 /*x0/zero*/, 5 /*x5/t0*/, 0b1000);

      /* Fix up the conditional jump, if there was one. */
      if (!hregIsInvalid(i->RISCV64in.XIndir.cond)) {
         /* beq cond, zero, delta */
         UInt cond  = iregEnc(i->RISCV64in.XIndir.cond);
         UInt delta = p - ptmp;
         /* delta_min = 4 (beq) + 4 (sd) + 2 (c.li) + 2 (c.jr) = 12 */
         vassert(delta >= 12 && delta < 4096 && (delta & 1) == 0);
         UInt imm12_1 = (delta >> 1) & 0x7ff;

         emit_B(ptmp, 0b1100011, imm12_1, 0b000, cond, 0 /*x0/zero*/);
      }

      goto done;
   }

   case RISCV64in_XAssisted: {
      /* First off, if this is conditional, create a conditional jump over the
         rest of it. Or at least, leave a space for it that we will shortly fill
         in. */
      UChar* ptmp = NULL;
      if (!hregIsInvalid(i->RISCV64in.XAssisted.cond)) {
         ptmp = p;
         p += 4;
      }

      /* Update the guest pc. */
      {
         /* sd r-dstGA, soff12(base) */
         UInt src    = iregEnc(i->RISCV64in.XAssisted.dstGA);
         UInt base   = iregEnc(i->RISCV64in.XAssisted.base);
         Int  soff12 = i->RISCV64in.XAssisted.soff12;
         vassert(soff12 >= -2048 && soff12 < 2048);
         UInt imm11_0 = soff12 & 0xfff;

         p = emit_S(p, 0b0100011, imm11_0, 0b011, base, src);
      }

      /* li s0, $magic_number */
      UInt trcval = 0;
      switch (i->RISCV64in.XAssisted.jk) {
      case Ijk_ClientReq:
         trcval = VEX_TRC_JMP_CLIENTREQ;
         break;
      case Ijk_Sys_syscall:
         trcval = VEX_TRC_JMP_SYS_SYSCALL;
         break;
      case Ijk_EmWarn:
         trcval = VEX_TRC_JMP_EMWARN;
         break;
      case Ijk_EmFail:
         trcval = VEX_TRC_JMP_EMFAIL;
         break;
      case Ijk_NoDecode:
         trcval = VEX_TRC_JMP_NODECODE;
         break;
      case Ijk_InvalICache:
         trcval = VEX_TRC_JMP_INVALICACHE;
         break;
      case Ijk_NoRedir:
         trcval = VEX_TRC_JMP_NOREDIR;
         break;
      case Ijk_SigILL:
         trcval = VEX_TRC_JMP_SIGILL;
         break;
      case Ijk_SigTRAP:
         trcval = VEX_TRC_JMP_SIGTRAP;
         break;
      case Ijk_SigBUS:
         trcval = VEX_TRC_JMP_SIGBUS;
         break;
      case Ijk_SigFPE_IntDiv:
         trcval = VEX_TRC_JMP_SIGFPE_INTDIV;
         break;
      case Ijk_SigFPE_IntOvf:
         trcval = VEX_TRC_JMP_SIGFPE_INTOVF;
         break;
      case Ijk_Boring:
         trcval = VEX_TRC_JMP_BORING;
         break;
      default:
         ppIRJumpKind(i->RISCV64in.XAssisted.jk);
         vpanic("emit_RISCV64Instr.RISCV64in_XAssisted: unexpected jump kind");
      }
      vassert(trcval != 0);
      p = imm64_to_ireg(p, 8 /*x8/s0*/, trcval);

      /* li t0, VG_(disp_cp_xassisted) */
      p = imm64_to_ireg(p, 5 /*x5/t0*/, (ULong)(Addr)disp_cp_xassisted);

      /* c.jr t0(0) */
      p = emit_CR(p, 0b10, 0 /*x0/zero*/, 5 /*x5/t0*/, 0b1000);

      /* Fix up the conditional jump, if there was one. */
      if (!hregIsInvalid(i->RISCV64in.XAssisted.cond)) {
         /* beq cond, zero, delta */
         UInt cond  = iregEnc(i->RISCV64in.XAssisted.cond);
         UInt delta = p - ptmp;
         /* delta_min = 4 (beq) + 4 (sd) + 2 (c.li) + 2 (c.li) + 2 (c.jr)
                      = 14 */
         vassert(delta >= 14 && delta < 4096 && (delta & 1) == 0);
         UInt imm12_1 = (delta >> 1) & 0x7ff;

         emit_B(ptmp, 0b1100011, imm12_1, 0b000, cond, 0 /*x0/zero*/);
      }

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
   vassert(p - &buf[0] <= 44);
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

   /* What we're expecting to see is:
        lui t0, disp_cp_chain_me_to_EXPECTED[47:28]'
        addiw t0, t0, disp_cp_chain_me_to_EXPECTED[27:16]'
        c.slli t0, 12
        addi t0, t0, disp_cp_chain_me_to_EXPECTED[15:4]'
        c.slli t0, 4
        c.addi t0, disp_cp_chain_me_to_EXPECTED[3:0]'
        c.jalr t0(0)
      viz
        <18 bytes generated by addr48_to_ireg_EXACTLY_18B>
        82 92
   */
   UChar* p = place_to_chain;
   vassert(((HWord)p & 1) == 0);
   vassert(is_addr48_to_ireg_EXACTLY_18B(p, 5 /*x5/t0*/,
                                         (ULong)disp_cp_chain_me_EXPECTED));
   vassert(p[18] == 0x82 && p[19] == 0x92);

   /* And what we want to change it to is:
        lui t0, place_to_jump[47:28]'
        addiw t0, t0, place_to_jump[27:16]'
        c.slli t0, 12
        addi t0, t0, place_to_jump[15:4]'
        c.slli t0, 4
        c.addi t0, place_to_jump[3:0]'
        c.jr t0(0)
      viz
        <18 bytes generated by addr48_to_ireg_EXACTLY_18B>
        82 82

      The replacement has the same length as the original.
   */
   (void)addr48_to_ireg_EXACTLY_18B(p, 5 /*x5/t0*/, (ULong)place_to_jump_to);
   p[18] = 0x82;
   p[19] = 0x82;

   VexInvalRange vir = {(HWord)p, 20};
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
