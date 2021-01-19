
/*--------------------------------------------------------------------*/
/*--- Support for doing system calls.      syscall-riscv64-linux.S ---*/
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

#include "pub_core_vkiscnums_asm.h"
#include "libvex_guest_offsets.h"


/* from vki-riscv64-linux.h */
#define VKI_SIG_SETMASK 2

.globl ML_(do_syscall_for_client_WRK)
ML_(do_syscall_for_client_WRK):
   unimp

1:
   unimp

2: unimp
3:

4:
   unimp

5:
   unimp

7:
   unimp

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

#endif // defined(VGP_riscv64_linux)

/* Let the linker know we don't need an executable stack */
MARK_STACK_NO_EXEC

/*--------------------------------------------------------------------*/
/*--- end                                                          ---*/
/*--------------------------------------------------------------------*/
