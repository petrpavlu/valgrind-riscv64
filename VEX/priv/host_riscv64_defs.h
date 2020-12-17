
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
#undef ST_IN

/*------------------------------------------------------------*/
/*--- Memory address expressions (amodes)                  ---*/
/*------------------------------------------------------------*/

typedef enum {
   RISCV64am_RI12 = 10, /* reg + soff12 */
} RISCV64AModeTag;

typedef struct {
   RISCV64AModeTag tag;
   union {
      struct {
         HReg reg;
         Int  soff12; /* -2048 .. +2047 */
      } RI12;
   } RISCV64am;
} RISCV64AMode;

RISCV64AMode* RISCV64AMode_RI12(HReg reg, Int soff12);

/*------------------------------------------------------------*/
/*--- Instructions                                         ---*/
/*------------------------------------------------------------*/

/* The kind of instructions. */
typedef enum {
   Rin_Imm64, /* Generate 64-bit literal to register */
} RISCV64InstrTag;

typedef struct {
   RISCV64InstrTag tag;
   union {
      struct {
         ULong imm64;
         HReg  dst;
      } Imm64;
   } Rin;
} RISCV64Instr;

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
