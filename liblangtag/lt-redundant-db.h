/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-redundant-db.h
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

#ifndef __LT_REDUNDANT_DB_H__
#define __LT_REDUNDANT_DB_H__

#include <liblangtag/lt-macros.h>
#include <liblangtag/lt-redundant.h>

LT_BEGIN_DECLS

/**
 * lt_redundant_db_t:
 *
 * All the fields in the <structname>lt_redundant_db_t</structname>
 * structure are private to the #lt_redundant_db_t implementation.
 */
typedef struct _lt_redundant_db_t	lt_redundant_db_t;


lt_redundant_db_t *lt_redundant_db_new   (void);
lt_redundant_db_t *lt_redundant_db_ref   (lt_redundant_db_t *redundantdb);
void               lt_redundant_db_unref (lt_redundant_db_t *redundantdb);
lt_redundant_t    *lt_redundant_db_lookup(lt_redundant_db_t *redundantdb,
                                          const char        *tag);

LT_END_DECLS

#endif /* __LT_REDUNDANT_DB_H__ */
