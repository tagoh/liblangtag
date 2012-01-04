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
#include "lt-lang-db.h"

int
main(int    argc,
     char **argv)
{
	lt_lang_db_t *langdb;

	setlocale(LC_ALL, "");

	langdb = lt_lang_db_new(LT_LANG_DB_READ_MINIMAL);

	if (g_strcmp0(argv[1], "list") == 0) {
		GList *l = lt_lang_db_get_languages(langdb), *ll;

		for (ll = l; ll != NULL; ll = g_list_next(ll)) {
			g_print("%s\n", (gchar *)ll->data);
		}
		g_list_free(l);
	} else if (g_strcmp0(argv[1], "code") == 0) {
		g_print("%s\n", lt_lang_db_lookup_code(langdb, argv[2], LT_LANG_CODE_ID));
		g_print("%s\n", lt_lang_db_lookup_code(langdb, argv[2], LT_LANG_CODE_PART1));
		g_print("%s\n", lt_lang_db_lookup_code(langdb, argv[2], LT_LANG_CODE_PART2));
	} else if (g_strcmp0(argv[1], "lang") == 0) {
		g_print("%s\n", lt_lang_db_lookup_language(langdb, argv[2]));
	}

	lt_lang_db_unref(langdb);

	return 0;
}
