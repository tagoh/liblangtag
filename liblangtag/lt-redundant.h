/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-redundant.h
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

#ifndef __LT_REDUNDANT_H__
#define __LT_REDUNDANT_H__

#include <liblangtag/lt-macros.h>

LT_BEGIN_DECLS

/**
 * lt_redundant_t:
 *
 * All the fields in the <structname>lt_redundant_t</structname>
 * structure are private to the #lt_redundant_t implementation.
 */
typedef struct _lt_redundant_t	lt_redundant_t;


lt_redundant_t *lt_redundant_ref              (lt_redundant_t       *redundant);
void            lt_redundant_unref            (lt_redundant_t       *redundant);
const char     *lt_redundant_get_better_tag   (const lt_redundant_t *redundant);
const char     *lt_redundant_get_tag          (const lt_redundant_t *redundant);
const char     *lt_redundant_get_preferred_tag(const lt_redundant_t *redundant);
const char     *lt_redundant_get_name         (const lt_redundant_t *redundant);
void            lt_redundant_dump             (const lt_redundant_t *redundant);
lt_bool_t       lt_redundant_compare          (const lt_redundant_t *v1,
                                               const lt_redundant_t *v2);

LT_END_DECLS

#endif /* __LT_REDUNDANT_H__ */
