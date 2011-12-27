/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-lang.h
 * Copyright (C) 2011 Akira TAGOH
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
#ifndef __LT_LANG_H__
#define __LT_LANG_H__

#include <glib.h>

G_BEGIN_DECLS

#define LT_LANG_SCOPE_BEGIN	2
#define LT_LANG_TYPE_BEGIN	8

enum _lt_lang_options_t {
	LT_LANG_READ_BIBLIOGRAPHIC     = 1 << 0,
	LT_LANG_READ_TERMINOLOGY       = 1 << 1,
	LT_LANG_READ_SCOPE_INDIVIDUAL  = 1 << (LT_LANG_SCOPE_BEGIN + 0), /* 2 */
	LT_LANG_READ_SCOPE_MACRO       = 1 << (LT_LANG_SCOPE_BEGIN + 1), /* 3 */
	LT_LANG_READ_SCOPE_COLLECTIONS = 1 << (LT_LANG_SCOPE_BEGIN + 2), /* 4 */
	LT_LANG_READ_SCOPE_DIALECTS    = 1 << (LT_LANG_SCOPE_BEGIN + 3), /* 5 */
	LT_LANG_READ_SCOPE_LOCAL_USE   = 1 << (LT_LANG_SCOPE_BEGIN + 4), /* 6 */
	LT_LANG_READ_SCOPE_SPECIAL     = 1 << (LT_LANG_SCOPE_BEGIN + 5), /* 7 */
	LT_LANG_READ_TYPE_LIVING       = 1 << (LT_LANG_TYPE_BEGIN + 0), /* 8 */
	LT_LANG_READ_TYPE_EXTINCT      = 1 << (LT_LANG_TYPE_BEGIN + 1), /* 9 */
	LT_LANG_READ_TYPE_ANCIENT      = 1 << (LT_LANG_TYPE_BEGIN + 2), /* 10 */
	LT_LANG_READ_TYPE_HISTORIC     = 1 << (LT_LANG_TYPE_BEGIN + 3), /* 11 */
	LT_LANG_READ_TYPE_CONSTRUCTED  = 1 << (LT_LANG_TYPE_BEGIN + 4), /* 12 */
	LT_LANG_END                    = 1 << (LT_LANG_TYPE_BEGIN + 5) /* 13 */
};
#define LT_LANG_READ_ALL		((LT_LANG_END - 1))
#define LT_LANG_READ_MINIMAL	((				\
	LT_LANG_READ_TERMINOLOGY|				\
	LT_LANG_READ_SCOPE_INDIVIDUAL|				\
	LT_LANG_READ_TYPE_LIVING))

enum _lt_lang_code_t {
	LT_LANG_CODE_1  = 1,
	LT_LANG_CODE_2B = 2,
	LT_LANG_CODE_2T = 3,
	LT_LANG_CODE_ID = LT_LANG_CODE_2T,
	LT_LANG_CODE_PART1 = LT_LANG_CODE_1,
	LT_LANG_CODE_PART2 = LT_LANG_CODE_2B,
	LT_LANG_CODE_END
};

typedef struct _lt_lang_t	lt_lang_t;
typedef enum _lt_lang_options_t	lt_lang_options_t;
typedef enum _lt_lang_code_t	lt_lang_code_t;


lt_lang_t   *lt_lang_new            (lt_lang_options_t  options);
lt_lang_t   *lt_lang_ref            (lt_lang_t         *parser);
void         lt_lang_unref          (lt_lang_t         *parser);
GList       *lt_lang_get_languages  (lt_lang_t         *lang);
const gchar *lt_lang_lookup_language(lt_lang_t         *lang,
                                     const gchar       *code);
const gchar *lt_lang_lookup_code    (lt_lang_t         *lang,
                                     const gchar       *language,
                                     lt_lang_code_t     type);

G_END_DECLS

#endif /* __LT_LANG_H__ */
