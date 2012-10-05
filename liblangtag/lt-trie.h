/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-trie.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_TRIE_H__
#define __LT_TRIE_H__

#include <liblangtag/lt-macros.h>

LT_BEGIN_DECLS

typedef struct _lt_trie_t	lt_trie_t;

lt_trie_t    *lt_trie_new    (void);
lt_trie_t    *lt_trie_ref    (lt_trie_t         *trie);
void          lt_trie_unref  (lt_trie_t         *trie);
lt_bool_t     lt_trie_add    (lt_trie_t         *trie,
                              const char        *key,
                              lt_pointer_t       data,
                              lt_destroy_func_t  func);
lt_bool_t     lt_trie_replace(lt_trie_t         *trie,
                              const char        *key,
                              lt_pointer_t       data,
                              lt_destroy_func_t  func);
lt_bool_t     lt_trie_remove (lt_trie_t         *trie,
                              const char        *key);
lt_pointer_t  lt_trie_lookup (lt_trie_t         *trie,
                              const char        *key);

LT_END_DECLS

#endif /* __LT_TRIE_H__ */
