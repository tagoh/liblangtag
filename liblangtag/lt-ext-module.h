/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-mdule.h
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

#ifndef __LT_EXT_MODULE_H__
#define __LT_EXT_MODULE_H__

#include <liblangtag/lt-macros.h>
#include <liblangtag/lt-error.h>
#include <liblangtag/lt-ext-module-data.h>
#include <liblangtag/lt-tag.h>

LT_BEGIN_DECLS

/**
 * LT_EXT_MODULE_VERSION:
 *
 * Static variable for the module version. this is used to ensure if the built
 * module is compatible with the runtime library.
 */
#define LT_EXT_MODULE_VERSION		1

/**
 * lt_ext_module_t:
 *
 * All the fields in the <structname>lt_ext_module_t</structname>
 * structure are private to the #lt_ext_module_t implementation.
 */
typedef struct _lt_ext_module_t		lt_ext_module_t;
typedef struct _lt_ext_module_funcs_t	lt_ext_module_funcs_t;

/**
 * lt_ext_module_version_func_t:
 *
 * The type of the module_get_version() that is required to implement
 * an extension module.
 *
 * Returns: a version number. this is the same to #LT_EXT_MODULE_VERSION
 *          when the module was built.
 */
typedef int                          (* lt_ext_module_version_func_t)   (void);
/**
 * lt_ext_module_get_funcs_func_t:
 *
 * The type of the module_get_funcs() that is required to implement
 * an extension module.
 *
 * Returns: (transfer none): a #lt_ext_module_funcs_t.
 */
typedef const lt_ext_module_funcs_t * (* lt_ext_module_get_funcs_func_t) (void);

/**
 * lt_ext_module_singleton_func_t:
 *
 * The type of the callback function used to obtain a singleton character
 * for Extension subtag that the module would support.
 *
 * Returns: a singleton character.
 */
typedef char                  (* lt_ext_module_singleton_func_t) (void);
/**
 * lt_ext_module_data_new_func_t:
 *
 * The type of the callback function used to create a new instance of
 * #lt_ext_module_data_t.
 * This is invoked when new Extension subtag appears and keep data.
 *
 * Returns: a new instance of #lt_ext_module_data_t.
 */
typedef lt_ext_module_data_t * (* lt_ext_module_data_new_func_t)  (void);
/**
 * lt_ext_module_precheck_func_t:
 * @data: a #lt_ext_module_data_t.
 * @tag: a #lt_tag_t.
 * @error: (allow-none): a #lt_error_t.
 *
 * The type of the callback function used to check @tag prior to process
 * parsing subtags for the extension.
 *
 * Returns: %TRUE if @tag is valid to process parsing subtags for the extension.
 *          otherwise %FALSE.
 */
typedef lt_bool_t (* lt_ext_module_precheck_func_t)  (lt_ext_module_data_t  *data,
						      const lt_tag_t        *tag,
						      lt_error_t           **error);
/**
 * lt_ext_module_parse_func_t:
 * @data: a #lt_ext_module_data_t.
 * @subtag: a subtag string to parse.
 * @error: (allow-none): a #lt_error_t.
 *
 * The type of the callback function used to parse tags.
 *
 * Returns: %TRUE if the @subtag is valid for Extension. otherwise %FALSE.
 */
typedef lt_bool_t (* lt_ext_module_parse_func_t)     (lt_ext_module_data_t  *data,
						      const char            *subtag,
						      lt_error_t           **error);
/**
 * lt_ext_module_get_tag_func_t:
 * @data: a #lt_ext_module_data_t.
 *
 * The type of the callback function used to obtain the tag.
 *
 * Returns: a tag string.
 */
typedef char * (* lt_ext_module_get_tag_func_t)   (lt_ext_module_data_t *data);
/**
 * lt_ext_module_validate_func_t:
 * @data: a #lt_ext_module_data_t.
 *
 * The type of the callback function used to validate the tags in @data.
 *
 * Returns: %TRUE if it's valid, otherwise %FALSE.
 */
typedef lt_bool_t (* lt_ext_module_validate_func_t)  (lt_ext_module_data_t *data);

/**
 * lt_ext_module_funcs_t:
 * @get_singleton: A callback function to obtain the singleton character
 *                 that are supposed in the module.
 * @create_data: A callback function to create a new instance of
 *               #lt_ext_module_data_t for the module.
 * @precheck_tag: A callback function to check tags prior to parse subtags.
 * @parse_tag: A callback function to parse a tag.
 * @get_tag: A callback function to obtain the tag string.
 * @validate_tag: A callback function to validate the tag.
 *
 * The <structname>lt_ext_module_funcs_t</structname> struct is a callback
 * collection to provide an accessor between #lt_extension_t
 * and #lt_ext_module_t and extend features.
 */
struct _lt_ext_module_funcs_t {
	const lt_ext_module_singleton_func_t get_singleton;
	const lt_ext_module_data_new_func_t  create_data;
	const lt_ext_module_precheck_func_t  precheck_tag;
	const lt_ext_module_parse_func_t     parse_tag;
	const lt_ext_module_get_tag_func_t   get_tag;
	const lt_ext_module_validate_func_t  validate_tag;
};


void             lt_ext_modules_load  (void);
void             lt_ext_modules_unload(void);
lt_ext_module_t *lt_ext_module_ref    (lt_ext_module_t *module);
void             lt_ext_module_unref  (lt_ext_module_t *module);

#ifdef LT_MODULE_PREFIX
#  define LT_MODULE_SYMBOL(__sym__)	LT_MODULE_SYMBOL_(LT_MODULE_PREFIX, __sym__)
#  define LT_MODULE_SYMBOL_(__prefix__,__sym__)	__prefix__##_module_##__sym__
#else
#  define LT_MODULE_SYMBOL(__sym__)	module_##__sym__
#endif

/**
 * module_get_version:
 *
 * Obtains the module version. this must be implemented in a module.
 *
 * Returns: a version number. this is the same to #LT_EXT_MODULE_VERSION
 *          when the module was built.
 */
int                          module_get_version(void);
/**
 * module_get_funcs:
 *
 * Obtains a #lt_ext_module_funcs_t, callback collection structure that
 * the module would process. this must be implemented in the external module.
 *
 * Returns: a #lt_ext_module_funcs_t.
 */
const lt_ext_module_funcs_t *module_get_funcs  (void);

LT_END_DECLS

#endif /* __LT_EXT_MODULE_H__ */
