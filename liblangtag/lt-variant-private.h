/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-variant-private.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_VARIANT_PRIVATE_H__
#define __LT_VARIANT_PRIVATE_H__

#include <glib.h>
#include "lt-variant.h"

G_BEGIN_DECLS

lt_variant_t *lt_variant_create           (void);
void          lt_variant_set_tag          (lt_variant_t *variant,
                                           const gchar  *subtag);
void          lt_variant_set_preferred_tag(lt_variant_t *variant,
                                           const gchar  *subtag);
void          lt_variant_set_name         (lt_variant_t *variant,
                                           const gchar  *description);
void          lt_variant_add_prefix       (lt_variant_t *variant,
                                           const gchar  *prefix);

G_END_DECLS

#endif /* __LT_VARIANT_PRIVATE_H__ */
