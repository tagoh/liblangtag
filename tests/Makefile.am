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
	test-lang-db				\
	test-region				\
	test-script				\
	$(NULL)
#
test_lang_db_SOURCES =	\
	lang-db.c	\
	$(NULL)
#
test_region_SOURCES =	\
	region.c	\
	$(NULL)
#
test_script_SOURCES =	\
	script.c	\
	$(NULL)
