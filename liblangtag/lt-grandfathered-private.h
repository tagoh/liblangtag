/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-grandfathered-private.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_GRANDFATHERED_PRIVATE_H__
#define __LT_GRANDFATHERED_PRIVATE_H__

#include <glib.h>
#include "lt-grandfathered.h"

G_BEGIN_DECLS

lt_grandfathered_t *lt_grandfathered_create           (void);
void                lt_grandfathered_set_tag          (lt_grandfathered_t *grandfathered,
                                                       const gchar        *subtag);
void                lt_grandfathered_set_name         (lt_grandfathered_t *grandfathered,
                                                       const gchar        *description);
void                lt_grandfathered_set_preferred_tag(lt_grandfathered_t *grandfathered,
                                                       const gchar        *subtag);

G_END_DECLS

#endif /* __LT_GRANDFATHERED_PRIVATE_H__ */
