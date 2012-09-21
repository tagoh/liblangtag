/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * check-grandfathered.c
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

static lt_grandfathered_db_t *db;

/************************************************************/
/* common functions                                         */
/************************************************************/
void
setup(void)
{
	lt_db_set_datadir(TEST_DATADIR);
	db = lt_db_get_grandfathered();
}

void
teardown(void)
{
	lt_grandfathered_db_unref(db);
}

/************************************************************/
/* Test cases                                               */
/************************************************************/
TDEF (lt_grandfathered_compare) {
	lt_grandfathered_t *e1, *e2;

	e1 = lt_grandfathered_db_lookup(db, "i-default");
	fail_unless(e1 != NULL, "No expected grandfathered found: 'i-default'");
	fail_unless(lt_grandfathered_compare(e1, e1), "grandfathered that points to the same object should be dealt as same");
	e2 = lt_grandfathered_db_lookup(db, "art-lojban");
	fail_unless(e2 != NULL, "No expected grandfathered found: 'art-lojban'");
	fail_unless(!lt_grandfathered_compare(e1, e2), "grandfathered that has different tag should be dealt as different.");
	lt_grandfathered_unref(e2);
	e2 = lt_grandfathered_db_lookup(db, "*");
	fail_unless(e2 == NULL, "No wildcard element for grandfathered");
	e2 = lt_grandfathered_db_lookup(db, "");
	fail_unless(e2 == NULL, "No empty element for grandfathered");
	lt_grandfathered_unref(e2);
	lt_grandfathered_unref(e1);
} TEND

/************************************************************/
Suite *
tester_suite(void)
{
	Suite *s = suite_create("lt_grandfathered_t");
	TCase *tc = tcase_create("Basic functionality");

	tcase_add_checked_fixture(tc, setup, teardown);

	T (lt_grandfathered_compare);

	suite_add_tcase(s, tc);

	return s;
}
