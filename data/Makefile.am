##
# Global definitions
NULL =
EXTRA_DIST =				\
	language-subtag-registry	\
	$(stamp_files)			\
	$(subtagregistry_DATA)		\
	$(ldmlbcp47_DATA)		\
	$(ldmlsupplemental_DATA)	\
	$(NULL)
MAINTAINERCLEANFILES =			\
	language-subtag-registry	\
	$(ldml_xml_files)		\
	core.zip			\
	$(stamp_files)			\
	$(NULL)
CLEANFILES =				\
	language-subtag-registry.xml	\
	$(NULL)
BUILT_FILES =				\
	language-subtag-registry	\
	language-subtag-registry.xml	\
	$(ldml_xml_files)		\
	$(NULL)

##
# Local definitions
ldml_xml_files =			\
	$(bcp47_xml_files)		\
	$(supplemental_xml_files)	\
	$(NULL)
bcp47_xml_files =			\
	common/bcp47/calendar.xml	\
	common/bcp47/collation.xml	\
	common/bcp47/currency.xml	\
	common/bcp47/number.xml		\
	common/bcp47/timezone.xml	\
	common/bcp47/variant.xml	\
	$(NULL)
supplemental_xml_files =		\
	common/supplemental/likelySubtags.xml	\
	$(NULL)
stamp_files =		\
	stamp-core-zip	\
	stamp-ldml-xml	\
	$(NULL)

##
# Local Rules
language-subtag-registry:
	wget -O $@ http://www.iana.org/assignments/language-subtag-registry || \
	curl -o $@ http://www.iana.org/assignments/language-subtag-registry
language-subtag-registry.xml: language-subtag-registry reg2xml Makefile
	$(builddir)/reg2xml $(srcdir)/language-subtag-registry > $@
stamp-core-zip:
	-rm core.zip
	wget http://unicode.org/Public/cldr/latest/core.zip ||	\
	curl http://unicode.org/Public/cldr/latest/core.zip;	\
	touch $@
stamp-ldml-xml: stamp-core-zip Makefile.am
	unzip -o core.zip $(bcp47_xml_files) $(supplemental_xml_files);	\
	touch $@
$(bcp47_xml_files): stamp-ldml-xml
$(supplemental_xml_files): stamp-ldml-xml

##
# Target platform
subtagregistrydir = $(datadir)/liblangtag
subtagregistry_DATA =			\
	language-subtag-registry.xml	\
	$(NULL)
ldmlbcp47dir = $(datadir)/liblangtag/common/bcp47
ldmlbcp47_DATA =		\
	$(bcp47_xml_files)	\
	$(NULL)
ldmlsupplementaldir = $(datadir)/liblangtag/common/supplemental
ldmlsupplemental_DATA =			\
	$(supplemental_xml_files)	\
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

-include $(top_srcdir)/git.mk
