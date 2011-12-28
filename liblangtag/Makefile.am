##
# Global definitions
NULL =
INCLUDES =					\
	-I$(top_srcdir)/liblangtag/		\
	-I$(top_srcdir)				\
	-D__LANGTAG_COMPILATION			\
	$(GLIB_CFLAGS)				\
	$(LIBXML2_CFLAGS)			\
	$(NULL)
LIBS =						\
	@LDFLAGS@				\
	$(GLIB_LIBS)				\
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
	lt-error.h				\
	lt-lang.h				\
	lt-script.h				\
	$(NULL)	
liblangtag_private_headers =			\
	lt-mem.h				\
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
	lt-error.c				\
	lt-lang.c				\
	lt-mem.c				\
	lt-script.c				\
	$(NULL)
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
