
/*--------------------------------------------------------------------*/
/*--- begin                                 libvex_guest_riscv64.h ---*/
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

#ifndef __LIBVEX_PUB_GUEST_RISCV64_H
#define __LIBVEX_PUB_GUEST_RISCV64_H

#include "libvex_basictypes.h"

/*------------------------------------------------------------*/
/*--- Vex's representation of the riscv64 CPU state.       ---*/
/*------------------------------------------------------------*/

typedef struct {
   /*   0 */ ULong host_EvC_FAILADDR;
   /*   8 */ UInt  host_EvC_COUNTER;
   /*  12 */ UInt  pad0;
   /*  16 */ ULong guest_x0;
   /*  24 */ ULong guest_x1;
   /*  32 */ ULong guest_x2;
   /*  40 */ ULong guest_x3;
   /*  48 */ ULong guest_x4;
   /*  56 */ ULong guest_x5;
   /*  64 */ ULong guest_x6;
   /*  72 */ ULong guest_x7;
   /*  80 */ ULong guest_x8;
   /*  88 */ ULong guest_x9;
   /*  96 */ ULong guest_x10;
   /* 104 */ ULong guest_x11;
   /* 112 */ ULong guest_x12;
   /* 120 */ ULong guest_x13;
   /* 128 */ ULong guest_x14;
   /* 136 */ ULong guest_x15;
   /* 144 */ ULong guest_x16;
   /* 152 */ ULong guest_x17;
   /* 160 */ ULong guest_x18;
   /* 168 */ ULong guest_x19;
   /* 176 */ ULong guest_x20;
   /* 184 */ ULong guest_x21;
   /* 192 */ ULong guest_x22;
   /* 200 */ ULong guest_x23;
   /* 208 */ ULong guest_x24;
   /* 216 */ ULong guest_x25;
   /* 224 */ ULong guest_x26;
   /* 232 */ ULong guest_x27;
   /* 240 */ ULong guest_x28;
   /* 248 */ ULong guest_x29;
   /* 256 */ ULong guest_x30;
   /* 264 */ ULong guest_x31;
   /* 272 */ ULong guest_pc;

   /* Various pseudo-regs mandated by Vex or Valgrind. */
   /* Emulation notes. */
   /* 280 */ UInt guest_EMNOTE;
   /* 284 */ UInt pad1;

   /* Translation-invalidation area description. Not used on riscv64 (there is
      no invalidate-icache insn), but needed so as to allow users of the library
      to uniformly assume that the guest state contains these two fields --
      otherwise there is compilation breakage. On riscv64, these two fields are
      set to zero by LibVEX_GuestRISCV64_initialise() and then should be ignored
      forever thereafter. */
   /* 288 */ ULong guest_CMSTART;
   /* 296 */ ULong guest_CMLEN;

   /* Used to record the unredirected guest address at the start of a
      translation whose start has been redirected. By reading this
      pseudo-register shortly afterwards, the translation can find out what the
      corresponding no-redirection address was. Note, this is only set for
      wrap-style redirects, not for replace-style ones. */
   /* 304 */ ULong guest_NRADDR;

   /* Needed for Darwin: pc at the last epc insn. Used when backing up to
      restart a syscall that has been interrupted by a signal. */
   /* 312 */ ULong guest_IP_AT_SYSCALL;

   /* Fallback LL/SC support. */
   /* 320 */ ULong guest_LLSC_SIZE; /* 0==no transaction, else 4 or 8. */
   /* 328 */ ULong guest_LLSC_ADDR; /* Address of the transaction. */
   /* 336 */ ULong guest_LLSC_DATA; /* Original value at ADDR, sign-extended. */

   /* Padding to 16 bytes. */
   /* 344 */ ULong pad_end_0;
   /* 352 */
} VexGuestRISCV64State;

/*------------------------------------------------------------*/
/*--- Utility functions for riscv64 guest stuff.           ---*/
/*------------------------------------------------------------*/

/* ALL THE FOLLOWING ARE VISIBLE TO LIBRARY CLIENT */

/* Initialise all guest riscv64 state. */
void LibVEX_GuestRISCV64_initialise(/*OUT*/ VexGuestRISCV64State* vex_state);

#endif /* ndef __LIBVEX_PUB_GUEST_RISCV64_H */

/*--------------------------------------------------------------------*/
/*---                                       libvex_guest_riscv64.h ---*/
/*--------------------------------------------------------------------*/
