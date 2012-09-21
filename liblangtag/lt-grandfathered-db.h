/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-grandfathered-db.h
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

#ifndef __LT_GRANDFATHERED_DB_H__
#define __LT_GRANDFATHERED_DB_H__

#include <glib.h>
#include <liblangtag/lt-grandfathered.h>

G_BEGIN_DECLS

/**
 * lt_grandfathered_db_t:
 *
 * All the fields in the <structname>lt_grandfathered_db_t</structname>
 * structure are private to the #lt_grandfathered_db_t implementation.
 */
typedef struct _lt_grandfathered_db_t	lt_grandfathered_db_t;


lt_grandfathered_db_t *lt_grandfathered_db_new   (void);
lt_grandfathered_db_t *lt_grandfathered_db_ref   (lt_grandfathered_db_t *grandfathereddb);
void                   lt_grandfathered_db_unref (lt_grandfathered_db_t *grandfathereddb);
lt_grandfathered_t    *lt_grandfathered_db_lookup(lt_grandfathered_db_t *grandfathereddb,
                                                  const gchar           *tag);

G_END_DECLS

#endif /* __LT_GRANDFATHERED_DB_H__ */
