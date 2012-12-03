dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.62)
AC_INIT([liblangtag], 0.4.0, [http://github.com/tagoh/liblangtag/issues])

. `dirname $0`/requires

AM_INIT_AUTOMAKE([1.11 -Wno-portability])
dnl Support silent build rules, requires at least automake-1.11.
dnl Enable by either passing --enable-silent-rules to configure or
dnl passing V=0 to make.
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

m4_pattern_allow(^LT_.*_DECLS$)
m4_pattern_allow(^LT_POINTER_.*$)

AM_MAINTAINER_MODE
AM_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4macros])

AC_PROG_LIBTOOL
AC_PROG_CC

GNOME_COMPILE_WARNINGS

GNOME_COMMON_INIT
GNOME_DEBUG_CHECK
GNOME_MAINTAINER_MODE_DEFINES

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
dnl    then set CURRENT to the so number +1 and set AGE to 0.
dnl

dnl for liblangtag.la
LT_CURRENT=1
LT_REVISION=0
LT_AGE=0

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

dnl for liblangtag-gobject.la
LT_G_CURRENT=2
LT_G_REVISION=0
LT_G_AGE=2

AC_SUBST(LT_G_CURRENT)
AC_SUBST(LT_G_REVISION)
AC_SUBST(LT_G_AGE)

dnl ======================================================================
dnl define variables
dnl ======================================================================
LANGTAG_MODULEDIR="${libdir}/liblangtag"
AC_SUBST(LANGTAG_MODULEDIR)

AM_CONDITIONAL(CROSS_COMPILING, test $cross_compiling = yes)

dnl check for flavours of varargs macros
AC_MSG_CHECKING(for ISO C99 varargs macros in C)
_save_cflags=$CFLAGS
CFLAGS=$(sed -e 's/\-Werror//g')
AC_TRY_COMPILE([],[
int a(int p1, int p2, int p3);
#define call_a(...) a(1,__VA_ARGS__)
call_a(2,3);
],[have_iso_c_varargs=yes;AC_DEFINE(LT_HAVE_ISO_VARARGS, 1, [Use ISO C99 varargs])],[have_iso_c_varargs=no])
CFLAGS=$_save_cflags
AC_MSG_RESULT($have_iso_c_varargs)

AC_MSG_CHECKING(for GNUC varargs macros)
_save_cflags=$CFLAGS
CFLAGS=$(sed -e 's/\-Werror//g')
AC_TRY_COMPILE([],[
int a(int p1, int p2, int p3);
#define call_a(params...) a(1,params)
call_a(2,3);
],[have_gnuc_varargs=yes;AC_DEFINE(LT_HAVE_GNUC_VARARGS, 1, [Use GNUC varargs macro])],[have_gnuc_varargs=no])
CFLAGS=$_save_cflags
AC_MSG_RESULT($have_gnuc_varargs)

dnl inline
AC_CACHE_CHECK([for __inline], [lt_cv_has__inline],
	[AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
	__inline int foo(void);
	__inline int foo() {return 0;}
	int main() {return foo();}
	]])],
	[lt_cv_has__inline=yes],
	[lt_cv_has__inline=no], [])
])
case x$lt_cv_has__inline in
xyes)
	AC_DEFINE(LT_HAVE___INLINE,1,[Have __inline keyword])
esac
AC_CACHE_CHECK([for __inline__], [lt_cv_has__inline__],
	[AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
	__inline__ int foo(void);
	__inline__ int foo() {return 0;}
	int main() {return foo();}
	]])],
	[lt_cv_has__inline__=yes],
	[lt_cv_has__inline__=no], [])
])
case x$lt_cv_has__inline__ in
xyes)
	AC_DEFINE(LT_HAVE___INLINE__,1,[Have __inline__ keyword])
esac
AC_CACHE_CHECK([for inline], [lt_cv_hasinline],
	[AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
	#undef inline
	inline int foo(void);
	inline int foo() {return 0;}
	int main() {return foo();}
	]])],
	[lt_cv_hasinline=yes],
	[lt_cv_hasinline=no], [])
])
case x$lt_cv_hasinline in
xyes)
	AC_DEFINE(LT_HAVE_INLINE,1,[Have inline keyword])
esac
AC_MSG_CHECKING(if inline functions in headers work)
AC_LINK_IFELSE([AC_LANG_SOURCE([[
#if defined (LT_HAVE_INLINE) && defined (__GNUC__) && defined (__STRICT_ANSI__)
#undef inline
#define inline __inline__
#elif !defined (LT_HAVE_INLINE)
#undef inline
#if defined (LT_HAVE___INLINE__)
#define inline __inline__
#elif defined (LT_HAVE___INLINE)
#define inline __inline
#endif
#endif

int lt_test_func2(int);

static inline int
lt_test_func1(void) {
  return lt_test_func2(1);
}
int main(void) {int i = 1; return --i;}]])],[lt_can_inline=yes;AC_DEFINE(LT_CAN_INLINE, 1, [Wheter inline statement can be used])],[lt_can_inline=no])
AC_MSG_RESULT($lt_can_inline)

dnl ---build-in atomic functions---
AC_CACHE_CHECK([for gcc atomic builtins], [lt_cv_has_atomic],
	[AC_LINK_IFELSE([AC_LANG_PROGRAM([], [[
int i, j = 0;
i = __sync_fetch_and_add(&j, 1);
__sync_synchronize();
j = __sync_fetch_and_sub(&i, 1);
return j;
       ]])], [lt_cv_has_atomic=yes],
       [_save_cflags="$CFLAGS"
       CFLAGS="$CFLAGS -march=i486"
       AC_LINK_IFELSE([AC_LANG_PROGRAM([], [[
int i, j = 0;
i = __sync_fetch_and_add(&j, 1);
__sync_synchronize();
j = __sync_fetch_and_sub(&i, 1);
return j;
       ]])], [AC_MSG_ERROR([liblangtag has to be built with -march=i486 or later.])])
       # the above code is just to inform the flag is required to be built.
       # so regardless of that, we deal with no-atomic-function here.
       CFLAGS="$_save_cflags"
       lt_cv_has_atomic=no])
])
if test "x$lt_cv_has_atomic" = "xyes"; then
	AC_DEFINE(LT_HAVE_ATOMIC_BUILTINS, 1, [Have buit-in atomic functions])
fi

dnl ---size---
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(void *)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

dnl ---alignment---
AC_CHECK_ALIGNOF(void *)

dnl ======================================================================
dnl functions testing
dnl ======================================================================
AX_CREATE_STDINT_H([liblangtag/lt-stdint.h])
AC_CHECK_HEADERS([dirent.h execinfo.h libgen.h sys/param.h])
AC_CHECK_FUNCS([backtrace dlopen strndup vasprintf vsnprintf])
AC_CHECK_VA_COPY

if test "x$ac_cv_func_dlopen" != xyes; then
	enable_modules=no
fi
if test "x$ac_cv_func_vsnprintf" = xyes; then
	AC_CACHE_CHECK([Whether vsnprintf is C99-compliant], [lt_cv_c99_vsnprintf],
		[AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <stdarg.h>
int
foo(char *format, ...)
{
	va_list ap;
	char c;
	int retval = 0;

	va_start(ap, format);
	if (vsnprintf(&c, 1, format, ap) < 0) {
		retval = -1;
	}
	va_end(ap);

	return retval;
}
int
main(void)
{
	char c;

	return foo("foo: %s", "bar");
}]])],
		[lt_cv_c99_vsnprintf=yes],[lt_cv_c99_vsnprintf=no],[lt_cv_c99_vsnprintf=no])])
	if test "x$lt_cv_c99_vsnprintf" = xyes; then
		AC_DEFINE(LT_HAVE_C99_VSNPRINTF, 1, [Have C99-compliant vsnprintf])
	fi
fi

dnl ======================================================================
dnl gettext stuff
dnl ======================================================================
dnl GETTEXT_PACKAGE=$PACKAGE
dnl AC_SUBST(GETTEXT_PACKAGE)
dnl AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package])
dnl 
dnl AM_GLIB_GNU_GETTEXT

dnl ======================================================================
dnl options
dnl ======================================================================
AC_ARG_WITH([locale-alias],
	[AC_HELP_STRING([--with-locale-alias], [set the path to licale.alias])],
	[with_locale_alias="$withval"],
	[with_locale_alias="${localedir}/locale.alias"])
AC_ARG_ENABLE([rebuild-locale-alias],
	[AC_HELP_STRING([--disable-rebuild-locale-alias],
		[disable rebuilding the locale.alias mapping table])],
	[enable_rebuild_locale_alias="$enableval"],
	[enable_rebuild_locale_alias=yes])
AC_ARG_ENABLE([test],
	[AC_HELP_STRING([--disable-test], [Disable tests])],
	[enable_test="$enableval"],
	[enable_test=yes])
AC_ARG_ENABLE([modules],
	[AC_HELP_STRING([--disable-modules], [Disable modules])],
	[enable_modules="$enableval"],
	[enable_modules=yes])
AC_ARG_ENABLE([rebuild-data],
	[AC_HELP_STRING([--disable-rebuild-data],
		[disable rebuilding the xml data])],
	[enable_rebuild_data="$enableval"],
	[enable_rebuild_data=yes])

dnl ======================================================================
dnl options - locale-alias
dnl ======================================================================
AC_MSG_CHECKING(existence of locale.alias)
if test "x$with_locale_alias" = xyes; then
	with_locale_alias="${localedir}/locale.alias"
fi
ac_lt_file="$with_locale_alias"
ac_lt_file_old=""

while test "$ac_lt_file_old" != "$ac_lt_file"; do
	ac_lt_file_old="$ac_lt_file"
	eval ac_lt_file="\"$ac_lt_file\""
done
if [[ -f "$ac_lt_file" ]]; then
	AC_MSG_RESULT($with_locale_alias)
	LOCALE_ALIAS=$with_locale_alias
	AC_SUBST(LOCALE_ALIAS)
else
	AC_MSG_RESULT([Using the prebuilt table])
fi

dnl ======================================================================
dnl options - rebuild-locale-alias
dnl ======================================================================
BUILD_LOCALEALIAS=\#
if test "x$enable_rebuild_locale_alias" = "xyes" -a "x$LOCALE_ALIAS" != "x"; then
	BUILD_LOCALEALIAS=
fi
AC_SUBST(BUILD_LOCALEALIAS)

dnl ======================================================================
dnl options - modules
dnl ======================================================================
MODULE_LIBS=
if test "x$enable_modules" != "xno"; then
	AC_MSG_CHECKING(for the suffix of module shared libraries)
	export SED
	shrext_cmds=`libtool --config | grep '^shrext_cmds='`
	eval $shrext_cmds
	module=yes eval std_shrext=$shrext_cmds
	lt_module_suffix=`echo $std_shrext | sed 's/^\.//'`
	AC_MSG_RESULT(.$lt_module_suffix)
	if test "x$lt_module_suffix" = x; then
		AC_MSG_ERROR(Unable to determine shared libreary suffix from libtool)
	fi
	AC_CHECK_LIB(dl, dlopen,
		[AC_CHECK_LIB(dl, dlsym, [MODULE_LIBS=-ldl])])

	AC_SUBST(MODULE_LIBS)
	AC_DEFINE(ENABLE_MODULE, 1, [Use modules])
	AC_DEFINE_UNQUOTED(LT_MODULE_SUFFIX, "$lt_module_suffix", [Shared library suffix])
fi
AM_CONDITIONAL(ENABLE_MODULE, test x$MODULE_LIBS != x)

dnl ======================================================================
dnl options - rebuild-data
dnl ======================================================================
AM_CONDITIONAL(REBUILD_DATA, test x$enable_rebuild_data = xyes)

dnl ======================================================================
dnl check pkg-config stuff
dnl ======================================================================
PKG_CHECK_MODULES(LIBXML2, libxml-2.0 >= $LIBXML2_REQUIRED)
PKG_CHECK_MODULES(GOBJECT, gobject-2.0 >= $GOBJECT_REQUIRED, has_gobject=yes, has_gobject=no)

if test x$enable_test != xno; then
	PKG_CHECK_MODULES(CHECK, check >= $CHECK_REQUIRED,
				 [use_check="yes"],
				 [use_check="no"])
else
	use_check=no
fi

AM_CONDITIONAL(ENABLE_UNIT_TEST, test x$use_check != xno)

dnl ======================================================================
dnl check another libraries
dnl ======================================================================
AX_PTHREAD([],
	[case $host_os in
	 cygwin* | mingw*)
	 ;;
	 *)
	 AC_MSG_ERROR([*** pthread library are required])
	 esac])

GOBJECT_INTROSPECTION_CHECK([1.30.0])
if test "x$enable_introspection" != "xyes"; then
	has_gobject=no
fi
AM_CONDITIONAL(ENABLE_GOBJECT, test x$has_gobject != xno)
GTK_DOC_CHECK(1.0)

dnl ======================================================================
dnl output
dnl ======================================================================
CFLAGS="$CFLAGS $WARN_CFLAGS"

AC_CONFIG_COMMANDS([liblangtag/lt-config.h],[
	outfile=liblangtag/lt-config.h-tmp
	cat > $outfile <<\____EOS
#ifndef __LT_CONFIG_H__
#define __LT_CONFIG_H__

#include "lt-macros.h"
____EOS

	cat >> $outfile <<____EOS

LT_BEGIN_DECLS
____EOS

	if test -z "$unknown_sizeof_void_p"; then
	cat >> $outfile<<____EOS

#define LT_POINTER_TO_INT(p)	((int) ${ptoi_cast} (p))
____EOS
	else
		echo "#error SIZEOF_VOID_P unknown"
	fi

	cat >> $outfile<<____EOS

LT_END_DECLS

#endif /* __LT_CONFIG_H__ */
____EOS

	if cmp -s $outfile liblangtag/lt-config.h; then
		AC_MSG_NOTICE([liblangtag/lt-config.h is unchanged])
		rm -f $outfile
	else
		mv $outfile liblangtag/lt-config.h
	fi
],[
	case $ac_cv_sizeof_void_p in
	$ac_cv_sizeof_int)
		ptoi_cast=''
		;;
	$ac_cv_sizeof_long)
		ptoi_cast='(long)'
		;;
	$ac_cv_sizeof_long_long)
		ptoi_cast='(long long)'
		;;
	*)
		unknown_sizeof_void_p=yes
		;;
	esac
])

AC_CONFIG_FILES([
	Makefile
	data/Makefile
	docs/Makefile
	docs/version.xml
	extensions/Makefile
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
