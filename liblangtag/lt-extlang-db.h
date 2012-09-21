/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extlang-db.h
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

#ifndef __LT_EXTLANG_DB_H__
#define __LT_EXTLANG_DB_H__

#include <glib.h>
#include <liblangtag/lt-extlang.h>

G_BEGIN_DECLS

/**
 * lt_extlang_db_t:
 *
 * All the fields in the <structname>lt_extlang_db_t</structname>
 * structure are private to the #lt_extlang_db_t implementation.
 */
typedef struct _lt_extlang_db_t	lt_extlang_db_t;


lt_extlang_db_t *lt_extlang_db_new   (void);
lt_extlang_db_t *lt_extlang_db_ref   (lt_extlang_db_t *extlangdb);
void             lt_extlang_db_unref (lt_extlang_db_t *extlangdb);
lt_extlang_t    *lt_extlang_db_lookup(lt_extlang_db_t *extlangdb,
                                      const gchar     *subtag);

G_END_DECLS

#endif /* __LT_EXTLANG_DB_H__ */
