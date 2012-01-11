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
	test-extlang-db				\
	test-grandfathered-db			\
	test-lang-db				\
	test-region-db				\
	test-script-db				\
	test-tag				\
	test-variant-db				\
	$(NULL)
#
test_extlang_db_SOURCES =	\
	extlang-db.c		\
	$(NULL)
#
test_grandfathered_db_SOURCES =	\
	grandfathered-db.c	\
	$(NULL)
#
test_lang_db_SOURCES =	\
	lang-db.c	\
	$(NULL)
#
test_region_db_SOURCES =	\
	region-db.c		\
	$(NULL)
#
test_script_db_SOURCES =	\
	script-db.c		\
	$(NULL)
#
test_tag_SOURCES =	\
	tag.c		\
	$(NULL)
#
test_variant_db_SOURCES =	\
	variant-db.c		\
	$(NULL)
