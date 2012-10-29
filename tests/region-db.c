/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * region-db.c
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
	lt_region_db_t *regiondb;
	lt_region_t *region;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	regiondb = lt_region_db_new();

	if (lt_strcmp0(argv[1], "list_keys") == 0) {
		const char *key;
		lt_iter_t *iter;

		iter = lt_iter_init((lt_iter_tmpl_t *)regiondb);
		while (lt_iter_next(iter,
				    (lt_pointer_t *)&key,
				    NULL)) {
			printf("%s\n", key);
		}
		lt_iter_finish(iter);
	} else if (lt_strcmp0(argv[1], "list_values") == 0) {
		lt_iter_t *iter;

		iter = lt_iter_init((lt_iter_tmpl_t *)regiondb);
		while (lt_iter_next(iter,
				    NULL,
				    (lt_pointer_t *)&region)) {
			lt_region_dump(region);
		}
		lt_iter_finish(iter);
	} else if (lt_strcmp0(argv[1], "lookup") == 0) {
		region = lt_region_db_lookup(regiondb, argv[2]);

		printf("%s (%s)\n", lt_region_get_tag(region), lt_region_get_name(region));
		lt_region_unref(region);
	}

	lt_region_db_unref(regiondb);

	return 0;
}
