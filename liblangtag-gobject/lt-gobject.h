/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-gobject.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_GOBJECT_H__
#define __LT_GOBJECT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define LT_TYPE_ERROR			lt_error_get_type()
#define LT_TYPE_EXT_MODULE		lt_ext_module_get_type()
#define LT_TYPE_EXT_MODULE_DATA		lt_ext_module_data_get_type()
#define LT_TYPE_EXTENSION		lt_extension_get_type()
#define LT_TYPE_EXTLANG_DB		lt_extlang_db_get_type()
#define LT_TYPE_EXTLANG			lt_extlang_get_type()
#define LT_TYPE_GRANDFATHERED_DB	lt_grandfathered_db_get_type()
#define LT_TYPE_GRANDFATHERED		lt_grandfathered_get_type()
#define LT_TYPE_LANG_DB			lt_lang_db_get_type()
#define LT_TYPE_LANG			lt_lang_get_type()
#define LT_TYPE_LIST			lt_list_get_type()
#define LT_TYPE_REDUNDANT_DB		lt_redundant_db_get_type()
#define LT_TYPE_REDUNDANT		lt_redundant_get_type()
#define LT_TYPE_REGION_DB		lt_region_db_get_type()
#define LT_TYPE_REGION			lt_region_get_type()
#define LT_TYPE_SCRIPT_DB		lt_script_db_get_type()
#define LT_TYPE_SCRIPT			lt_script_get_type()
#define LT_TYPE_STRING			lt_string_get_type()
#define LT_TYPE_TAG			lt_tag_get_type()
#define LT_TYPE_VARIANT_DB		lt_variant_db_get_type()
#define LT_TYPE_VARIANT			lt_variant_get_type()


GType lt_error_get_type           (void);
GType lt_ext_module_get_type      (void);
GType lt_ext_module_data_get_type (void);
GType lt_extension_get_type       (void);
GType lt_extlang_db_get_type      (void);
GType lt_extlang_get_type         (void);
GType lt_grandfathered_db_get_type(void);
GType lt_grandfathered_get_type   (void);
GType lt_lang_db_get_type         (void);
GType lt_lang_get_type            (void);
GType lt_list_get_type            (void);
GType lt_redundant_db_get_type    (void);
GType lt_redundant_get_type       (void);
GType lt_region_db_get_type       (void);
GType lt_region_get_type          (void);
GType lt_script_db_get_type       (void);
GType lt_script_get_type          (void);
GType lt_string_get_type          (void);
GType lt_tag_get_type             (void);
GType lt_variant_db_get_type      (void);
GType lt_variant_get_type         (void);

G_END_DECLS

#endif /* __LT_GOBJECT_H__ */
