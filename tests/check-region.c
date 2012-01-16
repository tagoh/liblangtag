/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * check-region.c
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

static lt_region_db_t *db;

/************************************************************/
/* common functions                                         */
/************************************************************/
void
setup(void)
{
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
