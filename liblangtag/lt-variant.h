/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-variant.h
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

#ifndef __LT_VARIANT_H__
#define __LT_VARIANT_H__

#include <glib.h>

G_BEGIN_DECLS

/**
 * lt_variant_t:
 *
 * All the fields in the <structname>lt_variant_t</structname>
 * structure are private to the #lt_variant_t implementation.
 */
typedef struct _lt_variant_t	lt_variant_t;


lt_variant_t *lt_variant_ref              (lt_variant_t       *variant);
void          lt_variant_unref            (lt_variant_t       *variant);
const gchar  *lt_variant_get_better_tag   (const lt_variant_t *variant);
const gchar  *lt_variant_get_tag          (const lt_variant_t *variant);
const gchar  *lt_variant_get_preferred_tag(const lt_variant_t *variant);
const gchar  *lt_variant_get_name         (const lt_variant_t *variant);
const GList  *lt_variant_get_prefix       (const lt_variant_t *variant);
void          lt_variant_dump             (const lt_variant_t *variant);
gboolean      lt_variant_compare          (const lt_variant_t *v1,
					   const lt_variant_t *v2);

G_END_DECLS

#endif /* __LT_VARIANT_H__ */
