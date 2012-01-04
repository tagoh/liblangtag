/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * region.c
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
#include "lt-region.h"

int
main(int    argc,
     char **argv)
{
	lt_region_t *region;

	setlocale(LC_ALL, "");

	region = lt_region_new();

	if (g_strcmp0(argv[1], "list") == 0) {
		GList *l = lt_region_get_regions(region), *ll;

		for (ll = l; ll != NULL; ll = g_list_next(ll)) {
			g_print("%s\n", (gchar *)ll->data);
		}
		g_list_free(l);
	} else if (g_strcmp0(argv[1], "code") == 0) {
		g_print("%s\n", lt_region_lookup_code(region, argv[2], LT_REGION_CODE_NUMERIC));
		g_print("%s\n", lt_region_lookup_code(region, argv[2], LT_REGION_CODE_ALPHA_2));
		g_print("%s\n", lt_region_lookup_code(region, argv[2], LT_REGION_CODE_ALPHA_3));
		g_print("%s\n", lt_region_lookup_code(region, argv[2], LT_REGION_CODE_ALPHA_4));
	} else if (g_strcmp0(argv[1], "region") == 0) {
		g_print("%s\n", lt_region_lookup_region(region, argv[2]));
	} else if (g_strcmp0(argv[1], "subcode") == 0) {
		g_print("%s\n", lt_region_lookup_subset_code(region, argv[2]));
	} else if (g_strcmp0(argv[1], "subname") == 0) {
		g_print("%s\n", lt_region_lookup_subset_name(region, argv[2]));
	} else if (g_strcmp0(argv[1], "subtype") == 0) {
		GList *l = lt_region_lookup_subset_type(region, argv[2]), *ll;

		for (ll = l; ll != NULL; ll = g_list_next(ll)) {
			g_print("%s\n", (gchar *)ll->data);
		}
		g_list_free(l);
	} else if (g_strcmp0(argv[1], "subnames") == 0) {
		GList *l = lt_region_lookup_subset_names(region, argv[2], argv[3]), *ll;

		for (ll = l; ll != NULL; ll = g_list_next(ll)) {
			g_print("%s\n", (gchar *)ll->data);
		}
		g_list_free(l);
	} else if (g_strcmp0(argv[1], "subcodes") == 0) {
		GList *l = lt_region_lookup_subset_codes(region, argv[2], argv[3]), *ll;

		for (ll = l; ll != NULL; ll = g_list_next(ll)) {
			g_print("%s\n", (gchar *)ll->data);
		}
		g_list_free(l);
	}

	lt_region_unref(region);

	return 0;
}
