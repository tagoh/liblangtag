/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * check-lang.c
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
