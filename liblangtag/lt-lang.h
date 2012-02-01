/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-lang.h
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
