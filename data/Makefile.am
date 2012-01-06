##
# Global definitions
NULL =
EXTRA_DIST =				\
	$(NULL)
MAINTAINERCLEANFILES =			\
	language-subtag-registry.xml	\
	$(NULL)
CLEANFILES =				\
	$(NULL)
BUILT_FILES =				\
	language-subtag-registry.xml	\
	$(NULL)

##
# Local definitions

##
# Local Rules
language-subtag-registry.xml:
	wget -O $@ http://langtag.net/registries/language-subtag-registry.xml || \
	curl -o $@ http://langtag.net/registries/language-subtag-registry.xml

##
# Target platform
subtagregistrydir = $(datadir)/liblangtag
subtagregistry_DATA =			\
	language-subtag-registry.xml	\
	$(NULL)
