/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-region-private.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_REGION_PRIVATE_H__
#define __LT_REGION_PRIVATE_H__

#include <glib.h>
#include "lt-region.h"

G_BEGIN_DECLS

lt_region_t *lt_region_create           (void);
void         lt_region_set_name         (lt_region_t *region,
                                         const gchar *description);
void         lt_region_set_tag          (lt_region_t *region,
                                         const gchar *subtag);
void         lt_region_set_preferred_tag(lt_region_t *region,
                                         const gchar *subtag);

G_END_DECLS

#endif /* __LT_REGION_PRIVATE_H__ */
