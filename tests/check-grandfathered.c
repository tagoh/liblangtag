/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * check-grandfathered.c
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
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
