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

#include "lt-mem.h"
#include "lt-database.h"


/**
 * SECTION:lt-database
 * @Short_Description: convenient function sets to deal with the language tags database.
 * @Title: Database
 *
 * This section describes convenient functions to obtain the database instance.
 */

static lt_lang_db_t          *__db_lang = NULL;
static lt_extlang_db_t       *__db_extlang = NULL;
static lt_script_db_t        *__db_script = NULL;
static lt_region_db_t        *__db_region = NULL;
static lt_variant_db_t       *__db_variant = NULL;
static lt_grandfathered_db_t *__db_grandfathered = NULL;


/*< private >*/

/*< public >*/
/**
 * lt_db_initialize:
 *
 * Initialize all of the language tags database instance.
 */
void
lt_db_initialize(void)
{
	lt_db_get_lang();
	lt_db_get_extlang();
	lt_db_get_script();
	lt_db_get_region();
	lt_db_get_variant();
	lt_db_get_grandfathered();
}

/**
 * lt_db_finalize:
 *
 * Decreases the reference count of the language tags database, which was
 * increased with lt_db_initialize().
 */
void
lt_db_finalize(void)
{
	lt_lang_db_unref(__db_lang);
	lt_extlang_db_unref(__db_extlang);
	lt_script_db_unref(__db_script);
	lt_region_db_unref(__db_region);
	lt_variant_db_unref(__db_variant);
	lt_grandfathered_db_unref(__db_grandfathered);
}

#define DEFUNC_GET_INSTANCE(__type__)					\
	lt_ ##__type__## _db_t *					\
	lt_db_get_ ##__type__ (void)					\
	{								\
		if (!__db_ ##__type__) {				\
			__db_ ##__type__ = lt_ ##__type__## _db_new();	\
			lt_mem_add_weak_pointer((lt_mem_t *)__db_ ##__type__, \
						(gpointer *)&__db_ ##__type__);	\
		} else {						\
			lt_ ##__type__## _db_ref(__db_ ##__type__);	\
		}							\
									\
		return __db_ ##__type__;				\
	}

/**
 * lt_db_get_lang:
 *
 * Obtains the instance of @lt_lang_db_t. This still allows to use without
 * lt_db_initialize(). but it will takes some time to load the database on
 * the memory every time.
 *
 * Returns: The instance of @lt_lang_db_t.
 */
DEFUNC_GET_INSTANCE(lang)
/**
 * lt_db_get_extlang:
 *
 * Obtains the instance of @lt_extlang_db_t. This still allows to use without
 * lt_db_initialize(). but it will takes some time to load the database on
 * the memory every time.
 *
 * Returns: The instance of @lt_extlang_db_t.
 */
DEFUNC_GET_INSTANCE(extlang)
/**
 * lt_db_get_grandfathered:
 *
 * Obtains the instance of @lt_grandfathered_db_t. This still allows to use
 * without lt_db_initialize(). but it will takes some time to load the database
 * on the memory every time.
 *
 * Returns: The instance of @lt_grandfathered_db_t.
 */
DEFUNC_GET_INSTANCE(grandfathered)
/**
 * lt_db_get_script:
 *
 * Obtains the instance of @lt_script_db_t. This still allows to use without
 * lt_db_initialize(). but it will takes some time to load the database on
 * the memory every time.
 *
 * Returns: The instance of @lt_script_db_t.
 */
DEFUNC_GET_INSTANCE(script)
/**
 * lt_db_get_region:
 *
 * Obtains the instance of @lt_region_db_t. This still allows to use without
 * lt_db_initialize(). but it will takes some time to load the database on
 * the memory every time.
 *
 * Returns: The instance of @lt_region_db_t.
 */
DEFUNC_GET_INSTANCE(region)
/**
 * lt_db_get_variant:
 *
 * Obtains the instance of @lt_variant_db_t. This still allows to use without
 * lt_db_initialize(). but it will takes some time to load the database on
 * the memory every time.
 *
 * Returns: The instance of @lt_variant_db_t.
 */
DEFUNC_GET_INSTANCE(variant)
