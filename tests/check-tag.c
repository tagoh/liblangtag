/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * check-tag.c
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <liblangtag/langtag.h>
#include "lt-utils.h"
#include "main.h"

/************************************************************/
/* common functions                                         */
/************************************************************/
void
setup(void)
{
	setenv("LANGTAG_EXT_MODULE_PATH", TEST_MODDIR, TRUE);
	lt_db_set_datadir(TEST_DATADIR);
	lt_db_initialize();
}

void
teardown(void)
{
	lt_db_finalize();
}

/************************************************************/
/* Test cases                                               */
/************************************************************/
TDEF (lt_tag_parse) {
	lt_tag_t *t1;

	t1 = lt_tag_new();
	fail_unless(t1 != NULL, "OOM");
	fail_unless(!lt_tag_parse(t1, "*", NULL), "parsing a wildcard isn't allowed.");
	fail_unless(!lt_tag_parse(t1, "ja-*", NULL), "parsing a wildcard isn't allowed.");
	fail_unless(!lt_tag_parse(t1, "ja*", NULL), "parsing a wildcard isn't allowed.");
	fail_unless(lt_tag_parse(t1, "en-GB-oed", NULL), "should be valid irregular entry");
	fail_unless(lt_tag_parse(t1, "En-gb-OeD", NULL), "should be valid irregular entry regardless of the case sensitivity");
	fail_unless(lt_tag_parse(t1, "mn-Cyrl-MN", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "MN-cYRL-mn", NULL), "should be valid entry regardless of the case sensitivity");
	fail_unless(lt_tag_parse(t1, "mN-cYrL-Mn", NULL), "should be valid entry regardless of the case sensitivity");
	fail_unless(!lt_tag_parse(t1, "blahblahblah", NULL), "should be an unknown tag.");
	fail_unless(lt_tag_parse(t1, "de-AT", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "sr-Latn-RS", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "es-419", NULL), "should be valid entry.");
	fail_unless(!lt_tag_parse(t1, "de-DE-1901-1901", NULL), "duplicate variants should be an error.");
	fail_unless(!lt_tag_parse(t1, "de-DE-1901-1996", NULL), "Most variants that share a prefix are mutually exclusive.");
	fail_unless(lt_tag_parse(t1, "sl-nedis", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "de-CH-1996", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "sl-rozaj", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "sl-rozaj-biske", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "sl-rozaj-biske-1994", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "sl-rozaj-njiva", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "sl-rozaj-njiva-1994", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "sl-rozaj-osojs", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "sl-rozaj-osojs-1994", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "sl-rozaj-solba", NULL), "should be valid entry.");
	fail_unless(lt_tag_parse(t1, "sl-rozaj-solba-1994", NULL), "should be valid entry.");
	fail_unless(!lt_tag_parse(t1, "en-baku1926", NULL), "not a valid variant for en");
	fail_unless(!lt_tag_parse(t1, "a-value", NULL), "An extension MUST follow at least a primary language subtag.");
	fail_unless(lt_tag_parse(t1, "de-a-value", NULL), "should be valid entry.");
	fail_unless(!lt_tag_parse(t1, "en-a-bbb-a-ccc", NULL), "Each singleton subtag for extension MUST appears at most one time in each tag.");
	fail_unless(!lt_tag_parse(t1, "tlh-a-b-foo", NULL), "Each singleton MUST be followed by at least one extension subtag.");
	fail_unless(!lt_tag_parse(t1, "sl-1994-rozaj-biske", NULL), "not a valid form");
	fail_unless(!lt_tag_parse(t1, "sl-rozaj-1994-biske", NULL), "not a valid form");
	fail_unless(lt_tag_parse(t1, "sl-Latn-rozaj", NULL), "should be valid entry");
	fail_unless(lt_tag_parse(t1, "sl-IT-rozaj", NULL), "should be valid entry");
	fail_unless(!lt_tag_parse(t1, "zh-cmn-u-ca-chinese", NULL), "extlang isn't allowed.");
	fail_unless(!lt_tag_parse(t1, "i-default-u-ca-chinese", NULL), "grandfathered isn't allowed");
	fail_unless(lt_tag_parse(t1, "ja-u-tz", NULL), "no type subtag should be still valid");
	fail_unless(lt_tag_parse(t1, "en-u-vt-0061", NULL), "should be valid entry");
	fail_unless(lt_tag_parse(t1, "en-u-vt-0061-0065", NULL), "should be valid entry");
	fail_unless(!lt_tag_parse(t1, "en-u-vt-A0", NULL), "not a valid form");
	fail_unless(!lt_tag_parse(t1, "en-u-vt-CODEPOINTS", NULL), "not a valid form");
	fail_unless(!lt_tag_parse(t1, "en-u-vt-U060C", NULL), "not a valid form");
	fail_unless(!lt_tag_parse(t1, "en-u-vt-110000", NULL), "not a valid form");
	fail_unless(lt_tag_parse(t1, "en-u-vt-10D40C", NULL), "should be valid entry");

	lt_tag_unref(t1);
} TEND

TDEF (lt_tag_parse_with_extra_token) {
	lt_tag_t *t1;

	t1 = lt_tag_new();
	fail_unless(t1 != NULL, "OOM");
	fail_unless(lt_tag_parse(t1, "ja", NULL), "should be valid entry");
	fail_unless(lt_tag_parse_with_extra_token(t1, "jp", NULL), "should be valid entry");

	lt_tag_unref(t1);
} TEND

TDEF (lt_tag_canonicalize) {
	lt_tag_t *t1;
	char *s;

	t1 = lt_tag_new();
	fail_unless(t1 != NULL, "OOM");
#if 0 /* which one is correct behavior? gan? or zh-gan? */
	fail_unless(lt_tag_parse(t1, "zh-gan", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "gan") == 0, "Unexpected result to be canonicalized.");
	free(s);
#endif
	fail_unless(lt_tag_parse(t1, "zh-yue", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "yue") == 0, "Unexpected result to be canonicalized.");
	free(s);
	fail_unless(lt_tag_parse(t1, "zh-yue-Hant-HK", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "yue-Hant-HK") == 0, "Unexpected result to be canonicalized.");
	free(s);
#if 0 /* which one is correct behavior? cmn? zh-cmn? */
	fail_unless(lt_tag_parse(t1, "zh-cmn", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "cmn") == 0, "Unexpected result to be canonicalized.");
	free(s);
#endif
	fail_unless(lt_tag_parse(t1, "en-Latn-US", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "en-US") == 0, "Unexpected result to be canonicalized.");
	free(s);
	fail_unless(lt_tag_parse(t1, "art-lojban", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "jbo") == 0, "Unexpected result to be canonicalized.");
	free(s);
	fail_unless(lt_tag_parse(t1, "iw", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "he") == 0, "Unexpected result to be canonicalized.");
	free(s);
	fail_unless(lt_tag_parse(t1, "i-klingon", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "tlh") == 0, "Unexpected result to be canonicalized.");
	free(s);
	fail_unless(lt_tag_parse(t1, "no-nyn", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "nn") == 0, "Unexpected result to be canonicalized.");
	free(s);
	fail_unless(lt_tag_parse(t1, "ja-Latn-hepburn-heploc", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "ja-Latn-alalc97") == 0, "Unexpected result to be canonicalized.");
	free(s);
	fail_unless(lt_tag_parse(t1, "zh-hakka", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "hak") == 0, "Unexpected result to be canonicalized.");
	free(s);
	fail_unless(lt_tag_parse(t1, "en-b-warble-a-babble", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "en-a-babble-b-warble") == 0, "Unexpected result to be canonicalized.");
	free(s);
	fail_unless(lt_tag_parse(t1, "hak-CN", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "zh-hak-CN") == 0, "Unexpected result to be canonicalized.");
	free(s);
	fail_unless(lt_tag_parse(t1, "en-BU", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "en-MM") == 0, "Unexpected result to be canonicalized.");
	free(s);
	fail_unless(lt_tag_parse(t1, "sgn-BR", NULL), "should be valid langtag.");
	s = lt_tag_canonicalize(t1, NULL);
	fail_unless(s != NULL, "Unable to be canonicalize.");
	fail_unless(lt_strcmp0(s, "sgn-bzs") == 0, "Unexpected result to be canonicalized.");
	free(s);

	lt_tag_unref(t1);
} TEND

TDEF (lt_tag_match) {
	lt_tag_t *t1;

	t1 = lt_tag_new();
	fail_unless(t1 != NULL, "OOM");
	fail_unless(lt_tag_parse(t1, "de-CH-1996", NULL), "should be valid langtag.");
	fail_unless(lt_tag_match(t1, "de-CH", NULL), "should match.");
	fail_unless(lt_tag_parse(t1, "de", NULL), "should be valid langtag.");
	fail_unless(!lt_tag_match(t1, "de-CH", NULL), "shouldn't match because the region subtag is missing.");
	fail_unless(lt_tag_parse(t1, "de-DE-1996", NULL), "should be valid langtag.");
	fail_unless(lt_tag_match(t1, "de-de", NULL), "should match.");
	fail_unless(lt_tag_parse(t1, "de-Deva", NULL), "should be valid langtag.");
	fail_unless(!lt_tag_match(t1, "de-de", NULL), "shouldn't match because the script tag and Region shouldn'be compared.");
	fail_unless(lt_tag_parse(t1, "de-Latn-DE", NULL), "should be valid langtag.");
	fail_unless(!lt_tag_match(t1, "de-de", NULL), "shouldn't match because the script tag is different.");
	fail_unless(lt_tag_match(t1, "*", NULL), "'*' should match everything.");
	fail_unless(!lt_tag_match(t1, "de*", NULL), "'de*' is invalid wildcard.");
	fail_unless(lt_tag_parse(t1, "de-DE", NULL), "should be valid langtag.");
	fail_unless(lt_tag_match(t1, "de-*-DE", NULL), "should match.");
	fail_unless(lt_tag_parse(t1, "de-de", NULL), "should be valid langtag.");
	fail_unless(lt_tag_match(t1, "de-*-DE", NULL), "should match.");
	fail_unless(lt_tag_parse(t1, "de-Latn-DE", NULL), "should be valid langtag.");
	fail_unless(lt_tag_match(t1, "de-*-DE", NULL), "should match.");
	fail_unless(lt_tag_parse(t1, "de-Latf-DE", NULL), "should be valid langtag.");
	fail_unless(lt_tag_match(t1, "de-*-DE", NULL), "should match.");
	fail_unless(lt_tag_parse(t1, "de-DE-x-goethe", NULL), "should be valid langtag.");
	fail_unless(lt_tag_match(t1, "de-*-DE", NULL), "should match.");
	fail_unless(lt_tag_parse(t1, "de-Latn-DE-1996", NULL), "should be valid langtag.");
	fail_unless(lt_tag_match(t1, "de-*-DE", NULL), "should match.");
	fail_unless(lt_tag_parse(t1, "de-Deva-DE", NULL), "should be valid langtag.");
	fail_unless(lt_tag_match(t1, "de-*-DE", NULL), "should match.");
	fail_unless(lt_tag_parse(t1, "de", NULL), "should be valid langtag.");
	fail_unless(!lt_tag_match(t1, "de-*-DE", NULL), "shouldn't match because of the missing region.");
	fail_unless(lt_tag_parse(t1, "de-x-DE", NULL), "should be valid langtag.");
	fail_unless(!lt_tag_match(t1, "de-*-DE", NULL), "shouldn't match because of the missing region.");
	fail_unless(lt_tag_parse(t1, "de-Deva", NULL), "should be valid langtag.");
	fail_unless(!lt_tag_match(t1, "de-*-DE", NULL), "shouldn't match because of the missing region.");

	lt_tag_unref(t1);
} TEND

TDEF (lt_tag_transform) {
	lt_tag_t *t1, *t2;

	t1 = lt_tag_new();
	fail_unless(t1 != NULL, "OOM");
	fail_unless(lt_tag_parse(t1, "zh-CN", NULL), "should be valid langtag.");
	t2 = lt_tag_transform(t1, NULL);
	fail_unless(t2 != NULL, "should be transformed");
	fail_unless(lt_strcmp0(lt_tag_get_string(t2), "zh-Hans-CN") == 0, "wrongly transformed");
	lt_tag_unref(t2);
	fail_unless(lt_tag_parse(t1, "zh-TW", NULL), "should be valid langtag.");
	t2 = lt_tag_transform(t1, NULL);
	fail_unless(t2 != NULL, "should be transformed");
	fail_unless(lt_strcmp0(lt_tag_get_string(t2), "zh-Hant-TW") == 0, "wrongly transformed");
	lt_tag_unref(t2);
	fail_unless(lt_tag_parse(t1, "zh-ZZZZ-SG", NULL), "should be valid langtag.");
	t2 = lt_tag_transform(t1, NULL);
	fail_unless(t2 != NULL, "should be transformed");
	fail_unless(lt_strcmp0(lt_tag_get_string(t2), "zh-Hans-SG") == 0, "wrongly transformed");
	lt_tag_unref(t2);
	fail_unless(lt_tag_parse(t1, "und-TW", NULL), "should be valid langtag.");
	t2 = lt_tag_transform(t1, NULL);
	fail_unless(t2 != NULL, "should be transformed");
	fail_unless(lt_strcmp0(lt_tag_get_string(t2), "zh-Hant-TW") == 0, "wrongly transformed");
	lt_tag_unref(t2);

	lt_tag_unref(t1);
} TEND

TDEF (lt_tag_convert_from_locale_string) {
	lt_tag_t *t1;

	t1 = lt_tag_convert_from_locale_string("ja_JP.UTF-8", NULL);
	fail_unless(t1 != NULL, "should be valid locale");
	fail_unless(lt_strcasecmp(lt_tag_get_string(t1), "ja-Jpan-JP") == 0, "wrongly converted to the tag");
	lt_tag_unref(t1);

	t1 = lt_tag_convert_from_locale_string("zh_CN.UTF-8", NULL);
	fail_unless(t1 != NULL, "should be valid locale");
	fail_unless(lt_strcasecmp(lt_tag_get_string(t1), "zh-Hans-CN") == 0, "wrongly converted to the tag");
	lt_tag_unref(t1);

	t1 = lt_tag_convert_from_locale_string("zh_TW.UTF-8", NULL);
	fail_unless(t1 != NULL, "should be valid locale");
	fail_unless(lt_strcasecmp(lt_tag_get_string(t1), "zh-Hant-TW") == 0, "wrongly converted to the tag");
	lt_tag_unref(t1);

	t1 = lt_tag_convert_from_locale_string("zh_SG.UTF-8", NULL);
	fail_unless(t1 != NULL, "should be valid locale");
	fail_unless(lt_strcasecmp(lt_tag_get_string(t1), "zh-Hans-SG") == 0, "wrongly converted to the tag");
	lt_tag_unref(t1);

	t1 = lt_tag_convert_from_locale_string("sr_RS.UTF-8", NULL);
	fail_unless(t1 != NULL, "should be valid locale");
	fail_unless(lt_strcasecmp(lt_tag_get_string(t1), "sr-Cyrl-RS") == 0, "wrongly converted to the tag");
	lt_tag_unref(t1);

} TEND

/************************************************************/
Suite *
tester_suite(void)
{
	Suite *s = suite_create("lt_tag_t");
	TCase *tc = tcase_create("Basic functionality");

	tcase_add_checked_fixture(tc, setup, teardown);

	T (lt_tag_parse);
	T (lt_tag_parse_with_extra_token);
	T (lt_tag_canonicalize);
	T (lt_tag_match);
	T (lt_tag_transform);
	T (lt_tag_convert_from_locale_string);

	suite_add_tcase(s, tc);

	return s;
}
