/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * check-trie.c
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

#include <glib.h> /* XXX: just shut up GHashTable dependency in lt-mem.h */
#include <liblangtag/langtag.h>
#include "lt-mem.h"
#include "lt-trie.h"
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
TDEF (lt_trie_new) {
	lt_trie_t *t;

	t = lt_trie_new();
	fail_unless(t != NULL, "Unable to allocate a memory");
	lt_trie_unref(t);
} TEND

TDEF (lt_trie_unref) {
	lt_trie_t *t;

	t = lt_trie_new();
	lt_mem_add_weak_pointer((lt_mem_t *)t, (lt_pointer_t *)&t);
	lt_trie_unref(t);
	fail_unless(t == NULL, "Not registered as a weak pointer properly");
} TEND

TDEF (lt_trie_add) {
	lt_trie_t *t = lt_trie_new();

	fail_unless(lt_trie_add(t, "a", "a", NULL), "Unable to add");
	fail_unless(!lt_trie_add(t, "a", "a", NULL), "Adding same key should fails");
	fail_unless(lt_trie_add(t, "foo", "foo", NULL), "Unable to add");
	fail_unless(lt_trie_add(t, "fooo", "fooo", NULL), "Unable to add");

	lt_trie_unref(t);
} TEND

TDEF (lt_trie_replace) {
	lt_trie_t *t = lt_trie_new();

	fail_unless(lt_trie_replace(t, "a", "a", NULL), "Unable to add");
	fail_unless(lt_trie_replace(t, "a", "a", NULL), "Adding same key should not fails on replacing");
	fail_unless(lt_trie_replace(t, "foo", "foo", NULL), "Unable to add");
	fail_unless(lt_trie_replace(t, "fooo", "foo", NULL), "Unable to add");

	lt_trie_unref(t);
} TEND

TDEF (lt_trie_remove) {
	lt_trie_t *t = lt_trie_new();

	fail_unless(lt_trie_add(t, "a", "a", NULL), "Unable to add");
	fail_unless(lt_trie_replace(t, "foo", "foo", NULL), "Unable to add");
	fail_unless(lt_trie_replace(t, "fooo", "foo", NULL), "Unable to add");
	fail_unless(lt_trie_remove(t, "foo"), "Unable to remove");
	fail_unless(!lt_trie_remove(t, "foo"), "should be removed already");
	fail_unless(lt_trie_remove(t, "fooo"), "Unable to remove");

	lt_trie_unref(t);
} TEND

TDEF (lt_trie_lookup) {
	lt_trie_t *t = lt_trie_new();
	lt_pointer_t p;

	fail_unless(lt_trie_add(t, "a", "a", NULL), "Unable to add");
	fail_unless((p = lt_trie_lookup(t, "a")) != NULL, "Unable to lookup");
	fail_unless(strcmp(p, "a") == 0, "not expected value on lookup");
	fail_unless(lt_trie_add(t, "foo", "foo", NULL), "Unable to add");
	fail_unless(lt_trie_add(t, "fooo", "fooo", NULL), "Unable to add");
	fail_unless((p = lt_trie_lookup(t, "foo")) != NULL, "Unable to lookup");
	fail_unless(strcmp(p, "foo") == 0, "not expected value on lookup");
	fail_unless((p = lt_trie_lookup(t, "fooo")) != NULL, "Unable to lookup");
	fail_unless(strcmp(p, "fooo") == 0, "not expected value on lookup");
	fail_unless(lt_trie_remove(t, "foo"), "Unable to remove");
	fail_unless(!lt_trie_lookup(t, "foo"), "Unable to lookup");
	fail_unless(lt_trie_lookup(t, "fooo") != NULL, "Unable to lookup");

	lt_trie_unref(t);
} TEND

/************************************************************/
Suite *
tester_suite(void)
{
	Suite *s = suite_create("lt_trie_t");
	TCase *tc = tcase_create("Basic functionality");

	tcase_add_checked_fixture(tc, setup, teardown);

	T (lt_trie_new);
	T (lt_trie_unref);
	T (lt_trie_add);
	T (lt_trie_replace);
	T (lt_trie_remove);
	T (lt_trie_lookup);

	suite_add_tcase(s, tc);

	return s;
}
