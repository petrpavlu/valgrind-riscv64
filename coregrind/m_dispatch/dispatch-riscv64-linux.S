
/*--------------------------------------------------------------------*/
/*--- The core dispatch loop, for jumping to a code address.       ---*/
/*---                                     dispatch-riscv64-linux.S ---*/
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

#include "pub_core_basics_asm.h"

#if defined(VGP_riscv64_linux)

#include "pub_core_dispatch_asm.h"
#include "pub_core_transtab_asm.h"
#include "libvex_guest_offsets.h"


/*------------------------------------------------------------*/
/*---                                                      ---*/
/*--- The dispatch loop.  VG_(disp_run_translations) is    ---*/
/*--- used to run all translations,                        ---*/
/*--- including no-redir ones.                             ---*/
/*---                                                      ---*/
/*------------------------------------------------------------*/

/*----------------------------------------------------*/
/*--- Entry and preamble (set everything up)       ---*/
/*----------------------------------------------------*/

/* signature:
void VG_(disp_run_translations)( UWord* two_words,
                                 void*  guest_state,
                                 Addr   host_addr );
*/
.text
.global VG_(disp_run_translations)
VG_(disp_run_translations):
	/* a0 holds two_words
	   a1 holds guest_state
	   a2 holds host_addr
	*/
	/* Push the callee-saved registers. Note this sequence maintains
	   16-alignment of sp. Also save a0 since it will be needed in the
	   postamble. */
	addi sp, sp, -112
	sd ra, 104(sp)
	sd s0, 96(sp)
	sd s1, 88(sp)
	sd s2, 80(sp)
	sd s3, 72(sp)
	sd s4, 64(sp)
	sd s5, 56(sp)
	sd s6, 48(sp)
	sd s7, 40(sp)
	sd s8, 32(sp)
	sd s9, 24(sp)
	sd s10, 16(sp)
	sd s11, 8(sp)
	sd a0, 0(sp)

	/* Set up the guest state pointer. */
	li t0, 2048
	add s0, a1, t0

	/* and jump into the code cache. Chained translations in the code cache
	   run, until for whatever reason, they can't continue. When that
	   happens, the translation in question will jump (or call) to one of
	   the continuation points VG_(cp_...) below. */
	jr a2
	/* NOTREACHED */

/*----------------------------------------------------*/
/*--- Postamble and exit.                          ---*/
/*----------------------------------------------------*/

postamble:
	/* At this point, t0 and t1 contain two words to be returned to the
	   caller. t0 holds a TRC value, and t1 optionally may hold another
	   word (for CHAIN_ME exits, the address of the place to patch.) */

	/* Restore int regs, including importantly a0 (two_words). */
	ld ra, 104(sp)
	ld s0, 96(sp)
	ld s1, 88(sp)
	ld s2, 80(sp)
	ld s3, 72(sp)
	ld s4, 64(sp)
	ld s5, 56(sp)
	ld s6, 48(sp)
	ld s7, 40(sp)
	ld s8, 32(sp)
	ld s9, 24(sp)
	ld s10, 16(sp)
	ld s11, 8(sp)
	ld a0, 0(sp)
	addi sp, sp, 112

	/* Stash return values. */
	sd t0, 0(a0)
	sd t1, 8(a0)
	ret

/*----------------------------------------------------*/
/*--- Continuation points                          ---*/
/*----------------------------------------------------*/

/* ------ Chain me to slow entry point ------ */
.global VG_(disp_cp_chain_me_to_slowEP)
VG_(disp_cp_chain_me_to_slowEP):
	/* We got called. The return address indicates where the patching needs
	   to happen. Collect the return address and, exit back to C land,
	   handing the caller the pair (Chain_me_F, RA). */
	li t0, VG_TRC_CHAIN_ME_TO_SLOW_EP
	mv t1, ra
	/* 4 = lui t0, disp_cp_chain_me_to_slowEP[47:28]'
	   4 = addiw t0, t0, disp_cp_chain_me_to_slowEP[27:16]'
	   2 = c.slli t0, 12
	   4 = addi t0, t0, disp_cp_chain_me_to_slowEP[15:4]'
	   2 = c.slli t0, 4
	   2 = c.addi t0, disp_cp_chain_me_to_slowEP[3:0]'
	   2 = c.jalr 0(t0)
	*/
	addi t1, t1, -(4+4+2+4+2+2+2)
	j postamble

/* ------ Chain me to fast entry point ------ */
.global VG_(disp_cp_chain_me_to_fastEP)
VG_(disp_cp_chain_me_to_fastEP):
	/* We got called. The return address indicates where the patching needs
	   to happen. Collect the return address and, exit back to C land,
	   handing the caller the pair (Chain_me_F, RA). */
	li t0, VG_TRC_CHAIN_ME_TO_FAST_EP
	mv t1, ra
	/* 4 = lui t0, disp_cp_chain_me_to_fastEP[47:28]'
	   4 = addiw t0, t0, disp_cp_chain_me_to_fastEP[27:16]'
	   2 = c.slli t0, 12
	   4 = addi t0, t0, disp_cp_chain_me_to_fastEP[15:4]'
	   2 = c.slli t0, 4
	   2 = c.addi t0, disp_cp_chain_me_to_fastEP[3:0]'
	   2 = c.jalr 0(t0)
	*/
	addi t1, t1, -(4+4+2+4+2+2+2)
	j postamble

/* ------ Indirect but boring jump ------ */
.global VG_(disp_cp_xindir)
VG_(disp_cp_xindir):
	unimp

/* ------ Assisted jump ------ */
.global VG_(disp_cp_xassisted)
VG_(disp_cp_xassisted):
	/* s0 contains the TRC. */
	mv t0, s0
	li t1, 0
	j postamble

/* ------ Event check failed ------ */
.global VG_(disp_cp_evcheck_fail)
VG_(disp_cp_evcheck_fail):
	li t0, VG_TRC_INNER_COUNTERZERO
	li t1, 0
	j postamble

.size VG_(disp_run_translations), .-VG_(disp_run_translations)

#endif // defined(VGP_riscv64_linux)

/* Let the linker know we don't need an executable stack */
MARK_STACK_NO_EXEC

/*--------------------------------------------------------------------*/
/*--- end                                 dispatch-riscv64-linux.S ---*/
/*--------------------------------------------------------------------*/
