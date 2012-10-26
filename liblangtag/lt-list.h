/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-list.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#if !defined (__LANGTAG_H__INSIDE) && !defined (__LANGTAG_COMPILATION)
#error "Only <liblangtag/langtag.h> can be included directly."
#endif

#ifndef __LT_LIST_H__
#define __LT_LIST_H__

#include <liblangtag/lt-macros.h>

LT_BEGIN_DECLS

/**
 * lt_list_t:
 *
 * All the fields in the <structname>lt_list_t</structname>
 * structure are private to the #lt_list_t implementation.
 */
typedef struct _lt_list_t	lt_list_t;

lt_list_t    *lt_list_new        (void);
lt_list_t    *lt_list_ref        (lt_list_t          *list);
void          lt_list_unref      (lt_list_t          *list);
void          lt_list_free       (lt_pointer_t        data);
lt_list_t    *lt_list_first      (lt_list_t          *list);
lt_list_t    *lt_list_last       (lt_list_t          *list);
lt_list_t    *lt_list_previous   (const lt_list_t    *list);
lt_list_t    *lt_list_next       (const lt_list_t    *list);
lt_pointer_t  lt_list_value      (const lt_list_t    *list);
size_t        lt_list_length     (const lt_list_t    *list);
lt_list_t    *lt_list_append     (lt_list_t          *list,
                                  lt_pointer_t        data,
                                  lt_destroy_func_t   func);
lt_list_t    *lt_list_prepend    (lt_list_t          *list,
				  lt_pointer_t        data,
				  lt_destroy_func_t   func);
lt_list_t    *lt_list_remove     (lt_list_t          *list,
                                  lt_pointer_t        data);
lt_list_t    *lt_list_delete     (lt_list_t          *list,
				  lt_pointer_t        data);
lt_list_t    *lt_list_delete_link(lt_list_t          *list,
				  lt_list_t          *link_);
lt_list_t    *lt_list_find       (lt_list_t          *list,
                                  const lt_pointer_t  data);
lt_list_t    *lt_list_find_custom(lt_list_t          *list,
                                  const lt_pointer_t  data,
                                  lt_compare_func_t   func);
lt_list_t    *lt_list_sort       (lt_list_t          *list,
				  lt_compare_func_t   func);
lt_list_t    *lt_list_pop        (lt_list_t          *list,
				  lt_pointer_t       *data);

LT_END_DECLS

#endif /* __LT_LIST_H__ */
