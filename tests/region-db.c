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

#include <locale.h>
#include "langtag.h"

int
main(int    argc,
     char **argv)
{
	lt_region_db_t *regiondb;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	regiondb = lt_region_db_new();

	if (g_strcmp0(argv[1], "list") == 0) {
	} else if (g_strcmp0(argv[1], "lookup") == 0) {
		lt_region_t *region = lt_region_db_lookup(regiondb, argv[2]);

		g_print("%s (%s)\n", lt_region_get_tag(region), lt_region_get_name(region));
		lt_region_unref(region);
	}

	lt_region_db_unref(regiondb);

	return 0;
}
