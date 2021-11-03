
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

RISCV64Instr* RISCV64Instr_ADDI(HReg dst, HReg src, Int simm12)
{
   RISCV64Instr* i          = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                   = RISCV64in_ADDI;
   i->RISCV64in.ADDI.dst    = dst;
   i->RISCV64in.ADDI.src    = src;
   i->RISCV64in.ADDI.simm12 = simm12;
   return i;
}

RISCV64Instr* RISCV64Instr_ADDW(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_ADDW;
   i->RISCV64in.ADDW.dst  = dst;
   i->RISCV64in.ADDW.src1 = src1;
   i->RISCV64in.ADDW.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_ADDIW(HReg dst, HReg src, Int simm12)
{
   RISCV64Instr* i           = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                    = RISCV64in_ADDIW;
   i->RISCV64in.ADDIW.dst    = dst;
   i->RISCV64in.ADDIW.src    = src;
   i->RISCV64in.ADDIW.simm12 = simm12;
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

RISCV64Instr* RISCV64Instr_SUBW(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_SUBW;
   i->RISCV64in.SUBW.dst  = dst;
   i->RISCV64in.SUBW.src1 = src1;
   i->RISCV64in.SUBW.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_XOR(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_XOR;
   i->RISCV64in.XOR.dst  = dst;
   i->RISCV64in.XOR.src1 = src1;
   i->RISCV64in.XOR.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_XORI(HReg dst, HReg src, Int simm12)
{
   RISCV64Instr* i          = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                   = RISCV64in_XORI;
   i->RISCV64in.XORI.dst    = dst;
   i->RISCV64in.XORI.src    = src;
   i->RISCV64in.XORI.simm12 = simm12;
   return i;
}

RISCV64Instr* RISCV64Instr_OR(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i      = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag               = RISCV64in_OR;
   i->RISCV64in.OR.dst  = dst;
   i->RISCV64in.OR.src1 = src1;
   i->RISCV64in.OR.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_AND(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_AND;
   i->RISCV64in.AND.dst  = dst;
   i->RISCV64in.AND.src1 = src1;
   i->RISCV64in.AND.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_SLL(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_SLL;
   i->RISCV64in.SLL.dst  = dst;
   i->RISCV64in.SLL.src1 = src1;
   i->RISCV64in.SLL.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_SRL(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_SRL;
   i->RISCV64in.SRL.dst  = dst;
   i->RISCV64in.SRL.src1 = src1;
   i->RISCV64in.SRL.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_SRA(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_SRA;
   i->RISCV64in.SRA.dst  = dst;
   i->RISCV64in.SRA.src1 = src1;
   i->RISCV64in.SRA.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_SLLI(HReg dst, HReg src, UInt uimm6)
{
   RISCV64Instr* i         = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                  = RISCV64in_SLLI;
   i->RISCV64in.SLLI.dst   = dst;
   i->RISCV64in.SLLI.src   = src;
   i->RISCV64in.SLLI.uimm6 = uimm6;
   return i;
}

RISCV64Instr* RISCV64Instr_SRLI(HReg dst, HReg src, UInt uimm6)
{
   RISCV64Instr* i         = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                  = RISCV64in_SRLI;
   i->RISCV64in.SRLI.dst   = dst;
   i->RISCV64in.SRLI.src   = src;
   i->RISCV64in.SRLI.uimm6 = uimm6;
   return i;
}

RISCV64Instr* RISCV64Instr_SRAI(HReg dst, HReg src, UInt uimm6)
{
   RISCV64Instr* i         = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                  = RISCV64in_SRAI;
   i->RISCV64in.SRAI.dst   = dst;
   i->RISCV64in.SRAI.src   = src;
   i->RISCV64in.SRAI.uimm6 = uimm6;
   return i;
}

RISCV64Instr* RISCV64Instr_SLLW(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_SLLW;
   i->RISCV64in.SLLW.dst  = dst;
   i->RISCV64in.SLLW.src1 = src1;
   i->RISCV64in.SLLW.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_SRLW(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_SRLW;
   i->RISCV64in.SRLW.dst  = dst;
   i->RISCV64in.SRLW.src1 = src1;
   i->RISCV64in.SRLW.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_SRAW(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_SRAW;
   i->RISCV64in.SRAW.dst  = dst;
   i->RISCV64in.SRAW.src1 = src1;
   i->RISCV64in.SRAW.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_SLT(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_SLT;
   i->RISCV64in.SLT.dst  = dst;
   i->RISCV64in.SLT.src1 = src1;
   i->RISCV64in.SLT.src2 = src2;
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

RISCV64Instr* RISCV64Instr_MUL(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_MUL;
   i->RISCV64in.MUL.dst  = dst;
   i->RISCV64in.MUL.src1 = src1;
   i->RISCV64in.MUL.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_MULH(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_MULH;
   i->RISCV64in.MULH.dst  = dst;
   i->RISCV64in.MULH.src1 = src1;
   i->RISCV64in.MULH.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_MULHU(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i         = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                  = RISCV64in_MULHU;
   i->RISCV64in.MULHU.dst  = dst;
   i->RISCV64in.MULHU.src1 = src1;
   i->RISCV64in.MULHU.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_DIV(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_DIV;
   i->RISCV64in.DIV.dst  = dst;
   i->RISCV64in.DIV.src1 = src1;
   i->RISCV64in.DIV.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_DIVU(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_DIVU;
   i->RISCV64in.DIVU.dst  = dst;
   i->RISCV64in.DIVU.src1 = src1;
   i->RISCV64in.DIVU.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_REM(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i       = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                = RISCV64in_REM;
   i->RISCV64in.REM.dst  = dst;
   i->RISCV64in.REM.src1 = src1;
   i->RISCV64in.REM.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_REMU(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_REMU;
   i->RISCV64in.REMU.dst  = dst;
   i->RISCV64in.REMU.src1 = src1;
   i->RISCV64in.REMU.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_MULW(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_MULW;
   i->RISCV64in.MULW.dst  = dst;
   i->RISCV64in.MULW.src1 = src1;
   i->RISCV64in.MULW.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_DIVW(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_DIVW;
   i->RISCV64in.DIVW.dst  = dst;
   i->RISCV64in.DIVW.src1 = src1;
   i->RISCV64in.DIVW.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_DIVUW(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i         = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                  = RISCV64in_DIVUW;
   i->RISCV64in.DIVUW.dst  = dst;
   i->RISCV64in.DIVUW.src1 = src1;
   i->RISCV64in.DIVUW.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_REMW(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_REMW;
   i->RISCV64in.REMW.dst  = dst;
   i->RISCV64in.REMW.src1 = src1;
   i->RISCV64in.REMW.src2 = src2;
   return i;
}

RISCV64Instr* RISCV64Instr_REMUW(HReg dst, HReg src1, HReg src2)
{
   RISCV64Instr* i         = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                  = RISCV64in_REMUW;
   i->RISCV64in.REMUW.dst  = dst;
   i->RISCV64in.REMUW.src1 = src1;
   i->RISCV64in.REMUW.src2 = src2;
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

RISCV64Instr* RISCV64Instr_LR_W(HReg dst, HReg addr)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_LR_W;
   i->RISCV64in.LR_W.dst  = dst;
   i->RISCV64in.LR_W.addr = addr;
   return i;
}

RISCV64Instr* RISCV64Instr_SC_W(HReg res, HReg src, HReg addr)
{
   RISCV64Instr* i        = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                 = RISCV64in_SC_W;
   i->RISCV64in.SC_W.res  = res;
   i->RISCV64in.SC_W.src  = src;
   i->RISCV64in.SC_W.addr = addr;
   return i;
}

RISCV64Instr* RISCV64Instr_CAS_W(HReg old, HReg addr, HReg expd, HReg data)
{
   RISCV64Instr* i         = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                  = RISCV64in_CAS_W;
   i->RISCV64in.CAS_W.old  = old;
   i->RISCV64in.CAS_W.addr = addr;
   i->RISCV64in.CAS_W.expd = expd;
   i->RISCV64in.CAS_W.data = data;
   return i;
}

RISCV64Instr* RISCV64Instr_CAS_D(HReg old, HReg addr, HReg expd, HReg data)
{
   RISCV64Instr* i         = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                  = RISCV64in_CAS_D;
   i->RISCV64in.CAS_D.old  = old;
   i->RISCV64in.CAS_D.addr = addr;
   i->RISCV64in.CAS_D.expd = expd;
   i->RISCV64in.CAS_D.data = data;
   return i;
}

RISCV64Instr* RISCV64Instr_FENCE(void)
{
   RISCV64Instr* i = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag          = RISCV64in_FENCE;
   return i;
}

RISCV64Instr* RISCV64Instr_CSEL(HReg dst, HReg iftrue, HReg iffalse, HReg cond)
{
   RISCV64Instr* i           = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                    = RISCV64in_CSEL;
   i->RISCV64in.CSEL.dst     = dst;
   i->RISCV64in.CSEL.iftrue  = iftrue;
   i->RISCV64in.CSEL.iffalse = iffalse;
   i->RISCV64in.CSEL.cond    = cond;
   return i;
}

RISCV64Instr*
RISCV64Instr_Call(RetLoc rloc, Addr64 target, HReg cond, Int nArgRegs)
{
   RISCV64Instr* i            = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag                     = RISCV64in_Call;
   i->RISCV64in.Call.rloc     = rloc;
   i->RISCV64in.Call.target   = target;
   i->RISCV64in.Call.cond     = cond;
   i->RISCV64in.Call.nArgRegs = nArgRegs;
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

RISCV64Instr* RISCV64Instr_EvCheck(HReg base_amCounter,
                                   Int  soff12_amCounter,
                                   HReg base_amFailAddr,
                                   Int  soff12_amFailAddr)
{
   RISCV64Instr* i = LibVEX_Alloc_inline(sizeof(RISCV64Instr));
   i->tag          = RISCV64in_EvCheck;
   i->RISCV64in.EvCheck.base_amCounter    = base_amCounter;
   i->RISCV64in.EvCheck.soff12_amCounter  = soff12_amCounter;
   i->RISCV64in.EvCheck.base_amFailAddr   = base_amFailAddr;
   i->RISCV64in.EvCheck.soff12_amFailAddr = soff12_amFailAddr;
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
   case RISCV64in_ADDI:
      vex_printf("addi    ");
      ppHRegRISCV64(i->RISCV64in.ADDI.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.ADDI.src);
      vex_printf(", %d", i->RISCV64in.ADDI.simm12);
      return;
   case RISCV64in_ADDW:
      vex_printf("addw    ");
      ppHRegRISCV64(i->RISCV64in.ADDW.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.ADDW.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.ADDW.src2);
      return;
   case RISCV64in_ADDIW:
      vex_printf("addiw   ");
      ppHRegRISCV64(i->RISCV64in.ADDIW.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.ADDIW.src);
      vex_printf(", %d", i->RISCV64in.ADDIW.simm12);
      return;
   case RISCV64in_SUB:
      vex_printf("sub     ");
      ppHRegRISCV64(i->RISCV64in.SUB.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SUB.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SUB.src2);
      return;
   case RISCV64in_SUBW:
      vex_printf("subw    ");
      ppHRegRISCV64(i->RISCV64in.SUBW.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SUBW.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SUBW.src2);
      return;
   case RISCV64in_XOR:
      vex_printf("xor     ");
      ppHRegRISCV64(i->RISCV64in.XOR.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.XOR.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.XOR.src2);
      return;
   case RISCV64in_XORI:
      vex_printf("xori    ");
      ppHRegRISCV64(i->RISCV64in.XORI.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.XORI.src);
      vex_printf(", %d", i->RISCV64in.XORI.simm12);
      return;
   case RISCV64in_OR:
      vex_printf("or      ");
      ppHRegRISCV64(i->RISCV64in.OR.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.OR.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.OR.src2);
      return;
   case RISCV64in_AND:
      vex_printf("and     ");
      ppHRegRISCV64(i->RISCV64in.AND.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.AND.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.AND.src2);
      return;
   case RISCV64in_SLL:
      vex_printf("sll     ");
      ppHRegRISCV64(i->RISCV64in.SLL.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SLL.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SLL.src2);
      return;
   case RISCV64in_SRL:
      vex_printf("srl     ");
      ppHRegRISCV64(i->RISCV64in.SRL.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SRL.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SRL.src2);
      return;
   case RISCV64in_SRA:
      vex_printf("sra     ");
      ppHRegRISCV64(i->RISCV64in.SRA.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SRA.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SRA.src2);
      return;
   case RISCV64in_SLLI:
      vex_printf("slli    ");
      ppHRegRISCV64(i->RISCV64in.SLLI.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SLLI.src);
      vex_printf(", %u", i->RISCV64in.SLLI.uimm6);
      return;
   case RISCV64in_SRLI:
      vex_printf("srli    ");
      ppHRegRISCV64(i->RISCV64in.SRLI.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SRLI.src);
      vex_printf(", %u", i->RISCV64in.SRLI.uimm6);
      return;
   case RISCV64in_SRAI:
      vex_printf("srai    ");
      ppHRegRISCV64(i->RISCV64in.SRAI.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SRAI.src);
      vex_printf(", %u", i->RISCV64in.SRAI.uimm6);
      return;
   case RISCV64in_SLLW:
      vex_printf("sllw    ");
      ppHRegRISCV64(i->RISCV64in.SLLW.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SLLW.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SLLW.src2);
      return;
   case RISCV64in_SRLW:
      vex_printf("srlw    ");
      ppHRegRISCV64(i->RISCV64in.SRLW.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SRLW.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SRLW.src2);
      return;
   case RISCV64in_SRAW:
      vex_printf("sraw    ");
      ppHRegRISCV64(i->RISCV64in.SRAW.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SRAW.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SRAW.src2);
      return;
   case RISCV64in_SLT:
      vex_printf("slt     ");
      ppHRegRISCV64(i->RISCV64in.SLT.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SLT.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SLT.src2);
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
   case RISCV64in_MUL:
      vex_printf("mul     ");
      ppHRegRISCV64(i->RISCV64in.MUL.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.MUL.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.MUL.src2);
      return;
   case RISCV64in_MULH:
      vex_printf("mulh    ");
      ppHRegRISCV64(i->RISCV64in.MULH.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.MULH.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.MULH.src2);
      return;
   case RISCV64in_MULHU:
      vex_printf("mulhu   ");
      ppHRegRISCV64(i->RISCV64in.MULHU.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.MULHU.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.MULHU.src2);
      return;
   case RISCV64in_DIV:
      vex_printf("div     ");
      ppHRegRISCV64(i->RISCV64in.DIV.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.DIV.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.DIV.src2);
      return;
   case RISCV64in_DIVU:
      vex_printf("divu    ");
      ppHRegRISCV64(i->RISCV64in.DIVU.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.DIVU.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.DIVU.src2);
      return;
   case RISCV64in_REM:
      vex_printf("rem     ");
      ppHRegRISCV64(i->RISCV64in.REM.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.REM.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.REM.src2);
      return;
   case RISCV64in_REMU:
      vex_printf("remu    ");
      ppHRegRISCV64(i->RISCV64in.REMU.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.REMU.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.REMU.src2);
      return;
   case RISCV64in_MULW:
      vex_printf("mulw    ");
      ppHRegRISCV64(i->RISCV64in.MULW.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.MULW.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.MULW.src2);
      return;
   case RISCV64in_DIVW:
      vex_printf("divw    ");
      ppHRegRISCV64(i->RISCV64in.DIVW.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.DIVW.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.DIVW.src2);
      return;
   case RISCV64in_DIVUW:
      vex_printf("divuw   ");
      ppHRegRISCV64(i->RISCV64in.DIVUW.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.DIVUW.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.DIVUW.src2);
      return;
   case RISCV64in_REMW:
      vex_printf("remw    ");
      ppHRegRISCV64(i->RISCV64in.REMW.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.REMW.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.REMW.src2);
      return;
   case RISCV64in_REMUW:
      vex_printf("remuw   ");
      ppHRegRISCV64(i->RISCV64in.REMUW.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.REMUW.src1);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.REMUW.src2);
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
   case RISCV64in_LR_W:
      vex_printf("lr.w    ");
      ppHRegRISCV64(i->RISCV64in.LR_W.dst);
      vex_printf(", (");
      ppHRegRISCV64(i->RISCV64in.LR_W.addr);
      vex_printf(")");
      return;
   case RISCV64in_SC_W:
      vex_printf("sc.w    ");
      ppHRegRISCV64(i->RISCV64in.SC_W.res);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.SC_W.src);
      vex_printf(", (");
      ppHRegRISCV64(i->RISCV64in.SC_W.addr);
      vex_printf(")");
      return;
   case RISCV64in_CAS_W:
      vex_printf("(CAS_W) 1: lr.w ");
      ppHRegRISCV64(i->RISCV64in.CAS_W.old);
      vex_printf(", (");
      ppHRegRISCV64(i->RISCV64in.CAS_W.addr);
      vex_printf("); bne ");
      ppHRegRISCV64(i->RISCV64in.CAS_W.old);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.CAS_W.expd);
      vex_printf(", 2f; sc.w t0, ");
      ppHRegRISCV64(i->RISCV64in.CAS_W.data);
      vex_printf(", (");
      ppHRegRISCV64(i->RISCV64in.CAS_W.addr);
      vex_printf("); bne t0, zero, 1b; 2:");
      return;
   case RISCV64in_CAS_D:
      vex_printf("(CAS_D) 1: lr.d ");
      ppHRegRISCV64(i->RISCV64in.CAS_D.old);
      vex_printf(", (");
      ppHRegRISCV64(i->RISCV64in.CAS_D.addr);
      vex_printf("); bne ");
      ppHRegRISCV64(i->RISCV64in.CAS_D.old);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.CAS_D.expd);
      vex_printf(", 2f; sc.d t0, ");
      ppHRegRISCV64(i->RISCV64in.CAS_D.data);
      vex_printf(", (");
      ppHRegRISCV64(i->RISCV64in.CAS_D.addr);
      vex_printf("); bne t0, zero, 1b; 2:");
      return;
   case RISCV64in_FENCE:
      vex_printf("fence");
      return;
   case RISCV64in_CSEL:
      vex_printf("(CSEL) beq ");
      ppHRegRISCV64(i->RISCV64in.CSEL.cond);
      vex_printf(", zero, 1f; c.mv ");
      ppHRegRISCV64(i->RISCV64in.CSEL.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.CSEL.iftrue);
      vex_printf("; c.j 2f; 1: c.mv ");
      ppHRegRISCV64(i->RISCV64in.CSEL.dst);
      vex_printf(", ");
      ppHRegRISCV64(i->RISCV64in.CSEL.iffalse);
      vex_printf("; 2:");
      return;
   case RISCV64in_Call:
      vex_printf("(Call) ");
      if (!hregIsInvalid(i->RISCV64in.Call.cond)) {
         vex_printf("beq ");
         ppHRegRISCV64(i->RISCV64in.Call.cond);
         vex_printf(", zero, 1f; ");
      }
      vex_printf("li t0, 0x%llx; c.jalr 0(t0) [nArgRegs=%d, ",
                 i->RISCV64in.Call.target, i->RISCV64in.Call.nArgRegs);
      ppRetLoc(i->RISCV64in.Call.rloc);
      vex_printf("]; 1:");
      return;
   case RISCV64in_XDirect:
      vex_printf("(xDirect) ");
      if (!hregIsInvalid(i->RISCV64in.XDirect.cond)) {
         vex_printf("beq ");
         ppHRegRISCV64(i->RISCV64in.XDirect.cond);
         vex_printf(", zero, 1f; ");
      }
      vex_printf("li t0, 0x%llx; sd t0, %d(", i->RISCV64in.XDirect.dstGA,
                 i->RISCV64in.XDirect.soff12);
      ppHRegRISCV64(i->RISCV64in.XDirect.base);
      vex_printf("); li t0, <%s>; c.jalr 0(t0); 1:",
                 i->RISCV64in.XDirect.toFastEP ? "disp_cp_chain_me_to_fastEP"
                                               : "disp_cp_chain_me_to_slowEP");
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
      vex_printf("); li t0, <disp_cp_xindir>; c.jr 0(t0); 1:");
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
      vex_printf("; li t0, <disp_cp_xassisted>; c.jr 0(t0); 1:");
      return;
   case RISCV64in_EvCheck:
      vex_printf("(evCheck) lw t0, %d(", i->RISCV64in.EvCheck.soff12_amCounter);
      ppHRegRISCV64(i->RISCV64in.EvCheck.base_amCounter);
      vex_printf("); c.addiw t0, -1; sw t0, %d(",
                 i->RISCV64in.EvCheck.soff12_amCounter);
      ppHRegRISCV64(i->RISCV64in.EvCheck.base_amCounter);
      vex_printf("); bge t0, zero, 1f; ld t0, %d(",
                 i->RISCV64in.EvCheck.soff12_amFailAddr);
      ppHRegRISCV64(i->RISCV64in.EvCheck.base_amFailAddr);
      vex_printf("); c.jr 0(t0); 1:");
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
   ru->regs[ru->size++]          = hregRISCV64_x18(); /* s2 */
   ru->regs[ru->size++]          = hregRISCV64_x19(); /* s3 */
   ru->regs[ru->size++]          = hregRISCV64_x20(); /* s4 */
   ru->regs[ru->size++]          = hregRISCV64_x21(); /* s5 */
   ru->regs[ru->size++]          = hregRISCV64_x22(); /* s6 */
   ru->regs[ru->size++]          = hregRISCV64_x23(); /* s7 */
   ru->regs[ru->size++]          = hregRISCV64_x24(); /* s8 */
   ru->regs[ru->size++]          = hregRISCV64_x25(); /* s9 */
   ru->regs[ru->size++]          = hregRISCV64_x26(); /* s10 */
   ru->regs[ru->size++]          = hregRISCV64_x27(); /* s11 */
   ru->regs[ru->size++]          = hregRISCV64_x10(); /* a0 */
   ru->regs[ru->size++]          = hregRISCV64_x11(); /* a1 */
   ru->regs[ru->size++]          = hregRISCV64_x12(); /* a2 */
   ru->regs[ru->size++]          = hregRISCV64_x13(); /* a3 */
   ru->regs[ru->size++]          = hregRISCV64_x14(); /* a4 */
   ru->regs[ru->size++]          = hregRISCV64_x15(); /* a5 */
   ru->regs[ru->size++]          = hregRISCV64_x16(); /* a6 */
   ru->regs[ru->size++]          = hregRISCV64_x17(); /* a7 */
   ru->allocable_end[HRcInt64]   = ru->size - 1;

   /* Floating-point registers, all of which are caller-saved. */
   ru->allocable_start[HRcFlt64] = ru->size;
   ru->regs[ru->size++]          = hregRISCV64_f0();  /* ft0 */
   ru->regs[ru->size++]          = hregRISCV64_f1();  /* ft1 */
   ru->regs[ru->size++]          = hregRISCV64_f2();  /* ft2 */
   ru->regs[ru->size++]          = hregRISCV64_f3();  /* ft3 */
   ru->regs[ru->size++]          = hregRISCV64_f4();  /* ft4 */
   ru->regs[ru->size++]          = hregRISCV64_f5();  /* ft5 */
   ru->regs[ru->size++]          = hregRISCV64_f6();  /* ft6 */
   ru->regs[ru->size++]          = hregRISCV64_f7();  /* ft7 */
   ru->regs[ru->size++]          = hregRISCV64_f10(); /* fa0 */
   ru->regs[ru->size++]          = hregRISCV64_f11(); /* fa1 */
   ru->regs[ru->size++]          = hregRISCV64_f12(); /* fa2 */
   ru->regs[ru->size++]          = hregRISCV64_f13(); /* fa3 */
   ru->regs[ru->size++]          = hregRISCV64_f14(); /* fa4 */
   ru->regs[ru->size++]          = hregRISCV64_f15(); /* fa5 */
   ru->regs[ru->size++]          = hregRISCV64_f16(); /* fa6 */
   ru->regs[ru->size++]          = hregRISCV64_f17(); /* fa7 */
   ru->regs[ru->size++]          = hregRISCV64_f28(); /* ft8 */
   ru->regs[ru->size++]          = hregRISCV64_f29(); /* ft9 */
   ru->regs[ru->size++]          = hregRISCV64_f30(); /* ft10 */
   ru->regs[ru->size++]          = hregRISCV64_f31(); /* ft11 */
   ru->allocable_end[HRcFlt64]   = ru->size - 1;
   ru->allocable                 = ru->size;

   /* Add the registers that are not available for allocation. */
   /* TODO */
   ru->regs[ru->size++] = hregRISCV64_x0(); /* zero */
   ru->regs[ru->size++] = hregRISCV64_x2(); /* sp */
   ru->regs[ru->size++] = hregRISCV64_x8(); /* s0 */

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
   case RISCV64in_ADDI:
      addHRegUse(u, HRmWrite, i->RISCV64in.ADDI.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.ADDI.src);
      return;
   case RISCV64in_ADDW:
      addHRegUse(u, HRmWrite, i->RISCV64in.ADDW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.ADDW.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.ADDW.src2);
      return;
   case RISCV64in_ADDIW:
      addHRegUse(u, HRmWrite, i->RISCV64in.ADDIW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.ADDIW.src);
      return;
   case RISCV64in_SUB:
      addHRegUse(u, HRmWrite, i->RISCV64in.SUB.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SUB.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.SUB.src2);
      return;
   case RISCV64in_SUBW:
      addHRegUse(u, HRmWrite, i->RISCV64in.SUBW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SUBW.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.SUBW.src2);
      return;
   case RISCV64in_XOR:
      addHRegUse(u, HRmWrite, i->RISCV64in.XOR.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.XOR.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.XOR.src2);
      return;
   case RISCV64in_XORI:
      addHRegUse(u, HRmWrite, i->RISCV64in.XORI.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.XORI.src);
      return;
   case RISCV64in_OR:
      addHRegUse(u, HRmWrite, i->RISCV64in.OR.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.OR.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.OR.src2);
      return;
   case RISCV64in_AND:
      addHRegUse(u, HRmWrite, i->RISCV64in.AND.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.AND.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.AND.src2);
      return;
   case RISCV64in_SLL:
      addHRegUse(u, HRmWrite, i->RISCV64in.SLL.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SLL.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.SLL.src2);
      return;
   case RISCV64in_SRL:
      addHRegUse(u, HRmWrite, i->RISCV64in.SRL.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SRL.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.SRL.src2);
      return;
   case RISCV64in_SRA:
      addHRegUse(u, HRmWrite, i->RISCV64in.SRA.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SRA.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.SRA.src2);
      return;
   case RISCV64in_SLLI:
      addHRegUse(u, HRmWrite, i->RISCV64in.SLLI.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SLLI.src);
      return;
   case RISCV64in_SRLI:
      addHRegUse(u, HRmWrite, i->RISCV64in.SRLI.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SRLI.src);
      return;
   case RISCV64in_SRAI:
      addHRegUse(u, HRmWrite, i->RISCV64in.SRAI.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SRAI.src);
      return;
   case RISCV64in_SLLW:
      addHRegUse(u, HRmWrite, i->RISCV64in.SLLW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SLLW.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.SLLW.src2);
      return;
   case RISCV64in_SRLW:
      addHRegUse(u, HRmWrite, i->RISCV64in.SRLW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SRLW.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.SRLW.src2);
      return;
   case RISCV64in_SRAW:
      addHRegUse(u, HRmWrite, i->RISCV64in.SRAW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SRAW.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.SRAW.src2);
      return;
   case RISCV64in_SLT:
      addHRegUse(u, HRmWrite, i->RISCV64in.SLT.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.SLT.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.SLT.src2);
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
   case RISCV64in_MUL:
      addHRegUse(u, HRmWrite, i->RISCV64in.MUL.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.MUL.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.MUL.src2);
      return;
   case RISCV64in_MULH:
      addHRegUse(u, HRmWrite, i->RISCV64in.MULH.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.MULH.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.MULH.src2);
      return;
   case RISCV64in_MULHU:
      addHRegUse(u, HRmWrite, i->RISCV64in.MULHU.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.MULHU.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.MULHU.src2);
      return;
   case RISCV64in_DIV:
      addHRegUse(u, HRmWrite, i->RISCV64in.DIV.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.DIV.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.DIV.src2);
      return;
   case RISCV64in_DIVU:
      addHRegUse(u, HRmWrite, i->RISCV64in.DIVU.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.DIVU.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.DIVU.src2);
      return;
   case RISCV64in_REM:
      addHRegUse(u, HRmWrite, i->RISCV64in.REM.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.REM.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.REM.src2);
      return;
   case RISCV64in_REMU:
      addHRegUse(u, HRmWrite, i->RISCV64in.REMU.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.REMU.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.REMU.src2);
      return;
   case RISCV64in_MULW:
      addHRegUse(u, HRmWrite, i->RISCV64in.MULW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.MULW.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.MULW.src2);
      return;
   case RISCV64in_DIVW:
      addHRegUse(u, HRmWrite, i->RISCV64in.DIVW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.DIVW.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.DIVW.src2);
      return;
   case RISCV64in_DIVUW:
      addHRegUse(u, HRmWrite, i->RISCV64in.DIVUW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.DIVUW.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.DIVUW.src2);
      return;
   case RISCV64in_REMW:
      addHRegUse(u, HRmWrite, i->RISCV64in.REMW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.REMW.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.REMW.src2);
      return;
   case RISCV64in_REMUW:
      addHRegUse(u, HRmWrite, i->RISCV64in.REMUW.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.REMUW.src1);
      addHRegUse(u, HRmRead, i->RISCV64in.REMUW.src2);
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
   case RISCV64in_LR_W:
      addHRegUse(u, HRmWrite, i->RISCV64in.LR_W.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.LR_W.addr);
      return;
   case RISCV64in_SC_W:
      addHRegUse(u, HRmWrite, i->RISCV64in.SC_W.res);
      addHRegUse(u, HRmRead, i->RISCV64in.SC_W.src);
      addHRegUse(u, HRmRead, i->RISCV64in.SC_W.addr);
      return;
   case RISCV64in_CAS_W:
      addHRegUse(u, HRmWrite, i->RISCV64in.CAS_W.old);
      addHRegUse(u, HRmRead, i->RISCV64in.CAS_W.addr);
      addHRegUse(u, HRmRead, i->RISCV64in.CAS_W.expd);
      addHRegUse(u, HRmRead, i->RISCV64in.CAS_W.data);
      return;
   case RISCV64in_CAS_D:
      addHRegUse(u, HRmWrite, i->RISCV64in.CAS_D.old);
      addHRegUse(u, HRmRead, i->RISCV64in.CAS_D.addr);
      addHRegUse(u, HRmRead, i->RISCV64in.CAS_D.expd);
      addHRegUse(u, HRmRead, i->RISCV64in.CAS_D.data);
      return;
   case RISCV64in_FENCE:
      return;
   case RISCV64in_CSEL:
      addHRegUse(u, HRmWrite, i->RISCV64in.CSEL.dst);
      addHRegUse(u, HRmRead, i->RISCV64in.CSEL.iftrue);
      addHRegUse(u, HRmRead, i->RISCV64in.CSEL.iffalse);
      addHRegUse(u, HRmRead, i->RISCV64in.CSEL.cond);
      return;
   case RISCV64in_Call:
      /* Logic and comments copied/modified from the arm64 backend. */
      /* First off, claim it trashes all the caller-saved registers which fall
         within the register allocator's jurisdiction. */
      addHRegUse(u, HRmWrite, hregRISCV64_x10());
      addHRegUse(u, HRmWrite, hregRISCV64_x11());
      addHRegUse(u, HRmWrite, hregRISCV64_x12());
      addHRegUse(u, HRmWrite, hregRISCV64_x13());
      addHRegUse(u, HRmWrite, hregRISCV64_x14());
      addHRegUse(u, HRmWrite, hregRISCV64_x15());
      addHRegUse(u, HRmWrite, hregRISCV64_x16());
      addHRegUse(u, HRmWrite, hregRISCV64_x17());
      addHRegUse(u, HRmWrite, hregRISCV64_f0());
      addHRegUse(u, HRmWrite, hregRISCV64_f1());
      addHRegUse(u, HRmWrite, hregRISCV64_f2());
      addHRegUse(u, HRmWrite, hregRISCV64_f3());
      addHRegUse(u, HRmWrite, hregRISCV64_f4());
      addHRegUse(u, HRmWrite, hregRISCV64_f5());
      addHRegUse(u, HRmWrite, hregRISCV64_f6());
      addHRegUse(u, HRmWrite, hregRISCV64_f7());
      addHRegUse(u, HRmWrite, hregRISCV64_f10());
      addHRegUse(u, HRmWrite, hregRISCV64_f11());
      addHRegUse(u, HRmWrite, hregRISCV64_f12());
      addHRegUse(u, HRmWrite, hregRISCV64_f13());
      addHRegUse(u, HRmWrite, hregRISCV64_f14());
      addHRegUse(u, HRmWrite, hregRISCV64_f15());
      addHRegUse(u, HRmWrite, hregRISCV64_f16());
      addHRegUse(u, HRmWrite, hregRISCV64_f17());
      addHRegUse(u, HRmWrite, hregRISCV64_f28());
      addHRegUse(u, HRmWrite, hregRISCV64_f29());
      addHRegUse(u, HRmWrite, hregRISCV64_f30());
      addHRegUse(u, HRmWrite, hregRISCV64_f31());
      /* Now we have to state any parameter-carrying registers which might be
         read. This depends on nArgRegs. */
      switch (i->RISCV64in.Call.nArgRegs) {
      case 8:
         addHRegUse(u, HRmRead, hregRISCV64_x17()); /*fallthru*/
      case 7:
         addHRegUse(u, HRmRead, hregRISCV64_x16()); /*fallthru*/
      case 6:
         addHRegUse(u, HRmRead, hregRISCV64_x15()); /*fallthru*/
      case 5:
         addHRegUse(u, HRmRead, hregRISCV64_x14()); /*fallthru*/
      case 4:
         addHRegUse(u, HRmRead, hregRISCV64_x13()); /*fallthru*/
      case 3:
         addHRegUse(u, HRmRead, hregRISCV64_x12()); /*fallthru*/
      case 2:
         addHRegUse(u, HRmRead, hregRISCV64_x11()); /*fallthru*/
      case 1:
         addHRegUse(u, HRmRead, hregRISCV64_x10());
         break;
      case 0:
         break;
      default:
         vpanic("getRegUsage_RISCV64Instr:Call:regparms");
      }
      /* Finally, add the condition register. */
      if (!hregIsInvalid(i->RISCV64in.Call.cond))
         addHRegUse(u, HRmRead, i->RISCV64in.Call.cond);
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
   case RISCV64in_EvCheck:
      addHRegUse(u, HRmRead, i->RISCV64in.EvCheck.base_amCounter);
      addHRegUse(u, HRmRead, i->RISCV64in.EvCheck.base_amFailAddr);
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
   case RISCV64in_ADDI:
      mapReg(m, &i->RISCV64in.ADDI.dst);
      mapReg(m, &i->RISCV64in.ADDI.src);
      return;
   case RISCV64in_ADDW:
      mapReg(m, &i->RISCV64in.ADDW.dst);
      mapReg(m, &i->RISCV64in.ADDW.src1);
      mapReg(m, &i->RISCV64in.ADDW.src2);
      return;
   case RISCV64in_ADDIW:
      mapReg(m, &i->RISCV64in.ADDIW.dst);
      mapReg(m, &i->RISCV64in.ADDIW.src);
      return;
   case RISCV64in_SUB:
      mapReg(m, &i->RISCV64in.SUB.dst);
      mapReg(m, &i->RISCV64in.SUB.src1);
      mapReg(m, &i->RISCV64in.SUB.src2);
      return;
   case RISCV64in_SUBW:
      mapReg(m, &i->RISCV64in.SUBW.dst);
      mapReg(m, &i->RISCV64in.SUBW.src1);
      mapReg(m, &i->RISCV64in.SUBW.src2);
      return;
   case RISCV64in_XOR:
      mapReg(m, &i->RISCV64in.XOR.dst);
      mapReg(m, &i->RISCV64in.XOR.src1);
      mapReg(m, &i->RISCV64in.XOR.src2);
      return;
   case RISCV64in_XORI:
      mapReg(m, &i->RISCV64in.XORI.dst);
      mapReg(m, &i->RISCV64in.XORI.src);
      return;
   case RISCV64in_OR:
      mapReg(m, &i->RISCV64in.OR.dst);
      mapReg(m, &i->RISCV64in.OR.src1);
      mapReg(m, &i->RISCV64in.OR.src2);
      return;
   case RISCV64in_AND:
      mapReg(m, &i->RISCV64in.AND.dst);
      mapReg(m, &i->RISCV64in.AND.src1);
      mapReg(m, &i->RISCV64in.AND.src2);
      return;
   case RISCV64in_SLL:
      mapReg(m, &i->RISCV64in.SLL.dst);
      mapReg(m, &i->RISCV64in.SLL.src1);
      mapReg(m, &i->RISCV64in.SLL.src2);
      return;
   case RISCV64in_SRL:
      mapReg(m, &i->RISCV64in.SRL.dst);
      mapReg(m, &i->RISCV64in.SRL.src1);
      mapReg(m, &i->RISCV64in.SRL.src2);
      return;
   case RISCV64in_SRA:
      mapReg(m, &i->RISCV64in.SRA.dst);
      mapReg(m, &i->RISCV64in.SRA.src1);
      mapReg(m, &i->RISCV64in.SRA.src2);
      return;
   case RISCV64in_SLLI:
      mapReg(m, &i->RISCV64in.SLLI.dst);
      mapReg(m, &i->RISCV64in.SLLI.src);
      return;
   case RISCV64in_SRLI:
      mapReg(m, &i->RISCV64in.SRLI.dst);
      mapReg(m, &i->RISCV64in.SRLI.src);
      return;
   case RISCV64in_SRAI:
      mapReg(m, &i->RISCV64in.SRAI.dst);
      mapReg(m, &i->RISCV64in.SRAI.src);
      return;
   case RISCV64in_SLLW:
      mapReg(m, &i->RISCV64in.SLLW.dst);
      mapReg(m, &i->RISCV64in.SLLW.src1);
      mapReg(m, &i->RISCV64in.SLLW.src2);
      return;
   case RISCV64in_SRLW:
      mapReg(m, &i->RISCV64in.SRLW.dst);
      mapReg(m, &i->RISCV64in.SRLW.src1);
      mapReg(m, &i->RISCV64in.SRLW.src2);
      return;
   case RISCV64in_SRAW:
      mapReg(m, &i->RISCV64in.SRAW.dst);
      mapReg(m, &i->RISCV64in.SRAW.src1);
      mapReg(m, &i->RISCV64in.SRAW.src2);
      return;
   case RISCV64in_SLT:
      mapReg(m, &i->RISCV64in.SLT.dst);
      mapReg(m, &i->RISCV64in.SLT.src1);
      mapReg(m, &i->RISCV64in.SLT.src2);
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
   case RISCV64in_MUL:
      mapReg(m, &i->RISCV64in.MUL.dst);
      mapReg(m, &i->RISCV64in.MUL.src1);
      mapReg(m, &i->RISCV64in.MUL.src2);
      return;
   case RISCV64in_MULH:
      mapReg(m, &i->RISCV64in.MULH.dst);
      mapReg(m, &i->RISCV64in.MULH.src1);
      mapReg(m, &i->RISCV64in.MULH.src2);
      return;
   case RISCV64in_MULHU:
      mapReg(m, &i->RISCV64in.MULHU.dst);
      mapReg(m, &i->RISCV64in.MULHU.src1);
      mapReg(m, &i->RISCV64in.MULHU.src2);
      return;
   case RISCV64in_DIV:
      mapReg(m, &i->RISCV64in.DIV.dst);
      mapReg(m, &i->RISCV64in.DIV.src1);
      mapReg(m, &i->RISCV64in.DIV.src2);
      return;
   case RISCV64in_DIVU:
      mapReg(m, &i->RISCV64in.DIVU.dst);
      mapReg(m, &i->RISCV64in.DIVU.src1);
      mapReg(m, &i->RISCV64in.DIVU.src2);
      return;
   case RISCV64in_REM:
      mapReg(m, &i->RISCV64in.REM.dst);
      mapReg(m, &i->RISCV64in.REM.src1);
      mapReg(m, &i->RISCV64in.REM.src2);
      return;
   case RISCV64in_REMU:
      mapReg(m, &i->RISCV64in.REMU.dst);
      mapReg(m, &i->RISCV64in.REMU.src1);
      mapReg(m, &i->RISCV64in.REMU.src2);
      return;
   case RISCV64in_MULW:
      mapReg(m, &i->RISCV64in.MULW.dst);
      mapReg(m, &i->RISCV64in.MULW.src1);
      mapReg(m, &i->RISCV64in.MULW.src2);
      return;
   case RISCV64in_DIVW:
      mapReg(m, &i->RISCV64in.DIVW.dst);
      mapReg(m, &i->RISCV64in.DIVW.src1);
      mapReg(m, &i->RISCV64in.DIVW.src2);
      return;
   case RISCV64in_DIVUW:
      mapReg(m, &i->RISCV64in.DIVUW.dst);
      mapReg(m, &i->RISCV64in.DIVUW.src1);
      mapReg(m, &i->RISCV64in.DIVUW.src2);
      return;
   case RISCV64in_REMW:
      mapReg(m, &i->RISCV64in.REMW.dst);
      mapReg(m, &i->RISCV64in.REMW.src1);
      mapReg(m, &i->RISCV64in.REMW.src2);
      return;
   case RISCV64in_REMUW:
      mapReg(m, &i->RISCV64in.REMUW.dst);
      mapReg(m, &i->RISCV64in.REMUW.src1);
      mapReg(m, &i->RISCV64in.REMUW.src2);
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
   case RISCV64in_LR_W:
      mapReg(m, &i->RISCV64in.LR_W.dst);
      mapReg(m, &i->RISCV64in.LR_W.addr);
      return;
   case RISCV64in_SC_W:
      mapReg(m, &i->RISCV64in.SC_W.res);
      mapReg(m, &i->RISCV64in.SC_W.src);
      mapReg(m, &i->RISCV64in.SC_W.addr);
      return;
   case RISCV64in_CAS_W:
      mapReg(m, &i->RISCV64in.CAS_W.old);
      mapReg(m, &i->RISCV64in.CAS_W.addr);
      mapReg(m, &i->RISCV64in.CAS_W.expd);
      mapReg(m, &i->RISCV64in.CAS_W.data);
      return;
   case RISCV64in_CAS_D:
      mapReg(m, &i->RISCV64in.CAS_D.old);
      mapReg(m, &i->RISCV64in.CAS_D.addr);
      mapReg(m, &i->RISCV64in.CAS_D.expd);
      mapReg(m, &i->RISCV64in.CAS_D.data);
      return;
   case RISCV64in_FENCE:
      return;
   case RISCV64in_CSEL:
      mapReg(m, &i->RISCV64in.CSEL.dst);
      mapReg(m, &i->RISCV64in.CSEL.iftrue);
      mapReg(m, &i->RISCV64in.CSEL.iffalse);
      mapReg(m, &i->RISCV64in.CSEL.cond);
      return;
   case RISCV64in_Call:
      if (!hregIsInvalid(i->RISCV64in.Call.cond))
         mapReg(m, &i->RISCV64in.Call.cond);
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
   case RISCV64in_EvCheck:
      mapReg(m, &i->RISCV64in.EvCheck.base_amCounter);
      mapReg(m, &i->RISCV64in.EvCheck.base_amFailAddr);
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
   case HRcInt64: {
      HReg base   = get_baseblock_register();
      Int  soff12 = offsetB - BASEBLOCK_OFFSET_ADJUSTMENT;
      vassert(soff12 >= -2048 && soff12 < 2048);
      *i1 = RISCV64Instr_SD(rreg, base, soff12);
      return;
   }
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
   case HRcInt64: {
      HReg base   = get_baseblock_register();
      Int  soff12 = offsetB - BASEBLOCK_OFFSET_ADJUSTMENT;
      vassert(soff12 >= -2048 && soff12 < 2048);
      *i1 = RISCV64Instr_LD(rreg, base, soff12);
      return;
   }
   default:
      ppHRegClass(rclass);
      vpanic("genReload_RISCV64: unimplemented regclass");
   }
}

RISCV64Instr* genMove_RISCV64(HReg from, HReg to, Bool mode64)
{
   vassert(mode64 == True);

   switch (hregClass(from)) {
   case HRcInt64:
      return RISCV64Instr_MV(to, from);
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

   UInt imm11_11 = (imm12_1 >> 10) & 0x1;
   UInt imm4_1   = (imm12_1 >> 0) & 0xf;
   UInt imm10_5  = (imm12_1 >> 4) & 0x3f;
   UInt imm12_12 = (imm12_1 >> 11) & 0x1;

   UInt the_insn = 0;

   the_insn |= opcode << 0;
   the_insn |= imm11_11 << 7;
   the_insn |= imm4_1 << 8;
   the_insn |= funct3 << 12;
   the_insn |= rs1 << 15;
   the_insn |= rs2 << 20;
   the_insn |= imm10_5 << 25;
   the_insn |= imm12_12 << 31;

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
   UInt imm5_5 = (imm5_0 >> 5) & 0x1;

   UShort the_insn = 0;

   the_insn |= opcode << 0;
   the_insn |= imm4_0 << 2;
   the_insn |= rd << 7;
   the_insn |= imm5_5 << 12;
   the_insn |= funct3 << 13;

   return emit16(p, the_insn);
}

/* Emit a CJ-type instruction. */
static UChar* emit_CJ(UChar* p, UInt opcode, UInt imm11_1, UInt funct3)
{
   vassert(opcode >> 2 == 0);
   vassert(imm11_1 >> 11 == 0);
   vassert(funct3 >> 3 == 0);

   UInt imm5_5   = (imm11_1 >> 4) & 0x1;
   UInt imm3_1   = (imm11_1 >> 0) & 0x7;
   UInt imm7_7   = (imm11_1 >> 6) & 0x1;
   UInt imm6_6   = (imm11_1 >> 5) & 0x1;
   UInt imm10_10 = (imm11_1 >> 9) & 0x1;
   UInt imm9_8   = (imm11_1 >> 7) & 0x3;
   UInt imm4_4   = (imm11_1 >> 3) & 0x1;
   UInt imm11_11 = (imm11_1 >> 10) & 0x1;

   UShort the_insn = 0;

   the_insn |= opcode << 0;
   the_insn |= imm5_5 << 2;
   the_insn |= imm3_1 << 3;
   the_insn |= imm7_7 << 6;
   the_insn |= imm6_6 << 7;
   the_insn |= imm10_10 << 8;
   the_insn |= imm9_8 << 9;
   the_insn |= imm4_4 << 11;
   the_insn |= imm11_11 << 12;
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

   /* Handle a constant that is out of the 32-bit signed integer range. */
   /* Strip the low 12 bits. */
   ULong imm11_0 = imm64 & 0xfff;

   /* Get the remaining adjusted upper bits. */
   ULong rem   = (simm64 + 0x800) >> 12;
   UInt  sham6 = 12 + __builtin_ctzll(rem);
   vassert(sham6 < 64);
   rem = vex_sx_to_64(rem >> (sham6 - 12), 64 - sham6);

   /* Generate instructions to load the upper bits. */
   p = imm64_to_ireg(p, dst, rem);
   /* c.slli dst, sham6 */
   p = emit_CI(p, 0b10, sham6, dst, 0b000);

   /* Add the low bits in. */
   if (imm11_0 == 0)
      return p;
   UInt imm5_0 = imm11_0 & 0x3f;
   if (vex_sx_to_64(imm5_0, 6) == vex_sx_to_64(imm11_0, 12)) {
      /* c.addi dst, imm5_0 */
      p = emit_CI(p, 0b01, imm5_0, dst, 0b000);
   } else {
      /* addi dst, dst, imm11_0 */
      p = emit_I(p, 0b0010011, dst, 0b000, dst, imm11_0);
   }

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
   rem      = (rem + 0x8) >> 4;
   imm15_4  = rem & 0xfff;
   rem      = (rem + 0x800) >> 12;
   imm27_16 = rem & 0xfff;
   rem      = (rem + 0x800) >> 12;
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
      p = emit_CI(p, 0b01, vex_sx_to_64(imm3_0, 4) & 0x3f, dst, 0b000);
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
   case RISCV64in_ADDI: {
      /* addi dst, src, simm12 */
      UInt dst    = iregEnc(i->RISCV64in.ADDI.dst);
      UInt src    = iregEnc(i->RISCV64in.ADDI.src);
      Int  simm12 = i->RISCV64in.ADDI.simm12;
      vassert(simm12 >= -2048 && simm12 < 2048);
      UInt imm11_0 = simm12 & 0xfff;

      p = emit_I(p, 0b0010011, dst, 0b000, src, imm11_0);
      goto done;
   }
   case RISCV64in_ADDW: {
      /* addw dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.ADDW.dst);
      UInt src1 = iregEnc(i->RISCV64in.ADDW.src1);
      UInt src2 = iregEnc(i->RISCV64in.ADDW.src2);

      p = emit_R(p, 0b0111011, dst, 0b000, src1, src2, 0b0000000);
      goto done;
   }
   case RISCV64in_ADDIW: {
      /* addiw dst, src, simm12 */
      UInt dst    = iregEnc(i->RISCV64in.ADDIW.dst);
      UInt src    = iregEnc(i->RISCV64in.ADDIW.src);
      Int  simm12 = i->RISCV64in.ADDIW.simm12;
      vassert(simm12 >= -2048 && simm12 < 2048);
      UInt imm11_0 = simm12 & 0xfff;

      p = emit_I(p, 0b0011011, dst, 0b000, src, imm11_0);
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
   case RISCV64in_SUBW: {
      /* subw dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.SUBW.dst);
      UInt src1 = iregEnc(i->RISCV64in.SUBW.src1);
      UInt src2 = iregEnc(i->RISCV64in.SUBW.src2);

      p = emit_R(p, 0b0111011, dst, 0b000, src1, src2, 0b0100000);
      goto done;
   }
   case RISCV64in_XOR: {
      /* xor dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.XOR.dst);
      UInt src1 = iregEnc(i->RISCV64in.XOR.src1);
      UInt src2 = iregEnc(i->RISCV64in.XOR.src2);

      p = emit_R(p, 0b0110011, dst, 0b100, src1, src2, 0b0000000);
      goto done;
   }
   case RISCV64in_XORI: {
      /* xori dst, src, simm12 */
      UInt dst    = iregEnc(i->RISCV64in.XORI.dst);
      UInt src    = iregEnc(i->RISCV64in.XORI.src);
      Int  simm12 = i->RISCV64in.XORI.simm12;
      vassert(simm12 >= -2048 && simm12 < 2048);
      UInt imm11_0 = simm12 & 0xfff;

      p = emit_I(p, 0b0010011, dst, 0b100, src, imm11_0);
      goto done;
   }
   case RISCV64in_OR: {
      /* or dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.OR.dst);
      UInt src1 = iregEnc(i->RISCV64in.OR.src1);
      UInt src2 = iregEnc(i->RISCV64in.OR.src2);

      p = emit_R(p, 0b0110011, dst, 0b110, src1, src2, 0b0000000);
      goto done;
   }
   case RISCV64in_AND: {
      /* and dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.AND.dst);
      UInt src1 = iregEnc(i->RISCV64in.AND.src1);
      UInt src2 = iregEnc(i->RISCV64in.AND.src2);

      p = emit_R(p, 0b0110011, dst, 0b111, src1, src2, 0b0000000);
      goto done;
   }
   case RISCV64in_SLL: {
      /* sll dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.SLL.dst);
      UInt src1 = iregEnc(i->RISCV64in.SLL.src1);
      UInt src2 = iregEnc(i->RISCV64in.SLL.src2);

      p = emit_R(p, 0b0110011, dst, 0b001, src1, src2, 0b0000000);
      goto done;
   }
   case RISCV64in_SRL: {
      /* srl dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.SRL.dst);
      UInt src1 = iregEnc(i->RISCV64in.SRL.src1);
      UInt src2 = iregEnc(i->RISCV64in.SRL.src2);

      p = emit_R(p, 0b0110011, dst, 0b101, src1, src2, 0b0000000);
      goto done;
   }
   case RISCV64in_SRA: {
      /* sra dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.SRA.dst);
      UInt src1 = iregEnc(i->RISCV64in.SRA.src1);
      UInt src2 = iregEnc(i->RISCV64in.SRA.src2);

      p = emit_R(p, 0b0110011, dst, 0b101, src1, src2, 0b0100000);
      goto done;
   }
   case RISCV64in_SLLI: {
      /* slli dst, src, uimm6 */
      UInt dst   = iregEnc(i->RISCV64in.SLLI.dst);
      UInt src   = iregEnc(i->RISCV64in.SLLI.src);
      UInt uimm6 = i->RISCV64in.SLLI.uimm6;
      vassert(uimm6 < 64);
      UInt uimm4_0 = uimm6 & 0x1f;
      UInt uimm5_5 = (uimm6 >> 5) & 0x1;

      p = emit_R(p, 0b0010011, dst, 0b001, src, uimm4_0,
                 0b000000 << 1 | uimm5_5);
      goto done;
   }
   case RISCV64in_SRLI: {
      /* srli dst, src, uimm6 */
      UInt dst   = iregEnc(i->RISCV64in.SRLI.dst);
      UInt src   = iregEnc(i->RISCV64in.SRLI.src);
      UInt uimm6 = i->RISCV64in.SRLI.uimm6;
      vassert(uimm6 < 64);
      UInt uimm4_0 = uimm6 & 0x1f;
      UInt uimm5_5 = (uimm6 >> 5) & 0x1;

      p = emit_R(p, 0b0010011, dst, 0b101, src, uimm4_0,
                 0b000000 << 1 | uimm5_5);
      goto done;
   }
   case RISCV64in_SRAI: {
      /* srai dst, src, uimm6 */
      UInt dst   = iregEnc(i->RISCV64in.SRAI.dst);
      UInt src   = iregEnc(i->RISCV64in.SRAI.src);
      UInt uimm6 = i->RISCV64in.SRAI.uimm6;
      vassert(uimm6 < 64);
      UInt uimm4_0 = uimm6 & 0x1f;
      UInt uimm5_5 = (uimm6 >> 5) & 0x1;

      p = emit_R(p, 0b0010011, dst, 0b101, src, uimm4_0,
                 0b010000 << 1 | uimm5_5);
      goto done;
   }
   case RISCV64in_SLLW: {
      /* sllw dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.SLLW.dst);
      UInt src1 = iregEnc(i->RISCV64in.SLLW.src1);
      UInt src2 = iregEnc(i->RISCV64in.SLLW.src2);

      p = emit_R(p, 0b0111011, dst, 0b001, src1, src2, 0b0000000);
      goto done;
   }
   case RISCV64in_SRLW: {
      /* srlw dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.SRLW.dst);
      UInt src1 = iregEnc(i->RISCV64in.SRLW.src1);
      UInt src2 = iregEnc(i->RISCV64in.SRLW.src2);

      p = emit_R(p, 0b0111011, dst, 0b101, src1, src2, 0b0000000);
      goto done;
   }
   case RISCV64in_SRAW: {
      /* sraw dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.SRAW.dst);
      UInt src1 = iregEnc(i->RISCV64in.SRAW.src1);
      UInt src2 = iregEnc(i->RISCV64in.SRAW.src2);

      p = emit_R(p, 0b0111011, dst, 0b101, src1, src2, 0b0100000);
      goto done;
   }
   case RISCV64in_SLT: {
      /* slt dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.SLT.dst);
      UInt src1 = iregEnc(i->RISCV64in.SLT.src1);
      UInt src2 = iregEnc(i->RISCV64in.SLT.src2);

      p = emit_R(p, 0b0110011, dst, 0b010, src1, src2, 0b0000000);
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
   case RISCV64in_MUL: {
      /* mul dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.MUL.dst);
      UInt src1 = iregEnc(i->RISCV64in.MUL.src1);
      UInt src2 = iregEnc(i->RISCV64in.MUL.src2);

      p = emit_R(p, 0b0110011, dst, 0b000, src1, src2, 0b0000001);
      goto done;
   }
   case RISCV64in_MULH: {
      /* mulh dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.MULH.dst);
      UInt src1 = iregEnc(i->RISCV64in.MULH.src1);
      UInt src2 = iregEnc(i->RISCV64in.MULH.src2);

      p = emit_R(p, 0b0110011, dst, 0b001, src1, src2, 0b0000001);
      goto done;
   }
   case RISCV64in_MULHU: {
      /* mulhu dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.MULHU.dst);
      UInt src1 = iregEnc(i->RISCV64in.MULHU.src1);
      UInt src2 = iregEnc(i->RISCV64in.MULHU.src2);

      p = emit_R(p, 0b0110011, dst, 0b011, src1, src2, 0b0000001);
      goto done;
   }
   case RISCV64in_DIV: {
      /* div dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.DIV.dst);
      UInt src1 = iregEnc(i->RISCV64in.DIV.src1);
      UInt src2 = iregEnc(i->RISCV64in.DIV.src2);

      p = emit_R(p, 0b0110011, dst, 0b100, src1, src2, 0b0000001);
      goto done;
   }
   case RISCV64in_DIVU: {
      /* divu dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.DIVU.dst);
      UInt src1 = iregEnc(i->RISCV64in.DIVU.src1);
      UInt src2 = iregEnc(i->RISCV64in.DIVU.src2);

      p = emit_R(p, 0b0110011, dst, 0b101, src1, src2, 0b0000001);
      goto done;
   }
   case RISCV64in_REM: {
      /* rem dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.REM.dst);
      UInt src1 = iregEnc(i->RISCV64in.REM.src1);
      UInt src2 = iregEnc(i->RISCV64in.REM.src2);

      p = emit_R(p, 0b0110011, dst, 0b110, src1, src2, 0b0000001);
      goto done;
   }
   case RISCV64in_REMU: {
      /* remu dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.REMU.dst);
      UInt src1 = iregEnc(i->RISCV64in.REMU.src1);
      UInt src2 = iregEnc(i->RISCV64in.REMU.src2);

      p = emit_R(p, 0b0110011, dst, 0b111, src1, src2, 0b0000001);
      goto done;
   }
   case RISCV64in_MULW: {
      /* mulw dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.MULW.dst);
      UInt src1 = iregEnc(i->RISCV64in.MULW.src1);
      UInt src2 = iregEnc(i->RISCV64in.MULW.src2);

      p = emit_R(p, 0b0111011, dst, 0b000, src1, src2, 0b0000001);
      goto done;
   }
   case RISCV64in_DIVW: {
      /* divw dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.DIVW.dst);
      UInt src1 = iregEnc(i->RISCV64in.DIVW.src1);
      UInt src2 = iregEnc(i->RISCV64in.DIVW.src2);

      p = emit_R(p, 0b0111011, dst, 0b100, src1, src2, 0b0000001);
      goto done;
   }
   case RISCV64in_DIVUW: {
      /* divuw dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.DIVUW.dst);
      UInt src1 = iregEnc(i->RISCV64in.DIVUW.src1);
      UInt src2 = iregEnc(i->RISCV64in.DIVUW.src2);

      p = emit_R(p, 0b0111011, dst, 0b101, src1, src2, 0b0000001);
      goto done;
   }
   case RISCV64in_REMW: {
      /* remw dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.REMW.dst);
      UInt src1 = iregEnc(i->RISCV64in.REMW.src1);
      UInt src2 = iregEnc(i->RISCV64in.REMW.src2);

      p = emit_R(p, 0b0111011, dst, 0b110, src1, src2, 0b0000001);
      goto done;
   }
   case RISCV64in_REMUW: {
      /* remuw dst, src1, src2 */
      UInt dst  = iregEnc(i->RISCV64in.REMUW.dst);
      UInt src1 = iregEnc(i->RISCV64in.REMUW.src1);
      UInt src2 = iregEnc(i->RISCV64in.REMUW.src2);

      p = emit_R(p, 0b0111011, dst, 0b111, src1, src2, 0b0000001);
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
   case RISCV64in_LR_W: {
      /* lr.w dst, (addr) */
      UInt dst  = iregEnc(i->RISCV64in.LR_W.dst);
      UInt addr = iregEnc(i->RISCV64in.LR_W.addr);

      p = emit_R(p, 0b0101111, dst, 0b010, addr, 0b00000, 0b0001000);
      goto done;
   }
   case RISCV64in_SC_W: {
      /* sc.w res, dst, (addr) */
      UInt res  = iregEnc(i->RISCV64in.SC_W.res);
      UInt src  = iregEnc(i->RISCV64in.SC_W.src);
      UInt addr = iregEnc(i->RISCV64in.SC_W.addr);

      p = emit_R(p, 0b0101111, res, 0b010, addr, src, 0b0001100);
      goto done;
   }
   case RISCV64in_CAS_W: {
      /* 1: lr.w old, (addr)
            bne old, expd, 2f
            sc.w t0, data, (addr)
            bne t0, zero, 1b
         2:
       */
      UInt old  = iregEnc(i->RISCV64in.CAS_W.old);
      UInt addr = iregEnc(i->RISCV64in.CAS_W.addr);
      UInt expd = iregEnc(i->RISCV64in.CAS_W.expd);
      UInt data = iregEnc(i->RISCV64in.CAS_W.data);

      p = emit_R(p, 0b0101111, old, 0b010, addr, 0b00000, 0b0001000);
      p = emit_B(p, 0b1100011, (12 >> 1) & 0xfff, 0b001, old, expd);
      p = emit_R(p, 0b0101111, 5 /*x5/t0*/, 0b010, addr, data, 0b0001100);
      p = emit_B(p, 0b1100011, (-12 >> 1) & 0xfff, 0b001, 5 /*x5/t0*/,
                 0 /*x0/zero*/);
      goto done;
   }
   case RISCV64in_CAS_D: {
      /* 1: lr.d old, (addr)
            bne old, expd, 2f
            sc.d t0, data, (addr)
            bne t0, zero, 1b
         2:
       */
      UInt old  = iregEnc(i->RISCV64in.CAS_D.old);
      UInt addr = iregEnc(i->RISCV64in.CAS_D.addr);
      UInt expd = iregEnc(i->RISCV64in.CAS_D.expd);
      UInt data = iregEnc(i->RISCV64in.CAS_D.data);

      p = emit_R(p, 0b0101111, old, 0b011, addr, 0b00000, 0b0001000);
      p = emit_B(p, 0b1100011, (12 >> 1) & 0xfff, 0b001, old, expd);
      p = emit_R(p, 0b0101111, 5 /*x5/t0*/, 0b011, addr, data, 0b0001100);
      p = emit_B(p, 0b1100011, (-12 >> 1) & 0xfff, 0b001, 5 /*x5/t0*/,
                 0 /*x0/zero*/);
      goto done;
   }
   case RISCV64in_FENCE: {
      /* fence */
      p = emit_I(p, 0b0001111, 0b00000, 0b000, 0b00000, 0b000011111111);
      goto done;
   }
   case RISCV64in_CSEL: {
      /*    beq cond, zero, 1f
            c.mv dst, iftrue
            c.j 2f
         1: c.mv dst, iffalse
         2:
       */
      UInt dst     = iregEnc(i->RISCV64in.CSEL.dst);
      UInt iftrue  = iregEnc(i->RISCV64in.CSEL.iftrue);
      UInt iffalse = iregEnc(i->RISCV64in.CSEL.iffalse);
      UInt cond    = iregEnc(i->RISCV64in.CSEL.cond);

      p = emit_B(p, 0b1100011, (8 >> 1) & 0xfff, 0b000, cond, 0 /*x0/zero*/);
      p = emit_CR(p, 0b10, iftrue, dst, 0b1000);
      p = emit_CJ(p, 0b01, (4 >> 1) & 0x7ff, 0b101);
      p = emit_CR(p, 0b10, iffalse, dst, 0b1000);
      goto done;
   }
   case RISCV64in_Call: {
      /*    beq cond, zero, 1f
            li t0, target
            c.jalr 0(t0)
         1:
       */
      UChar* ptmp = NULL;
      if (!hregIsInvalid(i->RISCV64in.Call.cond)) {
         ptmp = p;
         p += 4;
      }

      /* li t0, target */
      p = imm64_to_ireg(p, 5 /*x5/t0*/, i->RISCV64in.Call.target);

      /* c.jalr 0(t0) */
      p = emit_CR(p, 0b10, 0 /*x0/zero*/, 5 /*x5/t0*/, 0b1001);

      /* Fix up the conditional jump, if there was one. */
      if (!hregIsInvalid(i->RISCV64in.Call.cond)) {
         /* beq cond, zero, delta */
         UInt cond  = iregEnc(i->RISCV64in.Call.cond);
         UInt delta = p - ptmp;
         /* delta_min = 4 (beq) + 2 (c.li) + 2 (c.jalr) = 8 */
         vassert(delta >= 8 && delta < 4096 && (delta & 1) == 0);
         UInt imm12_1 = (delta >> 1) & 0xfff;

         emit_B(ptmp, 0b1100011, imm12_1, 0b000, cond, 0 /*x0/zero*/);
      }

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

      p = addr48_to_ireg_EXACTLY_18B(p, 5 /*x5/t0*/, (ULong)disp_cp_chain_me);

      /* c.jalr 0(t0) */
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
         UInt imm12_1 = (delta >> 1) & 0xfff;

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
      p = imm64_to_ireg(p, 5 /*x5/t0*/, (ULong)disp_cp_xindir);

      /* c.jr 0(t0) */
      p = emit_CR(p, 0b10, 0 /*x0/zero*/, 5 /*x5/t0*/, 0b1000);

      /* Fix up the conditional jump, if there was one. */
      if (!hregIsInvalid(i->RISCV64in.XIndir.cond)) {
         /* beq cond, zero, delta */
         UInt cond  = iregEnc(i->RISCV64in.XIndir.cond);
         UInt delta = p - ptmp;
         /* delta_min = 4 (beq) + 4 (sd) + 2 (c.li) + 2 (c.jr) = 12 */
         vassert(delta >= 12 && delta < 4096 && (delta & 1) == 0);
         UInt imm12_1 = (delta >> 1) & 0xfff;

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
      p = imm64_to_ireg(p, 5 /*x5/t0*/, (ULong)disp_cp_xassisted);

      /* c.jr 0(t0) */
      p = emit_CR(p, 0b10, 0 /*x0/zero*/, 5 /*x5/t0*/, 0b1000);

      /* Fix up the conditional jump, if there was one. */
      if (!hregIsInvalid(i->RISCV64in.XAssisted.cond)) {
         /* beq cond, zero, delta */
         UInt cond  = iregEnc(i->RISCV64in.XAssisted.cond);
         UInt delta = p - ptmp;
         /* delta_min = 4 (beq) + 4 (sd) + 2 (c.li) + 2 (c.li) + 2 (c.jr)
                      = 14 */
         vassert(delta >= 14 && delta < 4096 && (delta & 1) == 0);
         UInt imm12_1 = (delta >> 1) & 0xfff;

         emit_B(ptmp, 0b1100011, imm12_1, 0b000, cond, 0 /*x0/zero*/);
      }

      goto done;
   }

   case RISCV64in_EvCheck: {
      /*    lw t0, soff12_amCounter(base_amCounter)
            c.addiw t0, -1
            sw t0, soff12_amCounter(base_amCounter)
            bge t0, zero, 1f
            ld t0, soff12_amFailAddr(base_amFailAddr)
            c.jr 0(t0)
         1:
      */
      UInt base_amCounter = iregEnc(i->RISCV64in.EvCheck.base_amCounter);
      Int  soff12_amCounter = i->RISCV64in.EvCheck.soff12_amCounter;
      vassert(soff12_amCounter >= -2048 && soff12_amCounter < 2048);
      UInt imm11_0_amCounter = soff12_amCounter & 0xfff;

      UInt base_amFailAddr = iregEnc(i->RISCV64in.EvCheck.base_amFailAddr);
      Int  soff12_amFailAddr = i->RISCV64in.EvCheck.soff12_amFailAddr;
      vassert(soff12_amFailAddr >= -2048 && soff12_amFailAddr < 2048);
      UInt imm11_0_amFailAddr = soff12_amFailAddr & 0xfff;

      p = emit_I(p, 0b0000011, 5 /*x5/t0*/, 0b010, base_amCounter,
                 imm11_0_amCounter);
      p = emit_CI(p, 0b01, -1 & 0x3f, 5 /*x5/t0*/, 0b001);
      p = emit_S(p, 0b0100011, imm11_0_amCounter, 0b010, base_amCounter,
                 5 /*x5/t0*/);
      p = emit_B(p, 0b1100011, (10 >> 1) & 0xfff, 0b101, 5 /*x5/t0*/,
                 0 /*x0/zero*/);
      p = emit_I(p, 0b0000011, 5 /*x5/t0*/, 0b011, base_amFailAddr,
                 imm11_0_amFailAddr);
      p = emit_CR(p, 0b10, 0 /*x0/zero*/, 5 /*x5/t0*/, 0b1000);

      /* Crosscheck. */
      vassert(evCheckSzB_RISCV64() == p - buf);
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

/* Return the number of bytes emitted for an RISCV64in_EvCheck, as produced by
   emit_RISCV64Instr(). */
Int evCheckSzB_RISCV64(void) { return 20; }

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
        c.jalr 0(t0)
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
        c.jr 0(t0)
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

   /* What we're expecting to see is:
        lui t0, place_to_jump_to_EXPECTED[47:28]'
        addiw t0, t0, place_to_jump_to_EXPECTED[27:16]'
        c.slli t0, 12
        addi t0, t0, place_to_jump_to_EXPECTED[15:4]'
        c.slli t0, 4
        c.addi t0, place_to_jump_to_EXPECTED[3:0]'
        c.jr 0(t0)
      viz
        <18 bytes generated by addr48_to_ireg_EXACTLY_18B>
        82 82
   */
   UChar* p = place_to_unchain;
   vassert(((HWord)p & 1) == 0);
   vassert(is_addr48_to_ireg_EXACTLY_18B(p, 5 /*x5/t0*/,
                                         (ULong)place_to_jump_to_EXPECTED));
   vassert(p[18] == 0x82 && p[19] == 0x82);

   /* And what we want to change it to is:
        lui t0, disp_cp_chain_me[47:28]'
        addiw t0, t0, disp_cp_chain_me[27:16]'
        c.slli t0, 12
        addi t0, t0, disp_cp_chain_me[15:4]'
        c.slli t0, 4
        c.addi t0, disp_cp_chain_me[3:0]'
        c.jalr 0(t0)
      viz
        <18 bytes generated by addr48_to_ireg_EXACTLY_18B>
        82 92

      The replacement has the same length as the original.
   */
   (void)addr48_to_ireg_EXACTLY_18B(p, 5 /*x5/t0*/, (ULong)disp_cp_chain_me);
   p[18] = 0x82;
   p[19] = 0x89;

   VexInvalRange vir = {(HWord)p, 20};
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
