/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-lang.h
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

#ifndef __LT_LANG_H__
#define __LT_LANG_H__

#include <glib.h>

G_BEGIN_DECLS

/**
 * lt_lang_t:
 *
 * All the fields in the <structname>lt_lang_t</structname>
 * structure are private to the #lt_lang_t implementation.
 */
typedef struct _lt_lang_t	lt_lang_t;


lt_lang_t   *lt_lang_ref                (lt_lang_t       *lang);
void         lt_lang_unref              (lt_lang_t       *lang);
const gchar *lt_lang_get_name           (const lt_lang_t *lang);
const gchar *lt_lang_get_better_tag     (const lt_lang_t *lang);
const gchar *lt_lang_get_tag            (const lt_lang_t *lang);
const gchar *lt_lang_get_preferred_tag  (const lt_lang_t *lang);
const gchar *lt_lang_get_suppress_script(const lt_lang_t *lang);
const gchar *lt_lang_get_macro_language (const lt_lang_t *lang);
const gchar *lt_lang_get_scope          (const lt_lang_t *lang);
void         lt_lang_dump               (const lt_lang_t *lang);
gboolean     lt_lang_compare            (const lt_lang_t *v1,
					 const lt_lang_t *v2);

G_END_DECLS

#endif /* __LT_LANG_H__ */
