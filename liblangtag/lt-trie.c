/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-trie.c
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
#include <string.h>
#include "lt-mem.h"
#include "lt-iter-private.h"
#include "lt-messages.h"
#include "lt-trie.h"


typedef struct _lt_trie_node_t	lt_trie_node_t;
struct _lt_trie_node_t {
	lt_mem_t           parent;
	lt_trie_node_t    *node[255];
	lt_pointer_t       data;
	char               index_;
};
struct _lt_trie_t {
	lt_iter_tmpl_t  parent;
	lt_trie_node_t *root;
};

/*< private >*/
static lt_trie_node_t *
lt_trie_node_new(char index_)
{
	lt_trie_node_t *retval = lt_mem_alloc_object(sizeof (lt_trie_node_t));

	if (retval) {
		retval->index_ = index_;
	}

	return retval;
}

#if 0
static lt_trie_node_t *
lt_trie_node_ref(lt_trie_node_t *node)
{
	lt_return_val_if_fail (node != NULL, NULL);

	return lt_mem_ref(&node->parent);
}
#endif

static void
lt_trie_node_unref(lt_trie_node_t *node)
{
	if (node)
		lt_mem_unref(&node->parent);
}

static lt_bool_t
lt_trie_node_add(lt_trie_node_t    *node,
		 const char        *key,
		 lt_pointer_t       data,
		 lt_destroy_func_t  func,
		 lt_bool_t          replace)
{
	int index_;

	lt_return_val_if_fail (node != NULL, FALSE);
	lt_return_val_if_fail (key != NULL, FALSE);

	index_ = *key - 1;
	if (*key == 0) {
		if (node->data && !replace) {
			return FALSE;
		} else {
			if (node->data) {
				lt_mem_delete_ref(&node->parent, node->data);
			}
			node->data = data;
			if (func)
				lt_mem_add_ref(&node->parent, data, func);

			return TRUE;
		}
	}
	if (!node->node[index_]) {
		node->node[index_] = lt_trie_node_new(index_);
		if (!node->node[index_])
			return FALSE;
		lt_mem_add_ref(&node->parent, node->node[index_],
			       (lt_destroy_func_t)lt_trie_node_unref);
		lt_mem_add_weak_pointer(&node->node[index_]->parent,
					(lt_pointer_t *)&node->node[index_]);
	}

	return lt_trie_node_add(node->node[index_], key + 1, data, func, replace);
}

static lt_bool_t
lt_trie_node_remove(lt_trie_node_t *node,
		    lt_trie_node_t *parent,
		    const char     *key)
{
	int i, index_;
	lt_bool_t found = FALSE;

	lt_return_val_if_fail (node != NULL, FALSE);
	lt_return_val_if_fail (key != NULL, FALSE);

	index_ = *key - 1;
	if (*key == 0) {
		if (!node->data)
			return FALSE;
		lt_mem_delete_ref(&node->parent, node->data);
		node->data = NULL;
		for (i = 0; i < 255; i++) {
			found |= node->node[i] != NULL;
		}
		if (!found)
			lt_mem_delete_ref(&parent->parent, node);
		return TRUE;
	}
	if (!node->node[index_])
		return FALSE;

	return lt_trie_node_remove(node->node[index_], node, key + 1);
}

static const lt_pointer_t
lt_trie_node_lookup(lt_trie_node_t *node,
		    const char     *key)
{
	int index_;

	lt_return_val_if_fail (node != NULL, NULL);
	lt_return_val_if_fail (key != NULL, NULL);

	index_ = *key - 1;
	if (*key == 0)
		return node->data;
	if (!node->node[index_])
		return NULL;

	return lt_trie_node_lookup(node->node[index_], key + 1);
}

static lt_iter_t *
_lt_trie_iter_init(lt_iter_t *iter)
{
	lt_trie_iter_t *trie_iter = (lt_trie_iter_t *)iter;
	lt_trie_t *trie = (lt_trie_t *)iter->target;
	int i;

	trie_iter->pos_str = lt_string_new(NULL);
	trie_iter->stack = NULL;
	if (trie->root) {
		lt_trie_node_t *node = trie->root;

		for (i = 0; i < 255; i++) {
			if (node->node[i])
				trie_iter->stack = lt_list_append(trie_iter->stack, node->node[i], NULL);
		}
		/* add a terminator */
		trie_iter->stack = lt_list_append(trie_iter->stack, NULL, NULL);
	}

	return iter;
}

static void
_lt_trie_iter_fini(lt_iter_t *iter)
{
	lt_trie_iter_t *trie_iter = (lt_trie_iter_t *)iter;

	if (trie_iter->stack)
		lt_list_free(trie_iter->stack);
	lt_string_unref(trie_iter->pos_str);
}

static lt_bool_t
_lt_trie_iter_next(lt_iter_t    *iter,
		   lt_pointer_t *key,
		   lt_pointer_t *value)
{
	lt_trie_iter_t *trie_iter = (lt_trie_iter_t *)iter;
	int i;
	lt_trie_node_t *node = NULL;

	while (1) {
		if (lt_list_length(trie_iter->stack) == 0)
			break;
		trie_iter->stack = lt_list_pop(trie_iter->stack, (lt_pointer_t *)&node);
		if (node) {
			lt_string_append_c(trie_iter->pos_str, node->index_);
		} else {
			lt_string_truncate(trie_iter->pos_str, -1);
			continue;
		}
		for (i = 0; i < 255; i++) {
			if (node->node[i])
				trie_iter->stack = lt_list_append(trie_iter->stack, node->node[i], NULL);
		}
		/* add a terminator */
		trie_iter->stack = lt_list_append(trie_iter->stack, NULL, NULL);
		if (node->data) {
			if (key) {
				*key = strdup(lt_string_value(trie_iter->pos_str));
			}
			if (value)
				*value = node->data;

			return TRUE;
		}
	}

	return FALSE;
}

/*< protected >*/

/*< public >*/
lt_trie_t *
lt_trie_new(void)
{
	lt_trie_t *retval = lt_mem_alloc_object(sizeof (lt_trie_t));

	if (retval) {
		retval->parent.init = _lt_trie_iter_init;
		retval->parent.fini = _lt_trie_iter_fini;
		retval->parent.next = _lt_trie_iter_next;
	}

	return retval;
}

lt_trie_t *
lt_trie_ref(lt_trie_t *trie)
{
	lt_return_val_if_fail (trie != NULL, NULL);

	return lt_mem_ref((lt_mem_t *)trie);
}

void
lt_trie_unref(lt_trie_t *trie)
{
	if (trie)
		lt_mem_unref((lt_mem_t *)trie);
}

lt_bool_t
lt_trie_add(lt_trie_t         *trie,
	    const char        *key,
	    lt_pointer_t       data,
	    lt_destroy_func_t  func)
{
	lt_return_val_if_fail (trie != NULL, FALSE);
	lt_return_val_if_fail (key != NULL, FALSE);
	lt_return_val_if_fail (data != NULL, FALSE);

	if (!trie->root) {
		if ((trie->root = lt_trie_node_new(0)) == NULL)
			return FALSE;
		lt_mem_add_ref((lt_mem_t *)trie, trie->root,
			       (lt_destroy_func_t)lt_trie_node_unref);
		lt_mem_add_weak_pointer(&trie->root->parent, (lt_pointer_t *)&trie->root);
	}

	return lt_trie_node_add(trie->root, key, data, func, FALSE);
}

lt_bool_t
lt_trie_replace(lt_trie_t         *trie,
		const char        *key,
		lt_pointer_t       data,
		lt_destroy_func_t  func)
{
	lt_return_val_if_fail (trie != NULL, FALSE);
	lt_return_val_if_fail (key != NULL, FALSE);
	lt_return_val_if_fail (data != NULL, FALSE);

	if (!trie->root) {
		if ((trie->root = lt_trie_node_new(0)) == NULL)
			return FALSE;
		lt_mem_add_ref((lt_mem_t *)trie, trie->root,
			       (lt_destroy_func_t)lt_trie_node_unref);
	}

	return lt_trie_node_add(trie->root, key, data, func, TRUE);
}

lt_bool_t
lt_trie_remove(lt_trie_t  *trie,
	       const char *key)
{
	lt_return_val_if_fail (trie != NULL, FALSE);
	lt_return_val_if_fail (key != NULL, FALSE);
	lt_return_val_if_fail (*key != 0, FALSE);

	if (!trie->root)
		return FALSE;

	return lt_trie_node_remove(trie->root, NULL, key);
}

lt_pointer_t
lt_trie_lookup(lt_trie_t  *trie,
	       const char *key)
{
	lt_return_val_if_fail (trie != NULL, NULL);
	lt_return_val_if_fail (key != NULL, NULL);

	if (!trie->root)
		return NULL;

	return lt_trie_node_lookup(trie->root, key);
}

lt_list_t *
lt_trie_keys(lt_trie_t *trie)
{
	lt_trie_iter_t iter;
	lt_list_t *retval = NULL;
	lt_pointer_t key;

	lt_return_val_if_fail (trie != NULL, NULL);

	if (trie->root)
		return NULL;

	lt_iter_init((lt_iter_t *)&iter, &trie->parent);

	while (lt_iter_next((lt_iter_t *)&iter, &key, NULL)) {
		retval = lt_list_append(retval, key, free);
	}

	lt_iter_finish((lt_iter_t *)&iter);

	return retval;
}
