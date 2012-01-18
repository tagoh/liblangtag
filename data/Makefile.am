##
# Global definitions
NULL =
EXTRA_DIST =				\
	language-subtag-registry	\
	$(NULL)
MAINTAINERCLEANFILES =			\
	language-subtag-registry	\
	$(NULL)
CLEANFILES =				\
	language-subtag-registry.xml	\
	$(NULL)
BUILT_FILES =				\
	language-subtag-registry	\
	language-subtag-registry.xml	\
	$(NULL)

##
# Local definitions

##
# Local Rules
language-subtag-registry:
	wget -O $@ http://www.iana.org/assignments/language-subtag-registry || \
	curl -o $@ http://www.iana.org/assignments/language-subtag-registry
language-subtag-registry.xml: language-subtag-registry reg2xml Makefile
	$(builddir)/reg2xml $(srcdir)/language-subtag-registry > $@

##
# Target platform
subtagregistrydir = $(datadir)/liblangtag
subtagregistry_DATA =			\
	language-subtag-registry.xml	\
	$(NULL)
#
noinst_PROGRAMS =	\
	reg2xml		\
	$(NULL)
#
reg2xml_SOURCES =	\
	reg2xml.c	\
	$(NULL)
reg2xml_CFLAGS =			\
	$(LIBXML2_CFLAGS)		\
	$(GLIB_CFLAGS)			\
	-I$(top_srcdir)/liblangtag	\
	$(NULL)
reg2xml_LDFLAGS =					\
	$(LIBXML2_LIBS)					\
	$(GLIB_LIBS)					\
	$(top_builddir)/liblangtag/liblangtag.la	\
	$(NULL)
reg2xml_DEPENDENCIES =					\
	$(top_builddir)/liblangtag/liblangtag.la	\
	$(NULL)
