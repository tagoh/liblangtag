/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-division.c
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
#include "lt-division-private.h"
#include "lt-division.h"


struct _lt_division_t {
	lt_mem_t  parent;
	gchar    *country_code;
	gchar    *subset_type;
	gchar    *code;
	gchar    *name;
};

/*< private >*/
static const gchar *
lt_division_get(const lt_division_t *division,
		lt_division_vtype_t  type)
{
	const gchar *retval = NULL;

	g_return_val_if_fail (division != NULL, NULL);

	switch (type) {
	    case LT_DIVISION_COUNTRY_CODE:
		    retval = division->country_code;
		    break;
	    case LT_DIVISION_TYPE:
		    retval = division->subset_type;
		    break;
	    case LT_DIVISION_CODE:
		    retval = division->code;
		    break;
	    case LT_DIVISION_NAME:
		    retval = division->name;
		    break;
	    default:
		    g_warning("Unknown subset value type: %d", type);
		    break;
	}

	return retval;
}

/*< protected >*/
lt_division_t *
lt_division_create(void)
{
	lt_division_t *retval = lt_mem_alloc_object(sizeof (lt_division_t));

	return retval;
}

void
lt_division_set(lt_division_t       *division,
		lt_division_vtype_t  type,
		const gchar         *val)
{
	gchar **p;

	g_return_if_fail (division != NULL);
	g_return_if_fail (val != NULL);

	switch (type) {
	    case LT_DIVISION_COUNTRY_CODE:
		    p = &division->country_code;
		    break;
	    case LT_DIVISION_TYPE:
		    p = &division->subset_type;
		    break;
	    case LT_DIVISION_CODE:
		    p = &division->code;
		    break;
	    case LT_DIVISION_NAME:
		    p = &division->name;
		    break;
	    default:
		    g_warning("Unknown subset value type: %d", type);
		    return;
	}

	if (*p)
		lt_mem_remove_ref(&division->parent, *p);
	*p = g_strdup(val);
	lt_mem_add_ref(&division->parent, *p,
		       (lt_destroy_func_t)g_free);
}

/*< public >*/
lt_division_t *
lt_division_ref(lt_division_t *division)
{
	g_return_val_if_fail (division != NULL, NULL);

	return lt_mem_ref(&division->parent);
}

void
lt_division_unref(lt_division_t *division)
{
	if (division)
		lt_mem_unref(&division->parent);
}

const gchar *
lt_division_get_country_code(const lt_division_t *division)
{
	return lt_division_get(division, LT_DIVISION_COUNTRY_CODE);
}

const gchar *
lt_division_get_type(const lt_division_t *division)
{
	return lt_division_get(division, LT_DIVISION_TYPE);
}

const gchar *
lt_division_get_code(const lt_division_t *division)
{
	return lt_division_get(division, LT_DIVISION_CODE);
}

const gchar *
lt_division_get_name(const lt_division_t *division)
{
	return lt_division_get(division, LT_DIVISION_NAME);
}
