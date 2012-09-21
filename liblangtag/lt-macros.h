/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-macros.h
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

#ifndef __LT_MACROS_H__
#define __LT_MACROS_H__

#ifdef __cplusplus
#define LT_BEGIN_DECLS	extern "C" {
#define LT_END_DECLS	}
#else
#define LT_BEGIN_DECLS
#define LT_END_DECLS
#endif

LT_BEGIN_DECLS

typedef void *		lt_pointer_t;
typedef int		lt_bool_t;

LT_END_DECLS

#endif /* __LT_MACROS_H__ */
