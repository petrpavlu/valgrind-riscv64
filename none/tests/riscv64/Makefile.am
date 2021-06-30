
include $(top_srcdir)/Makefile.tool-tests.am

dist_noinst_SCRIPTS = filter_stderr

EXTRA_DIST = \
	atomic.stdout.exp atomic.stderr.exp atomic.vgtest \
	compressed.stdout.exp compressed.stderr.exp compressed.vgtest \
	integer.stdout.exp integer.stderr.exp integer.vgtest \
	muldiv.stdout.exp muldiv.stderr.exp muldiv.vgtest

check_PROGRAMS = \
	allexec \
	atomic \
	compressed \
	integer \
	muldiv

AM_CFLAGS    += @FLAG_M64@
AM_CXXFLAGS  += @FLAG_M64@
AM_CCASFLAGS += @FLAG_M64@

allexec_CFLAGS = $(AM_CFLAGS) @FLAG_W_NO_NONNULL@
