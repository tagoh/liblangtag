/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-mdule-private.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_EXT_MODULE_PRIVATE_H__
#define __LT_EXT_MODULE_PRIVATE_H__

#include <liblangtag/lt-macros.h>
#include <liblangtag/lt-ext-module-data.h>
#include <liblangtag/lt-ext-module.h>
#include <glib.h>

LT_BEGIN_DECLS

#define LT_MAX_EXT_MODULES	(('9' - '0' + 1) + ('z' - 'a' + 1) + 2)

lt_bool_t             lt_ext_module_validate_singleton   (char                   singleton);
int                   lt_ext_module_singleton_char_to_int(char                   singleton_c);
char                  lt_ext_module_singleton_int_to_char(int                    singleton);
lt_ext_module_t      *lt_ext_module_lookup               (char                   singleton_c);
lt_ext_module_t      *lt_ext_module_new                  (const char            *name);
const char           *lt_ext_module_get_name             (lt_ext_module_t       *module);
char                  lt_ext_module_get_singleton        (lt_ext_module_t       *module);
char                 *lt_ext_module_get_tag              (lt_ext_module_t       *module,
							  lt_ext_module_data_t  *data);
lt_ext_module_data_t *lt_ext_module_create_data          (lt_ext_module_t       *module);
lt_bool_t             lt_ext_module_parse_tag            (lt_ext_module_t       *module,
                                                          lt_ext_module_data_t  *data,
                                                          const char            *subtag,
                                                          GError               **error);
lt_bool_t             lt_ext_module_validate_tag         (lt_ext_module_t       *module,
							  lt_ext_module_data_t  *data);
lt_bool_t             lt_ext_module_precheck_tag         (lt_ext_module_t       *module,
							  lt_ext_module_data_t  *data,
							  const lt_tag_t        *tag,
							  GError               **error);

LT_END_DECLS

#endif /* __LT_EXT_MODULE_PRIVATE_H__ */
