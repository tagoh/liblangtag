/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-lang-db.h
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
#ifndef __LT_LANG_DB_H__
#define __LT_LANG_DB_H__

#include <glib.h>
#include <liblangtag/lt-lang.h>

G_BEGIN_DECLS

#define LT_LANG_DB_SCOPE_BEGIN	2
#define LT_LANG_DB_TYPE_BEGIN	8

enum _lt_lang_db_options_t {
	LT_LANG_DB_READ_BIBLIOGRAPHIC     = 1 << 0,
	LT_LANG_DB_READ_TERMINOLOGY       = 1 << 1,
	LT_LANG_DB_READ_SCOPE_INDIVIDUAL  = 1 << (LT_LANG_DB_SCOPE_BEGIN + 0), /* 2 */
	LT_LANG_DB_READ_SCOPE_MACRO       = 1 << (LT_LANG_DB_SCOPE_BEGIN + 1), /* 3 */
	LT_LANG_DB_READ_SCOPE_COLLECTIONS = 1 << (LT_LANG_DB_SCOPE_BEGIN + 2), /* 4 */
	LT_LANG_DB_READ_SCOPE_DIALECTS    = 1 << (LT_LANG_DB_SCOPE_BEGIN + 3), /* 5 */
	LT_LANG_DB_READ_SCOPE_LOCAL_USE   = 1 << (LT_LANG_DB_SCOPE_BEGIN + 4), /* 6 */
	LT_LANG_DB_READ_SCOPE_SPECIAL     = 1 << (LT_LANG_DB_SCOPE_BEGIN + 5), /* 7 */
	LT_LANG_DB_READ_TYPE_LIVING       = 1 << (LT_LANG_DB_TYPE_BEGIN + 0), /* 8 */
	LT_LANG_DB_READ_TYPE_EXTINCT      = 1 << (LT_LANG_DB_TYPE_BEGIN + 1), /* 9 */
	LT_LANG_DB_READ_TYPE_ANCIENT      = 1 << (LT_LANG_DB_TYPE_BEGIN + 2), /* 10 */
	LT_LANG_DB_READ_TYPE_HISTORIC     = 1 << (LT_LANG_DB_TYPE_BEGIN + 3), /* 11 */
	LT_LANG_DB_READ_TYPE_CONSTRUCTED  = 1 << (LT_LANG_DB_TYPE_BEGIN + 4), /* 12 */
	LT_LANG_DB_END                    = 1 << (LT_LANG_DB_TYPE_BEGIN + 5) /* 13 */
};
#define LT_LANG_DB_READ_ALL		((LT_LANG_DB_END - 1))
#define LT_LANG_DB_READ_MINIMAL	((				\
	LT_LANG_DB_READ_TERMINOLOGY|				\
	LT_LANG_DB_READ_SCOPE_INDIVIDUAL|			\
	LT_LANG_DB_READ_TYPE_LIVING))

typedef struct _lt_lang_db_t		lt_lang_db_t;
typedef enum _lt_lang_db_options_t	lt_lang_db_options_t;


lt_lang_db_t *lt_lang_db_new                 (lt_lang_db_options_t  options);
lt_lang_db_t *lt_lang_db_ref                 (lt_lang_db_t         *langdb);
void          lt_lang_db_unref               (lt_lang_db_t         *langdb);
GList        *lt_lang_db_get_languages       (lt_lang_db_t         *langdb);
lt_lang_t    *lt_lang_db_lookup_from_code    (lt_lang_db_t         *langdb,
                                              const gchar          *code);
lt_lang_t    *lt_lang_db_lookup_from_language(lt_lang_db_t         *langdb,
                                              const gchar          *language);

G_END_DECLS

#endif /* __LT_LANG_DB_H__ */
