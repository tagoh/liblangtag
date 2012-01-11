/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-database.h
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

#ifndef __LT_DATABASE_H__
#define __LT_DATABASE_H__

#include <glib.h>
#include <liblangtag/lt-extlang-db.h>
#include <liblangtag/lt-grandfathered-db.h>
#include <liblangtag/lt-lang-db.h>
#include <liblangtag/lt-region-db.h>
#include <liblangtag/lt-script-db.h>
#include <liblangtag/lt-variant-db.h>

G_BEGIN_DECLS

void                   lt_db_initialize       (void);
void                   lt_db_finalize         (void);
lt_lang_db_t          *lt_db_get_lang         (void);
lt_extlang_db_t       *lt_db_get_extlang      (void);
lt_script_db_t        *lt_db_get_script       (void);
lt_region_db_t        *lt_db_get_region       (void);
lt_variant_db_t       *lt_db_get_variant      (void);
lt_grandfathered_db_t *lt_db_get_grandfathered(void);

G_END_DECLS

#endif /* __LT_DATABASE_H__ */
