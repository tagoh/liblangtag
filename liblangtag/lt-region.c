/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-region.c
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
#include "lt-region.h"
#include "lt-region-private.h"


struct _lt_region_t {
	lt_mem_t                parent;
	lt_region_entry_type_t  type;
	gchar                  *name;
	gchar                  *numeric_code;
	union {
		struct {
			gchar *alpha_2_code;
			gchar *alpha_3_code;
			gchar *common_name;
			gchar *official_name;
		} _3166;
		struct {
			gchar *alpha_4_code;
			gchar *alpha_3_code;
		} _3166_3;
	} iso;
};


/*< private >*/

/*< protected >*/
lt_region_t *
lt_region_create(lt_region_entry_type_t type)
{
	lt_region_t *retval = lt_mem_alloc_object(sizeof (lt_region_t));

	if (retval) {
		retval->type = type;
	}

	return retval;
}

void
lt_region_set_name(lt_region_t *region,
		   const gchar *name)
{
	g_return_if_fail (region != NULL);
	g_return_if_fail (name != NULL);

	if (region->name)
		lt_mem_remove_ref(&region->parent, region->name);
	region->name = g_strdup(name);
	lt_mem_add_ref(&region->parent, region->name,
		       (lt_destroy_func_t)g_free);
}

void
lt_region_set_code(lt_region_t      *region,
		   lt_region_code_t  code_type,
		   const gchar      *code)
{
	gchar **p;

	g_return_if_fail (region != NULL);
	g_return_if_fail (code != NULL);

	switch (code_type) {
	    case LT_REGION_CODE_ALPHA_2:
		    g_return_if_fail (region->type == LT_REGION_ENTRY_3166);
		    p = &region->iso._3166.alpha_2_code;
		    break;
	    case LT_REGION_CODE_ALPHA_3:
		    if (region->type == LT_REGION_ENTRY_3166)
			    p = &region->iso._3166.alpha_3_code;
		    else if (region->type == LT_REGION_ENTRY_3166_3)
			    p = &region->iso._3166_3.alpha_3_code;
		    else {
			    g_warning("Unknown region entry type: %d\n", region->type);
			    return;
		    }
		    break;
	    case LT_REGION_CODE_NUMERIC:
		    p = &region->numeric_code;
		    break;
	    case LT_REGION_CODE_ALPHA_4:
		    g_return_if_fail (region->type == LT_REGION_ENTRY_3166_3);
		    p = &region->iso._3166_3.alpha_4_code;
		    break;
	    default:
		    g_warning("Unkonwn region code type: %d\n", code_type);
		    return;
	}
	if (*p)
		lt_mem_remove_ref(&region->parent, *p);
	*p = g_strdup(code);
	lt_mem_add_ref(&region->parent, *p,
		       (lt_destroy_func_t)g_free);
}

lt_region_entry_type_t
lt_region_get_type(const lt_region_t *region)
{
	g_return_val_if_fail (region != NULL, -1);

	return region->type;
}

/*< public >*/
lt_region_t *
lt_region_ref(lt_region_t *region)
{
	g_return_val_if_fail (region != NULL, NULL);

	return lt_mem_ref(&region->parent);
}

void
lt_region_unref(lt_region_t *region)
{
	if (region)
		lt_mem_unref(&region->parent);
}

const gchar *
lt_region_get_name(const lt_region_t *region)
{
	g_return_val_if_fail (region != NULL, NULL);

	return region->name;
}

gboolean
lt_region_has_alpha_2_code(const lt_region_t *region)
{
	return lt_region_get_type(region) == LT_REGION_ENTRY_3166;
}

gboolean
lt_region_has_alpha_4_code(const lt_region_t *region)
{
	return lt_region_get_type(region) == LT_REGION_ENTRY_3166_3;
}

const gchar *
lt_region_get_code(const lt_region_t *region,
		   lt_region_code_t   code_type)
{
	const gchar *retval;

	g_return_val_if_fail (region != NULL, NULL);

	switch (code_type) {
	    case LT_REGION_CODE_ALPHA_2:
		    g_return_val_if_fail (region->type == LT_REGION_ENTRY_3166, NULL);
		    retval = region->iso._3166.alpha_2_code;
		    break;
	    case LT_REGION_CODE_ALPHA_3:
		    if (region->type == LT_REGION_ENTRY_3166)
			    retval = region->iso._3166.alpha_3_code;
		    else if (region->type == LT_REGION_ENTRY_3166_3)
			    retval = region->iso._3166_3.alpha_3_code;
		    else {
			    g_warning("Unknown region entry type: %d\n", region->type);
			    retval = NULL;
		    }
		    break;
	    case LT_REGION_CODE_NUMERIC:
		    retval = region->numeric_code;
		    break;
	    case LT_REGION_CODE_ALPHA_4:
		    g_return_val_if_fail (region->type == LT_REGION_ENTRY_3166_3, NULL);
		    retval = region->iso._3166_3.alpha_4_code;
		    break;
	    default:
		    g_warning("Unkonwn region code type: %d\n", code_type);
		    retval = NULL;
		    break;
	}

	return retval;
}
