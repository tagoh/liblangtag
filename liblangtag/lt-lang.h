/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-lang.h
 * Copyright (C) 2012 Akira TAGOH
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
typedef enum _lt_lang_code_t	lt_lang_code_t;


lt_lang_t   *lt_lang_ref     (lt_lang_t       *lang);
void         lt_lang_unref   (lt_lang_t       *lang);
const gchar *lt_lang_get_name(const lt_lang_t *lang);
const gchar *lt_lang_get_code(const lt_lang_t *lang,
                              lt_lang_code_t   code_type);

G_END_DECLS

#endif /* __LT_LANG_H__ */
