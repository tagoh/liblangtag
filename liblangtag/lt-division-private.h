/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-division-private.h
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
#ifndef __LT_DIVISION_PRIVATE_H__
#define __LT_DIVISION_PRIVATE_H__

#include <glib.h>
#include "lt-division.h"

G_BEGIN_DECLS

enum _lt_division_vtype_t {
	LT_DIVISION_COUNTRY_CODE,
	LT_DIVISION_TYPE,
	LT_DIVISION_CODE,
	LT_DIVISION_NAME
};

typedef enum _lt_division_vtype_t	lt_division_vtype_t;


lt_division_t *lt_division_create(void);
void           lt_division_set   (lt_division_t       *division,
                                  lt_division_vtype_t  type,
                                  const gchar         *val);

G_END_DECLS

#endif /* __LT_DIVISION_PRIVATE_H__ */
