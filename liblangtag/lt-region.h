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

enum _lt_region_code_t {
	LT_REGION_CODE_NUMERIC = 1,
	LT_REGION_CODE_ALPHA_2,
	LT_REGION_CODE_ALPHA_3,
	LT_REGION_CODE_ALPHA_4,
};
enum _lt_region_subset_vtype_t {
	LT_REGION_SUBSET_COUNTRY_CODE,
	LT_REGION_SUBSET_TYPE,
	LT_REGION_SUBSET_ENTRY_CODE,
	LT_REGION_SUBSET_ENTRY_NAME
};

typedef struct _lt_region_t		lt_region_t;
typedef enum _lt_region_code_t		lt_region_code_t;
typedef enum _lt_region_subset_vtype_t	lt_region_subset_vtype_t;


lt_region_t *lt_region_new               (void);
lt_region_t *lt_region_ref               (lt_region_t      *region);
void         lt_region_unref             (lt_region_t      *region);
GList       *lt_region_get_regions       (lt_region_t      *region);
const gchar *lt_region_lookup_code       (lt_region_t      *region,
                                          const gchar      *language,
                                          lt_region_code_t  type);
const gchar *lt_region_lookup_region     (lt_region_t      *region,
                                          const gchar      *code);
GList       *lt_region_get_subset_type   (lt_region_t      *region,
                                          const gchar      *country_code);
GList       *lt_region_get_subset_names  (lt_region_t      *region,
                                          const gchar      *country_code,
                                          const gchar      *subset_name);
GList       *lt_region_get_subset_codes  (lt_region_t      *region,
                                          const gchar      *country_code,
                                          const gchar      *subset_name);
const gchar *lt_region_lookup_subset_code(lt_region_t      *region,
                                          const gchar      *subset_name);
const gchar *lt_region_lookup_subset_name(lt_region_t      *region,
                                          const gchar      *subset_code);

G_END_DECLS

#endif /* __LT_REGION_H__ */
