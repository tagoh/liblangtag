/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-database.h
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

#ifndef __LT_DATABASE_H__
#define __LT_DATABASE_H__

#include <glib.h>
#include <liblangtag/lt-extlang-db.h>
#include <liblangtag/lt-grandfathered-db.h>
#include <liblangtag/lt-lang-db.h>
#include <liblangtag/lt-redundant-db.h>
#include <liblangtag/lt-region-db.h>
#include <liblangtag/lt-script-db.h>
#include <liblangtag/lt-variant-db.h>

G_BEGIN_DECLS

void                   lt_db_set_datadir      (const gchar *path);
const gchar           *lt_db_get_datadir      (void);
void                   lt_db_initialize       (void);
void                   lt_db_finalize         (void);
lt_lang_db_t          *lt_db_get_lang         (void);
lt_extlang_db_t       *lt_db_get_extlang      (void);
lt_script_db_t        *lt_db_get_script       (void);
lt_region_db_t        *lt_db_get_region       (void);
lt_variant_db_t       *lt_db_get_variant      (void);
lt_grandfathered_db_t *lt_db_get_grandfathered(void);
lt_redundant_db_t     *lt_db_get_redundant    (void);

G_END_DECLS

#endif /* __LT_DATABASE_H__ */
