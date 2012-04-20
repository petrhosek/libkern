dnl Process this file with autoconf to produce a configure script
AC_PREREQ([2.60])
AC_INIT([libkern], [0.1], [p.hosek@imperial.ac.uk])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 foreign subdir-objects color-tests -Wall -Werror])
AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])
LT_INIT

AC_LANG([C])

dnl Toolchain checks
AC_PROG_CC_C99
if test x"$ac_cv_prog_cc_c99" = x"no"; then
	AC_MSG_ERROR([C compiler supporting C99 required])
fi
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
AM_PROG_AS

dnl Checks for typedefs
AC_TYPE_SIZE_T

dnl Check for sizes
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])
AC_CHECK_SIZEOF([size_t])

dnl Checks for header files
AC_HEADER_ASSERT
AC_HEADER_STDBOOL
AC_HEADER_STDC

AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_FILES([Makefile libkern.pc])

dnl Doxygen documentation
DX_INIT_DOXYGEN([libkern], [doxygen.cfg])
DX_DOXYGEN_FEATURE([ON])
DX_PS_FEATURE([OFF])

AC_OUTPUT
