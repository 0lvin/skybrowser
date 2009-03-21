#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([skybrowser],[0.3],[pauk.denis@gmail.com])
AC_CONFIG_SRCDIR([src/skybrowser.c])

AM_INIT_AUTOMAKE

AC_CONFIG_HEADER([config.h])

# Checks for programs.

AC_SEARCH_LIBS([strerror],[cposix])
AC_PROG_CC
AC_HEADER_STDC
DOLT

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h])

PKG_CHECK_MODULES(SOUP, libsoup-2.4 >= 2.4.1, have_soup="yes", have_soup="no")
AM_CONDITIONAL(HAVE_SOUP, test x$have_soup != xno)
AC_SUBST(SOUP_CFLAGS)
AC_SUBST(SOUP_LIBS)

GTKHTML_MODULES="gio-2.0 >= 2.18.0 gtk+-unix-print-2.0 >= 2.10.0 libgtkhtml-3.14"

PKG_CHECK_MODULES(GTKHTML, $GTKHTML_MODULES)
AC_SUBST(GTKHTML_CFLAGS)
AC_SUBST(GTKHTML_LIBS)
AC_SUBST(GTKHTML_MODULES)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_HEADER_STDC
AC_FUNC_MMAP
AC_CHECK_FUNCS([getcwd munmap strchr strcspn strstr])

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
