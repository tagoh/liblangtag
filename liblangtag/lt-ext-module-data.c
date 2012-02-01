/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-module-data.c
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "lt-mem.h"
#include "lt-ext-module-data.h"


/**
 * SECTION: lt-ext-module-data
 * @Short_Description: A container class for Extension module data.
 * @Title: Module - Data
 *
 * This container class provides a data access for a specific data of
 * extension subtag.
 */
typedef struct _lt_ext_module_data_private_t {
	lt_mem_t          parent;
	lt_destroy_func_t finalizer;
	gpointer          dummy[3];
} lt_ext_module_data_private_t;

/*< private >*/

/*< public >*/
/**
 * lt_ext_module_data_new:
 * @size: real size to allocate memory for the object.
 * @finalizer: a callback function to destroy the content.
 *
 * Create a new instance of #lt_ext_module_data_t. this function allows
 * to create an inherited instance like:
 * |[
 * struct _my_module_data_t {
 *	lt_ext_module_data_t  parent;
 *	...own members...
 * };
 * ]|
 *
 * Returns: (transfer full): a #lt_ext_module_data_t.
 */
lt_ext_module_data_t *
lt_ext_module_data_new(gsize             size,
		       lt_destroy_func_t finalizer)
{
	lt_ext_module_data_private_t *retval;

	g_assert(sizeof (lt_ext_module_data_t) == sizeof (lt_ext_module_data_private_t));

	if (size < sizeof (lt_ext_module_data_private_t))
		size += sizeof (lt_ext_module_data_private_t);

	retval = lt_mem_alloc_object(size);
	if (retval) {
		retval->finalizer = finalizer;
	}

	return (lt_ext_module_data_t *)retval;
}

/**
 * lt_ext_module_data_ref:
 * @data: a #lt_ext_module_data_t.
 *
 * Increases the reference count of @data.
 *
 * Returns: (transfer none): the same @data object.
 */
lt_ext_module_data_t *
lt_ext_module_data_ref(lt_ext_module_data_t *data)
{
	lt_ext_module_data_private_t *priv = (lt_ext_module_data_private_t *)data;

	g_return_val_if_fail (data != NULL, NULL);

	return lt_mem_ref(&priv->parent);
}

/**
 * lt_ext_module_data_unref:
 * @data: a #lt_ext_module_data_t.
 *
 * Decreases the reference count of @data. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_ext_module_data_unref(lt_ext_module_data_t *data)
{
	lt_ext_module_data_private_t *priv = (lt_ext_module_data_private_t *)data;

	if (data) {
		volatile gint ref_count = g_atomic_int_get(&priv->parent.ref_count);

		if (ref_count == 1) {
			if (priv->finalizer) {
				priv->finalizer(data);
			}
		}
		lt_mem_unref(&priv->parent);
	}
}
