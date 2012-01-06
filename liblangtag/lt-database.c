/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-database.c
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "lt-database.h"

static lt_lang_db_t    *__db_lang = NULL;
static lt_extlang_db_t *__db_extlang = NULL;
static lt_script_db_t  *__db_script = NULL;
static lt_region_db_t  *__db_region = NULL;
static lt_variant_db_t *__db_variant = NULL;


/*< private >*/

/*< public >*/
void
lt_db_initialize(void)
{
	lt_db_get_lang();
	lt_db_get_extlang();
	lt_db_get_script();
	lt_db_get_region();
	lt_db_get_variant();
}

void
lt_db_finalize(void)
{
	lt_lang_db_unref(__db_lang);
	lt_extlang_db_unref(__db_extlang);
	lt_script_db_unref(__db_script);
	lt_region_db_unref(__db_region);
	lt_variant_db_unref(__db_variant);
}

lt_lang_db_t *
lt_db_get_lang(void)
{
	if (!__db_lang)
		__db_lang = lt_lang_db_new(LT_LANG_DB_READ_MINIMAL);
	else
		lt_lang_db_ref(__db_lang);

	return __db_lang;
}

#define DEFUNC_GET_INSTANCE(__type__)					\
	lt_ ##__type__## _db_t *					\
	lt_db_get_ ##__type__ (void)					\
	{								\
		if (!__db_ ##__type__)					\
			__db_ ##__type__ = lt_ ##__type__## _db_new();	\
		else							\
			lt_ ##__type__## _db_ref(__db_ ##__type__);	\
									\
		return __db_ ##__type__;				\
	}

DEFUNC_GET_INSTANCE(extlang)
DEFUNC_GET_INSTANCE(script)
DEFUNC_GET_INSTANCE(region)
DEFUNC_GET_INSTANCE(variant)
