/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-mem.c
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

#include "lt-mem.h"


/*< private >*/

/*< protected >*/
void
lt_mem_gstring_free(GString *string)
{
	if (string)
		g_string_free(string, TRUE);
}

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
	GList *l;

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
			g_hash_table_destroy(object->refs);
		}
		for (l = object->weak_pointers; l != NULL; l = g_list_next(l)) {
			gpointer *p = (gpointer *)l->data;
			*p = NULL;
		}
		if (object->weak_pointers)
			g_list_free(object->weak_pointers);
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

void
lt_mem_add_weak_pointer(lt_mem_t *object,
			gpointer *p)
{
	g_return_if_fail (object != NULL);
	g_return_if_fail (p != NULL);

	if (!g_list_find(object->weak_pointers, p))
		object->weak_pointers = g_list_append(object->weak_pointers, p);
}

void
lt_mem_remove_weak_pointer(lt_mem_t *object,
			   gpointer *p)
{
	g_return_if_fail (object != NULL);
	g_return_if_fail (p != NULL);

	object->weak_pointers = g_list_remove(object->weak_pointers, p);
}
