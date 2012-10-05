-include $(INTROSPECTION_MAKEFILE)

##
# Global definitions
NULL =
INCLUDES =					\
	-I$(top_srcdir)/liblangtag/		\
	-I$(top_srcdir)				\
	-D__LANGTAG_COMPILATION			\
	$(GOBJECT_CFLAGS)			\
	$(NULL)
DEPS =							\
	$(top_builddir)/liblangtag/liblangtag.la	\
	$(NULL)
LIBS =							\
	@LDFLAGS@					\
	$(GOBJECT_LIBS)					\
	$(top_builddir)/liblangtag/liblangtag.la	\
	$(NULL)
EXTRA_DIST =					\
	gengir.sh				\
	$(NULL)
MAINTAINERCLEANFILES =					\
	$(stamp_files)					\
	$(NULL)
CLEANFILES =						\
	$(liblangtag_gobject_built_public_headers)	\
	$(liblangtag_gobject_built_private_headers)	\
	$(liblangtag_gobject_built_sources)		\
	xgen-lefh					\
	xgen-lefc					\
	$(NULL)
BUILT_FILES =						\
	$(liblangtag_gobject_built_public_headers)	\
	$(liblangtag_gobject_built_private_headers)	\
	$(liblangtag_gobject_built_sources)		\
	$(NULL)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS =			\
	--add-include-path=$(srcdir)		\
	--warn-all				\
	$(NULL)
INTROSPECTION_COMPILER_ARGS =			\
	--includedir=$(srcdir)			\
	$(NULL)


##
# Local definitions
liblangtag_gobject_public_headers =	\
	lt-gobject.h			\
	$(NULL)	
liblangtag_gobject_private_headers =	\
	$(NULL)
liblangtag_gobject_built_public_headers =	\
	$(NULL)
liblangtag_gobject_built_private_headers =	\
	lt-database.gir.h			\
	lt-error.gir.h				\
	lt-ext-module-data.gir.h		\
	lt-ext-module.gir.h			\
	lt-extension.gir.h			\
	lt-extlang-db.gir.h			\
	lt-extlang.gir.h			\
	lt-grandfathered-db.gir.h		\
	lt-grandfathered.gir.h			\
	lt-lang-db.gir.h			\
	lt-lang.gir.h				\
	lt-list.gir.h				\
	lt-macros.gir.h				\
	lt-redundant-db.gir.h			\
	lt-redundant.gir.h			\
	lt-region-db.gir.h			\
	lt-region.gir.h				\
	lt-script-db.gir.h			\
	lt-script.gir.h				\
	lt-string.gir.h				\
	lt-tag.gir.h				\
	lt-variant-db.gir.h			\
	lt-variant.gir.h			\
	$(NULL)
#
liblangtag_gobject_built_sources =	\
	lt-error.gir.c			\
	lt-ext-module-data.gir.c	\
	lt-ext-module.gir.c		\
	lt-extension.gir.c		\
	lt-extlang-db.gir.c		\
	lt-extlang.gir.c		\
	lt-grandfathered-db.gir.c	\
	lt-grandfathered.gir.c		\
	lt-lang-db.gir.c		\
	lt-lang.gir.c			\
	lt-list.gir.c			\
	lt-redundant-db.gir.c		\
	lt-redundant.gir.c		\
	lt-region-db.gir.c		\
	lt-region.gir.c			\
	lt-script-db.gir.c		\
	lt-script.gir.c			\
	lt-string.gir.c			\
	lt-tag.gir.c			\
	lt-variant-db.gir.c		\
	lt-variant.gir.c		\
	$(NULL)
liblangtag_gobject_sources =		\
	lt-gobject.c			\
	$(NULL)
#
stamp_files =				\
	$(NULL)


##
# Local Rules


##
# Target platform
lib_LTLIBRARIES = 		\
	liblangtag-gobject.la	\
	$(NULL)
#
liblangtagincdir = $(includedir)/liblangtag
liblangtaginc_HEADERS =					\
	$(liblangtag_gobject_public_headers)		\
	$(liblangtag_gobject_built_public_headers)	\
	$(NULL)
#
noinst_HEADERS =					\
	$(liblangtag_gobject_private_headers)		\
	$(liblangtag_gobject_built_private_headers)	\
	$(NULL)
#
liblangtag_gobject_la_SOURCES =				\
	$(liblangtag_gobject_sources)			\
	$(NULL)
liblangtag_gobject_la_CFLAGS =				\
	-DG_LOG_DOMAIN="\"LangTag-GObject\""		\
	$(NULL)
liblangtag_gobject_la_LDFLAGS =						\
	$(LDFLAGS)							\
	-version-info $(LT_G_CURRENT):$(LT_G_REVISION):$(LT_G_AGE)	\
	$(NULL)
#
if HAVE_INTROSPECTION
lt_headers = $(liblangtag_gobject_private_headers:.gir.h=.h)
lt_sources = $(liblangtag_gobject_built_sources:.gir.c=.c)
introspection_sources =					\
	$(liblangtag_gobject_built_private_headers)	\
	$(liblangtag_gobject_built_sources)		\
	$(liblangtag_gobject_public_headers)		\
	$(liblangtag_gobject_sources)			\
	$(NULL)

%.gir.h: $(top_builddir)/liblangtag/liblangtag.la gengir.sh Makefile
	$(AM_V_GEN) $(srcdir)/gengir.sh $(top_srcdir)/liblangtag $@
%.gir.c: $(top_builddir)/liblangtag/liblangtag.la gengir.sh Makefile
	$(AM_V_GEN) $(srcdir)/gengir.sh $(top_srcdir)/liblangtag $@

LangTag-0.2.gir: liblangtag-gobject.la Makefile
LangTag_0_2_gir_INCLUDES =		\
	GLib-2.0			\
	GObject-2.0			\
	$(NULL)
LangTag_0_2_gir_CFLAGS =		\
	$(INCLUDES)			\
	--symbol-prefix=lt		\
	--identifier-prefix=Lt		\
	-v				\
	$(NULL)
LangTag_0_2_gir_LIBS =			\
	liblangtag-gobject.la		\
	$(NULL)
LangTag_0_2_gir_FILES =			\
	$(introspection_sources)	\
	$(NULL)

INTROSPECTION_GIRS += LangTag-0.2.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

-include $(top_srcdir)/git.mk
