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

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	extlangdb = lt_extlang_db_new();

	if (lt_strcmp0(argv[1], "list") == 0) {
	} else if (lt_strcmp0(argv[1], "lookup") == 0) {
		lt_extlang_t *extlang = lt_extlang_db_lookup(extlangdb, argv[2]);

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
