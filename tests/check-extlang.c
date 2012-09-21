/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * check-extlang.c
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

static lt_extlang_db_t *db;

/************************************************************/
/* common functions                                         */
/************************************************************/
void
setup(void)
{
	lt_db_set_datadir(TEST_DATADIR);
	db = lt_db_get_extlang();
}

void
teardown(void)
{
	lt_extlang_db_unref(db);
}

/************************************************************/
/* Test cases                                               */
/************************************************************/
TDEF (lt_extlang_compare) {
	lt_extlang_t *e1, *e2;

	e1 = lt_extlang_db_lookup(db, "yue");
	fail_unless(e1 != NULL, "No expected extlang found: 'yue'");
	fail_unless(lt_extlang_compare(e1, e1), "extlang that points to the same object should be dealt as same");
	e2 = lt_extlang_db_lookup(db, "hks");
	fail_unless(e2 != NULL, "No expected extlang found: 'hks'");
	fail_unless(!lt_extlang_compare(e1, e2), "extlang that has different tag should be dealt as different.");
	lt_extlang_unref(e2);
	e2 = lt_extlang_db_lookup(db, "*");
	fail_unless(e2 != NULL, "No expected extlang found: '*'");
	fail_unless(lt_extlang_compare(e1, e2), "wildcard should be matched with any objects.");
	lt_extlang_unref(e2);
	e2 = lt_extlang_db_lookup(db, "");
	fail_unless(e2 != NULL, "No expected extlang found: ''");
	fail_unless(!lt_extlang_compare(e1, e2), "extlang that has different tag should be dealt as different even if it's an empty entry.");
	lt_extlang_unref(e2);
	lt_extlang_unref(e1);
} TEND

/************************************************************/
Suite *
tester_suite(void)
{
	Suite *s = suite_create("lt_extlang_t");
	TCase *tc = tcase_create("Basic functionality");

	tcase_add_checked_fixture(tc, setup, teardown);

	T (lt_extlang_compare);

	suite_add_tcase(s, tc);

	return s;
}
