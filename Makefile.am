NULL =
AUTOMAKE_OPTIONS = dist-bzip2
SUBDIRS = liblangtag extensions data docs

if ENABLE_GOBJECT
SUBDIRS += liblangtag-gobject
endif
#SUBDIRS += docs tests
SUBDIRS += tests

ACLOCAL_AMFLAGS = -I m4macros

CONFIGURE_DEPENDENCIES =		\
	requires			\
	$(NULL)

EXTRA_DIST =				\
	$(pkgconfig_DATA)		\
	$(noinst_DATA)			\
	requires			\
	$(NULL)

MAINTAINERCLEANFILES =			\
	$(srcdir)/ChangeLog		\
	$(srcdir)/INSTALL		\
	$(srcdir)/aclocal.m4		\
	$(srcdir)/config.guess		\
	$(srcdir)/config.h.in		\
	$(srcdir)/config.sub		\
	$(srcdir)/depcomp		\
	$(srcdir)/gtk-doc.make		\
	$(srcdir)/install-sh		\
	$(srcdir)/ltmain.sh		\
	$(srcdir)/missing		\
	$(srcdir)/mkinstalldirs		\
	$(srcdir)/m4macros/*.m4		\
	`find "$(srcdir)" -type f -name Makefile.in -print`	\
	$(NULL)
DISTCLEANFILES =			\
	$(pkgconfig_DATA)		\
	$(noinst_DATA)			\
	$(NULL)

DISTCHECK_CONFIGURE_FLAGS =		\
	--enable-introspection		\
	--enable-gtk-doc		\
	--enable-debug			\
	$(NULL)

#
#
#
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA =			\
	liblangtag.pc			\
	$(NULL)
if ENABLE_GOBJECT
pkgconfig_DATA += liblangtag-gobject.pc
endif
$(pkgconfig_DATA): $(top_builddir)/config.status
noinst_DATA =					\
	liblangtag-uninstalled.pc		\
	liblangtag-gobject-uninstalled.pc	\
	$(NULL)
$(noinst_DATA): $(top_builddir)/config.status
#
ChangeLog: $(srcdir)/ChangeLog
$(srcdir)/ChangeLog:
	$(AM_V_GEN) if test -d "$(srcdir)/.git"; then	\
	(GIT_DIR=$(top_srcdir)/.git ./missing --run	\
	 git log --stat) | fmt --split-only > $@.tmp	\
	&& mv -f $@.tmp $@				\
	|| ($(RM) $@.tmp;				\
	    echo Failed to generate ChangeLog.  your ChangeLog may be outdated >&2;	\
	    (test -f $@ || echo git-log is required to generate this file >> $@));	\
	else						\
	  test -f $@ ||					\
	  (echo A git checkout and git-log is required to generate ChangeLog >&2 &&	\
	   echo A git checkout and git-log is required to generate this file >> $@);	\
	fi

.PHONY: $(srcdir)/ChangeLog

-include $(top_srcdir)/git.mk
