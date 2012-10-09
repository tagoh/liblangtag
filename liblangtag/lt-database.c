/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-database.c
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include "lt-mem.h"
#include "lt-ext-module.h"
#include "lt-utils.h"
#include "lt-database.h"


/**
 * SECTION:lt-database
 * @Short_Description: convenient function sets to deal with the language tags database
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
static lt_redundant_db_t     *__db_redundant = NULL;

static char __lt_db_datadir[LT_PATH_MAX] = { 0 };


/*< private >*/

/*< public >*/
/**
 * lt_db_set_datadir:
 * @path: the directory where database files are installed.
 *
 * Set @path as the default location of the database files.
 * This has to be called before lt_db_initialize() or any
 * initialization for each databases.
 */
void
lt_db_set_datadir(const char *path)
{
	if (path) {
		strncpy(__lt_db_datadir, path, LT_PATH_MAX - 1);
		__lt_db_datadir[LT_PATH_MAX - 1] = 0;
	} else {
		__lt_db_datadir[0] = 0;
	}
}

/**
 * lt_db_get_datadir:
 *
 * Obtain the directory where database files are installed.
 *
 * Returns: the directory name.
 */
const char *
lt_db_get_datadir(void)
{
	static const char *__builtin_datadir = REGDATADIR;

	if (*__lt_db_datadir != 0)
		return __lt_db_datadir;
	return __builtin_datadir;
}

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
	lt_db_get_redundant();
	lt_ext_modules_load();
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
	lt_redundant_db_unref(__db_redundant);
	lt_ext_modules_unload();
}

#define DEFUNC_GET_INSTANCE(__type__)					\
	lt_ ##__type__## _db_t *					\
	lt_db_get_ ##__type__ (void)					\
	{								\
		if (!__db_ ##__type__) {				\
			__db_ ##__type__ = lt_ ##__type__## _db_new();	\
			lt_mem_add_weak_pointer((lt_mem_t *)__db_ ##__type__, \
						(lt_pointer_t *)&__db_ ##__type__); \
		} else {						\
			lt_ ##__type__## _db_ref(__db_ ##__type__);	\
		}							\
									\
		return __db_ ##__type__;				\
	}

/**
 * lt_db_get_lang:
 *
 * Obtains the instance of #lt_lang_db_t. This still allows to use without
 * lt_db_initialize(). but it will takes some time to load the database on
 * the memory every time.
 *
 * Returns: The instance of #lt_lang_db_t.
 */
DEFUNC_GET_INSTANCE(lang)
/**
 * lt_db_get_extlang:
 *
 * Obtains the instance of #lt_extlang_db_t. This still allows to use without
 * lt_db_initialize(). but it will takes some time to load the database on
 * the memory every time.
 *
 * Returns: The instance of #lt_extlang_db_t.
 */
DEFUNC_GET_INSTANCE(extlang)
/**
 * lt_db_get_grandfathered:
 *
 * Obtains the instance of #lt_grandfathered_db_t. This still allows to use
 * without lt_db_initialize(). but it will takes some time to load the database
 * on the memory every time.
 *
 * Returns: The instance of #lt_grandfathered_db_t.
 */
DEFUNC_GET_INSTANCE(grandfathered)
/**
 * lt_db_get_redundant:
 *
 * Obtains the instance of #lt_redundant_db_t. This still allows to use
 * without lt_db_initialize(). but it will takes some time to load the database
 * on the memory every time.
 *
 * Returns: The instance of #lt_redundant_db_t.
 */
DEFUNC_GET_INSTANCE(redundant)
/**
 * lt_db_get_region:
 *
 * Obtains the instance of #lt_region_db_t. This still allows to use without
 * lt_db_initialize(). but it will takes some time to load the database on
 * the memory every time.
 *
 * Returns: The instance of #lt_region_db_t.
 */
DEFUNC_GET_INSTANCE(region)
/**
 * lt_db_get_script:
 *
 * Obtains the instance of #lt_script_db_t. This still allows to use without
 * lt_db_initialize(). but it will takes some time to load the database on
 * the memory every time.
 *
 * Returns: The instance of #lt_script_db_t.
 */
DEFUNC_GET_INSTANCE(script)
/**
 * lt_db_get_variant:
 *
 * Obtains the instance of #lt_variant_db_t. This still allows to use without
 * lt_db_initialize(). but it will takes some time to load the database on
 * the memory every time.
 *
 * Returns: The instance of #lt_variant_db_t.
 */
DEFUNC_GET_INSTANCE(variant)
