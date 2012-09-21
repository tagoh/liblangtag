/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * grandfathered.c
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
	lt_grandfathered_db_t *grandfathereddb;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	grandfathereddb = lt_grandfathered_db_new();

	if (g_strcmp0(argv[1], "list") == 0) {
	} else if (g_strcmp0(argv[1], "lookup") == 0) {
		lt_grandfathered_t *grandfathered = lt_grandfathered_db_lookup(grandfathereddb, argv[2]);

		if (grandfathered) {
			g_print("desc: %s\n", lt_grandfathered_get_name(grandfathered));
			lt_grandfathered_unref(grandfathered);
		} else {
			g_print("no such grandfathered: %s\n", argv[2]);
		}
	}

	lt_grandfathered_db_unref(grandfathereddb);

	return 0;
}
