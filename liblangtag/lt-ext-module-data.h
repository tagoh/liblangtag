/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-module-data.h
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

#ifndef __LT_EXT_MODULE_DATA_H__
#define __LT_EXT_MODULE_DATA_H__

#include <liblangtag/lt-macros.h>
#include <glib.h>

LT_BEGIN_DECLS

/**
 * lt_ext_module_data_t:
 * @dummy: a dummy pointer for alignment.
 *
 * All the fields in the <structname>lt_ext_module_data_t</structname>
 * structure are private to the #lt_ext_module_data_t implementation.
 */
struct _lt_ext_module_data_t {
	lt_pointer_t dummy[8];
};

typedef struct _lt_ext_module_data_t	lt_ext_module_data_t;


lt_ext_module_data_t *lt_ext_module_data_new  (size_t                size,
                                               GDestroyNotify        finalizer);
lt_ext_module_data_t *lt_ext_module_data_ref  (lt_ext_module_data_t *data);
void                  lt_ext_module_data_unref(lt_ext_module_data_t *data);

LT_END_DECLS

#endif /* __LT_EXT_MODULE_DATA_H__ */
