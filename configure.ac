AC_INIT([indicator-xkbmod], [0.1], [abdellahchelli@gmail.com])

AC_CONFIG_SRCDIR([src/indicator-xkbmod.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])

AM_INIT_AUTOMAKE([foreign -Wall])

PKG_PROG_PKG_CONFIG
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL

PKG_CHECK_MODULES([DEPS], [appindicator-0.1])

AC_CHECK_HEADERS([string.h X11/XKBlib.h])

AC_CONFIG_FILES([Makefile src/Makefile data/Makefile])

AC_OUTPUT
