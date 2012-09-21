/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * check-region.c
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

static lt_region_db_t *db;

/************************************************************/
/* common functions                                         */
/************************************************************/
void
setup(void)
{
	lt_db_set_datadir(TEST_DATADIR);
	db = lt_db_get_region();
}

void
teardown(void)
{
	lt_region_db_unref(db);
}

/************************************************************/
/* Test cases                                               */
/************************************************************/
TDEF (lt_region_compare) {
	lt_region_t *e1, *e2;

	e1 = lt_region_db_lookup(db, "JP");
	fail_unless(e1 != NULL, "No expected region found: 'JP'");
	fail_unless(lt_region_compare(e1, e1), "region that points to the same object should be dealt as same");
	e2 = lt_region_db_lookup(db, "jp");
	fail_unless(e2 != NULL, "No expected region found: 'jp'");
	fail_unless(lt_region_compare(e1, e2), "even if looking up with different case, it should be same.");
	lt_region_unref(e2);
	e2 = lt_region_db_lookup(db, "CN");
	fail_unless(e2 != NULL, "No expected region found: 'CN'");
	fail_unless(!lt_region_compare(e1, e2), "region that has different tag should be dealt as different.");
	lt_region_unref(e2);
	e2 = lt_region_db_lookup(db, "*");
	fail_unless(e2 != NULL, "No expected region found: '*'");
	fail_unless(lt_region_compare(e1, e2), "wildcard should be matched with any objects.");
	lt_region_unref(e2);
	e2 = lt_region_db_lookup(db, "");
	fail_unless(e2 != NULL, "No expected region found: ''");
	fail_unless(!lt_region_compare(e1, e2), "region that has different tag should be dealt as different even if it's an empty entry.");
	lt_region_unref(e2);
	lt_region_unref(e1);
} TEND

/************************************************************/
Suite *
tester_suite(void)
{
	Suite *s = suite_create("lt_region_t");
	TCase *tc = tcase_create("Basic functionality");

	tcase_add_checked_fixture(tc, setup, teardown);

	T (lt_region_compare);

	suite_add_tcase(s, tc);

	return s;
}
