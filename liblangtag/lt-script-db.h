/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-script-db.h
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

#ifndef __LT_SCRIPT_DB_H__
#define __LT_SCRIPT_DB_H__

#include <glib.h>
#include <liblangtag/lt-script.h>

G_BEGIN_DECLS

typedef struct _lt_script_db_t	lt_script_db_t;


lt_script_db_t *lt_script_db_new        (void);
lt_script_db_t *lt_script_db_ref        (lt_script_db_t *scriptdb);
void            lt_script_db_unref      (lt_script_db_t *scriptdb);
GList          *lt_script_db_get_scripts(lt_script_db_t *scriptdb);
lt_script_t    *lt_script_db_lookup     (lt_script_db_t *scriptdb,
                                         const gchar    *script_name_or_alpha_code_or_num_code);

G_END_DECLS

#endif /* __LT_SCRIPT_DB_H__ */
