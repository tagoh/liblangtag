##
# Global definitions
NULL =
SUBDIRS =
INCLUDES =								\
	-I$(top_srcdir)/liblangtag					\
	-I$(top_srcdir)							\
	-DTEST_DATADIR="\"$(abs_top_builddir)/data\""			\
	-DTEST_MODDIR="\"$(abs_top_builddir)/extensions/.libs\""	\
	-D__LANGTAG_COMPILATION						\
	$(CHECK_CFLAGS)							\
	$(NULL)
DEPS =							\
	$(top_builddir)/liblangtag/liblangtag.la	\
	$(NULL)
LIBS =							\
	@LDFLAGS@					\
	$(top_builddir)/liblangtag/liblangtag.la	\
	$(CHECK_LIBS)					\
	$(NULL)
EXTRA_DIST =					\
	$(NULL)
TESTS =						\
	$(NULL)


##
# Local definitions
common_private_headers =			\
	main.h					\
	$(NULL)
common_sources =				\
	main.c					\
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
if ENABLE_UNIT_TEST
testcases =					\
	check-extlang				\
	check-grandfathered			\
	check-lang				\
	check-list				\
	check-region				\
	check-script				\
	check-tag				\
	check-trie				\
	check-variant				\
	$(NULL)
noinst_PROGRAMS +=		\
	$(testcases)		\
	$(NULL)
TESTS +=			\
	$(testcases)		\
	$(NULL)
endif
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
#
if ENABLE_UNIT_TEST
check_extlang_SOURCES =		\
	check-extlang.c		\
	$(common_sources)	\
	$(NULL)
check_grandfathered_SOURCES =	\
	check-grandfathered.c	\
	$(common_sources)	\
	$(NULL)
check_lang_SOURCES =		\
	check-lang.c		\
	$(common_sources)	\
	$(NULL)
check_list_SOURCES =		\
	check-list.c		\
	$(common_sources)	\
	$(NULL)
check_region_SOURCES =		\
	check-region.c		\
	$(common_sources)	\
	$(NULL)
check_script_SOURCES =		\
	check-script.c		\
	$(common_sources)	\
	$(NULL)
check_tag_SOURCES =		\
	check-tag.c		\
	$(common_sources)	\
	$(NULL)
check_trie_SOURCES =		\
	check-trie.c		\
	$(common_sources)	\
	$(NULL)
check_variant_SOURCES =		\
	check-variant.c		\
	$(common_sources)	\
	$(NULL)
endif

-include $(top_srcdir)/git.mk
