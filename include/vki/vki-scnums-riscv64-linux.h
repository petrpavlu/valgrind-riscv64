
/*--------------------------------------------------------------------*/
/*--- System call numbers for riscv64-linux.                       ---*/
/*---                                   vki-scnums-riscv64-linux.h ---*/
/*--------------------------------------------------------------------*/

/*
   This file is part of Valgrind, a dynamic binary instrumentation
   framework.

   Copyright (C) 2020-2022 Petr Pavlu
      petr.pavlu@dagobah.cz

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
#define __NR_eventfd2 19
#define __NR_epoll_create1 20
#define __NR_epoll_pwait 22
#define __NR_dup 23
#define __NR_dup3 24
#define __NR3264_fcntl 25
#define __NR_ioctl 29
#define __NR_mknodat 33
#define __NR_mkdirat 34
#define __NR_unlinkat 35
#define __NR_linkat 37
#define __NR_ftruncate 46
#define __NR_faccessat 48
#define __NR_chdir 49
#define __NR_fchmodat 53
#define __NR_fchownat 54
#define __NR_openat 56
#define __NR_close 57
#define __NR_pipe2 59
#define __NR_getdents64 61
#define __NR3264_lseek 62
#define __NR_read 63
#define __NR_write 64
#define __NR_readv 65
#define __NR_writev 66
#define __NR_pread64 67
#define __NR_pwrite64 68
#define __NR_preadv 69
#define __NR_pwritev 70
#define __NR_pselect6 72
#define __NR_ppoll 73
#define __NR_signalfd4 74
#define __NR_readlinkat 78
#define __NR3264_fstatat 79
#define __NR3264_fstat 80
#define __NR_timerfd_create 85
#define __NR_timerfd_settime 86
#define __NR_timerfd_gettime 87
#define __NR_utimensat 88
#define __NR_capget 90
#define __NR_exit 93
#define __NR_exit_group 94
#define __NR_set_tid_address 96
#define __NR_futex 98
#define __NR_set_robust_list 99
#define __NR_getitimer 102
#define __NR_setitimer 103
#define __NR_clock_gettime 113
#define __NR_clock_nanosleep 115
#define __NR_syslog 116
#define __NR_ptrace 117
#define __NR_sched_setaffinity 122
#define __NR_sched_yield 124
#define __NR_kill 129
#define __NR_tkill 130
#define __NR_tgkill 131
#define __NR_sigaltstack 132
#define __NR_rt_sigsuspend 133
#define __NR_rt_sigaction 134
#define __NR_rt_sigprocmask 135
#define __NR_rt_sigtimedwait 137
#define __NR_rt_sigqueueinfo 138
#define __NR_rt_sigreturn 139
#define __NR_setregid 143
#define __NR_setgid 144
#define __NR_setreuid 145
#define __NR_setuid 146
#define __NR_setresuid 147
#define __NR_getresuid 148
#define __NR_setresgid 149
#define __NR_getresgid 150
#define __NR_setfsuid 151
#define __NR_setfsgid 152
#define __NR_times 153
#define __NR_setpgid 154
#define __NR_getpgid 155
#define __NR_setsid 157
#define __NR_getgroups 158
#define __NR_setgroups 159
#define __NR_uname 160
#define __NR_getrusage 165
#define __NR_umask 166
#define __NR_prctl 167
#define __NR_gettimeofday 169
#define __NR_getpid 172
#define __NR_getppid 173
#define __NR_getuid 174
#define __NR_geteuid 175
#define __NR_getgid 176
#define __NR_getegid 177
#define __NR_gettid 178
#define __NR_sysinfo 179
#define __NR_mq_open 180
#define __NR_mq_unlink 181
#define __NR_mq_timedsend 182
#define __NR_mq_timedreceive 183
#define __NR_mq_notify 184
#define __NR_mq_getsetattr 185
#define __NR_semget 190
#define __NR_semctl 191
#define __NR_semtimedop 192
#define __NR_shmget 194
#define __NR_shmctl 195
#define __NR_shmat 196
#define __NR_socket 198
#define __NR_socketpair 199
#define __NR_bind 200
#define __NR_listen 201
#define __NR_accept 202
#define __NR_connect 203
#define __NR_getsockname 204
#define __NR_getpeername 205
#define __NR_sendto 206
#define __NR_recvfrom 207
#define __NR_setsockopt 208
#define __NR_getsockopt 209
#define __NR_sendmsg 211
#define __NR_recvmsg 212
#define __NR_brk 214
#define __NR_munmap 215
#define __NR_mremap 216
#define __NR_clone 220
#define __NR_execve 221
#define __NR3264_mmap 222
#define __NR_mprotect 226
#define __NR_msync 227
#define __NR_mincore 232
#define __NR_madvise 233
#define __NR_arch_specific_syscall 244
#define __NR_wait4 260
#define __NR_prlimit64 261
#define __NR_process_vm_readv 270
#define __NR_process_vm_writev 271
#define __NR_renameat2 276
#define __NR_getrandom 278
#define __NR_memfd_create 279
#define __NR_execveat 281
#define __NR_membarrier 283
#define __NR_copy_file_range 285
#define __NR_preadv2 286
#define __NR_pwritev2 287
#define __NR_statx 291

#define __NR_fcntl __NR3264_fcntl
#define __NR_lseek __NR3264_lseek
#define __NR_newfstatat __NR3264_fstatat
#define __NR_fstat __NR3264_fstat
#define __NR_mmap __NR3264_mmap

#define __NR_riscv_flush_icache (__NR_arch_specific_syscall + 15)

#endif /* __VKI_SCNUMS_RISCV64_LINUX_H */

/*--------------------------------------------------------------------*/
/*--- end                               vki-scnums-riscv64-linux.h ---*/
/*--------------------------------------------------------------------*/
