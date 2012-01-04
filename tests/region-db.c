/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * region-db.c
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
#include "lt-region-db.h"

int
main(int    argc,
     char **argv)
{
	lt_region_db_t *regiondb;

	setlocale(LC_ALL, "");

	regiondb = lt_region_db_new();

	if (g_strcmp0(argv[1], "list") == 0) {
		GList *l = lt_region_db_get_regions(regiondb), *ll;

		for (ll = l; ll != NULL; ll = g_list_next(ll)) {
			g_print("%s\n", (gchar *)ll->data);
		}
		g_list_free(l);
	} else if (g_strcmp0(argv[1], "code") == 0) {
		lt_region_t *region = lt_region_db_lookup_region_from_language(regiondb, argv[2]);

		g_print("N: %s\n", lt_region_get_code(region, LT_REGION_CODE_NUMERIC));
		g_print("2: %s\n", lt_region_has_alpha_2_code(region) ? lt_region_get_code(region, LT_REGION_CODE_ALPHA_2) : "N/A");
		g_print("3: %s\n", lt_region_get_code(region, LT_REGION_CODE_ALPHA_3));
		g_print("4: %s\n", lt_region_has_alpha_4_code(region) ? lt_region_get_code(region, LT_REGION_CODE_ALPHA_4) : "N/A");
		lt_region_unref(region);
	} else if (g_strcmp0(argv[1], "region") == 0) {
		lt_region_t *region = lt_region_db_lookup_region_from_code(regiondb, argv[2]);

		g_print("%s\n", lt_region_get_name(region));
		lt_region_unref(region);
	} else if (g_strcmp0(argv[1], "subcode") == 0) {
		lt_division_t *div = lt_region_db_lookup_division(regiondb, argv[2]);

		g_print("%s\n", lt_division_get_code(div));
		lt_division_unref(div);
	} else if (g_strcmp0(argv[1], "subname") == 0) {
		lt_division_t *div = lt_region_db_lookup_division(regiondb, argv[2]);

		g_print("%s\n", lt_division_get_name(div));
		lt_division_unref(div);
	} else if (g_strcmp0(argv[1], "subtype") == 0) {
		GList *l = lt_region_db_get_division_type(regiondb, argv[2]), *ll;

		for (ll = l; ll != NULL; ll = g_list_next(ll)) {
			g_print("%s\n", (gchar *)ll->data);
		}
		g_list_free(l);
	} else if (g_strcmp0(argv[1], "subnames") == 0) {
		GList *l = lt_region_db_get_divisions(regiondb, argv[2], argv[3]), *ll;

		for (ll = l; ll != NULL; ll = g_list_next(ll)) {
			lt_division_t *div = ll->data;

			g_print("%s\n", lt_division_get_name(div));
			lt_division_unref(div);
		}
		g_list_free(l);
	} else if (g_strcmp0(argv[1], "subcodes") == 0) {
		GList *l = lt_region_db_get_divisions(regiondb, argv[2], argv[3]), *ll;

		for (ll = l; ll != NULL; ll = g_list_next(ll)) {
			lt_division_t *div = ll->data;

			g_print("%s\n", lt_division_get_code(div));
			lt_division_unref(div);
		}
		g_list_free(l);
	}

	lt_region_db_unref(regiondb);

	return 0;
}
