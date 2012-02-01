##
# Global definitions
NULL =
SUBDIRS =
INCLUDES =							\
	-I$(top_srcdir)/liblangtag/				\
	-I$(top_srcdir)						\
	-D__LANGTAG_COMPILATION					\
	-DBUILDDIR="\"$(abs_top_builddir)\""			\
	-DREGDATADIR="\"$(datadir)/liblangtag\""		\
	-DLANGTAG_EXT_MODULE_PATH="\"$(LANGTAG_MODULEDIR)\""	\
	$(GLIB_CFLAGS)						\
	$(GMODULE_CFLAGS)					\
	$(LIBXML2_CFLAGS)					\
	$(NULL)
LIBS =						\
	@LDFLAGS@				\
	$(GLIB_LIBS)				\
	$(GMODULE_LIBS)				\
	$(LIBXML2_LIBS)				\
	$(NULL)
EXTRA_DIST =					\
	$(NULL)
MAINTAINERCLEANFILES =				\
	$(liblangtag_built_public_headers)	\
	$(liblangtag_built_private_headers)	\
	$(liblangtag_built_sources)		\
	$(stamp_files)				\
	$(NULL)
CLEANFILES =					\
	xgen-h					\
	xgen-c					\
	$(NULL)
BUILT_FILES =					\
	$(liblangtag_built_public_headers)	\
	$(liblangtag_built_private_headers)	\
	$(liblangtag_built_sources)		\
	$(NULL)

##
# Local definitions
liblangtag_public_headers =			\
	lt-database.h				\
	lt-error.h				\
	lt-ext-module.h				\
	lt-ext-module-data.h			\
	lt-extension.h				\
	lt-extlang.h				\
	lt-extlang-db.h				\
	lt-grandfathered.h			\
	lt-grandfathered-db.h			\
	lt-lang.h				\
	lt-lang-db.h				\
	lt-redundant.h				\
	lt-redundant-db.h			\
	lt-region.h				\
	lt-region-db.h				\
	lt-script.h				\
	lt-script-db.h				\
	lt-tag.h				\
	lt-variant.h				\
	lt-variant-db.h				\
	$(NULL)
liblangtag_private_headers =			\
	lt-ext-module-private.h			\
	lt-extension-private.h			\
	lt-extlang-private.h			\
	lt-grandfathered-private.h		\
	lt-lang-private.h			\
	lt-mem.h				\
	lt-redundant-private.h			\
	lt-region-private.h			\
	lt-script-private.h			\
	lt-tag-private.h			\
	lt-utils.h				\
	lt-variant-private.h			\
	lt-xml.h				\
	$(NULL)
liblangtag_built_public_headers =		\
	$(NULL)
liblangtag_built_private_headers =		\
	$(NULL)
#
liblangtag_built_sources =			\
	$(NULL)
liblangtag_sources =				\
	$(liblangtag_built_sources)		\
	lt-database.c				\
	lt-error.c				\
	lt-ext-module.c				\
	lt-ext-module-data.c			\
	lt-extension.c				\
	lt-extlang.c				\
	lt-extlang-db.c				\
	lt-grandfathered.c			\
	lt-grandfathered-db.c			\
	lt-lang.c				\
	lt-lang-db.c				\
	lt-mem.c				\
	lt-redundant.c				\
	lt-redundant-db.c			\
	lt-region.c				\
	lt-region-db.c				\
	lt-script.c				\
	lt-script-db.c				\
	lt-tag.c				\
	lt-utils.c				\
	lt-variant.c				\
	lt-variant-db.c				\
	lt-xml.c				\
	$(NULL)
if ENABLE_GMODULE
SUBDIRS += extensions
endif
#
stamp_files =					\
	$(NULL)


##
# Local Rules


##
# Target platform
lib_LTLIBRARIES = 				\
	liblangtag.la				\
	$(NULL)
#
liblangtagincdir = $(includedir)/liblangtag
liblangtaginc_HEADERS =				\
	langtag.h				\
	$(liblangtag_public_headers)		\
	$(liblangtag_built_public_headers)	\
	$(NULL)
#
noinst_HEADERS =				\
	$(liblangtag_private_headers)		\
	$(liblangtag_built_private_headers)	\
	$(NULL)
#
liblangtag_la_SOURCES =			\
	$(liblangtag_sources)		\
	$(NULL)
liblangtag_la_CFLAGS =				\
	-DG_LOG_DOMAIN="\"LangTag\""		\
	$(NULL)
liblangtag_la_LDFLAGS =						\
	$(LDFLAGS)						\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)	\
	$(NULL)
