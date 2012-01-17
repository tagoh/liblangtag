/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-error.c
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
