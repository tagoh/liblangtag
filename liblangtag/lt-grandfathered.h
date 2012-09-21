/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-grandfathered.h
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

#ifndef __LT_GRANDFATHERED_H__
#define __LT_GRANDFATHERED_H__

#include <liblangtag/lt-macros.h>

LT_BEGIN_DECLS

/**
 * lt_grandfathered_t:
 *
 * All the fields in the <structname>lt_grandfathered_t</structname>
 * structure are private to the #lt_grandfathered_t implementation.
 */
typedef struct _lt_grandfathered_t	lt_grandfathered_t;


lt_grandfathered_t *lt_grandfathered_ref              (lt_grandfathered_t       *grandfathered);
void                lt_grandfathered_unref            (lt_grandfathered_t       *grandfathered);
const char         *lt_grandfathered_get_better_tag   (const lt_grandfathered_t *grandfathered);
const char         *lt_grandfathered_get_tag          (const lt_grandfathered_t *grandfathered);
const char         *lt_grandfathered_get_preferred_tag(const lt_grandfathered_t *grandfathered);
const char         *lt_grandfathered_get_name         (const lt_grandfathered_t *grandfathered);
void                lt_grandfathered_dump             (const lt_grandfathered_t *grandfathered);
lt_bool_t           lt_grandfathered_compare          (const lt_grandfathered_t *v1,
						       const lt_grandfathered_t *v2);

LT_END_DECLS

#endif /* __LT_GRANDFATHERED_H__ */
