/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-error.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#if !defined (__LANGTAG_H__INSIDE) && !defined (__LANGTAG_COMPILATION)
#error "Only <liblangtag/langtag.h> can be included directly."
#endif

#ifndef __LT_ERROR_H__
#define __LT_ERROR_H__

#include <liblangtag/lt-macros.h>

LT_BEGIN_DECLS

/**
 * lt_error_type_t:
 * @LT_ERR_UNKNOWN: unknown error happened.
 * @LT_ERR_SUCCESS: an operation is succeeded.
 * @LT_ERR_OOM: Out of memory occurred.
 * @LT_ERR_FAIL_ON_XML: an error happened in libxml2.
 * @LT_ERR_EOT: No tokens to scan.
 * @LT_ERR_FAIL_ON_SCANNER: an error happened in the scanner.
 * @LT_ERR_NO_TAG: No tags to process.
 * @LT_ERR_INVALID: Invalid operation.
 * @LT_ERR_ANY: No real error, but just a flag to query all of errors or ask
 *              if any errors happen
 *
 * Error code used in this library.
*/
enum _lt_error_type_t {
	LT_ERR_UNKNOWN = -1,
	LT_ERR_SUCCESS = 0,
	LT_ERR_OOM,
	LT_ERR_FAIL_ON_XML,
	LT_ERR_EOT,
	LT_ERR_FAIL_ON_SCANNER,
	LT_ERR_NO_TAG,
	LT_ERR_INVALID,
	LT_ERR_ANY
};

/**
 * lt_error_t:
 *
 * All the fields in the <structname>lt_error_t</structname>
 * structure are private to the #lt_error_t implementation.
 */
typedef struct _lt_error_t	lt_error_t;
typedef enum _lt_error_type_t	lt_error_type_t;

lt_error_t *lt_error_new   (void);
lt_error_t *lt_error_ref   (lt_error_t       *error);
void        lt_error_unref (lt_error_t       *error);
lt_error_t *lt_error_set   (lt_error_t      **error,
			    lt_error_type_t   type,
                            const char       *message,
			    ...) LT_GNUC_PRINTF (3, 4);
void        lt_error_clear (lt_error_t       *error);
lt_bool_t   lt_error_is_set(lt_error_t       *error,
			    lt_error_type_t   type);
void        lt_error_print (lt_error_t       *error,
			    lt_error_type_t   type);

LT_END_DECLS

#endif /* __LT_ERROR_H__ */
