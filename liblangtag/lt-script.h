/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-script.h
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

#ifndef __LT_SCRIPT_H__
#define __LT_SCRIPT_H__

#include <liblangtag/lt-macros.h>

LT_BEGIN_DECLS

/**
 * lt_script_t:
 *
 * All the fields in the <structname>lt_script_t</structname>
 * structure are private to the #lt_script_t implementation.
 */
typedef struct _lt_script_t	lt_script_t;


lt_script_t *lt_script_ref                (lt_script_t       *script);
void         lt_script_unref              (lt_script_t       *script);
const char  *lt_script_get_name           (const lt_script_t *script);
const char  *lt_script_get_tag            (const lt_script_t *script);
void         lt_script_dump               (const lt_script_t *script);
const char  *lt_script_convert_to_modifier(const lt_script_t *script);
lt_bool_t    lt_script_compare            (const lt_script_t *v1,
					   const lt_script_t *v2);

LT_END_DECLS

#endif /* __LT_SCRIPT_H__ */
