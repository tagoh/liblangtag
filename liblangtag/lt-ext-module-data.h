/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-module-data.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#if !defined (__LANGTAG_H__INSIDE) && !defined (__LANGTAG_COMPILATION)
#error "Only <liblangtag/langtag.h> can be included directly."
#endif

#ifndef __LT_EXT_MODULE_DATA_H__
#define __LT_EXT_MODULE_DATA_H__

#include <glib.h>

G_BEGIN_DECLS

/**
 * lt_ext_module_data_t:
 * @dummy: a dummy pointer for alignment.
 *
 * All the fields in the <structname>lt_ext_module_data_t</structname>
 * structure are private to the #lt_ext_module_data_t implementation.
 */
struct _lt_ext_module_data_t {
	gpointer dummy[8];
};

typedef struct _lt_ext_module_data_t	lt_ext_module_data_t;


lt_ext_module_data_t *lt_ext_module_data_new  (gsize                 size,
                                               GDestroyNotify        finalizer);
lt_ext_module_data_t *lt_ext_module_data_ref  (lt_ext_module_data_t *data);
void                  lt_ext_module_data_unref(lt_ext_module_data_t *data);

G_END_DECLS

#endif /* __LT_EXT_MODULE_DATA_H__ */
