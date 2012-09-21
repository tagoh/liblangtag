/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * tag.c
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

#include <locale.h>
#include "langtag.h"

int
main(int    argc,
     char **argv)
{
	lt_tag_t *tag;

	setlocale(LC_ALL, "");

	lt_db_set_datadir(TEST_DATADIR);
	lt_db_initialize();
	tag = lt_tag_new();
	if (g_strcmp0(argv[1], "help") == 0) {
	  help:
		g_print("Usage: %s <command> ...\n"
			"commands: canonicalize, dump, from_locale, lookup, match, to_locale, transform\n",
			argv[0]);
	} else if (g_strcmp0(argv[1], "canonicalize") == 0) {
		gchar *s;

		if (lt_tag_parse(tag, argv[2], NULL)) {
			s = lt_tag_canonicalize(tag, NULL);
			g_print("%s -> %s\n", argv[2], s);
			g_free(s);
		}
	} else if (g_strcmp0(argv[1], "dump") == 0) {
		if (lt_tag_parse(tag, argv[2], NULL))
			lt_tag_dump(tag);
	} else if (g_strcmp0(argv[1], "from_locale") == 0) {
		lt_tag_unref(tag);
		tag = lt_tag_convert_from_locale(NULL);
		if (tag) {
			const gchar *s = lt_tag_get_string(tag);

			g_print("Tag: %s\n", s);
			lt_tag_dump(tag);
		}
	} else if (g_strcmp0(argv[1], "match") == 0) {
		if (lt_tag_parse(tag, argv[2], NULL)) {
			if (lt_tag_match(tag, argv[3], NULL))
				g_print("%s matches with %s\n", argv[3], argv[2]);
			else
				g_print("%s doesn't match with %s\n", argv[3], argv[2]);
		}
	} else if (g_strcmp0(argv[1], "lookup") == 0) {
		if (lt_tag_parse(tag, argv[2], NULL)) {
			gchar *result = lt_tag_lookup(tag, argv[3], NULL);
			if (result)
				g_print("%s\n", result);
			else
				g_print("%s doesn't match with %s\n", argv[3], argv[2]);
			g_free(result);
		}
	} else if (g_strcmp0(argv[1], "to_locale") == 0) {
		gchar *l;

		if (lt_tag_parse(tag, argv[2], NULL)) {
			l = lt_tag_convert_to_locale(tag, NULL);
			g_print("%s -> %s\n", argv[2], l);
			g_free(l);
		}
	} else if (g_strcmp0(argv[1], "transform") == 0) {
		gchar *r;

		if (lt_tag_parse(tag, argv[2], NULL)) {
			r = lt_tag_transform(tag, NULL);
			g_print("%s -> %s\n", argv[2], r);
			g_free(r);
		}
	} else {
		goto help;
	}
	if (tag)
		lt_tag_unref(tag);
	lt_db_finalize();

	return 0;
}
