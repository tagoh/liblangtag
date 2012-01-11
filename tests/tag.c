/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * tag.c
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
	lt_tag_t *tag;

	setlocale(LC_ALL, "");

	lt_db_initialize();
	tag = lt_tag_new();
	if (g_strcmp0(argv[1], "locale") == 0) {
		gchar *l;

		lt_tag_parse(tag, argv[2], NULL);
		l = lt_tag_convert_to_locale(tag, NULL);
		g_print("%s -> %s\n", argv[2], l);
		g_free(l);
	} else if (g_strcmp0(argv[1], "dump") == 0) {
		lt_tag_parse(tag, argv[2], NULL);
		lt_tag_dump(tag);
	}
	lt_tag_unref(tag);
	lt_db_finalize();
	lt_db_finalize();

	return 0;
}
