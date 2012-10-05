/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-list.c
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

#include <glib.h> /* XXX: just shut up GHashTable dependency in lt-mem.h */
#include <stdlib.h>
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-list.h"

struct _lt_list_t {
	lt_mem_t      parent;
	lt_list_t    *prev;
	lt_list_t    *next;
	lt_pointer_t  value;
};

/*< private >*/
static void
_lt_list_update(lt_pointer_t data)
{
	lt_list_t *list = data;

	if (list->next)
		list->next->prev = list->prev;
	if (list->prev)
		list->prev->next = list->next;
}

static lt_list_t *
_lt_list_sort_merge(lt_list_t         *l1,
		    lt_list_t         *l2,
		    lt_compare_func_t  func)
{
	int result;
	lt_list_t list, *l = &list, *lprev = NULL;

	while (l1 && l2) {
		result = func(lt_list_value(l1), lt_list_value(l2));
		if (result <= 0) {
			l->next = l1;
			l1 = lt_list_next(l1);
		} else {
			l->next = l2;
			l2 = lt_list_next(l2);
		}
		l = lt_list_next(l);
		l->prev = lprev;
		lprev = l;
	}
	l->next = l1 ? l1 : l2;
	l->next->prev = l;

	return list.next;
}

/*< protected >*/

/*< public >*/
lt_list_t *
lt_list_new(void)
{
	return lt_mem_alloc_object(sizeof (lt_list_t));
}

lt_list_t *
lt_list_ref(lt_list_t *list)
{
	lt_return_val_if_fail (list != NULL, NULL);

	return lt_mem_ref(&list->parent);
}

void
lt_list_unref(lt_list_t *list)
{
	if (list)
		lt_mem_unref(&list->parent);
}

void
lt_list_free(lt_pointer_t data)
{
	lt_list_t *l = data, *list;

	while (l) {
		list = l;
		l = l->next;
		lt_list_unref(list);
	}
}

lt_list_t *
lt_list_first(lt_list_t *list)
{
	if (list) {
		while (list->prev)
			list = list->prev;
	}

	return list;
}

lt_list_t *
lt_list_last(lt_list_t *list)
{
	if (list) {
		while (list->next)
			list = list->next;
	}

	return list;
}

lt_list_t *
lt_list_previous(const lt_list_t *list)
{
	return list ? list->prev : NULL;
}

lt_list_t *
lt_list_next(const lt_list_t *list)
{
	return list ? list->next : NULL;
}

lt_pointer_t
lt_list_value(const lt_list_t *list)
{
	lt_return_val_if_fail (list != NULL, NULL);

	return list->value;
}

size_t
lt_list_length(const lt_list_t *list)
{
	size_t retval = 0;
	const lt_list_t *l = list;

	while (l) {
		l = lt_list_next(l);
		retval++;
	}

	return retval;
}

lt_list_t *
lt_list_append(lt_list_t         *list,
	       lt_pointer_t       data,
	       lt_destroy_func_t  func)
{
	lt_list_t *l = lt_list_new();
	lt_list_t *last;

	l->value = data;
	l->next = NULL;
	lt_mem_add_ref(&l->parent, l, _lt_list_update);
	if (func)
		lt_mem_add_ref(&l->parent, data, func);
	if (list) {
		last = lt_list_last(list);
		last->next = l;
		l->prev = last;
	} else {
		l->prev = NULL;
		list = l;
	}

	return list;
}

lt_list_t *
lt_list_remove(lt_list_t    *list,
	       lt_pointer_t  value)
{
	lt_list_t *l = list;

	while (l) {
		if (l->value == value) {
			list = lt_list_delete_link(list, l);
			break;
		} else {
			l = l->next;
		}
	}

	return list;
}

lt_list_t *
lt_list_delete(lt_list_t    *list,
	       lt_pointer_t  value)
{
	lt_list_t *l = list;

	while (l) {
		if (l->value == value) {
			lt_mem_delete_ref(&l->parent, value);
			list = lt_list_delete_link(list, l);
			break;
		} else {
			l = l->next;
		}
	}

	return list;
}

lt_list_t *
lt_list_delete_link(lt_list_t *list,
		    lt_list_t *link_)
{
	if (link_) {
		if (link_ == list)
			list = list->next;
		lt_list_unref(link_);
	}

	return list;
}

lt_list_t *
lt_list_copy(lt_list_t *list)
{
	lt_list_t *l;

	lt_return_val_if_fail (list != NULL, NULL);

	for (l = list; l != NULL; l = lt_list_next(l)) {
		lt_list_ref(l);
	}

	return list;
}

lt_list_t *
lt_list_find(lt_list_t          *list,
	     const lt_pointer_t  value)
{
	while (list) {
		if (list->value == value)
			break;
		list = list->next;
	}

	return list;
}

lt_list_t *
lt_list_find_custom(lt_list_t          *list,
		    const lt_pointer_t  data,
		    lt_compare_func_t   func)
{
	lt_return_val_if_fail (func != NULL, NULL);

	while (list) {
		if (!func(list->value, data))
			break;
		list = list->next;
	}

	return list;
}

lt_list_t *
lt_list_sort(lt_list_t         *list,
	     lt_compare_func_t  func)
{
	lt_list_t *a, *b;
	size_t i = 0, j = 0;

	lt_return_val_if_fail (list != NULL, NULL);

	if (!list->next)
		return list;

	a = b = list;
	while (b->next) {
		b = lt_list_next(b);
		j++;
		if ((j / 2) > i) {
			a = lt_list_next(a);
			i++;
		}
	}
	b = a->next;
	a->next = NULL;
	b->prev = NULL;

	return _lt_list_sort_merge(lt_list_sort(list, func),
				   lt_list_sort(b, func),
				   func);
}
