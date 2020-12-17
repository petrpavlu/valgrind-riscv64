
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
   Int r;

   /* Be generic for all virtual regs. */
   if (hregIsVirtual(reg))
      return ppHReg(reg);

   /* Be specific for real regs. */
   switch (hregClass(reg)) {
   case HRcInt64:
      r = hregEncoding(reg);
      vassert(r >= 0 && r < 31);
      return vex_printf("x%d", r);
   default:
      vpanic("ppHRegRISCV64");
   }
}

/*------------------------------------------------------------*/
/*--- Instructions                                         ---*/
/*------------------------------------------------------------*/

void ppRISCV64Instr(const RISCV64Instr* i, Bool mode64)
{
   vassert(mode64 == True);
   switch (i->tag) {
#if 0
      case RISCV64in_Arith:
         vex_printf("%s    ", i->RISCV64in.Arith.isAdd ? "add" : "sub");
         ppHRegRISCV64(i->RISCV64in.Arith.dst);
         vex_printf(", ");
         ppHRegRISCV64(i->RISCV64in.Arith.argL);
         vex_printf(", ");
         ppRISCV64RIA(i->RISCV64in.Arith.argR);
         return;
#endif
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
   RRegUniverse*       ru = &all_regs;

   if (LIKELY(initialised))
      return ru;

   RRegUniverse__init(ru);

   /* Add the registers that are available to the register allocator. */
   /* TODO */
   ru->allocable_start[HRcInt64] = ru->size;
   ru->regs[ru->size++] = hregRISCV64_x18();
   ru->regs[ru->size++] = hregRISCV64_x19();
   ru->regs[ru->size++] = hregRISCV64_x20();
   ru->regs[ru->size++] = hregRISCV64_x21();
   ru->regs[ru->size++] = hregRISCV64_x22();
   ru->regs[ru->size++] = hregRISCV64_x23();
   ru->regs[ru->size++] = hregRISCV64_x24();
   ru->regs[ru->size++] = hregRISCV64_x25();
   ru->regs[ru->size++] = hregRISCV64_x26();
   ru->regs[ru->size++] = hregRISCV64_x27();

   ru->allocable_end[HRcInt64] = ru->size - 1;
   ru->allocable = ru->size;

   /* Add the registers that are not available for allocation. */
   /* TODO */

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
#if 0
      case RISCV64Ain_Arith:
         addHRegUse(u, HRmWrite, i->Ain.Imm64.dst);
         return;
#endif
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
#if 0
      case RISCV64Ain_Imm64:
         mapReg(m, &i->Ain.Imm64.dst);
         return;
#endif
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
   HRegClass rclass;
   vassert(offsetB >= 0);
   vassert(!hregIsVirtual(rreg));
   vassert(mode64 == True);
   *i1 = NULL;
   rclass = hregClass(rreg);
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
   HRegClass rclass;
   vassert(offsetB >= 0);
   vassert(!hregIsVirtual(rreg));
   vassert(mode64 == True);
   *i1 = NULL;
   rclass = hregClass(rreg);
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

/*---------------------------------------------------------------*/
/*--- Code generation                                         ---*/
/*---------------------------------------------------------------*/

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
   UChar* p = &buf[0];
   vassert(nbuf >= 64);
   vassert(mode64 == True);

   /* vex_printf("asm  "); ppRISCV6464Instr(i, mode64); vex_printf("\n"); */

   switch (i->tag) {
#if 0
      case RISCV64Ain_Imm64:
         *p++ = toUChar(0x48 + (1 & iregEnc3(i->Ain.Imm64.dst)));
         *p++ = toUChar(0xB8 + iregEnc210(i->Ain.Imm64.dst));
         p = emit64(p, i->Ain.Imm64.imm64);
         goto done;
#endif

      default:
         goto bad;
   }

bad:
   ppRISCV64Instr(i, mode64);
   vpanic("emit_RISCV64Instr");
   /*NOTREACHED*/

done:
   vassert(p - &buf[0] <= 64);
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
