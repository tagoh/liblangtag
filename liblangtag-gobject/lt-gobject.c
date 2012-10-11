/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-gobject.c
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

#include "lt-gobject.h"
#include "langtag.h"

#define LT_DEFINE_BOXED_TYPE(__name__, __Name__)			\
	GType								\
	lt_ ## __name__ ## _get_type(void)				\
	{								\
		static volatile gsize type = 0;				\
		if (g_once_init_enter(&type)) {				\
			GType t = g_boxed_type_register_static(g_intern_static_string("Lt" #__Name__), \
							       (GBoxedCopyFunc)lt_ ## __name__ ## _ref,	\
							       (GBoxedFreeFunc)lt_ ## __name__ ## _unref); \
			g_once_init_leave(&type, t);			\
		}							\
		return type;						\
	}

LT_DEFINE_BOXED_TYPE(error, Error)
LT_DEFINE_BOXED_TYPE(ext_module, ExtModule)
LT_DEFINE_BOXED_TYPE(ext_module_data, ExtModuleData)
LT_DEFINE_BOXED_TYPE(extension, Extension)
LT_DEFINE_BOXED_TYPE(extlang_db, ExtlangDb)
LT_DEFINE_BOXED_TYPE(extlang, Extlang)
LT_DEFINE_BOXED_TYPE(grandfathered_db, GrandfatheredDb)
LT_DEFINE_BOXED_TYPE(grandfathered, Grandfathered)
LT_DEFINE_BOXED_TYPE(lang_db, LangDb)
LT_DEFINE_BOXED_TYPE(lang, Lang)
LT_DEFINE_BOXED_TYPE(list, List)
LT_DEFINE_BOXED_TYPE(redundant_db, RedundantDb)
LT_DEFINE_BOXED_TYPE(redundant, Redundant)
LT_DEFINE_BOXED_TYPE(region_db, RegionDb)
LT_DEFINE_BOXED_TYPE(region, Region)
LT_DEFINE_BOXED_TYPE(script_db, ScriptDb)
LT_DEFINE_BOXED_TYPE(script, Script)
LT_DEFINE_BOXED_TYPE(string, String)
LT_DEFINE_BOXED_TYPE(tag, Tag)
LT_DEFINE_BOXED_TYPE(variant_db, VariantDb)
LT_DEFINE_BOXED_TYPE(variant, Variant)
