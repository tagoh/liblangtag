/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * check-lang.c
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

#include <liblangtag/langtag.h>
#include "main.h"

static lt_lang_db_t *db;

/************************************************************/
/* common functions                                         */
/************************************************************/
void
setup(void)
{
	lt_db_set_datadir(TEST_DATADIR);
	db = lt_db_get_lang();
}

void
teardown(void)
{
	lt_lang_db_unref(db);
}

/************************************************************/
/* Test cases                                               */
/************************************************************/
TDEF (lt_lang_compare) {
	lt_lang_t *e1, *e2;

	e1 = lt_lang_db_lookup(db, "ja");
	fail_unless(e1 != NULL, "No expected lang found: 'ja'");
	fail_unless(lt_lang_compare(e1, e1), "lang that points to the same object should be dealt as same");
	e2 = lt_lang_db_lookup(db, "zh");
	fail_unless(e2 != NULL, "No expected lang found: 'zh'");
	fail_unless(!lt_lang_compare(e1, e2), "lang that has different tag should be dealt as different.");
	lt_lang_unref(e2);
	e2 = lt_lang_db_lookup(db, "*");
	fail_unless(e2 != NULL, "No expected lang found: '*'");
	fail_unless(lt_lang_compare(e1, e2), "wildcard should be matched with any objects.");
	lt_lang_unref(e2);
	e2 = lt_lang_db_lookup(db, "");
	fail_unless(e2 == NULL, "No empty element for lang");
	lt_lang_unref(e2);
	lt_lang_unref(e1);
} TEND

/************************************************************/
Suite *
tester_suite(void)
{
	Suite *s = suite_create("lt_lang_t");
	TCase *tc = tcase_create("Basic functionality");

	tcase_add_checked_fixture(tc, setup, teardown);

	T (lt_lang_compare);

	suite_add_tcase(s, tc);

	return s;
}
