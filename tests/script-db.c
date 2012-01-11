/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * script-db.c
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
	lt_script_db_t *scriptdb;

	setlocale(LC_ALL, "");

	scriptdb = lt_script_db_new();

	if (g_strcmp0(argv[1], "list") == 0) {
		GList *l = lt_script_db_get_scripts(scriptdb), *ll;

		for (ll = l; ll != NULL; ll = g_list_next(ll)) {
			g_print("%s\n", (gchar *)ll->data);
		}
		g_list_free(l);
	} else if (g_strcmp0(argv[1], "code") == 0) {
		lt_script_t *script = lt_script_db_lookup(scriptdb, argv[2]);

		g_print("%s\n", lt_script_get_alpha_code(script));
		g_print("%s\n", lt_script_get_numeric_code(script));
		lt_script_unref(script);
	} else if (g_strcmp0(argv[1], "script") == 0) {
		lt_script_t *script = lt_script_db_lookup(scriptdb, argv[2]);

		g_print("%s\n", lt_script_get_name(script));
		lt_script_unref(script);
	}

	lt_script_db_unref(scriptdb);

	return 0;
}
