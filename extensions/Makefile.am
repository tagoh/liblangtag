##
# Global definitions
NULL =
INCLUDES =						\
	-I$(top_srcdir)/liblangtag/			\
	-I$(top_srcdir)					\
	-D__LANGTAG_COMPILATION				\
	-DBUILDDIR="\"$(abs_top_builddir)\""		\
	-DREGDATADIR="\"$(datadir)/liblangtag\""	\
	$(LIBXML2_CFLAGS)				\
	$(NULL)
LIBS =							\
	@LDFLAGS@					\
	$(LIBXML2_LIBS)					\
	$(NULL)
EXTRA_DIST =					\
	$(NULL)
MAINTAINERCLEANFILES =				\
	$(NULL)
CLEANFILES =					\
	$(NULL)
BUILT_FILES =					\
	$(NULL)

##
# Local definitions


##
# Local Rules


##
# Target platform
moduledir = $(LANGTAG_MODULEDIR)
module_LTLIBRARIES = $(NULL)
noinst_LTLIBRARIES = $(NULL)
#
modules =				\
	liblangtag-ext-ldml-t.la	\
	liblangtag-ext-ldml-u.la	\
	$(NULL)
#
if ENABLE_MODULE
module_LTLIBRARIES += $(modules)
module_t_cflags =
module_u_cflags =
LIBS += $(top_builddir)/liblangtag/liblangtag.la
else
noinst_LTLIBRARIES += $(modules)
module_t_cflags = -DLT_MODULE_PREFIX=lt_module_ext_t
module_u_cflags = -DLT_MODULE_PREFIX=lt_module_ext_u
endif
#
liblangtag_ext_ldml_t_la_SOURCES =	\
	lt-ext-ldml-t.c			\
	$(NULL)
liblangtag_ext_ldml_t_la_CFLAGS =		\
	$(module_t_cflags)			\
	$(NULL)
liblangtag_ext_ldml_t_la_LDFLAGS =		\
	-avoid-version				\
	-module					\
	$(NULL)
#
liblangtag_ext_ldml_u_la_SOURCES =	\
	lt-ext-ldml-u.c			\
	$(NULL)
liblangtag_ext_ldml_u_la_CFLAGS =		\
	$(module_u_cflags)			\
	$(NULL)
liblangtag_ext_ldml_u_la_LDFLAGS =		\
	-avoid-version				\
	-module					\
	$(NULL)

-include $(top_srcdir)/git.mk
