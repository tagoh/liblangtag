/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-gobject.c
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

LT_DEFINE_BOXED_TYPE(extlang_db, ExtlangDb)
LT_DEFINE_BOXED_TYPE(extlang, Extlang)
LT_DEFINE_BOXED_TYPE(grandfathered_db, GrandfatheredDb)
LT_DEFINE_BOXED_TYPE(grandfathered, Grandfathered)
LT_DEFINE_BOXED_TYPE(lang_db, LangDb)
LT_DEFINE_BOXED_TYPE(lang, Lang)
LT_DEFINE_BOXED_TYPE(redundant_db, RedundantDb)
LT_DEFINE_BOXED_TYPE(redundant, Redundant)
LT_DEFINE_BOXED_TYPE(region_db, RegionDb)
LT_DEFINE_BOXED_TYPE(region, Region)
LT_DEFINE_BOXED_TYPE(script_db, ScriptDb)
LT_DEFINE_BOXED_TYPE(script, Script)
LT_DEFINE_BOXED_TYPE(tag, Tag)
LT_DEFINE_BOXED_TYPE(variant_db, VariantDb)
LT_DEFINE_BOXED_TYPE(variant, Variant)
