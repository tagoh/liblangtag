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

#include <glib.h> /* XXX: just shut up GHashTable dependency in lt-mem.h */
#include <stdlib.h>
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-trie.h"


typedef struct _lt_trie_node_t	lt_trie_node_t;
struct _lt_trie_node_t {
	lt_mem_t           parent;
	lt_trie_node_t    *node[255];
	lt_destroy_func_t  destroy_func;
	lt_pointer_t       data;
};
struct _lt_trie_t {
	lt_mem_t        parent;
	lt_trie_node_t *root;
};

/*< private >*/
static lt_trie_node_t *
lt_trie_node_new(void)
{
	return lt_mem_alloc_object(sizeof (lt_trie_node_t));
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
	lt_return_val_if_fail (node != NULL, FALSE);
	lt_return_val_if_fail (key != NULL, FALSE);

	if (*key == 0) {
		if (node->data && !replace) {
			return FALSE;
		} else {
			if (node->data) {
				lt_mem_delete_ref(&node->parent, node->data);
				if (node->destroy_func)
					node->destroy_func(node->data);
			}
			node->data = data;
			node->destroy_func = func;
			if (func)
				lt_mem_add_ref(&node->parent, data, func);

			return TRUE;
		}
	}
	if (!node->node[(*key) - 1]) {
		node->node[(*key) - 1] = lt_trie_node_new();
		if (!node->node[(*key) - 1])
			return FALSE;
		lt_mem_add_ref(&node->parent, node->node[(*key) - 1],
			       (lt_destroy_func_t)lt_trie_node_unref);
		lt_mem_add_weak_pointer(&node->node[(*key) - 1]->parent,
					(lt_pointer_t *)&node->node[(*key) - 1]);
	}

	return lt_trie_node_add(node->node[(*key) - 1], key + 1, data, func, replace);
}

static lt_bool_t
lt_trie_node_remove(lt_trie_node_t *node,
		    const char     *key)
{
	int i;
	lt_bool_t found = FALSE;

	lt_return_val_if_fail (node != NULL, FALSE);
	lt_return_val_if_fail (key != NULL, FALSE);

	if (*key == 0) {
		if (!node->data)
			return FALSE;
		lt_mem_delete_ref(&node->parent, node->data);
		if (node->destroy_func)
			node->destroy_func(node->data);
		node->data = NULL;
		for (i = 0; i < 255; i++) {
			found |= node->node[i] != NULL;
		}
		if (!found)
			lt_trie_node_unref(node);
		return TRUE;
	}
	if (!node->node[(*key) - 1])
		return FALSE;

	return lt_trie_node_remove(node->node[(*key) - 1], key + 1);
}

static const lt_pointer_t
lt_trie_node_lookup(lt_trie_node_t *node,
		    const char     *key)
{
	lt_return_val_if_fail (node != NULL, NULL);
	lt_return_val_if_fail (key != NULL, NULL);

	if (*key == 0)
		return node->data;
	if (!node->node[(*key) - 1])
		return NULL;

	return lt_trie_node_lookup(node->node[(*key) - 1], key + 1);
}

/*< protected >*/

/*< public >*/
lt_trie_t *
lt_trie_new(void)
{
	return lt_mem_alloc_object(sizeof (lt_trie_t));
}

lt_trie_t *
lt_trie_ref(lt_trie_t *trie)
{
	lt_return_val_if_fail (trie != NULL, NULL);

	return lt_mem_ref(&trie->parent);
}

void
lt_trie_unref(lt_trie_t *trie)
{
	if (trie)
		lt_mem_unref(&trie->parent);
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
		if ((trie->root = lt_trie_node_new()) == NULL)
			return FALSE;
		lt_mem_add_ref(&trie->parent, trie->root,
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
		if ((trie->root = lt_trie_node_new()) == NULL)
			return FALSE;
		lt_mem_add_ref(&trie->parent, trie->root,
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

	if (!trie->root)
		return FALSE;

	return lt_trie_node_remove(trie->root, key);
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
