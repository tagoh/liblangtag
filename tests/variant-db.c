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

#include <locale.h>
#include "langtag.h"

int
main(int    argc,
     char **argv)
{
	lt_variant_db_t *variantdb;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	variantdb = lt_variant_db_new();

	if (g_strcmp0(argv[1], "list") == 0) {
	} else if (g_strcmp0(argv[1], "lookup") == 0) {
		lt_variant_t *variant = lt_variant_db_lookup(variantdb, argv[2]);
		const GList *prefix, *l;

		if (variant) {
			g_print("desc: %s\n", lt_variant_get_name(variant));
			prefix = lt_variant_get_prefix(variant);
			for (l = prefix; l != NULL; l = g_list_next(l)) {
				g_print("prefix: %s\n", (gchar *)l->data);
			}
			lt_variant_unref(variant);
		} else {
			g_print("no such variant: %s\n", argv[2]);
		}
	}

	lt_variant_db_unref(variantdb);

	return 0;
}
