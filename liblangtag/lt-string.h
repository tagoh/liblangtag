/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-string.h
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

#ifndef __LT_STRING_H__
#define __LT_STRING_H__

#include <liblangtag/lt-macros.h>

LT_BEGIN_DECLS

typedef struct _lt_string_t	lt_string_t;

lt_string_t *lt_string_new            (const char        *string);
lt_string_t *lt_string_ref            (lt_string_t       *string);
void         lt_string_unref          (lt_string_t       *string);
char        *lt_string_free           (lt_string_t       *string,
                                       lt_bool_t          free_segment);
size_t       lt_string_length         (const lt_string_t *string);
const char  *lt_string_value          (const lt_string_t *string);
lt_string_t *lt_string_truncate       (lt_string_t       *string,
                                       ssize_t            len);
void         lt_string_clear          (lt_string_t       *string);
lt_bool_t    lt_string_append_c       (lt_string_t       *string,
                                       char               c);
lt_bool_t    lt_string_append         (lt_string_t       *string,
                                       const char        *str);
lt_bool_t    lt_string_append_filename(lt_string_t       *string,
                                       const char        *path,
				       ...) LT_GNUC_NULL_TERMINATED;
lt_bool_t    lt_string_append_printf  (lt_string_t       *string,
                                       const char        *format,
				       ...) LT_GNUC_PRINTF (2, 3);
lt_string_t *lt_string_replace_c      (lt_string_t       *string,
                                       size_t             pos,
                                       char               c);
char         lt_string_at             (lt_string_t       *string,
                                       ssize_t            pos);

LT_END_DECLS

#endif /* __LT_STRING_H__ */
