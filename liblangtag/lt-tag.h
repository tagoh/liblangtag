/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-tag.h
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

#ifndef __LT_TAG_H__
#define __LT_TAG_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct _lt_tag_t	lt_tag_t;


lt_tag_t *lt_tag_new              (void);
lt_tag_t *lt_tag_ref              (lt_tag_t     *tag);
void      lt_tag_unref            (lt_tag_t     *tag);
gboolean  lt_tag_parse            (lt_tag_t     *tag,
                                   const gchar  *langtag,
                                   GError      **error);
gchar    *lt_tag_canonicalize     (lt_tag_t     *tag,
                                   GError      **error);
gchar    *lt_tag_convert_to_locale(lt_tag_t     *tag,
                                   GError      **error);
void      lt_tag_dump             (lt_tag_t     *tag);

G_END_DECLS

#endif /* __LT_TAG_H__ */
