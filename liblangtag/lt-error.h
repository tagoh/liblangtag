/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-error.h
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
#ifndef __LT_ERROR_H__
#define __LT_ERROR_H__

#include <glib.h>

G_BEGIN_DECLS

/**
 * LT_ERROR:
 *
 * A #GQuark value being used in this library.
 */
#define LT_ERROR	(lt_error_get_quark())

/**
 * lt_error_t:
 * @LT_ERR_UNKNOWN: unknown error happened.
 * @LT_ERR_SUCCESS: an operation is succeeded.
 * @LT_ERR_OOM: Out of memory occurred.
 * @LT_ERR_FAIL_ON_XML: an error happened in libxml2.
 * @LT_ERR_EOT: No tokens to scan.
 * @LT_ERR_FAIL_ON_SCANNER: an error happened in the scanner.
 * @LT_ERR_NO_TAG: No tags to process.
 * @LT_ERR_END: No real error, but just a terminator.
 *
 * Error code used in this library.
*/
enum _lt_error_t {
	LT_ERR_UNKNOWN = -1,
	LT_ERR_SUCCESS = 0,
	LT_ERR_OOM,
	LT_ERR_FAIL_ON_XML,
	LT_ERR_EOT,
	LT_ERR_FAIL_ON_SCANNER,
	LT_ERR_NO_TAG,
	LT_ERR_END
};

typedef enum _lt_error_t	lt_error_t;


GQuark lt_error_get_quark(void);

G_END_DECLS

#endif /* __LT_ERROR_H__ */
