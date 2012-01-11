/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * grandfathered.c
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
	lt_grandfathered_db_t *grandfathereddb;

	setlocale(LC_ALL, "");

	grandfathereddb = lt_grandfathered_db_new();

	if (g_strcmp0(argv[1], "list") == 0) {
	} else if (g_strcmp0(argv[1], "lookup") == 0) {
		lt_grandfathered_t *grandfathered = lt_grandfathered_db_lookup(grandfathereddb, argv[2]);

		if (grandfathered) {
			g_print("desc: %s\n", lt_grandfathered_get_name(grandfathered));
			lt_grandfathered_unref(grandfathered);
		} else {
			g_print("no such grandfathered: %s\n", argv[2]);
		}
	}

	lt_grandfathered_db_unref(grandfathereddb);

	return 0;
}
