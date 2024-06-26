
/*--------------------------------------------------------------------*/
/*--- Support for doing system calls.      syscall-arm64-freebsd.S ---*/
/*--------------------------------------------------------------------*/

/*
  This file is part of Valgrind, a dynamic binary instrumentation
  framework.

  Copyright (C) 2024 Paul Floyd
     pjfloyd@wanadoo.fr

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

#if defined(VGP_arm64_freebsd)

#include "pub_core_vkiscnums_asm.h"
#include "libvex_guest_offsets.h"

/*----------------------------------------------------------------*/
/*
        Perform a syscall for the client.  This will run a syscall
        with the client's specific per-thread signal mask.

        The structure of this function is such that, if the syscall is
        interrupted by a signal, we can determine exactly what
        execution state we were in with respect to the execution of
        the syscall by examining the value of IP in the signal
        handler.  This means that we can always do the appropriate
        thing to precisely emulate the kernel's signal/syscall
        interactions.

        The syscall number is taken from the argument, even though it
        should also be in guest_state->guest_X8.  The syscall result
	is written back to guest_state->guest_X0 on completion.

        Returns 0 if the syscall was successfully called (even if the
        syscall itself failed), or a nonzero error code in the lowest
        8 bits if one of the sigprocmasks failed (there's no way to
        determine which one failed).  And there's no obvious way to
        recover from that either, but nevertheless we want to know.

        VG_(fixup_guest_state_after_syscall_interrupted) does the
        thread state fixup in the case where we were interrupted by a
        signal.

        Prototype:

   UWord ML_(do_syscall_for_client_WRK)(
              Int syscallno,                 // x0
              void* guest_state,             // x1
              const vki_sigset_t *sysmask,   // x2
              const vki_sigset_t *postmask,  // x3
              Int nsigwords)                 // x4
*/
/* from vki-arm64-freebsd.h */
#define VKI_SIG_SETMASK 3

.globl ML_(do_syscall_for_client_WRK)
ML_(do_syscall_for_client_WRK):

   /* Stash callee-saves and our args on the stack */
   stp  x29, x30, [sp, #-16]!
   stp  x27, x28, [sp, #-16]!
   stp  x25, x26, [sp, #-16]!
   stp  x23, x24, [sp, #-16]!
   stp  x21, x22, [sp, #-16]!
   stp  x19, x20, [sp, #-16]!
   stp  x4,  x5,  [sp, #-16]!
   stp  x2,  x3,  [sp, #-16]!
   stp  x0,  x1,  [sp, #-16]!

1:

   mov x8, #__NR_sigprocmask
   mov x0, #VKI_SIG_SETMASK
   mov x1, x2 /* sysmask */
   mov x2, x3 /* postmask */
   mov x3, x4 /* nsigwords */
   svc 0x00000000

   ldr x9, [sp, #8] /* saved x1 == guest_state */

   ldr x8, [sp, #0] /* saved x0 == syscall# */
   ldr x0, [x9, #OFFSET_arm64_X0]
   ldr x1, [x9, #OFFSET_arm64_X1]
   ldr x2, [x9, #OFFSET_arm64_X2]
   ldr x3, [x9, #OFFSET_arm64_X3]
   ldr x4, [x9, #OFFSET_arm64_X4]
   ldr x5, [x9, #OFFSET_arm64_X5]
   ldr x6, [x9, #OFFSET_arm64_X6]
   ldr x7, [x9, #OFFSET_arm64_X7]

2: svc 0x00000000
3:

   /* stash returned carry flag */
   mov x4, 1
   csel x4, x4, xzr, cs

   ldr x5, [sp, #8] /* saved x1 == guest_state */
   str x0, [x5, #OFFSET_arm64_X0]
   str x1, [x5, #OFFSET_arm64_X1]

   /* save carry flag to VEX */
   mov x0, x4       /* arg1 = new flag */
   ldr x1, [sp, #8] /* arg2 = vex state */
   mov x20, 1
   str x20, [x1, #OFFSET_arm64_SETC]
   bl LibVEX_GuestARM64_put_nzcv_c
   ldr x1, [sp, #8] /* arg2 = vex state */
   mov x20, 0
   str x20, [x1, #OFFSET_arm64_SETC]

4:
   mov x8, #__NR_sigprocmask
   mov x0, #VKI_SIG_SETMASK
   ldr x1, [sp, #24] /* saved x3 == postmask */
   mov x2, #0
   ldr x3, [sp, #32] /* saved x4 == nsigwords */
   cmn     xzr, xzr /* clear the carry flag */
   svc 0x00000000

   blt 7f

5: /* Success: return zero */
   mov  x0, #0
   ldp  xzr, x1,  [sp], #16
   ldp  x2,  x3,  [sp], #16
   ldp  x4,  x5,  [sp], #16
   ldp  x19, x20, [sp], #16
   ldp  x21, x22, [sp], #16
   ldp  x23, x24, [sp], #16
   ldp  x25, x26, [sp], #16
   ldp  x27, x28, [sp], #16
   ldp  x29, x30, [sp], #16
   ret

7: /* Failure: return 0x8000 | error code */
   orr  x0, x0, #0x8000
   ldp  xzr, x1,  [sp], #16
   ldp  x2,  x3,  [sp], #16
   ldp  x4,  x5,  [sp], #16
   ldp  x19, x20, [sp], #16
   ldp  x21, x22, [sp], #16
   ldp  x23, x24, [sp], #16
   ldp  x25, x26, [sp], #16
   ldp  x27, x28, [sp], #16
   ldp  x29, x30, [sp], #16
   ret

.section .rodata
/* export the ranges so that
   VG_(fixup_guest_state_after_syscall_interrupted) can do the
   right thing */

.align 3
.globl ML_(blksys_setup)
.globl ML_(blksys_restart)
.globl ML_(blksys_complete)
.globl ML_(blksys_committed)
.globl ML_(blksys_finished)
ML_(blksys_setup):      .quad 1b
ML_(blksys_restart):    .quad 2b
ML_(blksys_complete):   .quad 3b
ML_(blksys_committed):  .quad 4b
ML_(blksys_finished):   .quad 5b

#endif // defined(VGP_arm_freebsd)

/* Let the linker know we don't need an executable stack */
MARK_STACK_NO_EXEC

/*--------------------------------------------------------------------*/
/*--- end                                                          ---*/
/*--------------------------------------------------------------------*/
