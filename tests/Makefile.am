##
# Global definitions
NULL =
SUBDIRS =
INCLUDES =					\
	-I$(top_srcdir)/liblangtag		\
	-I$(top_srcdir)				\
	$(GLIB_CFLAGS)				\
	$(NULL)
DEPS =							\
	$(top_builddir)/liblangtag/liblangtag.la	\
	$(NULL)
LIBS =							\
	@LDFLAGS@					\
	$(top_builddir)/liblangtag/liblangtag.la	\
	$(NULL)
EXTRA_DIST =					\
	$(NULL)
TESTS =						\
	$(NULL)


##
# Local definitions
common_private_headers =			\
	$(NULL)
common_sources =				\
	$(NULL)

##
# Local Rules


##
# Target platform
noinst_HEADERS =				\
	$(common_private_headers)		\
	$(NULL)
noinst_PROGRAMS =				\
	test-lang				\
	test-script				\
	$(NULL)
#
test_lang_SOURCES =	\
	lang.c		\
	$(NULL)
#
test_script_SOURCES =	\
	script.c	\
	$(NULL)
