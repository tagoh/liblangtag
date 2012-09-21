/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-utils.c
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

#include <string.h>
#include "lt-utils.h"


/*< private >*/

/*< public >*/
gchar *
lt_strlower(gchar *string)
{
	gsize len;
	gchar *p = string;

	g_return_val_if_fail (string != NULL, NULL);

	len = strlen(string);
	while (len) {
		*p = g_ascii_tolower(*p);
		p++;
		len--;
	}

	return string;
}
