/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-gobject.h
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
#ifndef __LT_GOBJECT_H__
#define __LT_GOBJECT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define LT_TYPE_EXTLANG_DB		lt_extlang_db_get_type()
#define LT_TYPE_EXTLANG			lt_extlang_get_type()
#define LT_TYPE_GRANDFATHERED_DB	lt_grandfathered_db_get_type()
#define LT_TYPE_GRANDFATHERED		lt_grandfathered_get_type()
#define LT_TYPE_LANG_DB			lt_lang_db_get_type()
#define LT_TYPE_LANG			lt_lang_get_type()
#define LT_TYPE_REDUNDANT_DB		lt_redundant_db_get_type()
#define LT_TYPE_REDUNDANT		lt_redundant_get_type()
#define LT_TYPE_REGION_DB		lt_region_db_get_type()
#define LT_TYPE_REGION			lt_region_get_type()
#define LT_TYPE_SCRIPT_DB		lt_script_db_get_type()
#define LT_TYPE_SCRIPT			lt_script_get_type()
#define LT_TYPE_TAG			lt_tag_get_type()
#define LT_TYPE_VARIANT_DB		lt_variant_db_get_type()
#define LT_TYPE_VARIANT			lt_variant_get_type()


GType lt_extlang_db_get_type      (void);
GType lt_extlang_get_type         (void);
GType lt_grandfathered_db_get_type(void);
GType lt_grandfathered_get_type   (void);
GType lt_lang_db_get_type         (void);
GType lt_lang_get_type            (void);
GType lt_redundant_db_get_type    (void);
GType lt_redundant_get_type       (void);
GType lt_region_db_get_type       (void);
GType lt_region_get_type          (void);
GType lt_script_db_get_type       (void);
GType lt_script_get_type          (void);
GType lt_tag_get_type             (void);
GType lt_variant_db_get_type      (void);
GType lt_variant_get_type         (void);

G_END_DECLS

#endif /* __LT_GOBJECT_H__ */
