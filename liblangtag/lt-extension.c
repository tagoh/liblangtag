/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extension.c
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include "lt-error.h"
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-utils.h"
#include "lt-ext-module-private.h"
#include "lt-extension.h"
#include "lt-extension-private.h"


/**
 * SECTION: lt-extension
 * @Short_Description: A container class for Extension subtag
 * @Title: Container - Extension
 *
 * This container class provides a data access to Extension subtag entry.
 */
struct _lt_extension_t {
	lt_mem_t              parent;
	lt_string_t          *cached_tag;
	lt_ext_module_t      *module;
	int                   singleton;
	lt_ext_module_data_t *extensions[LT_MAX_EXT_MODULES + 1];
};

/*< private >*/

/*< protected >*/
lt_extension_t *
lt_extension_create(void)
{
	lt_extension_t *retval = lt_mem_alloc_object(sizeof (lt_extension_t));

	if (retval) {
		retval->cached_tag = lt_string_new(NULL);
		lt_mem_add_ref(&retval->parent, retval->cached_tag,
			       (lt_destroy_func_t)lt_string_unref);
	}

	return retval;
}

lt_bool_t
lt_extension_has_singleton(lt_extension_t *extension,
			   char            singleton_c)
{
	int singleton;

	lt_return_val_if_fail (extension != NULL, FALSE);

	singleton = lt_ext_module_singleton_char_to_int(singleton_c);
	if (singleton < 0)
		return FALSE;

	return extension->extensions[singleton] != NULL;
}

lt_bool_t
lt_extension_add_singleton(lt_extension_t  *extension,
			   char             singleton_c,
			   const lt_tag_t  *tag,
			   lt_error_t     **error)
{
	int singleton = lt_ext_module_singleton_char_to_int(singleton_c);
	lt_ext_module_t *m;
	lt_ext_module_data_t *d;
	lt_error_t *err = NULL;

	lt_return_val_if_fail (extension != NULL, FALSE);
	lt_return_val_if_fail (singleton_c != 'X' && singleton_c != 'x', FALSE);
	lt_return_val_if_fail (!lt_extension_has_singleton(extension, singleton_c), FALSE);
	lt_return_val_if_fail (singleton >= 0, FALSE);

	m = lt_ext_module_lookup(singleton_c);
	d = lt_ext_module_create_data(m);
	if (!d) {
		lt_ext_module_unref(m);
		lt_error_set(&err, LT_ERR_OOM,
			     "Unable to create an instance of lt_ext_module_data_t.");

		goto bail;
	}
	if (tag && !lt_ext_module_precheck_tag(m, d, tag, &err)) {
		lt_ext_module_data_unref(d);
		lt_ext_module_unref(m);

		goto bail;
	}
	if (extension->module)
		lt_mem_delete_ref(&extension->parent, extension->module);
	extension->module = m;
	lt_mem_add_ref(&extension->parent, extension->module,
		       (lt_destroy_func_t)lt_ext_module_unref);
	extension->extensions[singleton] = d;
	lt_mem_add_ref(&extension->parent, extension->extensions[singleton],
		       (lt_destroy_func_t)lt_ext_module_data_unref);
	extension->singleton = singleton;

	if (lt_string_length(extension->cached_tag) > 0)
		lt_string_append_printf(extension->cached_tag, "-%c", singleton_c);
	else
		lt_string_append_c(extension->cached_tag, singleton_c);

  bail:
	if (lt_error_is_set(err, LT_ERR_ANY)) {
		if (error)
			*error = lt_error_ref(err);
		else
			lt_error_print(err, LT_ERR_ANY);
		lt_error_unref(err);

		return FALSE;
	}

	return TRUE;
}

lt_bool_t
lt_extension_add_tag(lt_extension_t  *extension,
		     const char      *subtag,
		     lt_error_t     **error)
{
	lt_bool_t retval;
	lt_error_t *err = NULL;

	lt_return_val_if_fail (extension != NULL, FALSE);
	lt_return_val_if_fail (subtag != NULL, FALSE);
	lt_return_val_if_fail (extension->module != NULL, FALSE);
	lt_return_val_if_fail (extension->extensions[extension->singleton] != NULL, FALSE);

	retval = lt_ext_module_parse_tag(extension->module,
					 extension->extensions[extension->singleton],
					 subtag,
					 &err);
	if (retval)
		lt_string_append_printf(extension->cached_tag, "-%s", subtag);

	if (lt_error_is_set(err, LT_ERR_ANY)) {
		if (error)
			*error = lt_error_ref(err);
		else
			lt_error_print(err, LT_ERR_ANY);
		lt_error_unref(err);
		retval = FALSE;
	}

	return retval;
}

void
lt_extension_cancel_tag(lt_extension_t *extension)
{
	lt_return_if_fail (extension != NULL);

	if (extension->module && extension->extensions[extension->singleton]) {
		char *tags, singleton[4], *p, *lastp;
		lt_list_t *l = NULL, *ll;

		lt_mem_delete_ref(&extension->parent, extension->module);
		extension->module = NULL;
		lt_mem_delete_ref(&extension->parent, extension->extensions[extension->singleton]);
		extension->extensions[extension->singleton] = NULL;

		lastp = p = tags = strdup(lt_string_value(extension->cached_tag));
		while (p) {
			p = strchr(p, '-');
			if (p) {
				*p = 0;
				p++;
			}
			l = lt_list_append(l, lastp, NULL);
			lastp = p;
		}
		singleton[0] = lt_ext_module_singleton_int_to_char(extension->singleton);
		singleton[1] = 0;
		lt_string_clear(extension->cached_tag);
		for (ll = l; ll != NULL; ll = lt_list_next(ll)) {
			if (lt_strcmp0(lt_list_value(ll), singleton) == 0) {
				if (ll == l)
					l = NULL;
				lt_list_free(ll);
				break;
			}
			if (lt_string_length(extension->cached_tag) > 0)
				lt_string_append_printf(extension->cached_tag,
							"-%s", (char *)lt_list_value(ll));
			else
				lt_string_append(extension->cached_tag, lt_list_value(ll));
		}
		lt_list_free(l);
		if (tags)
			free(tags);
	}
}

lt_extension_t *
lt_extension_copy(lt_extension_t *extension)
{
	lt_extension_t *retval;
	int i;

	lt_return_val_if_fail (extension != NULL, NULL);

	retval = lt_extension_create();
	if (retval) {
		lt_string_append(retval->cached_tag, lt_string_value(extension->cached_tag));
		if (extension->module) {
			retval->module = lt_ext_module_ref(extension->module);
			lt_mem_add_ref(&retval->parent, retval->module,
				       (lt_destroy_func_t)lt_ext_module_unref);
			retval->singleton = extension->singleton;
			for (i = 0; i < LT_MAX_EXT_MODULES; i++) {
				if (extension->extensions[i]) {
					retval->extensions[i] = lt_ext_module_data_ref(extension->extensions[i]);
					lt_mem_add_ref(&retval->parent, retval->extensions[i],
						       (lt_destroy_func_t)lt_ext_module_data_unref);
				}
			}
		}
	}

	return retval;
}

lt_bool_t
lt_extension_validate_state(lt_extension_t *extension)
{
	lt_bool_t retval = TRUE;

	lt_return_val_if_fail (extension != NULL, FALSE);

	if (extension->module) {
		retval = lt_ext_module_validate_tag(extension->module,
						    extension->extensions[extension->singleton]);
	}

	return retval;
}

/*< public >*/
/**
 * lt_extension_ref:
 * @extension: a #lt_extension_t.
 *
 * Increases the reference count of @extension.
 *
 * Returns: (transfer none): the same @extension object.
 */
lt_extension_t *
lt_extension_ref(lt_extension_t *extension)
{
	lt_return_val_if_fail (extension != NULL, NULL);

	return lt_mem_ref(&extension->parent);
}

/**
 * lt_extension_unref:
 * @extension: a #lt_extension_t.
 *
 * Decreases the reference count of @extension. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_extension_unref(lt_extension_t *extension)
{
	if (extension)
		lt_mem_unref(&extension->parent);
}

/**
 * lt_extension_get_tag:
 * @extension: a #lt_extension_t.
 *
 * Obtain the tag string.
 *
 * Returns: the tag string.
 */
const char *
lt_extension_get_tag(lt_extension_t *extension)
{
	lt_return_val_if_fail (extension != NULL, NULL);

	return lt_string_value(extension->cached_tag);
}

/**
 * lt_extension_get_canonicalized_tag:
 * @extension: a #lt_extension_t.
 *
 * Generate a canonicalized tag.
 *
 * Returns: a string. this must be freed.
 */
char *
lt_extension_get_canonicalized_tag(lt_extension_t *extension)
{
	lt_string_t *string;
	int i;
	char c, *s;
	lt_ext_module_t *m;

	lt_return_val_if_fail (extension != NULL, NULL);

	string = lt_string_new(NULL);

	for (i = 0; i < LT_MAX_EXT_MODULES; i++) {
		if (extension->extensions[i]) {
			if (lt_string_length(string) > 0)
				lt_string_append_c(string, '-');
			c = lt_ext_module_singleton_int_to_char(i);
			lt_string_append_c(string, c);
			if (c != ' ' && c != '*') {
				m = lt_ext_module_lookup(c);
				if (m) {
					s = lt_ext_module_get_tag(m, extension->extensions[i]);
					lt_string_append_printf(string, "-%s", s);
					free(s);
					lt_ext_module_unref(m);
				} else {
					lt_warning("Unable to obtain the certain module instance: singleton = '%c", c);
					break;
				}
			}
		}
	}

	return lt_string_free(string, FALSE);
}

/**
 * lt_extension_dump:
 * @extension: a #lt_extension_t.
 *
 * Dumps the container information to the standard output.
 */
void
lt_extension_dump(lt_extension_t *extension)
{
	int i;
	char c;
	char *s;
	lt_ext_module_t *m;

	lt_return_if_fail (extension != NULL);

	lt_info("Extensions:");
	for (i = 0; i < LT_MAX_EXT_MODULES; i++) {
		if (extension->extensions[i]) {
			c = lt_ext_module_singleton_int_to_char(i);
			if (c == ' ') {
				lt_info(" '' [empty]");
			} else if (c == '*') {
				lt_info(" '*' [wildcard]");
			} else {
				m = lt_ext_module_lookup(c);
				if (m) {
					s = lt_ext_module_get_tag(m, extension->extensions[i]);
					lt_info("  %c-%s", c, s);
					free(s);
					lt_ext_module_unref(m);
				} else {
					lt_warning("  [failed to obtain the module instance: singleton = '%c', data = %p]",
						   c, extension->extensions[i]);
				}
			}
		}
	}
}

/**
 * lt_extension_compare:
 * @v1: a #lt_extension_t.
 * @v2: a #lt_extension_t.
 *
 * Compare if @v1 and @v2 is the same object or not.
 *
 * Returns: %TRUE if it's the same, otherwise %FALSE.
 */
lt_bool_t
lt_extension_compare(const lt_extension_t *v1,
		     const lt_extension_t *v2)
{
	int i;
	char *s1 = NULL, *s2 = NULL;
	lt_ext_module_t *m = NULL;
	lt_bool_t retval = TRUE;

	if (v1 == v2)
		return TRUE;

	if ((v1 && v1->extensions[LT_MAX_EXT_MODULES - 1]) ||
	    (v2 && v2->extensions[LT_MAX_EXT_MODULES - 1]))
		return TRUE;

	if ((!v1 && v2) ||
	    (v1 && !v2))
		return FALSE;

	for (i = 0; i < LT_MAX_EXT_MODULES - 2; i++) {
		if ((!v1->extensions[i] && v2->extensions[i]) ||
		    (v1->extensions[i] && !v2->extensions[i])) {
			retval = FALSE;
			break;
		}

		if (m)
			lt_ext_module_unref(m);
		m = lt_ext_module_lookup(lt_ext_module_singleton_int_to_char(i));
		if (s1)
			free(s1);
		if (s2)
			free(s2);
		s1 = lt_ext_module_get_tag(m, v1->extensions[i]);
		s2 = lt_ext_module_get_tag(m, v2->extensions[i]);
		if (lt_strcmp0(s1, s2)) {
			retval = FALSE;
			break;
		}
	}

	if (m)
		lt_ext_module_unref(m);
	if (s1)
		free(s1);
	if (s2)
		free(s2);

	return retval;
}

/**
 * lt_extension_truncate:
 * @extension: a #lt_extension_t.
 *
 * Truncate the last extension.
 *
 * Returns: %TRUE if a subtag of the extension is truncated. otherwise %FALSE.
 */
lt_bool_t
lt_extension_truncate(lt_extension_t  *extension)
{
	int i;
	lt_bool_t retval = FALSE;

	lt_return_val_if_fail (extension != NULL, FALSE);

	for (i = LT_MAX_EXT_MODULES - 1; i >= 0; i--) {
		if (extension->extensions[i]) {
			lt_mem_delete_ref(&extension->parent, extension->extensions[i]);
			extension->extensions[i] = NULL;
			retval = TRUE;
			break;
		}
	}

	return retval;
}
