/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extension-private.h
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
#ifndef __LT_EXTENSION_PRIVATE_H__
#define __LT_EXTENSION_PRIVATE_H__

#include <glib.h>
#include "lt-ext-module.h"
#include "lt-extension.h"

G_BEGIN_DECLS

lt_extension_t *lt_extension_create       (void);
lt_extension_t *lt_extension_copy         (lt_extension_t  *extension);
gboolean        lt_extension_has_singleton(lt_extension_t  *extension,
                                           gchar            singleton_c);
gboolean        lt_extension_add_singleton(lt_extension_t  *extension,
                                           gchar            singleton_c);
gboolean        lt_extension_add_tag      (lt_extension_t  *extension,
                                           const gchar     *subtag,
                                           GError         **error);
void            lt_extension_cancel_tag   (lt_extension_t  *extension);
gboolean        lt_extension_validate     (lt_extension_t  *extension);

G_END_DECLS

#endif /* __LT_EXTENSION_PRIVATE_H__ */
