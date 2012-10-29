/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * variant.c
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
	lt_variant_db_t *variantdb;
	lt_variant_t *variant;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	variantdb = lt_variant_db_new();

	if (lt_strcmp0(argv[1], "list_keys") == 0) {
		const char *key;
		lt_iter_t *iter;

		iter = lt_iter_init((lt_iter_tmpl_t *)variantdb);
		while (lt_iter_next(iter,
				    (lt_pointer_t *)&key,
				    NULL)) {
			printf("%s\n", key);
		}
		lt_iter_finish(iter);
	} else if (lt_strcmp0(argv[1], "list_values") == 0) {
		lt_iter_t *iter;

		iter = lt_iter_init((lt_iter_tmpl_t *)variantdb);
		while (lt_iter_next(iter,
				    NULL,
				    (lt_pointer_t *)&variant)) {
			lt_variant_dump(variant);
		}
		lt_iter_finish(iter);
	} else if (lt_strcmp0(argv[1], "lookup") == 0) {
		variant = lt_variant_db_lookup(variantdb, argv[2]);
		const lt_list_t *prefix, *l;

		if (variant) {
			printf("desc: %s\n", lt_variant_get_name(variant));
			prefix = lt_variant_get_prefix(variant);
			for (l = prefix; l != NULL; l = lt_list_next(l)) {
				printf("prefix: %s\n", (char *)lt_list_value(l));
			}
			lt_variant_unref(variant);
		} else {
			printf("no such variant: %s\n", argv[2]);
		}
	}

	lt_variant_db_unref(variantdb);

	return 0;
}
