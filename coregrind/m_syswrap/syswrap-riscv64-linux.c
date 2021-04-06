
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
#include "pub_core_transtab.h"      // VG_(discard_translations)

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

/* ---------------------------------------------------------------------
   PRE/POST wrappers for riscv64/Linux-specific syscalls
   ------------------------------------------------------------------ */

#define PRE(name)  DEFN_PRE_TEMPLATE(riscv64_linux, name)
#define POST(name) DEFN_POST_TEMPLATE(riscv64_linux, name)

/* Add prototypes for the wrappers declared here, so that gcc doesn't
   harass us for not having prototypes.  Really this is a kludge --
   the right thing to do is to make these wrappers 'static' since they
   aren't visible outside this file, but that requires even more macro
   magic. */

DECL_TEMPLATE(riscv64_linux, sys_mmap);
DECL_TEMPLATE(riscv64_linux, sys_riscv_flush_icache);

PRE(sys_mmap)
{
   PRINT("sys_mmap ( %#lx, %lu, %lu, %#lx, %lu, %lu )", ARG1, ARG2, ARG3, ARG4,
         ARG5, ARG6);
   PRE_REG_READ6(long, "mmap", unsigned long, start, unsigned long, length,
                 unsigned long, prot, unsigned long, flags, unsigned long, fd,
                 unsigned long, offset);

   SysRes r =
      ML_(generic_PRE_sys_mmap)(tid, ARG1, ARG2, ARG3, ARG4, ARG5, ARG6);
   SET_STATUS_from_SysRes(r);
}

PRE(sys_riscv_flush_icache)
{
   PRINT("sys_riscv_flush_icache ( %#lx, %lx, %#lx )", ARG1, ARG2, ARG3);
   PRE_REG_READ3(long, "riscv_flush_icache", unsigned long, start,
                 unsigned long, end, unsigned long, flags);

   VG_(discard_translations)
   ((Addr)ARG1, (ULong)ARG2 - (ULong)ARG1, "PRE(sys_riscv_flush_icache)");
   SET_STATUS_Success(0);
}

#undef PRE
#undef POST

/* ---------------------------------------------------------------------
   The riscv64/Linux syscall table
   ------------------------------------------------------------------ */

/* Add a riscv64-linux specific wrapper to a syscall table. */
#define PLAX_(sysno, name) WRAPPER_ENTRY_X_(riscv64_linux, sysno, name)
#define PLAXY(sysno, name) WRAPPER_ENTRY_XY(riscv64_linux, sysno, name)

/* This table maps from __NR_xxx syscall numbers to the appropriate PRE/POST
   sys_foo() wrappers on riscv64. */
static SyscallTableEntry syscall_main_table[] = {
   LINX_(__NR_faccessat, sys_faccessat),   /* 48 */
   LINXY(__NR_openat, sys_openat),         /* 56 */
   GENXY(__NR_close, sys_close),           /* 57 */
   GENXY(__NR_read, sys_read),             /* 63 */
   GENX_(__NR_write, sys_write),           /* 64 */
   GENX_(__NR_writev, sys_writev),         /* 66 */
   LINXY(__NR_newfstatat, sys_newfstatat), /* 79 */
   LINX_(__NR_exit_group, sys_exit_group), /* 94 */
   GENXY(__NR_uname, sys_newuname),        /* 160 */
   GENX_(__NR_brk, sys_brk),               /* 214 */
   GENXY(__NR_munmap, sys_munmap),         /* 215 */
   PLAX_(__NR_mmap, sys_mmap),             /* 222 */
   GENXY(__NR_mprotect, sys_mprotect),     /* 226 */
   PLAX_(__NR_riscv_flush_icache, sys_riscv_flush_icache), /* 259 */
};

SyscallTableEntry* ML_(get_linux_syscall_entry)(UInt sysno)
{
   const UInt syscall_main_table_size =
      sizeof(syscall_main_table) / sizeof(syscall_main_table[0]);

   /* Is it in the contiguous initial section of the table? */
   if (sysno < syscall_main_table_size) {
      SyscallTableEntry* sys = &syscall_main_table[sysno];
      if (sys->before == NULL)
         return NULL; /* no entry */
      else
         return sys;
   }

   /* Can't find a wrapper. */
   return NULL;
}

#endif // defined(VGP_riscv64_linux)

/*--------------------------------------------------------------------*/
/*--- end                                  syswrap-riscv64-linux.c ---*/
/*--------------------------------------------------------------------*/
