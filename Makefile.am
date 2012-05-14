ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(top_srcdir)/include

MAINTAINERCLEANFILES = \
	Makefile.in

lib_LTLIBRARIES = libkern.la
libkern_la_SOURCES = \
	lib/atomic.c \
	lib/bitmap.c \
	lib/bitops.c \
	lib/rbtree.c
libkern_la_LDFLAGS = -version-info 0:0:0
pkginclude_HEADERS = \
	include/atomic.h \
	include/bitmap.h \
	include/bitops.h \
	include/common.h \
	include/compiler.h \
	include/hash.h \
	include/hlist.h \
	include/htable.h \
	include/jhash.h \
	include/kernel.h \
	include/lheap.h \
	include/list.h \
	include/log2.h \
	include/rbtree.h
pkgconfig_DATA = libkern.pc
pkgconfigdir = $(libdir)/pkgconfig

include aminclude.am

TESTS =
TESTS_ENVIRONMENT = \
	TEST_SOURCE_DIR=$(srcdir)/tests \
	TEST_BUILD_DIR=$(top_builddir)/tests

check_PROGRAMS =
check_SCRIPTS =

unit_test_CPPFLAGS = -I$(top_srcdir)/tests -DUNIT_TESTING=1

TESTS += tests/list_test
check_PROGRAMS += tests/list_test
tests_list_test_SOURCES = tests/list_test.c
tests_list_test_CPPFLAGS = $(unit_test_CPPFLAGS)
tests_list_test_LDADD = $(top_builddir)/libkern.la

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

EXTRA_DIST = autogen.sh libtool $(SCRIPTS)
