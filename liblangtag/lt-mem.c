/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-mem.c
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


/*< private >*/

/*< public >*/
gpointer
lt_mem_alloc_object(gsize size)
{
	lt_mem_t *retval;

	g_return_val_if_fail (size > 0, NULL);

	retval = g_malloc0(size);
	if (retval) {
		retval->ref_count = 1;
		retval->refs = NULL;
		retval->size = size;
	}

	return retval;
}

gpointer
lt_mem_ref(lt_mem_t *object)
{
	g_return_val_if_fail (object != NULL, NULL);

	g_atomic_int_inc(&object->ref_count);

	return object;
}

void
lt_mem_unref(lt_mem_t *object)
{
	g_return_if_fail (object != NULL);

	if (g_atomic_int_dec_and_test(&object->ref_count)) {
		if (object->refs) {
			GHashTableIter iter;
			gpointer p, unref;

			g_hash_table_iter_init(&iter, object->refs);
			while (g_hash_table_iter_next(&iter, &p, &unref)) {
				if (unref) {
					((lt_destroy_func_t)unref)(p);
				}
			}
		}
		g_hash_table_destroy(object->refs);
		g_free(object);
	}
}

void
lt_mem_add_ref(lt_mem_t          *object,
	       gpointer           p,
	       lt_destroy_func_t  func)
{
	g_return_if_fail (object != NULL);
	g_return_if_fail (p != NULL);
	g_return_if_fail (func != NULL);

	if (!object->refs) {
		object->refs = g_hash_table_new(g_direct_hash,
						g_direct_equal);
	}
	g_hash_table_replace(object->refs,
			     p, func);
}

void
lt_mem_remove_ref(lt_mem_t *object,
		  gpointer  p)
{
	g_return_if_fail (object != NULL);
	g_return_if_fail (p != NULL);

	if (object->refs) {
		lt_destroy_func_t unref;

		if ((unref = g_hash_table_lookup(object->refs, p))) {
			unref(p);
			g_hash_table_remove(object->refs, p);
		}
	}
}

void
lt_mem_delete_ref(lt_mem_t *object,
		  gpointer  p)
{
	g_return_if_fail (object != NULL);
	g_return_if_fail (p != NULL);

	if (object->refs) {
		g_hash_table_remove(object->refs, p);
	}
}
