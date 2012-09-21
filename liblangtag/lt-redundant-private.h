/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-redundant-private.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_REDUNDANT_PRIVATE_H__
#define __LT_REDUNDANT_PRIVATE_H__

#include <glib.h>
#include "lt-redundant.h"

G_BEGIN_DECLS

lt_redundant_t *lt_redundant_create           (void);
void            lt_redundant_set_tag          (lt_redundant_t *redundant,
                                               const gchar    *subtag);
void            lt_redundant_set_name         (lt_redundant_t *redundant,
                                               const gchar    *description);
void            lt_redundant_set_preferred_tag(lt_redundant_t *redundant,
                                               const gchar    *subtag);

G_END_DECLS

#endif /* __LT_REDUNDANT_PRIVATE_H__ */
