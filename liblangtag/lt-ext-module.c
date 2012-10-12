/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-module.c
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

#include <ctype.h>
#include <dirent.h>
#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-ext-module-data.h"
#include "lt-ext-module.h"
#include "lt-ext-module-private.h"
#include "lt-utils.h"


/**
 * SECTION: lt-ext-module
 * @Short_Description: A module class to extend features in #lt_extension_t.
 * @Title: Module - Accessor
 *
 * This class provides functionality to extend features in #lt_extension_t,
 * such as validating tags more strictly.
 */
struct _lt_ext_module_t {
	lt_mem_t                     parent;
	char                        *name;
	lt_pointer_t                 module;
	const lt_ext_module_funcs_t *funcs;
};

typedef struct _lt_ext_default_data_t {
	lt_ext_module_data_t  parent;
	lt_string_t          *tags;
} lt_ext_default_data_t;

static lt_ext_module_data_t *_lt_ext_default_create_data (void);
static lt_bool_t             _lt_ext_default_precheck_tag(lt_ext_module_data_t  *data,
							  const lt_tag_t        *tag,
							  lt_error_t           **error);
static lt_bool_t             _lt_ext_default_parse_tag   (lt_ext_module_data_t  *data,
							  const char            *subtag,
							  lt_error_t           **error);
static char                 *_lt_ext_default_get_tag     (lt_ext_module_data_t  *data);
static lt_bool_t             _lt_ext_default_validate_tag(lt_ext_module_data_t  *data);
static lt_ext_module_data_t *_lt_ext_eaw_create_data     (void);
static lt_bool_t             _lt_ext_eaw_precheck_tag    (lt_ext_module_data_t  *data,
							  const lt_tag_t        *tag,
							  lt_error_t           **error);
static lt_bool_t             _lt_ext_eaw_parse_tag       (lt_ext_module_data_t  *data,
							  const char            *subtag,
							  lt_error_t           **error);
static char                 *_lt_ext_eaw_get_tag         (lt_ext_module_data_t  *data);
static lt_bool_t             _lt_ext_eaw_validate_tag    (lt_ext_module_data_t  *data);

#ifndef ENABLE_MODULE
extern const lt_ext_module_funcs_t *LT_MODULE_SYMBOL_ (lt_module_ext_t, get_funcs) (void);
extern const lt_ext_module_funcs_t *LT_MODULE_SYMBOL_ (lt_module_ext_u, get_funcs) (void);
#endif

static lt_ext_module_t *__lt_ext_modules[LT_MAX_EXT_MODULES + 1];
static lt_ext_module_t *__lt_ext_default_handler;
static lt_bool_t __lt_ext_module_initialized = FALSE;
static const lt_ext_module_funcs_t __default_funcs = {
	NULL,
	_lt_ext_default_create_data,
	_lt_ext_default_precheck_tag,
	_lt_ext_default_parse_tag,
	_lt_ext_default_get_tag,
	_lt_ext_default_validate_tag,
};
static const lt_ext_module_funcs_t __empty_and_wildcard_funcs = {
	NULL,
	_lt_ext_eaw_create_data,
	_lt_ext_eaw_precheck_tag,
	_lt_ext_eaw_parse_tag,
	_lt_ext_eaw_get_tag,
	_lt_ext_eaw_validate_tag,
};

/*< private >*/
static void
_lt_ext_default_destroy_data(lt_pointer_t data)
{
	lt_ext_default_data_t *d = data;

	lt_string_unref(d->tags);
}

static lt_ext_module_data_t *
_lt_ext_default_create_data(void)
{
	lt_ext_module_data_t *retval = lt_ext_module_data_new(sizeof (lt_ext_default_data_t),
							      _lt_ext_default_destroy_data);

	if (retval) {
		lt_ext_default_data_t *data = (lt_ext_default_data_t *)retval;

		data->tags = lt_string_new(NULL);
	}

	return retval;
}

static lt_bool_t
_lt_ext_default_precheck_tag(lt_ext_module_data_t  *data,
			     const lt_tag_t        *tag,
			     lt_error_t           **error)
{
	return TRUE;
}

static lt_bool_t
_lt_ext_default_parse_tag(lt_ext_module_data_t  *data,
			  const char            *subtag,
			  lt_error_t           **error)
{
	lt_ext_default_data_t *d = (lt_ext_default_data_t *)data;

	if (lt_string_length(d->tags) > 0)
		lt_string_append_printf(d->tags, "-%s", subtag);
	else
		lt_string_append(d->tags, subtag);

	return TRUE;
}

static char *
_lt_ext_default_get_tag(lt_ext_module_data_t *data)
{
	lt_ext_default_data_t *d = (lt_ext_default_data_t *)data;

	return strdup(lt_string_value(d->tags));
}

static lt_bool_t
_lt_ext_default_validate_tag(lt_ext_module_data_t *data)
{
	return TRUE;
}

static void
_lt_ext_eaw_destroy_data(lt_pointer_t data)
{
}

static lt_ext_module_data_t *
_lt_ext_eaw_create_data(void)
{
	lt_ext_module_data_t *retval = lt_ext_module_data_new(sizeof (lt_ext_module_data_t),
							      _lt_ext_eaw_destroy_data);

	return retval;
}

static lt_bool_t
_lt_ext_eaw_precheck_tag(lt_ext_module_data_t  *data,
			 const lt_tag_t        *tag,
			 lt_error_t           **error)
{
	/* not allowed to process any extensions */

	return FALSE;
}

static lt_bool_t
_lt_ext_eaw_parse_tag(lt_ext_module_data_t  *data,
		      const char            *subtag,
		      lt_error_t           **error)
{
	/* not allowed to add any tags */

	return FALSE;
}

static char *
_lt_ext_eaw_get_tag(lt_ext_module_data_t *data)
{
	return strdup("");
}

static lt_bool_t
_lt_ext_eaw_validate_tag(lt_ext_module_data_t *data)
{
	return TRUE;
}

#if ENABLE_MODULE
static lt_bool_t
lt_ext_module_load(lt_ext_module_t *module)
{
	lt_bool_t retval = FALSE;
	lt_string_t *fullname = lt_string_new(NULL);
	char *filename = lt_strdup_printf("liblangtag-ext-%s." LT_MODULE_SUFFIX,
					  module->name);
	char *path_list, *p, *s, *path;
	const char *env = getenv("LANGTAG_EXT_MODULE_PATH");
	size_t len;

	if (!env) {
		path_list = strdup(
#ifdef GNOME_ENABLE_DEBUG
			BUILDDIR LT_DIR_SEPARATOR_S "extensions" LT_SEARCHPATH_SEPARATOR_S
			BUILDDIR LT_DIR_SEPARATOR_S "extensions" LT_DIR_SEPARATOR_S ".libs" LT_SEARCHPATH_SEPARATOR_S
#endif
			LANGTAG_EXT_MODULE_PATH);
	} else {
		path_list = strdup(env);
	}

	s = path_list;
	do {
		if (!s)
			break;
		p = strchr(s, LT_SEARCHPATH_SEPARATOR);
		if (p == s) {
			s++;
			continue;
		}
		path = s;
		if (p) {
			*p = 0;
			p++;
		}
		s = p;
		while (*path && isspace(*path))
			path++;
		len = strlen(path);
		while (len > 0 && isspace(path[len - 1]))
			len--;
		path[len] = 0;
		if (path[0] != 0) {
			lt_string_clear(fullname);
			if (!lt_string_append_filename(fullname, path, filename, NULL)) {
				lt_critical("Unable to allocate a memory");
				break;
			}
			module->module = dlopen(lt_string_value(fullname),
						RTLD_LAZY|RTLD_LOCAL);
			if (module->module) {
				lt_pointer_t func;

				lt_mem_add_ref(&module->parent, module->module,
					       (lt_destroy_func_t)dlclose);
				func = dlsym(module->module, "module_get_version");
				if (!func) {
					lt_warning(dlerror());
					break;
				}
				if (((lt_ext_module_version_func_t)func)() != LT_EXT_MODULE_VERSION) {
					lt_warning("`%s' isn't satisfied the required module version.",
						   filename);
					break;
				}
				func = dlsym(module->module, "module_get_funcs");
				if (!func) {
					lt_warning(dlerror());
					break;
				}
				if (!(module->funcs = ((lt_ext_module_get_funcs_func_t)func)())) {
					lt_warning("No function table for `%s'",
						   filename);
					break;
				}
				lt_debug(LT_MSGCAT_MODULE,
					 "Loading the external extension handler module: %s",
					 lt_string_value(fullname));
				retval = TRUE;
			}
		}
	} while (1);
	if (!retval)
		lt_warning("No such modules: %s", module->name);

	lt_string_unref(fullname);
	free(filename);
	free(path_list);

	return retval;
}
#endif /* ENABLE_MODULE */

static lt_ext_module_t *
lt_ext_module_new_with_data(const char                  *name,
			    const lt_ext_module_funcs_t *funcs)
{
	lt_ext_module_t *retval;

	lt_return_val_if_fail (name != NULL, NULL);
	lt_return_val_if_fail (funcs != NULL, NULL);

	retval = lt_mem_alloc_object(sizeof (lt_ext_module_t));
	if (retval) {
		retval->name = strdup(name);
		lt_mem_add_ref(&retval->parent, retval->name,
			       (lt_destroy_func_t)free);
		retval->funcs = funcs;

		lt_debug(LT_MSGCAT_MODULE, "Loading the internal extension handler: %s", name);
	}

	return retval;
}

/*< protected >*/
lt_bool_t
lt_ext_module_validate_singleton(char singleton)
{
	return (singleton >= '0' && singleton <= '9') ||
		(singleton >= 'A' && singleton <= 'W') ||
		(singleton >= 'Y' && singleton <= 'Z') ||
		(singleton >= 'a' && singleton <= 'w') ||
		(singleton >= 'y' && singleton <= 'z') ||
		singleton == ' ' ||
		singleton == '*';
}

int
lt_ext_module_singleton_char_to_int(char singleton_c)
{
	int retval = -1;

	if (!lt_ext_module_validate_singleton(singleton_c))
		fprintf(stderr, "XXXXXXXXx: %c\n", singleton_c);
	lt_return_val_if_fail (lt_ext_module_validate_singleton(singleton_c), -1);

	if (singleton_c >= '0' && singleton_c <= '9') {
		retval = singleton_c - '0';
	} else if ((singleton_c >= 'a' && singleton_c <= 'z') ||
		   (singleton_c >= 'A' && singleton_c <= 'Z')) {
		retval = tolower(singleton_c) - 'a' + 10;
	} else if (singleton_c == ' ') {
		retval = LT_MAX_EXT_MODULES - 2;
	} else if (singleton_c == '*') {
		retval = LT_MAX_EXT_MODULES - 1;
	}

	return retval;
}

char
lt_ext_module_singleton_int_to_char(int singleton)
{
	char retval;

	lt_return_val_if_fail (singleton >= 0, 0);
	lt_return_val_if_fail (singleton < LT_MAX_EXT_MODULES, 0);

	if ((singleton - 10) < 0)
		retval = singleton + '0';
	else if (singleton == (LT_MAX_EXT_MODULES - 2))
		retval = ' ';
	else if (singleton == LT_MAX_EXT_MODULES - 1)
		retval = '*';
	else
		retval = singleton - 10 + 'a';

	return retval;
}

lt_ext_module_t *
lt_ext_module_new(const char *name)
{
	lt_ext_module_t *retval = NULL;

	lt_return_val_if_fail (name != NULL, NULL);

#ifdef ENABLE_MODULE
	retval = lt_mem_alloc_object(sizeof (lt_ext_module_t));

	if (retval) {
		char *n = strdup(name);
		char *filename = basename(n), *module = NULL;
		static const char *prefix = "liblangtag-ext-";
		static size_t prefix_len = 0;
		char singleton_c;
		int singleton;

		if (prefix_len == 0)
			prefix_len = strlen(prefix);

		if (strncmp(filename, prefix, prefix_len) == 0) {
			size_t len = strlen(&filename[prefix_len]);
			size_t suffix_len = strlen(LT_MODULE_SUFFIX) + 1;

			if (len > suffix_len &&
			    lt_strcmp0(&filename[prefix_len + len - suffix_len], "." LT_MODULE_SUFFIX) == 0) {
				module = strndup(&filename[prefix_len], len - suffix_len);
				module[len - suffix_len] = 0;
			}
		}
		if (!module)
			module = strdup(filename);
		retval->name = module;
		lt_mem_add_ref(&retval->parent, retval->name,
			       (lt_destroy_func_t)free);

		free(n);

		if (!lt_ext_module_load(retval)) {
			lt_ext_module_unref(retval);
			return NULL;
		}
		singleton_c = lt_ext_module_get_singleton(retval);
		if (singleton_c == ' ' ||
		    singleton_c == '*') {
			lt_warning("Not allowed to override the internal handlers for special singleton.");
			lt_ext_module_unref(retval);
			return NULL;
		}
		singleton = lt_ext_module_singleton_char_to_int(singleton_c);
		if (singleton < 0) {
			lt_warning("Invalid singleton: `%c' - `%s'",
				   singleton_c, 
				   retval->name);
			lt_ext_module_unref(retval);
			return NULL;
		}
		if (__lt_ext_modules[singleton]) {
			lt_warning("Duplicate extension module: %s",
				  retval->name);
			lt_ext_module_unref(retval);
			return NULL;
		}
		__lt_ext_modules[singleton] = retval;
		lt_mem_add_weak_pointer(&retval->parent,
					(lt_pointer_t *)&__lt_ext_modules[singleton]);
	}
#endif /* ENABLE_MODULE */

	return retval;
}

lt_ext_module_t *
lt_ext_module_lookup(char singleton_c)
{
	int singleton = lt_ext_module_singleton_char_to_int(singleton_c);

	lt_return_val_if_fail (singleton >= 0, NULL);
	lt_return_val_if_fail (__lt_ext_module_initialized, NULL);

	if (!__lt_ext_modules[singleton])
		return lt_ext_module_ref(__lt_ext_default_handler);

	return lt_ext_module_ref(__lt_ext_modules[singleton]);
}

const char *
lt_ext_module_get_name(lt_ext_module_t *module)
{
	lt_return_val_if_fail (module != NULL, NULL);

	return module->name;
}

char
lt_ext_module_get_singleton(lt_ext_module_t *module)
{
	lt_return_val_if_fail (module != NULL, 0);
	lt_return_val_if_fail (module->funcs != NULL, 0);
	lt_return_val_if_fail (module->funcs->get_singleton != NULL, 0);

	return module->funcs->get_singleton();
}

lt_ext_module_data_t *
lt_ext_module_create_data(lt_ext_module_t *module)
{
	lt_return_val_if_fail (module != NULL, NULL);
	lt_return_val_if_fail (module->funcs != NULL, NULL);
	lt_return_val_if_fail (module->funcs->create_data != NULL, NULL);

	return module->funcs->create_data();
}

lt_bool_t
lt_ext_module_parse_tag(lt_ext_module_t       *module,
			lt_ext_module_data_t  *data,
			const char            *subtag,
			lt_error_t           **error)
{
	lt_return_val_if_fail (module != NULL, FALSE);
	lt_return_val_if_fail (data != NULL, FALSE);
	lt_return_val_if_fail (subtag != NULL, FALSE);
	lt_return_val_if_fail (module->funcs != NULL, FALSE);
	lt_return_val_if_fail (module->funcs->parse_tag != NULL, FALSE);

	return module->funcs->parse_tag(data, subtag, error);
}

char *
lt_ext_module_get_tag(lt_ext_module_t      *module,
		      lt_ext_module_data_t *data)
{
	lt_return_val_if_fail (module != NULL, NULL);
	lt_return_val_if_fail (data != NULL, NULL);
	lt_return_val_if_fail (module->funcs != NULL, NULL);
	lt_return_val_if_fail (module->funcs->get_tag != NULL, NULL);

	return module->funcs->get_tag(data);
}

lt_bool_t
lt_ext_module_validate_tag(lt_ext_module_t      *module,
			   lt_ext_module_data_t *data)
{
	lt_return_val_if_fail (module != NULL, FALSE);
	lt_return_val_if_fail (data != NULL, FALSE);
	lt_return_val_if_fail (module->funcs != NULL, FALSE);
	lt_return_val_if_fail (module->funcs->validate_tag != NULL, FALSE);

	return module->funcs->validate_tag(data);
}

lt_bool_t
lt_ext_module_precheck_tag(lt_ext_module_t       *module,
			   lt_ext_module_data_t  *data,
			   const lt_tag_t        *tag,
			   lt_error_t           **error)
{
	lt_error_t *err = NULL;
	lt_bool_t retval;

	lt_return_val_if_fail (module != NULL, FALSE);
	lt_return_val_if_fail (data != NULL, FALSE);
	lt_return_val_if_fail (module->funcs != NULL, FALSE);
	lt_return_val_if_fail (module->funcs->precheck_tag != NULL, FALSE);

	retval = module->funcs->precheck_tag(data, tag, &err);
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

/*< public >*/
/**
 * lt_ext_modules_load:
 *
 * Load all of the modules on the system, including the internal accessor.
 * This has to be invoked before processing something with #lt_extension_t.
 * or lt_db_initialize() does.
 */
void
lt_ext_modules_load(void)
{
#ifdef ENABLE_MODULE
	const char *env = getenv("LANGTAG_EXT_MODULE_PATH");
	char *path_list, *s, *p, *path;
	size_t suffix_len = strlen(LT_MODULE_SUFFIX) + 1;

	if (__lt_ext_module_initialized)
		return;
	if (!env) {
		path_list = strdup(
#ifdef GNOME_ENABLE_DEBUG
			BUILDDIR LT_DIR_SEPARATOR_S "extensions" LT_SEARCHPATH_SEPARATOR_S
			BUILDDIR LT_DIR_SEPARATOR_S "extensions" LT_DIR_SEPARATOR_S ".libs" LT_SEARCHPATH_SEPARATOR_S
#endif
			LANGTAG_EXT_MODULE_PATH);
	} else {
		path_list = strdup(env);
	}
	s = path_list;
	do {
		DIR *dir;

		if (!s)
			break;
		p = strchr(s, LT_SEARCHPATH_SEPARATOR);
		if (s == p) {
			s++;
			continue;
		}
		path = s;
		if (p) {
			*p = 0;
			p++;
		}
		s = p;

		dir = opendir(path);
		if (dir) {
			struct dirent dent, *dresult;
			size_t len;

			while (1) {
				if (readdir_r(dir, &dent, &dresult) || dresult == NULL)
					break;

				len = strlen(dent.d_name);
				if (len > suffix_len &&
				    lt_strcmp0(&dent.d_name[len - suffix_len],
					       "." LT_MODULE_SUFFIX) == 0) {
					lt_ext_module_new(dent.d_name);
				}
			}
			closedir(dir);
		}
	} while (1);

	free(path_list);
#else /* !ENABLE_MODULE */
	const lt_ext_module_funcs_t *f;
	int c;

#define REGISTER(_ext_)							\
	f = LT_MODULE_SYMBOL_ (lt_module_ext_##_ext_, get_funcs) ();	\
	c = lt_ext_module_singleton_char_to_int(f->get_singleton());	\
	__lt_ext_modules[c] = lt_ext_module_new_with_data(#_ext_, f);	\
	lt_mem_add_weak_pointer(&__lt_ext_modules[c]->parent,		\
				(lt_pointer_t *)&__lt_ext_modules[c]);

	REGISTER (t);
	REGISTER (u);

#undef REGISTER
#endif /* ENABLE_MODULE */
	__lt_ext_default_handler = lt_ext_module_new_with_data("default",
							       &__default_funcs);
	lt_mem_add_weak_pointer(&__lt_ext_default_handler->parent,
				(lt_pointer_t *)&__lt_ext_default_handler);
	__lt_ext_modules[LT_MAX_EXT_MODULES - 2] = lt_ext_module_new_with_data("empty",
									       &__empty_and_wildcard_funcs);
	lt_mem_add_weak_pointer(&__lt_ext_modules[LT_MAX_EXT_MODULES - 2]->parent,
				(lt_pointer_t *)&__lt_ext_modules[LT_MAX_EXT_MODULES - 2]);
	__lt_ext_modules[LT_MAX_EXT_MODULES - 1] = lt_ext_module_new_with_data("wildcard",
									   &__empty_and_wildcard_funcs);
	lt_mem_add_weak_pointer(&__lt_ext_modules[LT_MAX_EXT_MODULES - 1]->parent,
				(lt_pointer_t *)&__lt_ext_modules[LT_MAX_EXT_MODULES - 1]);
	__lt_ext_module_initialized = TRUE;
}

/**
 * lt_ext_modules_unload:
 *
 * Unload all of the modules already loaded.
 */
void
lt_ext_modules_unload(void)
{
	int i;

	if (!__lt_ext_module_initialized)
		return;
	for (i = 0; i < LT_MAX_EXT_MODULES; i++) {
		if (__lt_ext_modules[i])
			lt_ext_module_unref(__lt_ext_modules[i]);
	}
	lt_ext_module_unref(__lt_ext_default_handler);
	__lt_ext_module_initialized = FALSE;
}

/**
 * lt_ext_module_ref:
 * @module: a #lt_ext_module_t.
 *
 * Increases the reference count of @module.
 *
 * Returns: (transfer none): the same @module object.
 */
lt_ext_module_t *
lt_ext_module_ref(lt_ext_module_t *module)
{
	lt_return_val_if_fail (module != NULL, NULL);

	return lt_mem_ref(&module->parent);
}

/**
 * lt_ext_module_unref:
 * @module: a #lt_ext_module_t.
 *
 * Decreases the reference count of @module. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_ext_module_unref(lt_ext_module_t *module)
{
	if (module)
		lt_mem_unref(&module->parent);
}
