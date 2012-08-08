/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lang-db.c
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
	lt_lang_db_t *langdb;
	lt_lang_t *lang;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	langdb = lt_lang_db_new();

	if (g_strcmp0(argv[1], "list") == 0) {
	} else if (g_strcmp0(argv[1], "lookup") == 0) {
		lang = lt_lang_db_lookup(langdb, argv[2]);
		if (!lang) {
			g_print("No entry for %s\n", argv[2]);
		} else {
			g_print("%s (%s)\n",
				lt_lang_get_tag(lang),
				lt_lang_get_name(lang));
		}
		lt_lang_unref(lang);
	}

	lt_lang_db_unref(langdb);

	return 0;
}
