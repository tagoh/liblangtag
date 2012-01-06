/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * variant.c
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
#include "lt-variant-db.h"

int
main(int    argc,
     char **argv)
{
	lt_variant_db_t *variantdb;

	setlocale(LC_ALL, "");

	variantdb = lt_variant_db_new();

	if (g_strcmp0(argv[1], "list") == 0) {
	} else if (g_strcmp0(argv[1], "lookup") == 0) {
		lt_variant_t *variant = lt_variant_db_lookup(variantdb, argv[2]);

		if (variant) {
			g_print("desc: %s\n", lt_variant_get_name(variant));
			g_print("prefix: %s\n", lt_variant_get_prefix(variant));
			lt_variant_unref(variant);
		} else {
			g_print("no such variant: %s\n", argv[2]);
		}
	}

	lt_variant_db_unref(variantdb);

	return 0;
}
