/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-mdule-private.h
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
#ifndef __LT_EXT_MODULE_PRIVATE_H__
#define __LT_EXT_MODULE_PRIVATE_H__

#include <glib.h>
#include <liblangtag/lt-ext-module-data.h>
#include <liblangtag/lt-ext-module.h>

G_BEGIN_DECLS

#define LT_MAX_EXT_MODULES	(('9' - '0' + 1) + ('z' - 'a' + 1) + 2)

gboolean              lt_ext_module_validate_singleton   (gchar                  singleton);
gint                  lt_ext_module_singleton_char_to_int(gchar                  singleton_c);
gchar                 lt_ext_module_singleton_int_to_char(gint                   singleton);
lt_ext_module_t      *lt_ext_module_lookup               (gchar                  singleton_c);
lt_ext_module_t      *lt_ext_module_new                  (const gchar           *name);
const gchar          *lt_ext_module_get_name             (lt_ext_module_t       *module);
gchar                 lt_ext_module_get_singleton        (lt_ext_module_t       *module);
gchar                *lt_ext_module_get_tag              (lt_ext_module_t       *module,
							  lt_ext_module_data_t  *data);
lt_ext_module_data_t *lt_ext_module_create_data          (lt_ext_module_t       *module);
gboolean              lt_ext_module_parse_tag            (lt_ext_module_t       *module,
                                                          lt_ext_module_data_t  *data,
                                                          const gchar           *subtag,
                                                          GError               **error);

G_END_DECLS

#endif /* __LT_EXT_MODULE_PRIVATE_H__ */
