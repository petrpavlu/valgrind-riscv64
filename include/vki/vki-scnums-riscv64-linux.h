
/*--------------------------------------------------------------------*/
/*--- System call numbers for riscv64-linux.                       ---*/
/*---                                   vki-scnums-riscv64-linux.h ---*/
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

#ifndef __VKI_SCNUMS_RISCV64_LINUX_H
#define __VKI_SCNUMS_RISCV64_LINUX_H

// From linux-5.10.4/arch/riscv/include/uapi/asm/unistd.h
// is a #include of
//      linux-5.10.4/include/uapi/asm-generic/unistd.h

#define __NR_getxattr 8
#define __NR_getcwd 17
#define __NR_dup 23
#define __NR_dup3 24
#define __NR3264_fcntl 25
#define __NR_ioctl 29
#define __NR_mknodat 33
#define __NR_unlinkat 35
#define __NR_faccessat 48
#define __NR_openat 56
#define __NR_close 57
#define __NR_pipe2 59
#define __NR_getdents64 61
#define __NR3264_lseek 62
#define __NR_read 63
#define __NR_write 64
#define __NR_writev 66
#define __NR_pread64 67
#define __NR_ppoll 73
#define __NR_readlinkat 78
#define __NR3264_fstatat 79
#define __NR_exit_group 94
#define __NR_futex 98
#define __NR_clock_gettime 113
#define __NR_ptrace 117
#define __NR_sched_yield 124
#define __NR_kill 129
#define __NR_tkill 130
#define __NR_rt_sigaction 134
#define __NR_rt_sigprocmask 135
#define __NR_rt_sigtimedwait 137
#define __NR_rt_sigreturn 139
#define __NR_getpgid 155
#define __NR_getgroups 158
#define __NR_uname 160
#define __NR_getrusage 165
#define __NR_prctl 167
#define __NR_gettimeofday 169
#define __NR_getpid 172
#define __NR_getppid 173
#define __NR_geteuid 175
#define __NR_getegid 177
#define __NR_gettid 178
#define __NR_semctl 191
#define __NR_shmctl 195
#define __NR_socket 198
#define __NR_connect 203
#define __NR_getsockname 204
#define __NR_getpeername 205
#define __NR_sendto 206
#define __NR_setsockopt 208
#define __NR_getsockopt 209
#define __NR_brk 214
#define __NR_munmap 215
#define __NR_mremap 216
#define __NR_clone 220
#define __NR_execve 221
#define __NR3264_mmap 222
#define __NR_mprotect 226
#define __NR_arch_specific_syscall 244
#define __NR_wait4 260
#define __NR_prlimit64 261
#define __NR_renameat2 276
#define __NR_statx 291

#define __NR_fcntl __NR3264_fcntl
#define __NR_lseek __NR3264_lseek
#define __NR_newfstatat __NR3264_fstatat
#define __NR_mmap __NR3264_mmap

#define __NR_riscv_flush_icache (__NR_arch_specific_syscall + 15)

#endif /* __VKI_SCNUMS_RISCV64_LINUX_H */

/*--------------------------------------------------------------------*/
/*--- end                               vki-scnums-riscv64-linux.h ---*/
/*--------------------------------------------------------------------*/
