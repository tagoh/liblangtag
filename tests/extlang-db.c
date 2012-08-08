/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * extlang.c
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
	lt_extlang_db_t *extlangdb;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	extlangdb = lt_extlang_db_new();

	if (g_strcmp0(argv[1], "list") == 0) {
	} else if (g_strcmp0(argv[1], "lookup") == 0) {
		lt_extlang_t *extlang = lt_extlang_db_lookup(extlangdb, argv[2]);

		if (extlang) {
			g_print("desc: %s\n", lt_extlang_get_name(extlang));
			g_print("lang: %s\n", lt_extlang_get_macro_language(extlang));
			lt_extlang_unref(extlang);
		} else {
			g_print("no such extlang: %s\n", argv[2]);
		}
	}

	lt_extlang_db_unref(extlangdb);

	return 0;
}
