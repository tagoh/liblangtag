/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extension.h
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

#ifndef __LT_EXTENSION_H__
#define __LT_EXTENSION_H__

#include <glib.h>

G_BEGIN_DECLS

/**
 * lt_extension_t:
 *
 * All the fields in the <structname>lt_extension_t</structname>
 * structure are private to the #lt_extension_t implementation.
 */
typedef struct _lt_extension_t		lt_extension_t;


lt_extension_t *lt_extension_ref                  (lt_extension_t       *extension);
void            lt_extension_unref                (lt_extension_t       *extension);
const gchar    *lt_extension_get_tag              (lt_extension_t       *extension);
gchar          *lt_extension_get_canonicalized_tag(lt_extension_t       *extension);
void            lt_extension_dump                 (lt_extension_t       *extension);
gboolean        lt_extension_compare              (const lt_extension_t *v1,
                                                   const lt_extension_t *v2);
gboolean        lt_extension_truncate             (lt_extension_t       *extension);

G_END_DECLS

#endif /* __LT_EXTENSION_H__ */
