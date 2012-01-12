/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extlang.h
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

#ifndef __LT_EXTLANG_H__
#define __LT_EXTLANG_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct _lt_extlang_t	lt_extlang_t;


lt_extlang_t *lt_extlang_ref               (lt_extlang_t       *extlang);
void          lt_extlang_unref             (lt_extlang_t       *extlang);
const gchar  *lt_extlang_get_tag           (const lt_extlang_t *extlang);
const gchar  *lt_extlang_get_name          (const lt_extlang_t *extlang);
const gchar  *lt_extlang_get_macro_language(const lt_extlang_t *extlang);
void          lt_extlang_dump              (const lt_extlang_t *extlang);

G_END_DECLS

#endif /* __LT_EXTLANG_H__ */
