/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-tag.h
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

#ifndef __LT_TAG_H__
#define __LT_TAG_H__

#include <liblangtag/lt-macros.h>
#include <liblangtag/lt-lang.h>
#include <liblangtag/lt-extlang.h>
#include <liblangtag/lt-script.h>
#include <liblangtag/lt-region.h>
#include <liblangtag/lt-extension.h>
#include <liblangtag/lt-grandfathered.h>

LT_BEGIN_DECLS

/**
 * lt_tag_t:
 *
 * All the fields in the <structname>lt_tag_t</structname>
 * structure are private to the #lt_tag_t implementation.
 */
typedef struct _lt_tag_t	lt_tag_t;


lt_tag_t                 *lt_tag_new                   (void);
lt_tag_t                 *lt_tag_ref                   (lt_tag_t        *tag);
void                      lt_tag_unref                 (lt_tag_t        *tag);
lt_bool_t                 lt_tag_parse                 (lt_tag_t        *tag,
                                                        const char      *tag_string,
                                                        GError         **error);
lt_bool_t                 lt_tag_parse_with_extra_token(lt_tag_t        *tag,
                                                        const char      *tag_string,
                                                        GError         **error);
void                      lt_tag_clear                 (lt_tag_t        *tag);
lt_tag_t                 *lt_tag_copy                  (const lt_tag_t  *tag);
lt_bool_t                 lt_tag_truncate              (lt_tag_t        *tag,
                                                        GError         **error);
const char               *lt_tag_get_string            (lt_tag_t        *tag);
char                     *lt_tag_canonicalize          (lt_tag_t        *tag,
                                                        GError         **error);
char                     *lt_tag_convert_to_locale     (lt_tag_t        *tag,
                                                        GError         **error);
lt_tag_t                 *lt_tag_convert_from_locale   (GError         **error);
void                      lt_tag_dump                  (const lt_tag_t  *tag);
lt_bool_t                 lt_tag_compare               (const lt_tag_t  *v1,
                                                        const lt_tag_t  *v2);
lt_bool_t                 lt_tag_match                 (const lt_tag_t  *v1,
                                                        const char      *v2,
                                                        GError         **error);
char                     *lt_tag_lookup                (const lt_tag_t  *tag,
                                                        const char      *pattern,
                                                        GError         **error);
char                     *lt_tag_transform             (lt_tag_t        *tag,
                                                        GError         **error);
const lt_lang_t          *lt_tag_get_language          (const lt_tag_t  *tag);
const lt_extlang_t       *lt_tag_get_extlang           (const lt_tag_t  *tag);
const lt_script_t        *lt_tag_get_script            (const lt_tag_t  *tag);
const lt_region_t        *lt_tag_get_region            (const lt_tag_t  *tag);
const GList              *lt_tag_get_variants          (const lt_tag_t  *tag);
const lt_extension_t     *lt_tag_get_extension         (const lt_tag_t  *tag);
const GString            *lt_tag_get_privateuse        (const lt_tag_t  *tag);
const lt_grandfathered_t *lt_tag_get_grandfathered     (const lt_tag_t  *tag);

LT_END_DECLS

#endif /* __LT_TAG_H__ */
