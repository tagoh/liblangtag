/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-variant.h
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

#ifndef __LT_VARIANT_H__
#define __LT_VARIANT_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct _lt_variant_t	lt_variant_t;


lt_variant_t *lt_variant_ref              (lt_variant_t       *variant);
void          lt_variant_unref            (lt_variant_t       *variant);
const gchar  *lt_variant_get_better_tag   (const lt_variant_t *variant);
const gchar  *lt_variant_get_tag          (const lt_variant_t *variant);
const gchar  *lt_variant_get_preferred_tag(const lt_variant_t *variant);
const gchar  *lt_variant_get_name         (const lt_variant_t *variant);
const GList  *lt_variant_get_prefix       (const lt_variant_t *variant);
void          lt_variant_dump             (const lt_variant_t *variant);
gboolean      lt_variant_compare          (const lt_variant_t *v1,
					   const lt_variant_t *v2);

G_END_DECLS

#endif /* __LT_VARIANT_H__ */
