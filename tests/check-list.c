/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * check-list.c
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
#include "liblangtag/lt-mem.h"
#include "main.h"

/************************************************************/
/* common functions                                         */
/************************************************************/
void
setup(void)
{
}

void
teardown(void)
{
}

/************************************************************/
/* Test cases                                               */
/************************************************************/
TDEF (lt_list_new) {
	lt_list_t *l;

	l = lt_list_new();
	fail_unless(l != NULL, "Allocation failed");
	lt_list_unref(l);
} TEND

TDEF (lt_list_unref) {
	lt_list_t *l;

	l = lt_list_new();
	lt_mem_add_weak_pointer((lt_mem_t *)l, (lt_pointer_t *)&l);
	lt_list_unref(l);
	fail_unless(l == NULL, "Not registered as a weak pointer properly");
} TEND

TDEF (lt_list_append) {
	lt_list_t *l = lt_list_append(NULL, strdup("foo"), free);
	lt_list_t *t;

	fail_unless(l != NULL, "Allocation failed");
	fail_unless(lt_list_value(l) != NULL, "failed to obtain value");
	fail_unless(strcmp(lt_list_value(l), "foo") == 0, "Failed to compare value");
	fail_unless(lt_list_next(l) == NULL, "only one list element should be available");

	l = lt_list_append(l, strdup("bar"), free);
	fail_unless(l != NULL, "Allocation failed");
	fail_unless(lt_list_next(l) != NULL, "Allocation failed");
	fail_unless(lt_list_value(lt_list_next(l)) != NULL, "failed to obtain value");
	fail_unless(strcmp(lt_list_value(lt_list_next(l)), "bar") == 0, "Failed to compare value");
	fail_unless(lt_list_next(lt_list_next(l)) == NULL, "only one list element should be available");

	t = lt_list_next(l);
	lt_list_unref(l);
	fail_unless(lt_list_previous(t) == NULL, "Failed to update the link");
	fail_unless(lt_list_next(t) == NULL, "Failed to update the link");
	fail_unless(lt_list_value(t) != NULL, "Something goes wrong");
	fail_unless(strcmp(lt_list_value(t), "bar") == 0, "Failed to compare value");
	lt_list_free(t);
} TEND

/************************************************************/
Suite *
tester_suite(void)
{
	Suite *s = suite_create("lt_list_t");
	TCase *tc = tcase_create("Basic functionality");

	tcase_add_checked_fixture(tc, setup, teardown);

	T (lt_list_new);
	T (lt_list_unref);
	T (lt_list_append);

	suite_add_tcase(s, tc);

	return s;
}
