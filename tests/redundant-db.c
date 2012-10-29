/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * redundant-db.c
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

#include <stdio.h>
#include <locale.h>
#include "langtag.h"
#include "lt-utils.h"

int
main(int    argc,
     char **argv)
{
	lt_redundant_db_t *redundantdb;
	lt_redundant_t *redundant;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	redundantdb = lt_redundant_db_new();

	if (lt_strcmp0(argv[1], "list_keys") == 0) {
		const char *key;
		lt_iter_t *iter;

		iter = lt_iter_init((lt_iter_tmpl_t *)redundantdb);
		while (lt_iter_next(iter,
				    (lt_pointer_t *)&key,
				    NULL)) {
			printf("%s\n", key);
		}
		lt_iter_finish(iter);
	} else if (lt_strcmp0(argv[1], "list_values") == 0) {
		lt_iter_t *iter;

		iter = lt_iter_init((lt_iter_tmpl_t *)redundantdb);
		while (lt_iter_next(iter,
				    NULL,
				    (lt_pointer_t *)&redundant)) {
			lt_redundant_dump(redundant);
		}
		lt_iter_finish(iter);
	} else if (lt_strcmp0(argv[1], "lookup") == 0) {
		redundant = lt_redundant_db_lookup(redundantdb, argv[2]);
		if (!redundant) {
			printf("No entry for %s\n", argv[2]);
		} else {
			printf("%s (%s)\n",
			       lt_redundant_get_tag(redundant),
			       lt_redundant_get_name(redundant));
		}
		lt_redundant_unref(redundant);
	}

	lt_redundant_db_unref(redundantdb);

	return 0;
}
