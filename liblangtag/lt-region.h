/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-region.h
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
#ifndef __LT_REGION_H__
#define __LT_REGION_H__

#include <glib.h>

G_BEGIN_DECLS

typedef struct _lt_region_t	lt_region_t;


lt_region_t *lt_region_ref     (lt_region_t       *region);
void         lt_region_unref   (lt_region_t       *region);
const gchar *lt_region_get_name(const lt_region_t *region);
const gchar *lt_region_get_tag (const lt_region_t *region);

G_END_DECLS

#endif /* __LT_REGION_H__ */
