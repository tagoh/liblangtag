/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extlang.h
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

#ifndef __LT_EXTLANG_H__
#define __LT_EXTLANG_H__

#include <glib.h>

G_BEGIN_DECLS

/**
 * lt_extlang_t:
 *
 * All the fields in the <structname>lt_extlang_t</structname>
 * structure are private to the #lt_extlang_t implementation.
 */
typedef struct _lt_extlang_t	lt_extlang_t;


lt_extlang_t *lt_extlang_ref               (lt_extlang_t       *extlang);
void          lt_extlang_unref             (lt_extlang_t       *extlang);
const gchar  *lt_extlang_get_tag           (const lt_extlang_t *extlang);
const gchar  *lt_extlang_get_preferred_tag (const lt_extlang_t *extlang);
const gchar  *lt_extlang_get_name          (const lt_extlang_t *extlang);
const gchar  *lt_extlang_get_macro_language(const lt_extlang_t *extlang);
const gchar  *lt_extlang_get_prefix        (const lt_extlang_t *extlang);
void          lt_extlang_dump              (const lt_extlang_t *extlang);
gboolean      lt_extlang_compare           (const lt_extlang_t *v1,
					    const lt_extlang_t *v2);

G_END_DECLS

#endif /* __LT_EXTLANG_H__ */
