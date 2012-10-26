/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * extlang.c
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
	lt_extlang_db_t *extlangdb;
	lt_extlang_t *extlang;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	extlangdb = lt_extlang_db_new();

	if (lt_strcmp0(argv[1], "list_keys") == 0) {
		const char *key;
		lt_iter_t *iter;

		iter = lt_iter_init((lt_iter_tmpl_t *)extlangdb);
		while (lt_iter_next(iter,
				    (lt_pointer_t *)&key,
				    NULL)) {
			printf("%s\n", key);
		}
		lt_iter_finish(iter);
	} else if (lt_strcmp0(argv[1], "list_values") == 0) {
		lt_iter_t *iter;

		iter = lt_iter_init((lt_iter_tmpl_t *)extlangdb);
		while (lt_iter_next(iter,
				    NULL,
				    (lt_pointer_t *)&extlang)) {
			lt_extlang_dump(extlang);
		}
		lt_iter_finish(iter);
	} else if (lt_strcmp0(argv[1], "lookup") == 0) {
		extlang = lt_extlang_db_lookup(extlangdb, argv[2]);

		if (extlang) {
			printf("desc: %s\n", lt_extlang_get_name(extlang));
			printf("lang: %s\n", lt_extlang_get_macro_language(extlang));
			lt_extlang_unref(extlang);
		} else {
			printf("no such extlang: %s\n", argv[2]);
		}
	}

	lt_extlang_db_unref(extlangdb);

	return 0;
}
