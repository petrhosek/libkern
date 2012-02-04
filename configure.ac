dnl Process this file with autoconf to produce a configure script
AC_PREREQ([2.59])
AC_INIT([libase], [0.1], [p.hosek@imperial.ac.uk])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AM_MAINTAINER_MODE
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

dnl Operating system checks
AC_MSG_CHECKING([for supported operating system])
case "${host_os}" in
linux*)
	HOST_OS=linux
	AC_DEFINE([LINUX], 1, [Define for the Linux kernel.])
	;;
*)
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([Operating system ${host_os} not supported])
	;;
esac
AC_SUBST([HOST_OS])
AM_CONDITIONAL([LINUX], [test x$HOST_OS = xlinux])

dnl System architecture checks
AC_MSG_CHECKING([for supported architecture])
case "${host_cpu}" in
i[[3456]]86|pentium)
	HOST_CPU=x86
	AC_DEFINE([X86], 1, [Define for the i386 architecture.])
;;
x86?64*|amd64)
	HOST_CPU=x86_64
	AC_DEFINE([X86_64], 1, [Define for the AMD x86-64 architecture.])
;;
*)
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([Architecture ${host_cpu} not supported])
esac
AC_SUBST([HOST_CPU])
AM_CONDITIONAL([X86], [test x$HOST_CPU = xx86])
AM_CONDITIONAL([X86_64], [test x$HOST_CPU = xx86_64])

dnl Checks for typedefs
AC_TYPE_SIZE_T

dnl Check for sizes
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([long long])

dnl Checks for header files
AC_HEADER_ASSERT
AC_HEADER_STDBOOL
AC_HEADER_STDC
AC_HEADER_TIME

dnl Checks for library functions
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(select socket waitpid pread pwrite)
if test x$ac_cv_func_strerror = xno; then
  AC_CHECK_LIB(iberty, main)
fi

AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_FILES([Makefile])

dnl Doxygen documentation
DX_INIT_DOXYGEN([libase], [doxygen.cfg])
DX_DOXYGEN_FEATURE([ON])
DX_PS_FEATURE([OFF])

AC_OUTPUT
