/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-grandfathered.c
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
#include "lt-grandfathered.h"
#include "lt-grandfathered-private.h"


struct _lt_grandfathered_t {
	lt_mem_t  parent;
	gchar    *tag;
	gchar    *description;
};

/*< private >*/

/*< protected >*/
lt_grandfathered_t *
lt_grandfathered_create(void)
{
	lt_grandfathered_t *retval;

	retval = lt_mem_alloc_object(sizeof (lt_grandfathered_t));

	return retval;
}

void
lt_grandfathered_set_tag(lt_grandfathered_t *grandfathered,
			 const gchar        *tag)
{
	g_return_if_fail (grandfathered != NULL);
	g_return_if_fail (tag != NULL);

	if (grandfathered->tag)
		lt_mem_remove_ref(&grandfathered->parent, grandfathered->tag);
	grandfathered->tag = g_strdup(tag);
	lt_mem_add_ref(&grandfathered->parent, grandfathered->tag,
		       (lt_destroy_func_t)g_free);
}

void
lt_grandfathered_set_name(lt_grandfathered_t *grandfathered,
			  const gchar        *description)
{
	g_return_if_fail (grandfathered != NULL);
	g_return_if_fail (description != NULL);

	if (grandfathered->description)
		lt_mem_remove_ref(&grandfathered->parent, grandfathered->description);
	grandfathered->description = g_strdup(description);
	lt_mem_add_ref(&grandfathered->parent, grandfathered->description,
		       (lt_destroy_func_t)g_free);
}

/*< public >*/
lt_grandfathered_t *
lt_grandfathered_ref(lt_grandfathered_t *grandfathered)
{
	g_return_val_if_fail (grandfathered != NULL, NULL);

	return lt_mem_ref(&grandfathered->parent);
}

void
lt_grandfathered_unref(lt_grandfathered_t *grandfathered)
{
	if (grandfathered)
		lt_mem_unref(&grandfathered->parent);
}

const gchar *
lt_grandfathered_get_tag(const lt_grandfathered_t *grandfathered)
{
	g_return_val_if_fail (grandfathered != NULL, NULL);

	return grandfathered->tag;
}

const gchar *
lt_grandfathered_get_name(const lt_grandfathered_t *grandfathered)
{
	g_return_val_if_fail (grandfathered != NULL, NULL);

	return grandfathered->description;
}
