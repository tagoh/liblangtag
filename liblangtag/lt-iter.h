/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-iter.h
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

#ifndef __LT_ITER_H__
#define __LT_ITER_H__

#include <liblangtag/lt-macros.h>

LT_BEGIN_DECLS

typedef struct _lt_iter_tmpl_t	lt_iter_tmpl_t;
typedef struct _lt_iter_t	lt_iter_t;

struct _lt_iter_t {
	lt_iter_tmpl_t *target;
};

lt_iter_t *lt_iter_init  (lt_iter_tmpl_t *tmpl);
void       lt_iter_finish(lt_iter_t      *iter);
lt_bool_t  lt_iter_next  (lt_iter_t      *iter,
                          lt_pointer_t   *key,
                          lt_pointer_t   *val);

LT_END_DECLS

#endif /* __LT_ITER_H__ */
