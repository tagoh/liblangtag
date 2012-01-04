/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-region-db.h
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
#ifndef __LT_REGION_DB_H__
#define __LT_REGION_DB_H__

#include <glib.h>
#include <liblangtag/lt-division.h>
#include <liblangtag/lt-region.h>

G_BEGIN_DECLS

typedef struct _lt_region_db_t		lt_region_db_t;


lt_region_db_t *lt_region_db_new                        (void);
lt_region_db_t *lt_region_db_ref                        (lt_region_db_t *regiondb);
void            lt_region_db_unref                      (lt_region_db_t *regiondb);
GList          *lt_region_db_get_regions                (lt_region_db_t *regiondb);
lt_region_t    *lt_region_db_lookup_region_from_language(lt_region_db_t *regiondb,
                                                         const gchar    *language);
lt_region_t    *lt_region_db_lookup_region_from_code    (lt_region_db_t *regiondb,
                                                         const gchar    *code);
GList          *lt_region_db_get_division_type          (lt_region_db_t *regiondb,
                                                         const gchar    *country_code);
GList          *lt_region_db_get_divisions              (lt_region_db_t *regiondb,
                                                         const gchar    *country_code,
                                                         const gchar    *division_type);
lt_division_t  *lt_region_db_lookup_division            (lt_region_db_t *regiondb,
                                                         const gchar    *name_or_code);

G_END_DECLS

#endif /* __LT_REGION_DB_H__ */
