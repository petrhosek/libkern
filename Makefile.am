ACLOCAL_AMFLAGS = -I m4

MAINTAINERCLEANFILES = \
	Makefile.in

lib_LTLIBRARIES = libase.la
libase_la_SOURCES = \
	lib/bitmap.c \
	lib/bitops.c \
	lib/rbtree.c
pkginclude_HEADERS = \
	include/bitmap.h \
	include/bitops.h \
	include/hash.h \
	include/hlist.h \
	include/htable.h \
	include/kernel.h \
	include/lheap.h \
	include/list.h \
	include/rbtree.h

EXTRA_DIST = autogen.sh
