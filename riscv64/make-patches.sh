#!/bin/bash
#
# Split riscv64 changes into a patch series.

valgrind_new="
coregrind/m_dispatch/dispatch-riscv64-linux.S
coregrind/m_gdbserver/riscv64-*.xml
coregrind/m_gdbserver/valgrind-low-riscv64.c
coregrind/m_sigframe/sigframe-riscv64-linux.c
coregrind/m_syswrap/syscall-riscv64-linux.S
coregrind/m_syswrap/syswrap-riscv64-linux.c
include/vki/vki-posixtypes-riscv64-linux.h
include/vki/vki-riscv64-linux.h
include/vki/vki-scnums-riscv64-linux.h
"

vex_new="
VEX/priv/guest_riscv64_defs.h
VEX/priv/guest_riscv64_helpers.c
VEX/priv/guest_riscv64_toIR.c
VEX/priv/host_riscv64_defs.c
VEX/priv/host_riscv64_defs.h
VEX/priv/host_riscv64_isel.c
VEX/pub/libvex_guest_riscv64.h
"

tests_new="
memcheck/tests/riscv64-linux/
none/tests/riscv64/
"

valgrind_mod="
.gitignore
Makefile.all.am
Makefile.tool.am
Makefile.vex.am
cachegrind/cg_arch.c
cachegrind/cg_branchpred.c
configure.ac
coregrind/Makefile.am
coregrind/launcher-linux.c
coregrind/m_aspacemgr/aspacemgr-common.c
coregrind/m_cache.c
coregrind/m_coredump/coredump-elf.c
coregrind/m_debuginfo/d3basics.c
coregrind/m_debuginfo/debuginfo.c
coregrind/m_debuginfo/priv_storage.h
coregrind/m_debuginfo/readdwarf.c
coregrind/m_debuginfo/readelf.c
coregrind/m_debuginfo/storage.c
coregrind/m_debuglog.c
coregrind/m_gdbserver/target.c
coregrind/m_gdbserver/valgrind_low.h
coregrind/m_initimg/initimg-linux.c
coregrind/m_libcassert.c
coregrind/m_libcfile.c
coregrind/m_libcproc.c
coregrind/m_machine.c
coregrind/m_main.c
coregrind/m_options.c
coregrind/m_redir.c
coregrind/m_scheduler/scheduler.c
coregrind/m_signals.c
coregrind/m_stacktrace.c
coregrind/m_syscall.c
coregrind/m_syswrap/priv_syswrap-linux.h
coregrind/m_syswrap/priv_types_n_macros.h
coregrind/m_syswrap/syswrap-generic.c
coregrind/m_syswrap/syswrap-linux.c
coregrind/m_syswrap/syswrap-main.c
coregrind/m_trampoline.S
coregrind/m_translate.c
coregrind/pub_core_basics.h
coregrind/pub_core_debuginfo.h
coregrind/pub_core_machine.h
coregrind/pub_core_mallocfree.h
coregrind/pub_core_syscall.h
coregrind/pub_core_trampoline.h
coregrind/pub_core_transtab.h
coregrind/pub_core_transtab_asm.h
coregrind/vgdb-invoker-ptrace.c
drd/drd_bitmap.h
drd/drd_load_store.c
include/Makefile.am
include/pub_tool_basics.h
include/pub_tool_guest.h
include/pub_tool_machine.h
include/pub_tool_redir.h
include/pub_tool_vkiscnums_asm.h
include/valgrind.h.in
include/vki/vki-linux.h
memcheck/mc_machine.c
"

vex_mod="
VEX/Makefile-gcc
VEX/auxprogs/genoffsets.c
VEX/priv/host_generic_regs.h
VEX/priv/main_main.c
VEX/priv/main_util.h
VEX/pub/libvex.h
VEX/pub/libvex_basictypes.h
VEX/pub/libvex_ir.h
VEX/useful/test_main.c
"

tests_mod="
helgrind/tests/annotate_hbefore.c
helgrind/tests/tc07_hbl1.c
helgrind/tests/tc08_hbl2.c
helgrind/tests/tc11_XCHG.c
memcheck/tests/Makefile.am
memcheck/tests/atomic_incs.c
memcheck/tests/leak-segv-jmp.c
memcheck/tests/leak-segv-jmp.stderr.exp
memcheck/tests/leak.h
none/tests/Makefile.am
none/tests/allexec_prepare_prereq
none/tests/faultstatus.c
none/tests/libvex_test.c
tests/arch_test.c
tests/platform_test
"

# Parse command line arguments.
version=v1
while getopts 'v:' opt; do
   case $opt in
      v)
         version="v$OPTARG"
         ;;
      ?)
         exit 1
         ;;
   esac
done
shift $((OPTIND - 1))
if [ $# -gt 0 ]; then
   echo "$0: illegal non-option argument: $1" >&2
   exit 1
fi

# Generate the patches.
function patch() {
   local index=$1
   local subject=$2
   local authors=$3
   local files=$4

   echo "From 0000000000000000000000000000000000000000 Mon Sep 17 00:00:00 2001"
   echo "From: Petr Pavlu <petr.pavlu@dagobah.cz>"
   echo "Date: $(date --rfc-email)"
   echo "Subject: [PATCH $version $index/6] riscv64: Add initial support: $subject"
   echo ""
   echo "The following people contributed to the initial RISC-V support:"
   echo "$authors"
   echo "---"
   git diff --stat --patch master..riscv64-linux -- $files
}

authors=$(
  git log --pretty=format:"%aN <%aE>" --no-merges master..riscv64-linux | \
  sort | uniq --count | sort --numeric --reverse | cut --characters=9- )

patch 1 "new port-specific Valgrind files" "$authors" "$valgrind_new" \
   > "$version-0001-riscv64-valgrind-new.patch"
echo "Wrote $version-0001-riscv64-valgrind-new.patch."

patch 2 "new port-specific VEX files" "$authors" "$vex_new" \
   > "$version-0002-riscv64-vex-new.patch"
echo "Wrote $version-0002-riscv64-vex-new.patch."

patch 3 "new port-specific test files" "$authors" "$tests_new" \
   > "$version-0003-riscv64-tests-new.patch"
echo "Wrote $version-0003-riscv64-tests-new.patch."

patch 4 "Valgrind modifications" "$authors" "$valgrind_mod" \
   > "$version-0004-riscv64-valgrind-mod.patch"
echo "Wrote $version-0004-riscv64-valgrind-mod.patch."

patch 5 "VEX modifications" "$authors" "$vex_mod" \
   > "$version-0005-riscv64-vex-mod.patch"
echo "Wrote $version-0005-riscv64-vex-mod.patch."

patch 6 "test modifications" "$authors" "$tests_mod" \
   > "$version-0006-riscv64-tests-mod.patch"
echo "Wrote $version-0006-riscv64-tests-mod.patch."

# Print modified/new files which are not included in any patch.
changed=$(git diff master..riscv64-linux | \
   sed --quiet --expression='s,^\+\+\+ b/,,p' | sort)
matched=$(sed --quiet --expression='s,^\+\+\+ b/,,p' \
   "$version-0001-riscv64-valgrind-new.patch" \
   "$version-0002-riscv64-vex-new.patch" \
   "$version-0003-riscv64-tests-new.patch" \
   "$version-0004-riscv64-valgrind-mod.patch" \
   "$version-0005-riscv64-vex-mod.patch" \
   "$version-0006-riscv64-tests-mod.patch" | \
   sort)

echo "Modified/new files not included in any patch:"
echo ${changed[@]} ${matched[@]} | tr ' ' '\n' | sort | uniq --unique | \
   sed 's/^/   /'
