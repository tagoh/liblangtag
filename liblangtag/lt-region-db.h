/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-region-db.h
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

#ifndef __LT_REGION_DB_H__
#define __LT_REGION_DB_H__

#include <liblangtag/lt-macros.h>
#include <liblangtag/lt-region.h>

LT_BEGIN_DECLS

/**
 * lt_region_db_t:
 *
 * All the fields in the <structname>lt_region_db_t</structname>
 * structure are private to the #lt_region_db_t implementation.
 */
typedef struct _lt_region_db_t		lt_region_db_t;


lt_region_db_t *lt_region_db_new   (void);
lt_region_db_t *lt_region_db_ref   (lt_region_db_t *regiondb);
void            lt_region_db_unref (lt_region_db_t *regiondb);
lt_region_t    *lt_region_db_lookup(lt_region_db_t *regiondb,
                                    const char     *language_or_code);

LT_END_DECLS

#endif /* __LT_REGION_DB_H__ */
