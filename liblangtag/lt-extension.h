/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extension.h
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

#ifndef __LT_EXTENSION_H__
#define __LT_EXTENSION_H__

#include <liblangtag/lt-macros.h>

LT_BEGIN_DECLS

/**
 * lt_extension_t:
 *
 * All the fields in the <structname>lt_extension_t</structname>
 * structure are private to the #lt_extension_t implementation.
 */
typedef struct _lt_extension_t		lt_extension_t;


lt_extension_t *lt_extension_ref                  (lt_extension_t       *extension);
void            lt_extension_unref                (lt_extension_t       *extension);
const char     *lt_extension_get_tag              (lt_extension_t       *extension);
char           *lt_extension_get_canonicalized_tag(lt_extension_t       *extension);
void            lt_extension_dump                 (lt_extension_t       *extension);
lt_bool_t       lt_extension_compare              (const lt_extension_t *v1,
                                                   const lt_extension_t *v2);
lt_bool_t       lt_extension_truncate             (lt_extension_t       *extension);

LT_END_DECLS

#endif /* __LT_EXTENSION_H__ */
