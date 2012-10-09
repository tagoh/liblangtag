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

struct _lt_mem_slist_t {
	lt_mem_slist_t    *next;
	lt_pointer_t       key;
	lt_destroy_func_t  func;
};

lt_mem_slist_t *lt_mem_slist_new        (void);
void            lt_mem_slist_free       (lt_mem_slist_t     *list);
lt_mem_slist_t *lt_mem_slist_last       (lt_mem_slist_t     *list);
lt_mem_slist_t *lt_mem_slist_append     (lt_mem_slist_t     *list,
                                         lt_pointer_t        key,
                                         lt_destroy_func_t   func);
lt_mem_slist_t *lt_mem_slist_delete     (lt_mem_slist_t     *list,
                                         lt_pointer_t       *data);
lt_mem_slist_t *lt_mem_slist_delete_link(lt_mem_slist_t     *list,
                                         lt_mem_slist_t     *link_);
lt_mem_slist_t *lt_mem_slist_find       (lt_mem_slist_t     *list,
                                         const lt_pointer_t  data);


/*< private >*/

/*< protected >*/
lt_mem_slist_t *
lt_mem_slist_new(void)
{
	return malloc(sizeof (lt_mem_slist_t));
}

void
lt_mem_slist_free(lt_mem_slist_t *list)
{
	lt_mem_slist_t *l = list;

	while (l) {
		list = l;
		l = l->next;
		free(list);
	}
}

lt_mem_slist_t *
lt_mem_slist_last(lt_mem_slist_t *list)
{
	if (list) {
		while (list->next)
			list = list->next;
	}

	return list;
}

lt_mem_slist_t *
lt_mem_slist_append(lt_mem_slist_t    *list,
		    lt_pointer_t       key,
		    lt_destroy_func_t  func)
{
	lt_mem_slist_t *l = lt_mem_slist_new();
	lt_mem_slist_t *last;

	l->key = key;
	l->func = func;
	l->next = NULL;
	if (list) {
		last = lt_mem_slist_last(list);
		last->next = l;
	} else {
		list = l;
	}

	return list;
}

lt_mem_slist_t *
lt_mem_slist_delete(lt_mem_slist_t *list,
		    lt_pointer_t   *data)
{
	lt_mem_slist_t *l = list;

	while (l) {
		if (l->key == data) {
			list = lt_mem_slist_delete_link(list, l);
			break;
		} else {
			l = l->next;
		}
	}

	return list;
}

lt_mem_slist_t *
lt_mem_slist_delete_link(lt_mem_slist_t *list,
			 lt_mem_slist_t *link_)
{
	lt_mem_slist_t *prev = NULL, *l = list;

	while (l) {
		if (l == link_) {
			if (prev)
				prev->next = l->next;
			if (list == l)
				list = list->next;
			free(link_);
			break;
		}
		prev = l;
		l = l->next;
	}

	return list;
}

lt_mem_slist_t *
lt_mem_slist_find(lt_mem_slist_t     *list,
		  const lt_pointer_t  data)
{
	while (list) {
		if (list->key == data)
			break;
		list = list->next;
	}

	return list;
}

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
		lt_mem_slist_t *ll, *l;

		if (object->refs) {
			ll = object->refs;
			while (ll) {
				l = ll;
				ll = ll->next;
				if (l->func)
					l->func(l->key);
				free(l);
			}
		}
		if (object->weak_pointers) {
			ll = object->weak_pointers;
			while (ll) {
				lt_pointer_t *p;

				l = ll;
				ll = ll->next;
				p = (lt_pointer_t *)l->key;
				*p = NULL;
				free(l);
			}
		}
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

	object->refs = lt_mem_slist_append(object->refs, p, func);
}

void
lt_mem_remove_ref(lt_mem_t     *object,
		  lt_pointer_t  p)
{
	lt_mem_slist_t *l;

	lt_return_if_fail (object != NULL);
	lt_return_if_fail (p != NULL);

	if ((l = lt_mem_slist_find(object->refs, p)) != NULL) {
		if (l->func)
			l->func(l->key);
		object->refs = lt_mem_slist_delete_link(object->refs, l);
	}
}

void
lt_mem_delete_ref(lt_mem_t     *object,
		  lt_pointer_t  p)
{
	lt_mem_slist_t *l;

	lt_return_if_fail (object != NULL);
	lt_return_if_fail (p != NULL);

	if ((l = lt_mem_slist_find(object->refs, p)) != NULL) {
		object->refs = lt_mem_slist_delete_link(object->refs, l);
	}
}

void
lt_mem_add_weak_pointer(lt_mem_t     *object,
			lt_pointer_t *p)
{
	lt_return_if_fail (object != NULL);
	lt_return_if_fail (p != NULL);

	if (!lt_mem_slist_find(object->weak_pointers, p))
		object->weak_pointers = lt_mem_slist_append(object->weak_pointers, p, NULL);
}

void
lt_mem_remove_weak_pointer(lt_mem_t     *object,
			   lt_pointer_t *p)
{
	lt_return_if_fail (object != NULL);
	lt_return_if_fail (p != NULL);

	object->weak_pointers = lt_mem_slist_delete(object->weak_pointers, p);
}
