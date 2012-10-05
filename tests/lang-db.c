/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lang-db.c
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
	lt_lang_db_t *langdb;
	lt_lang_t *lang;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	langdb = lt_lang_db_new();

	if (lt_strcmp0(argv[1], "list") == 0) {
	} else if (lt_strcmp0(argv[1], "lookup") == 0) {
		lang = lt_lang_db_lookup(langdb, argv[2]);
		if (!lang) {
			printf("No entry for %s\n", argv[2]);
		} else {
			printf("%s (%s)\n",
			       lt_lang_get_tag(lang),
			       lt_lang_get_name(lang));
		}
		lt_lang_unref(lang);
	}

	lt_lang_db_unref(langdb);

	return 0;
}
