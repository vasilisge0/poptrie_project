#
# Copyright (c) 2016,2018 Hirochika Asai.
# All rights reserved.
#
# Authors:
#      Hirochika Asai  <asai@jar.jp>
#

AC_PREREQ([2.71])
AC_INIT([radix-tree],[0.1.0],[asai@jar.jp])
AC_CONFIG_SRCDIR([radix.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([subdir-objects])
AC_SUBST(SUBDIRS)

# arguments
AC_ARG_ENABLE(debug,
  [  --enable-debug    Turn on debugging [default no]],
  [case "${enableval}" in
    yes) debug=yes; AC_MSG_RESULT(Checking for debug... yes); AC_DEFINE(DEBUG, 1, debugging option) ;;
    no)  debug=no;;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
  esac],[debug=no])
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
#AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
#AC_CHECK_FUNCS([])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
