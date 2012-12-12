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

#include <stdlib.h>
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-list.h"


/**
 * SECTION: lt-list
 * @Short_Description: linked lists
 * @Title: Doubly-Linked Lists
 *
 * The #lt_list_t object and its associated functions provide a standard
 * doubly-linked list data structure.
 */
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
lt_list_t *
lt_list_new(void)
{
	return lt_mem_alloc_object(sizeof (lt_list_t));
}

/*< public >*/

/**
 * lt_list_ref:
 * @list: a #lt_list_t.
 *
 * Increases the reference count of @list.
 *
 * Returns: (transfer none): the same @list object.
 */
lt_list_t *
lt_list_ref(lt_list_t *list)
{
	lt_return_val_if_fail (list != NULL, NULL);

	return lt_mem_ref(&list->parent);
}

/**
 * lt_list_unref:
 * @list: a #lt_list_t.
 *
 * Decreases the reference count of @list. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_list_unref(lt_list_t *list)
{
	if (list)
		lt_mem_unref(&list->parent);
}

/**
 * lt_list_free:
 * @data: a #lt_list_t.
 *
 * Frees all of the memory used by a #lt_list_t.
 */
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

/**
 * lt_list_first:
 * @list: a #lt_list_t.
 *
 * Gets the first element in a #lt_list_t.
 *
 * Returns: (transfer none): the first element in the #lt_list_t
 *          or %NULL if the #lt_list_t has no elements.
 */
lt_list_t *
lt_list_first(lt_list_t *list)
{
	if (list) {
		while (list->prev)
			list = list->prev;
	}

	return list;
}

/**
 * lt_list_last:
 * @list: a #lt_list_t.
 *
 * Gets the last element in a #lt_list_t.
 *
 * Returns: (transfer none): the last element in the #lt_list_t
 *          or %NULL if the #lt_list_t has no elements.
 */
lt_list_t *
lt_list_last(lt_list_t *list)
{
	if (list) {
		while (list->next)
			list = list->next;
	}

	return list;
}

/**
 * lt_list_previous:
 * @list: a #lt_list_t.
 *
 * Gets the previous element in a #lt_list_t.
 *
 * Returns: (transfer none): the previous element, or %NULL if there are no previous elements.
 */
lt_list_t *
lt_list_previous(const lt_list_t *list)
{
	return list ? list->prev : NULL;
}

/**
 * lt_list_next:
 * @list: a #lt_list_t.
 *
 * Gets the next element in a #lt_list_t.
 *
 * Returns: (transfer none): the next element, or %NULL if there are no more elements.
 */
lt_list_t *
lt_list_next(const lt_list_t *list)
{
	return list ? list->next : NULL;
}

/**
 * lt_list_value:
 * @list: a #lt_list_t.
 *
 * Gets a value in a #lt_list_t.
 *
 * Returns: (transfer none): a pointer to be set to the #lt_list_t.
 */
lt_pointer_t
lt_list_value(const lt_list_t *list)
{
	lt_return_val_if_fail (list != NULL, NULL);

	return list->value;
}

/**
 * lt_list_length:
 * @list: a #lt_list_t.
 *
 * Gets the number of elements in a #lt_list_t.
 *
 * Returns: the number of elements in the #lt_list_t.
 */
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

/**
 * lt_list_append:
 * @list: a #lt_list_t.
 * @data: the data for the new element
 * @func: (scope async): the call back function to destroy @data or %NULL
 *
 * Adds a new element on to the end of the list.
 *
 * Returns: the new start of the #lt_list_t.
 */
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

/**
 * lt_list_prepend:
 * @list: a #lt_list_t
 * @data: the data for the new element
 * @func: (scope async): the call back function to destroy @data or %NULL
 *
 * Adds a new element on to the start of the list.
 *
 * Returns: the new start of the #lt_list_t.
 */
lt_list_t *
lt_list_prepend(lt_list_t         *list,
		lt_pointer_t       data,
		lt_destroy_func_t  func)
{
	lt_list_t *l = lt_list_new();

	l->value = data;
	l->next = list;
	lt_mem_add_ref(&l->parent, l, _lt_list_update);
	if (func)
		lt_mem_add_ref(&l->parent, data, func);
	if (list) {
		l->prev = list->prev;
		if (list->prev)
			list->prev->next = l;
		list->prev = l;
	} else {
		l->prev = NULL;
	}

	return l;
}

#if 0
/**
 * lt_list_remove:
 * @list: a #lt_list_t.
 * @data: the data of the element to remove.
 *
 * Removes an element from a #lt_list_t.
 * If two elements contain the same data, only the first is removed.
 * If none of the elements contain the data, the #lt_list_t is unchanged.
 * This works similar to lt_list_delete() though, the difference is
 * this won't calls the finalizer to destroy the data in the element.
 *
 * Returns: the new start of the #lt_list_t.
 */
lt_list_t *
lt_list_remove(lt_list_t    *list,
	       lt_pointer_t  data)
{
	lt_list_t *l = list;

	while (l) {
		if (l->value == data) {
			lt_mem_remove_ref(&l->parent, value);
			list = lt_list_delete_link(list, l);
			break;
		} else {
			l = l->next;
		}
	}

	return list;
}

/**
 * lt_list_delete:
 * @list: a #lt_list_t.
 * @data: the data of the element to remove.
 *
 * Removes an element from a #lt_list_t.
 * If two elements contain the same data, only the first is removed.
 * If none of the elements contain the data, the #lt_list_t is unchanged.
 *
 * Returns: the new start of the #lt_list_t.
 */
lt_list_t *
lt_list_delete(lt_list_t    *list,
	       lt_pointer_t  data)
{
	lt_list_t *l = list;

	while (l) {
		if (l->value == data) {
			list = lt_list_delete_link(list, l);
			break;
		} else {
			l = l->next;
		}
	}

	return list;
}
#endif

/**
 * lt_list_delete_link:
 * @list: a #lt_list_t
 * @link_: node to delete from @list
 *
 * Removes the node @link_ from the @list and frees it.
 *
 * Returns: the new head of @list
 */
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

/**
 * lt_list_find:
 * @list: a #lt_list_t
 * @data: the element data to find
 *
 * Finds the element in a #lt_list_t which
 * contains the given data.
 *
 * Returns: the found #lt_list_t element, or %NULL if it's not found
 */
lt_list_t *
lt_list_find(lt_list_t          *list,
	     const lt_pointer_t  data)
{
	while (list) {
		if (list->value == data)
			break;
		list = list->next;
	}

	return list;
}

/**
 * lt_list_find_custom:
 * @list: a #lt_list_t
 * @data: the data passed to the function
 * @func: (scope call): the function to call for each element.
 *        It should return 0 when the desired element is found
 *
 * Finds an element in a #lt_list_t, using a supplied function to
 * find the desired element. It iterates over the list, calling
 * the given function which should return 0 when the desired
 * element is found. The function takes two const #lt_pointer_t
 * arguments, the #lt_list_t element's data as the first argument
 * and the given data.
 *
 * Returns: the found #lt_list_t element, or %NULL if it's not found
 */
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

/**
 * lt_list_sort:
 * @list: a #lt_list_t
 * @func: (scope call): the comparison function used to sort the #lt_list_t.
 *        This function is passed the data from 2 elements of the #lt_list_t
 *        and should return 0 if they are equal, a negative value if the
 *        first element comes before the second, or a positive value if
 *        the first element comes after the second.
 *
 * Sorts a #lt_list_t using the given comparison function.
 *
 * Returns: the start of the sorted #lt_list_t
 */
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

/**
 * lt_list_pop:
 * @list: a #lt_list_t
 * @data: a pointer to set the data in the first element
 *
 * Sets the data in the first element to @data and drop the element.
 *
 * Returns: the new head of @list.
 */
lt_list_t *
lt_list_pop(lt_list_t    *list,
	    lt_pointer_t *data)
{
	lt_return_val_if_fail (list != NULL, NULL);

	if (list->value)
		lt_mem_remove_ref(&list->parent, list->value);
	if (data)
		*data = list->value;
	list = lt_list_delete_link(list, list);

	return list;
}
