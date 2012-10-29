/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * script-db.c
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
	lt_script_db_t *scriptdb;
	lt_script_t *script;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	scriptdb = lt_script_db_new();

	if (lt_strcmp0(argv[1], "list_keys") == 0) {
		const char *key;
		lt_iter_t *iter;

		iter = lt_iter_init((lt_iter_tmpl_t *)scriptdb);
		while (lt_iter_next(iter,
				    (lt_pointer_t *)&key,
				    NULL)) {
			printf("%s\n", key);
		}
		lt_iter_finish(iter);
	} else if (lt_strcmp0(argv[1], "list_values") == 0) {
		lt_iter_t *iter;

		iter = lt_iter_init((lt_iter_tmpl_t *)scriptdb);
		while (lt_iter_next(iter,
				    NULL,
				    (lt_pointer_t *)&script)) {
			lt_script_dump(script);
		}
		lt_iter_finish(iter);
	} else if (lt_strcmp0(argv[1], "lookup") == 0) {
		script = lt_script_db_lookup(scriptdb, argv[2]);

		printf("%s (%s)\n",
		       lt_script_get_tag(script),
		       lt_script_get_name(script));
		lt_script_unref(script);
	}

	lt_script_db_unref(scriptdb);

	return 0;
}
