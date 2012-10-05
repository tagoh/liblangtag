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

#include <glib.h> /* XXX: GHashTable and atomic functions are used */
#include <stdlib.h>
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-list.h"


/*< private >*/
static void
_lt_mem_weak_pointer_cb(lt_pointer_t data)
{
	lt_pointer_t *p = (lt_pointer_t *)data;

	*p = NULL;
}

/*< protected >*/

/*< public >*/
lt_pointer_t
lt_mem_alloc_object(size_t size)
{
	lt_mem_t *retval;

	lt_return_val_if_fail (size > 0, NULL);

	retval = calloc(1, size);
	if (retval) {
		retval->ref_count = 1;
		retval->refs = NULL;
		retval->size = size;
	}

	return retval;
}

lt_pointer_t
lt_mem_ref(lt_mem_t *object)
{
	lt_return_val_if_fail (object != NULL, NULL);

	g_atomic_int_inc(&object->ref_count);

	return object;
}

void
lt_mem_unref(lt_mem_t *object)
{
	lt_return_if_fail (object != NULL);

	if (g_atomic_int_dec_and_test(&object->ref_count)) {
		if (object->refs) {
			GHashTableIter iter;
			lt_pointer_t p, unref;

			g_hash_table_iter_init(&iter, object->refs);
			while (g_hash_table_iter_next(&iter, &p, &unref)) {
				if (unref) {
					((lt_destroy_func_t)unref)(p);
				}
			}
			g_hash_table_destroy(object->refs);
		}
		if (object->weak_pointers)
			lt_list_free(object->weak_pointers);
		free(object);
	}
}

void
lt_mem_add_ref(lt_mem_t          *object,
	       lt_pointer_t       p,
	       lt_destroy_func_t  func)
{
	lt_return_if_fail (object != NULL);
	lt_return_if_fail (p != NULL);
	lt_return_if_fail (func != NULL);

	if (!object->refs) {
		object->refs = g_hash_table_new(g_direct_hash,
						g_direct_equal);
	}
	g_hash_table_replace(object->refs,
			     p, func);
}

void
lt_mem_remove_ref(lt_mem_t     *object,
		  lt_pointer_t  p)
{
	lt_return_if_fail (object != NULL);
	lt_return_if_fail (p != NULL);

	if (object->refs) {
		lt_destroy_func_t unref;

		if ((unref = g_hash_table_lookup(object->refs, p))) {
			unref(p);
			g_hash_table_remove(object->refs, p);
		}
	}
}

void
lt_mem_delete_ref(lt_mem_t     *object,
		  lt_pointer_t  p)
{
	lt_return_if_fail (object != NULL);
	lt_return_if_fail (p != NULL);

	if (object->refs) {
		g_hash_table_remove(object->refs, p);
	}
}

void
lt_mem_add_weak_pointer(lt_mem_t     *object,
			lt_pointer_t *p)
{
	lt_return_if_fail (object != NULL);
	lt_return_if_fail (p != NULL);

	if (!lt_list_find(object->weak_pointers, p))
		object->weak_pointers = lt_list_append(object->weak_pointers, p,
						       _lt_mem_weak_pointer_cb);
}

void
lt_mem_remove_weak_pointer(lt_mem_t     *object,
			   lt_pointer_t *p)
{
	lt_return_if_fail (object != NULL);
	lt_return_if_fail (p != NULL);

	object->weak_pointers = lt_list_delete(object->weak_pointers, p);
}
