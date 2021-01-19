
/*--------------------------------------------------------------------*/
/*--- Create/destroy signal delivery frames.                       ---*/
/*---                                     sigframe-riscv64-linux.c ---*/
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
#include "pub_core_threadstate.h"
#include "pub_core_aspacemgr.h"
#include "pub_core_libcbase.h"
#include "pub_core_libcassert.h"
#include "pub_core_libcprint.h"
#include "pub_core_machine.h"
#include "pub_core_options.h"
#include "pub_core_sigframe.h"
#include "pub_core_signals.h"
#include "pub_core_tooliface.h"
#include "pub_core_trampoline.h"
#include "priv_sigframe.h"
/* TODO Review and implement. */

/* EXPORTED */
void VG_(sigframe_create)( ThreadId tid,
                           Bool on_altstack,
                           Addr sp_top_of_frame,
                           const vki_siginfo_t *siginfo,
                           const struct vki_ucontext *siguc,
                           void *handler,
                           UInt flags,
                           const vki_sigset_t *mask,
                           void *restorer )
{
   I_die_here;
}

/*------------------------------------------------------------*/
/*--- Destroying signal frames                             ---*/
/*------------------------------------------------------------*/

/* EXPORTED */
void VG_(sigframe_destroy)( ThreadId tid, Bool isRT )
{
   I_die_here;
}

#endif // defined(VGP_riscv64_linux)

/*--------------------------------------------------------------------*/
/*--- end                                 sigframe-riscv64-linux.c ---*/
/*--------------------------------------------------------------------*/
