/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * check-variant.c
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

static lt_variant_db_t *db;

/************************************************************/
/* common functions                                         */
/************************************************************/
void
setup(void)
{
	lt_db_set_datadir(TEST_DATADIR);
	db = lt_db_get_variant();
}

void
teardown(void)
{
	lt_variant_db_unref(db);
}

/************************************************************/
/* Test cases                                               */
/************************************************************/
TDEF (lt_variant_compare) {
	lt_variant_t *e1, *e2;

	e1 = lt_variant_db_lookup(db, "1901");
	fail_unless(e1 != NULL, "No expected variant found: '1901'");
	fail_unless(lt_variant_compare(e1, e1), "variant that points to the same object should be dealt as same");
	e2 = lt_variant_db_lookup(db, "1996");
	fail_unless(e2 != NULL, "No expected variant found: '1996'");
	fail_unless(!lt_variant_compare(e1, e2), "variant that has different tag should be dealt as different.");
	lt_variant_unref(e2);
	e2 = lt_variant_db_lookup(db, "*");
	fail_unless(e2 != NULL, "No expected variant found: '*'");
	fail_unless(lt_variant_compare(e1, e2), "wildcard should be matched with any objects.");
	lt_variant_unref(e2);
	e2 = lt_variant_db_lookup(db, "");
	fail_unless(e2 != NULL, "No expected variant found: ''");
	fail_unless(!lt_variant_compare(e1, e2), "variant that has different tag should be dealt as different even if it's an empty entry.");
	lt_variant_unref(e2);
	lt_variant_unref(e1);
} TEND

/************************************************************/
Suite *
tester_suite(void)
{
	Suite *s = suite_create("lt_variant_t");
	TCase *tc = tcase_create("Basic functionality");

	tcase_add_checked_fixture(tc, setup, teardown);

	T (lt_variant_compare);

	suite_add_tcase(s, tc);

	return s;
}
