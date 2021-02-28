
/*--------------------------------------------------------------------*/
/*--- begin                                    host_riscv64_isel.c ---*/
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
#include "main_globals.h"
#include "main_util.h"

/*------------------------------------------------------------*/
/*--- ISelEnv                                              ---*/
/*------------------------------------------------------------*/

/* This carries around:

   - A mapping from IRTemp to IRType, giving the type of any IRTemp we might
     encounter. This is computed before insn selection starts, and does not
     change.

   - A mapping from IRTemp to HReg. This tells the insn selector which virtual
     register is associated with each IRTemp temporary. This is computed before
     insn selection starts, and does not change. We expect this mapping to map
     precisely the same set of IRTemps as the type mapping does.

     - vregmap   holds the primary register for the IRTemp.
     - vregmapHI is only used for 128-bit integer-typed IRTemps. It holds the
                 identity of a second 64-bit virtual HReg, which holds the high
                 half of the value.

   - The code array, that is, the insns selected so far.

   - A counter, for generating new virtual registers.

   - The host hardware capabilities word. This is set at the start and does not
     change.

   - A Bool for indicating whether we may generate chain-me instructions for
     control flow transfers, or whether we must use XAssisted.

   - The maximum guest address of any guest insn in this block. Actually, the
     address of the highest-addressed byte from any insn in this block. Is set
     at the start and does not change. This is used for detecting jumps which
     are definitely forward-edges from this block, and therefore can be made
     (chained) to the fast entry point of the destination, thereby avoiding the
     destination's event check.

   - An IRExpr*, which may be NULL, holding the IR expression (an
     IRRoundingMode-encoded value) to which the FPU's rounding mode was most
     recently set. Setting to NULL is always safe. Used to avoid redundant
     settings of the FPU's rounding mode, as described in
     set_FPCR_rounding_mode() below.

   Note, this is all (well, mostly) host-independent.
*/

typedef struct {
   /* Constant -- are set at the start and do not change. */
   IRTypeEnv* type_env;

   HReg* vregmap;
   HReg* vregmapHI;
   Int   n_vregmap;

   UInt hwcaps;

   Bool   chainingAllowed;
   Addr64 max_ga;

   /* These are modified as we go along. */
   HInstrArray* code;
   Int          vreg_ctr;

   IRExpr* previous_rm;
} ISelEnv;

static HReg lookupIRTemp(ISelEnv* env, IRTemp tmp)
{
   vassert(tmp >= 0);
   vassert(tmp < env->n_vregmap);
   return env->vregmap[tmp];
}

static void addInstr(ISelEnv* env, RISCV64Instr* instr)
{
   addHInstr(env->code, instr);
   if (vex_traceflags & VEX_TRACE_VCODE) {
      ppRISCV64Instr(instr, True /*mode64*/);
      vex_printf("\n");
   }
}

static HReg newVRegI(ISelEnv* env)
{
   HReg reg = mkHReg(True /*virtual*/, HRcInt64, 0, env->vreg_ctr);
   env->vreg_ctr++;
   return reg;
}

/*------------------------------------------------------------*/
/*--- ISEL: Forward declarations                           ---*/
/*------------------------------------------------------------*/

/* These are organised as iselXXX and iselXXX_wrk pairs. The iselXXX_wrk do the
   real work, but are not to be called directly. For each XXX, iselXXX calls its
   iselXXX_wrk counterpart, then checks that all returned registers are virtual.
   You should not call the _wrk version directly. */

static HReg iselIntExpr_R(ISelEnv* env, IRExpr* e);

/*------------------------------------------------------------*/
/*--- ISEL: Misc helpers                                   ---*/
/*------------------------------------------------------------*/

static HReg get_baseblock_register(void) { return hregRISCV64_x8(); }
#define BASEBLOCK_OFFSET_ADJUSTMENT 2048

/*------------------------------------------------------------*/
/*--- ISEL: Integer expressions (64/32 bit)                ---*/
/*------------------------------------------------------------*/

/* Select insns for an integer-typed expression, and add them to the code list.
   Return a reg holding the result. This reg will be a virtual register. THE
   RETURNED REG MUST NOT BE MODIFIED. If you want to modify it, ask for a new
   vreg, copy it in there, and modify the copy. The register allocator will do
   its best to map both vregs to the same real register, so the copies will
   often disappear later in the game.

   This should handle expressions of 64- and 32-bit type. All results are
   returned in a 64-bit register. For 32-bit expressions, the upper 32 bits are
   arbitrary, so you should mask or sign extend partial values if necessary.
*/

/* -------------------------- Reg --------------------------- */

/* DO NOT CALL THIS DIRECTLY ! */
static HReg iselIntExpr_R_wrk(ISelEnv* env, IRExpr* e)
{
   IRType ty = typeOfIRExpr(env->type_env, e);
   vassert(ty == Ity_I64 || ty == Ity_I32 || ty == Ity_I16 || ty == Ity_I8 ||
           ty == Ity_I1);

   switch (e->tag) {
   /* ------------------------ TEMP ------------------------- */
   case Iex_RdTmp: {
      return lookupIRTemp(env, e->Iex.RdTmp.tmp);
   }

   /* ------------------------ LOAD ------------------------- */
   case Iex_Load: {
      if (e->Iex.Load.end != Iend_LE)
         goto irreducible;

      HReg dst = newVRegI(env);
      /* TODO Optimize the cases with small imm Add64/Sub64. */
      HReg addr = iselIntExpr_R(env, e->Iex.Load.addr);

      if (ty == Ity_I64)
         addInstr(env, RISCV64Instr_LD(dst, addr, 0));
      else if (ty == Ity_I32)
         addInstr(env, RISCV64Instr_LW(dst, addr, 0));
      else if (ty == Ity_I16)
         addInstr(env, RISCV64Instr_LH(dst, addr, 0));
      else if (ty == Ity_I8)
         addInstr(env, RISCV64Instr_LB(dst, addr, 0));
      else
         goto irreducible;
      return dst;
   }

   /* ---------------------- BINARY OP ---------------------- */
   case Iex_Binop: {
      switch (e->Iex.Binop.op) {
      case Iop_Add64: {
         HReg dst  = newVRegI(env);
         HReg argL = iselIntExpr_R(env, e->Iex.Binop.arg1);
         /* TODO Optimize for small imms by generating addi. */
         HReg argR = iselIntExpr_R(env, e->Iex.Binop.arg2);
         addInstr(env, RISCV64Instr_ADD(dst, argL, argR));
         return dst;
      }
      case Iop_Sub64: {
         HReg dst  = newVRegI(env);
         HReg argL = iselIntExpr_R(env, e->Iex.Binop.arg1);
         /* TODO Optimize for small imms by generating addi. */
         HReg argR = iselIntExpr_R(env, e->Iex.Binop.arg2);
         addInstr(env, RISCV64Instr_SUB(dst, argL, argR));
         return dst;
      }
      case Iop_Or64: {
         HReg dst  = newVRegI(env);
         HReg argL = iselIntExpr_R(env, e->Iex.Binop.arg1);
         HReg argR = iselIntExpr_R(env, e->Iex.Binop.arg2);
         addInstr(env, RISCV64Instr_OR(dst, argL, argR));
         return dst;
      }
      case Iop_And64: {
         HReg dst  = newVRegI(env);
         HReg argL = iselIntExpr_R(env, e->Iex.Binop.arg1);
         HReg argR = iselIntExpr_R(env, e->Iex.Binop.arg2);
         addInstr(env, RISCV64Instr_AND(dst, argL, argR));
         return dst;
      }
      case Iop_Shl64: {
         HReg dst  = newVRegI(env);
         HReg argL = iselIntExpr_R(env, e->Iex.Binop.arg1);
         /* TODO Optimize for small imms by generating slli. */
         HReg argR = iselIntExpr_R(env, e->Iex.Binop.arg2);
         addInstr(env, RISCV64Instr_SLL(dst, argL, argR));
         return dst;
      }
      case Iop_Shl32: {
         HReg dst  = newVRegI(env);
         HReg argL = iselIntExpr_R(env, e->Iex.Binop.arg1);
         HReg argR = iselIntExpr_R(env, e->Iex.Binop.arg2);
         addInstr(env, RISCV64Instr_SLLW(dst, argL, argR));
         return dst;
      }
      case Iop_Shr64: {
         HReg dst  = newVRegI(env);
         HReg argL = iselIntExpr_R(env, e->Iex.Binop.arg1);
         HReg argR = iselIntExpr_R(env, e->Iex.Binop.arg2);
         addInstr(env, RISCV64Instr_SRL(dst, argL, argR));
         return dst;
      }
      case Iop_Sar32: {
         HReg dst  = newVRegI(env);
         HReg argL = iselIntExpr_R(env, e->Iex.Binop.arg1);
         HReg argR = iselIntExpr_R(env, e->Iex.Binop.arg2);
         addInstr(env, RISCV64Instr_SRAW(dst, argL, argR));
         return dst;
      }
      case Iop_CmpEQ64: {
         HReg tmp  = newVRegI(env);
         HReg argL = iselIntExpr_R(env, e->Iex.Binop.arg1);
         HReg argR = iselIntExpr_R(env, e->Iex.Binop.arg2);
         addInstr(env, RISCV64Instr_SUB(tmp, argL, argR));
         HReg dst = newVRegI(env);
         addInstr(env, RISCV64Instr_SLTIU(dst, tmp, 1));
         return dst;
      }
      case Iop_CmpNE64: {
         HReg tmp  = newVRegI(env);
         HReg argL = iselIntExpr_R(env, e->Iex.Binop.arg1);
         HReg argR = iselIntExpr_R(env, e->Iex.Binop.arg2);
         addInstr(env, RISCV64Instr_SUB(tmp, argL, argR));
         HReg dst = newVRegI(env);
         addInstr(env, RISCV64Instr_SLTU(dst, hregRISCV64_x0(), tmp));
         return dst;
      }
      case Iop_CmpLT64U: {
         HReg dst  = newVRegI(env);
         HReg argL = iselIntExpr_R(env, e->Iex.Binop.arg1);
         HReg argR = iselIntExpr_R(env, e->Iex.Binop.arg2);
         addInstr(env, RISCV64Instr_SLTU(dst, argL, argR));
         return dst;
      }
      default:
         break;
      }

      break;
   }

   /* ---------------------- UNARY OP ----------------------- */
   case Iex_Unop: {
      switch (e->Iex.Unop.op) {
      case Iop_Not1: {
         HReg dst = newVRegI(env);
         HReg src = iselIntExpr_R(env, e->Iex.Unop.arg);
         addInstr(env, RISCV64Instr_SLTU(dst, hregRISCV64_x0(), src));
         return dst;
      }
      case Iop_8Uto64: {
         HReg dst = newVRegI(env);
         HReg src = iselIntExpr_R(env, e->Iex.Unop.arg);
         HReg mask = newVRegI(env);
         addInstr(env, RISCV64Instr_LI(mask, 0xff));
         addInstr(env, RISCV64Instr_AND(dst, src, mask));
         return dst;
      }
      case Iop_16Uto64: {
         HReg dst = newVRegI(env);
         HReg src = iselIntExpr_R(env, e->Iex.Unop.arg);
         HReg mask = newVRegI(env);
         addInstr(env, RISCV64Instr_LI(mask, 0xffff));
         addInstr(env, RISCV64Instr_AND(dst, src, mask));
         return dst;
      }
      case Iop_32Sto64: {
         HReg dst = newVRegI(env);
         HReg src = iselIntExpr_R(env, e->Iex.Unop.arg);
         addInstr(env, RISCV64Instr_ADDIW(dst, src, 0));
         return dst;
      }
      case Iop_64to8:
      case Iop_64to16:
      case Iop_64to32:
         /* These are no-ops. */
         return iselIntExpr_R(env, e->Iex.Unop.arg);
      default:
         break;
      }

      break;
   }

   /* ------------------------- GET ------------------------- */
   case Iex_Get: {
      HReg dst  = newVRegI(env);
      HReg base = get_baseblock_register();
      Int  off  = e->Iex.Get.offset - BASEBLOCK_OFFSET_ADJUSTMENT;
      vassert(off >= -2048 && off < 2048);

      if (ty == Ity_I64)
         addInstr(env, RISCV64Instr_LD(dst, base, off));
      else if (ty == Ity_I32)
         addInstr(env, RISCV64Instr_LW(dst, base, off));
      else if (ty == Ity_I16)
         addInstr(env, RISCV64Instr_LH(dst, base, off));
      else if (ty == Ity_I8)
         addInstr(env, RISCV64Instr_LB(dst, base, off));
      else
         goto irreducible;
      return dst;
   }

   /* ----------------------- LITERAL ----------------------- */
   /* 64/32/16/8-bit literals. */
   case Iex_Const: {
      ULong u;
      HReg  dst = newVRegI(env);
      switch (e->Iex.Const.con->tag) {
      case Ico_U64:
         u = e->Iex.Const.con->Ico.U64;
         break;
      case Ico_U32:
         u = e->Iex.Const.con->Ico.U32;
         break;
      case Ico_U16:
         u = e->Iex.Const.con->Ico.U16;
         break;
      case Ico_U8:
         u = e->Iex.Const.con->Ico.U8;
         break;
      default:
         goto irreducible;
      }
      addInstr(env, RISCV64Instr_LI(dst, u));
      return dst;
   }

   default:
      break;
   }

   /* We get here if no pattern matched. */
irreducible:
   ppIRExpr(e);
   vpanic("iselIntExpr_R: cannot reduce tree");
}

static HReg iselIntExpr_R(ISelEnv* env, IRExpr* e)
{
   HReg r = iselIntExpr_R_wrk(env, e);

   /* Sanity checks ... */
   vassert(hregClass(r) == HRcInt64);
   vassert(hregIsVirtual(r));

   return r;
}

/*------------------------------------------------------------*/
/*--- ISEL: Statements                                     ---*/
/*------------------------------------------------------------*/

static void iselStmt(ISelEnv* env, IRStmt* stmt)
{
   if (vex_traceflags & VEX_TRACE_VCODE) {
      vex_printf("\n-- ");
      ppIRStmt(stmt);
      vex_printf("\n");
   }

   switch (stmt->tag) {
   /* ------------------------ STORE ------------------------ */
   /* Little-endian write to memory. */
   case Ist_Store: {
      IRType tyd = typeOfIRExpr(env->type_env, stmt->Ist.Store.data);
      if (tyd == Ity_I64 || tyd == Ity_I32 || tyd == Ity_I16 || tyd == Ity_I8) {
         HReg src = iselIntExpr_R(env, stmt->Ist.Store.data);
         /* TODO Optimize the cases with small imm Add64/Sub64. */
         HReg addr = iselIntExpr_R(env, stmt->Ist.Store.addr);

         if (tyd == Ity_I64)
            addInstr(env, RISCV64Instr_SD(src, addr, 0));
         else if (tyd == Ity_I32)
            addInstr(env, RISCV64Instr_SW(src, addr, 0));
         else if (tyd == Ity_I16)
            addInstr(env, RISCV64Instr_SH(src, addr, 0));
         else if (tyd == Ity_I8)
            addInstr(env, RISCV64Instr_SB(src, addr, 0));
         else
            vassert(0);
         return;
      }
      break;
   }

   /* ------------------------- PUT ------------------------- */
   /* Write guest state, fixed offset. */
   case Ist_Put: {
      IRType tyd = typeOfIRExpr(env->type_env, stmt->Ist.Put.data);
      if (tyd == Ity_I64 || tyd == Ity_I32 || tyd == Ity_I16 || tyd == Ity_I8) {
         HReg src  = iselIntExpr_R(env, stmt->Ist.Put.data);
         HReg base = get_baseblock_register();
         Int  off  = stmt->Ist.Put.offset - BASEBLOCK_OFFSET_ADJUSTMENT;
         vassert(off >= -2048 && off < 2048);

         if (tyd == Ity_I64)
            addInstr(env, RISCV64Instr_SD(src, base, off));
         else if (tyd == Ity_I32)
            addInstr(env, RISCV64Instr_SW(src, base, off));
         else if (tyd == Ity_I16)
            addInstr(env, RISCV64Instr_SH(src, base, off));
         else if (tyd == Ity_I8)
            addInstr(env, RISCV64Instr_SB(src, base, off));
         else
            vassert(0);
         return;
      }
      break;
   }

   /* ------------------------- TMP ------------------------- */
   /* Assign value to temporary. */
   case Ist_WrTmp: {
      IRType ty = typeOfIRTemp(env->type_env, stmt->Ist.WrTmp.tmp);
      if (ty == Ity_I64 || ty == Ity_I32 || ty == Ity_I16 || ty == Ity_I8) {
         HReg dst = lookupIRTemp(env, stmt->Ist.WrTmp.tmp);
         HReg src = iselIntExpr_R(env, stmt->Ist.WrTmp.data);
         addInstr(env, RISCV64Instr_MV(dst, src));
         return;
      }
      break;
   }

   /* --------------------- INSTR MARK ---------------------- */
   /* Doesn't generate any executable code ... */
   case Ist_IMark:
      return;

   /* ------------------------ EXIT ------------------------- */
   case Ist_Exit: {
      if (stmt->Ist.Exit.dst->tag != Ico_U64)
         vpanic("iselStmt(riscv64): Ist_Exit: dst is not a 64-bit value");

      HReg cond   = iselIntExpr_R(env, stmt->Ist.Exit.guard);
      HReg base   = get_baseblock_register();
      Int  soff12 = stmt->Ist.Exit.offsIP - BASEBLOCK_OFFSET_ADJUSTMENT;
      vassert(soff12 >= -2048 && soff12 < 2048);

      /* Case: boring transfer to known address. */
      if (stmt->Ist.Exit.jk == Ijk_Boring) {
         if (env->chainingAllowed) {
            /* .. almost always true .. */
            /* Skip the event check at the dst if this is a forwards edge. */
            Bool toFastEP = (Addr64)stmt->Ist.Exit.dst->Ico.U64 > env->max_ga;
            if (0)
               vex_printf("%s", toFastEP ? "Y" : ",");
            addInstr(env, RISCV64Instr_XDirect(stmt->Ist.Exit.dst->Ico.U64,
                                               base, soff12, cond, toFastEP));
         } else {
            /* .. very occasionally .. */
            /* We can't use chaining, so ask for an assisted transfer, as
               that's the only alternative that is allowable. */
            HReg r = iselIntExpr_R(env, IRExpr_Const(stmt->Ist.Exit.dst));
            addInstr(env,
                     RISCV64Instr_XAssisted(r, base, soff12, cond, Ijk_Boring));
         }
         return;
      }

      /* Case: assisted transfer to arbitrary address. */
      switch (stmt->Ist.Exit.jk) {
      /* Keep this list in sync with that for iselNext below. */
      case Ijk_ClientReq:
      case Ijk_NoDecode:
      case Ijk_NoRedir:
      case Ijk_Sys_syscall:
      case Ijk_InvalICache:
      case Ijk_FlushDCache:
      case Ijk_SigTRAP:
      case Ijk_Yield: {
         HReg r = iselIntExpr_R(env, IRExpr_Const(stmt->Ist.Exit.dst));
         addInstr(env, RISCV64Instr_XAssisted(r, base, soff12, cond,
                                              stmt->Ist.Exit.jk));
         return;
      }
      default:
         break;
      }

      /* Do we ever expect to see any other kind? */
      goto stmt_fail;
   }

   default:
      break;
   }

stmt_fail:
   ppIRStmt(stmt);
   vpanic("iselStmt");
}

/*------------------------------------------------------------*/
/*--- ISEL: Basic block terminators (Nexts)                ---*/
/*------------------------------------------------------------*/

static void iselNext(ISelEnv* env, IRExpr* next, IRJumpKind jk, Int offsIP)
{
   if (vex_traceflags & VEX_TRACE_VCODE) {
      vex_printf("\n-- PUT(%d) = ", offsIP);
      ppIRExpr(next);
      vex_printf("; exit-");
      ppIRJumpKind(jk);
      vex_printf("\n");
   }

   HReg base   = get_baseblock_register();
   Int  soff12 = offsIP - BASEBLOCK_OFFSET_ADJUSTMENT;
   vassert(soff12 >= -2048 && soff12 < 2048);

   /* Case: boring transfer to known address. */
   if (next->tag == Iex_Const) {
      IRConst* cdst = next->Iex.Const.con;
      vassert(cdst->tag == Ico_U64);
      if (jk == Ijk_Boring || jk == Ijk_Call) {
         /* Boring transfer to known address. */
         if (env->chainingAllowed) {
            /* .. almost always true .. */
            /* Skip the event check at the dst if this is a forwards edge. */
            Bool toFastEP = (Addr64)cdst->Ico.U64 > env->max_ga;
            if (0)
               vex_printf("%s", toFastEP ? "X" : ".");
            addInstr(env, RISCV64Instr_XDirect(cdst->Ico.U64, base, soff12,
                                               INVALID_HREG, toFastEP));
         } else {
            /* .. very occasionally .. */
            /* We can't use chaining, so ask for an assisted transfer, as that's
               the only alternative that is allowable. */
            HReg r = iselIntExpr_R(env, next);
            addInstr(env, RISCV64Instr_XAssisted(r, base, soff12, INVALID_HREG,
                                                 Ijk_Boring));
         }
         return;
      }
   }

   /* Case: call/return (==boring) transfer to any address. */
   switch (jk) {
   case Ijk_Boring:
   case Ijk_Ret:
   case Ijk_Call: {
      HReg r = iselIntExpr_R(env, next);
      if (env->chainingAllowed)
         addInstr(env, RISCV64Instr_XIndir(r, base, soff12, INVALID_HREG));
      else
         addInstr(env, RISCV64Instr_XAssisted(r, base, soff12, INVALID_HREG,
                                              Ijk_Boring));
      return;
   }
   default:
      break;
   }

   /* Case: assisted transfer to arbitrary address. */
   switch (jk) {
   /* Keep this list in sync with that for Ist_Exit above. */
   case Ijk_ClientReq:
   case Ijk_NoDecode:
   case Ijk_NoRedir:
   case Ijk_Sys_syscall:
   case Ijk_InvalICache:
   case Ijk_FlushDCache:
   case Ijk_SigTRAP:
   case Ijk_Yield: {
      HReg r = iselIntExpr_R(env, next);
      addInstr(env, RISCV64Instr_XAssisted(r, base, soff12, INVALID_HREG, jk));
      return;
   }
   default:
      break;
   }

   vex_printf("\n-- PUT(%d) = ", offsIP);
   ppIRExpr(next);
   vex_printf("; exit-");
   ppIRJumpKind(jk);
   vex_printf("\n");
   vassert(0); /* Are we expecting any other kind? */
}

/*------------------------------------------------------------*/
/*--- Insn selector top-level                              ---*/
/*------------------------------------------------------------*/

/* Translate an entire SB to riscv64 code. */

HInstrArray* iselSB_RISCV64(const IRSB*        bb,
                            VexArch            arch_host,
                            const VexArchInfo* archinfo_host,
                            const VexAbiInfo*  vbi /*UNUSED*/,
                            Int                offs_Host_EvC_Counter,
                            Int                offs_Host_EvC_FailAddr,
                            Bool               chainingAllowed,
                            Bool               addProfInc,
                            Addr               max_ga)
{
   Int      i, j;
   HReg     hreg, hregHI;
   ISelEnv* env;

   /* TODO */
#if 0
   RISCV64AMode *amCounter, *amFailAddr;
#endif

   /* Do some sanity checks. */
   vassert(arch_host == VexArchRISCV64);

   /* Check that the host's endianness is as expected. */
   vassert(archinfo_host->endness == VexEndnessLE);

   /* Guard against unexpected space regressions. */
   vassert(sizeof(RISCV64Instr) <= 32);

   /* Make up an initial environment to use. */
   env           = LibVEX_Alloc_inline(sizeof(ISelEnv));
   env->vreg_ctr = 0;

   /* Set up output code array. */
   env->code = newHInstrArray();

   /* Copy BB's type env. */
   env->type_env = bb->tyenv;

   /* Make up an IRTemp -> virtual HReg mapping. This doesn't change as we go
      along. */
   env->n_vregmap = bb->tyenv->types_used;
   env->vregmap   = LibVEX_Alloc_inline(env->n_vregmap * sizeof(HReg));
   env->vregmapHI = LibVEX_Alloc_inline(env->n_vregmap * sizeof(HReg));

   /* and finally ... */
   env->chainingAllowed = chainingAllowed;
   env->hwcaps          = archinfo_host->hwcaps;
   env->previous_rm     = NULL;
   env->max_ga          = max_ga;

   /* For each IR temporary, allocate a suitably-kinded virtual register. */
   j = 0;
   for (i = 0; i < env->n_vregmap; i++) {
      hregHI = hreg = INVALID_HREG;
      switch (bb->tyenv->types[i]) {
      case Ity_I1:
      case Ity_I8:
      case Ity_I16:
      case Ity_I32:
      case Ity_I64:
         hreg = mkHReg(True, HRcInt64, 0, j++);
         break;
      default:
         ppIRType(bb->tyenv->types[i]);
         vpanic("iselBB(riscv64): IRTemp type");
      }
      env->vregmap[i]   = hreg;
      env->vregmapHI[i] = hregHI;
   }
   env->vreg_ctr = j;

   /* The very first instruction must be an event check. */
   /* TODO */
#if 0
   amCounter  = ARM64AMode_RI9(hregRISCV64_x8(), offs_Host_EvC_Counter);
   amFailAddr = ARM64AMode_RI9(hregRISCV64_x8(), offs_Host_EvC_FailAddr);
   addInstr(env, ARM64Instr_EvCheck(amCounter, amFailAddr));
#endif

   /* TODO */
#if 0
   /* Possibly a block counter increment (for profiling). At this point we don't
      know the address of the counter, so just pretend it is zero. It will have
      to be patched later, but before this translation is used, by a call to
      LibVEX_patchProfCtr(). */
   if (addProfInc)
      addInstr(env, ARM64Instr_ProfInc());
#endif

   /* Ok, finally we can iterate over the statements. */
   for (i = 0; i < bb->stmts_used; i++)
      iselStmt(env, bb->stmts[i]);

   iselNext(env, bb->next, bb->jumpkind, bb->offsIP);

   /* Record the number of vregs we used. */
   env->code->n_vregs = env->vreg_ctr;
   return env->code;
}

/*--------------------------------------------------------------------*/
/*--- end                                      host_riscv64_isel.c ---*/
/*--------------------------------------------------------------------*/
