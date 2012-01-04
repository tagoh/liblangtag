/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-lang-private.h
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
#ifndef __LT_LANG_PRIVATE_H__
#define __LT_LANG_PRIVATE_H__

#include <glib.h>
#include "lt-lang.h"

G_BEGIN_DECLS

enum _lt_lang_entry_type_t {
	LT_LANG_639_2,
	LT_LANG_639_3,
	LT_LANG_END
};

typedef enum _lt_lang_entry_type_t	lt_lang_entry_type_t;


lt_lang_t *lt_lang_create  (lt_lang_entry_type_t  type);
void       lt_lang_set_name(lt_lang_t            *lang,
                            const gchar          *name);
void       lt_lang_set_code(lt_lang_t            *lang,
                            lt_lang_code_t        code_type,
                            const gchar          *code);

G_END_DECLS

#endif /* __LT_LANG_PRIVATE_H__ */
