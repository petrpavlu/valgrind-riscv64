
/*--------------------------------------------------------------------*/
/*--- Platform-specific syscalls stuff.  syswrap-riscv64-linux.c -----*/
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

#if defined(VGP_riscv64_linux)

#include "pub_core_basics.h"
#include "pub_core_vki.h"
#include "pub_core_vkiscnums.h"
#include "pub_core_threadstate.h"
#include "pub_core_aspacemgr.h"
#include "pub_core_libcbase.h"
#include "pub_core_libcassert.h"
#include "pub_core_libcprint.h"
#include "pub_core_libcsignal.h"
#include "pub_core_options.h"
#include "pub_core_scheduler.h"
#include "pub_core_sigframe.h"      // For VG_(sigframe_destroy)()
#include "pub_core_syscall.h"
#include "pub_core_syswrap.h"
#include "pub_core_tooliface.h"

#include "priv_types_n_macros.h"
#include "priv_syswrap-generic.h"   /* for decls of generic wrappers */
#include "priv_syswrap-linux.h"     /* for decls of linux-ish wrappers */
/* TODO Review includes. */


/* ---------------------------------------------------------------------
   clone() handling
   ------------------------------------------------------------------ */

/* Call f(arg1), but first switch stacks, using 'stack' as the new
   stack, and use 'retaddr' as f's return-to address.  Also, clear all
   the integer registers before entering f.*/
__attribute__((noreturn))
void ML_(call_on_new_stack_0_1) ( Addr stack,
                                  Addr retaddr,
                                  void (*f)(Word),
                                  Word arg1 );
//    a0 = stack
//    a1 = retaddr
//    a2 = f
//    a3 = arg1
asm(
".text\n"
".globl vgModuleLocal_call_on_new_stack_0_1\n"
"vgModuleLocal_call_on_new_stack_0_1:\n"
"   mv    sp, a0\n\t" /* Stack pointer */
"   mv    ra, a1\n\t" /* Return address */
"   mv    a0, a3\n\t" /* First argument */
"   li    t0, 0\n\t"  /* Clear our GPRs */
"   li    t1, 0\n\t"
"   li    t2, 0\n\t"
"   li    s0, 0\n\t"
"   li    s1, 0\n\t"
/* don't zero out a0, already set to the first argument */
"   li    a1, 0\n\t"
/* don't zero out a2, holds the target function f() */
"   li    a3, 0\n\t"
"   li    a4, 0\n\t"
"   li    a5, 0\n\t"
"   li    a6, 0\n\t"
"   li    a7, 0\n\t"
"   li    s2, 0\n\t"
"   li    s3, 0\n\t"
"   li    s4, 0\n\t"
"   li    s5, 0\n\t"
"   li    s6, 0\n\t"
"   li    s7, 0\n\t"
"   li    s8, 0\n\t"
"   li    s9, 0\n\t"
"   li    s10, 0\n\t"
"   li    s11, 0\n\t"
"   li    t3, 0\n\t"
"   li    t4, 0\n\t"
"   li    t5, 0\n\t"
"   li    t6, 0\n\t"
"   jr    a2\n\t"
".previous\n"
);

/* ---------------------------------------------------------------------
   More thread stuff
   ------------------------------------------------------------------ */

/* riscv64 doesn't have any architecture specific thread stuff that needs to be
   cleaned up. */
void VG_(cleanup_thread) ( ThreadArchState* arch )
{
}

SyscallTableEntry* ML_(get_linux_syscall_entry) ( UInt sysno )
{
   /* Can't find a wrapper */
   return NULL;
}

#endif // defined(VGP_riscv64_linux)

/*--------------------------------------------------------------------*/
/*--- end                                  syswrap-riscv64-linux.c ---*/
/*--------------------------------------------------------------------*/
