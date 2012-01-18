dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.62)
AC_INIT([liblangtag], 0.1, [http://github.com/tagoh/liblangtag/issues])

. `dirname $0`/requires

AM_INIT_AUTOMAKE([1.11 -Wno-portability])
AM_MAINTAINER_MODE
AM_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4macros])

AC_PROG_LIBTOOL
AC_PROG_CC

GNOME_COMPILE_WARNINGS

GNOME_COMMON_INIT
GNOME_DEBUG_CHECK
GNOME_MAINTAINER_MODE_DEFINES

CFLAGS="$CFLAGS $WARN_CFLAGS"

dnl ======================================================================
dnl Libraries versioning
dnl ======================================================================
dnl Quote from Updating library version information at libtool.info
dnl and renumbering
dnl
dnl 1. Update the version information only immediately before a public
dnl    release of your software.  More frequent updates are unnecessary,
dnl    and only guarantee that the current interface number gets larger
dnl    faster.
dnl 2. If the library source code has changed at all since the last
dnl    update, then increment REVISION (`C:R:A' becomes `C:r+1:A')
dnl 3. If any interfaces have been added, removed, or changed since the
dnl    last update, increment CURRENT, and set REVISION to 0.
dnl 4. If any interfaces have been added since the last public release,
dnl    then increment AGE.
dnl 5. If any interfaces have been removed since the last public release,
dnl    then set AGE to 0.
dnl

dnl for liblangtag.la
LT_CURRENT=0
LT_REVISION=0
LT_AGE=0

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

dnl for liblangtag-gobject.la
LT_G_CURRENT=0
LT_G_REVISION=0
LT_G_AGE=0

AC_SUBST(LT_G_CURRENT)
AC_SUBST(LT_G_REVISION)
AC_SUBST(LT_G_AGE)

dnl ======================================================================
dnl define variables
dnl ======================================================================

dnl ======================================================================
dnl functions testing
dnl ======================================================================

dnl ======================================================================
dnl gettext stuff
dnl ======================================================================
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package])

AM_GLIB_GNU_GETTEXT

dnl ======================================================================
dnl check pkg-config stuff
dnl ======================================================================
PKG_CHECK_MODULES(LIBXML2, libxml-2.0 >= $LIBXML2_REQUIRED)
PKG_CHECK_MODULES(GOBJECT, gobject-2.0 >= $GOBJECT_REQUIRED, has_gobject=yes, has_gobject=no)
PKG_CHECK_MODULES(CHECK, check >= $CHECK_REQUIRED,
			 [use_check="yes"],
			 [use_check="no"])

AM_CONDITIONAL(ENABLE_GOBJECT, test x$has_gobject != xno)
AM_CONDITIONAL(ENABLE_UNIT_TEST, test x$use_check != xno)

dnl ======================================================================
dnl check another libraries
dnl ======================================================================
AM_PATH_GLIB_2_0($GLIB_REQUIRED, :,
	AC_MSG_ERROR([
*** GLIB $GLIB_REQUIRED_VERSION or better is required. The latest version of
*** GLIB is always available from ftp://ftp.gtk.org/.]),
    glib)

GOBJECT_INTROSPECTION_CHECK([1.30.0])
GTK_DOC_CHECK(1.0)

dnl ======================================================================
dnl options
dnl ======================================================================

dnl ======================================================================
dnl output
dnl ======================================================================
AC_CONFIG_FILES([
	Makefile
	data/Makefile
	docs/Makefile
	docs/version.xml
	liblangtag/Makefile
	liblangtag-gobject/Makefile
	liblangtag.pc
	liblangtag-uninstalled.pc
	liblangtag-gobject.pc
	liblangtag-gobject-uninstalled.pc
	tests/Makefile
])
AC_OUTPUT

dnl ======================================================================
dnl result
dnl ======================================================================
echo ""
echo "========== Build Information =========="
echo " CFLAGS:                 $CFLAGS"
echo " LDFLAGS:                $LDFLAGS"
