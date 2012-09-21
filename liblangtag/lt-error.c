/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-error.c
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

#include "lt-error.h"


/**
 * SECTION:lt-error
 * @Short_Description: Error handling
 * @Title: Error
 *
 * This section describes the error handling in this library.
 */
/*< private >*/

/*< public >*/
/**
 * lt_error_get_quark:
 *
 * Obtains #GQuark for #GError.
 *
 * Returns: a #GQuark.
 */
GQuark
lt_error_get_quark(void)
{
	static GQuark quark = 0;

	if (quark == 0)
		quark = g_quark_from_static_string("lt-error-quark");

	return quark;
}
