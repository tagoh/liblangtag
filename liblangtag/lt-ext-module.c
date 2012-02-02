/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-module.c
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <dirent.h>
#include <string.h>
#include <glib.h>
#include <gmodule.h>
#include "lt-mem.h"
#include "lt-ext-module-data.h"
#include "lt-ext-module.h"
#include "lt-ext-module-private.h"


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
	gchar                       *name;
	GModule                     *module;
	const lt_ext_module_funcs_t *funcs;
};

typedef struct _lt_ext_default_data_t {
	lt_ext_module_data_t  parent;
	GString              *tags;
} lt_ext_default_data_t;

static lt_ext_module_data_t *_lt_ext_default_create_data (void);
static gboolean              _lt_ext_default_precheck_tag(lt_ext_module_data_t  *data,
							  const lt_tag_t        *tag,
							  GError               **error);
static gboolean              _lt_ext_default_parse_tag   (lt_ext_module_data_t  *data,
							  const gchar           *subtag,
							  GError               **error);
static gchar                *_lt_ext_default_get_tag     (lt_ext_module_data_t  *data);
static gboolean              _lt_ext_default_validate_tag(lt_ext_module_data_t  *data);
static lt_ext_module_data_t *_lt_ext_eaw_create_data     (void);
static gboolean              _lt_ext_eaw_precheck_tag    (lt_ext_module_data_t  *data,
							  const lt_tag_t        *tag,
							  GError               **error);
static gboolean              _lt_ext_eaw_parse_tag       (lt_ext_module_data_t  *data,
							  const gchar           *subtag,
							  GError               **error);
static gchar                *_lt_ext_eaw_get_tag         (lt_ext_module_data_t  *data);
static gboolean              _lt_ext_eaw_validate_tag    (lt_ext_module_data_t  *data);


static lt_ext_module_t *__lt_ext_modules[LT_MAX_EXT_MODULES + 1];
static lt_ext_module_t *__lt_ext_default_handler;
static gboolean __lt_ext_module_initialized = FALSE;
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
_lt_ext_default_destroy_data(gpointer data)
{
	lt_ext_default_data_t *d = data;

	g_string_free(d->tags, TRUE);
}

static lt_ext_module_data_t *
_lt_ext_default_create_data(void)
{
	lt_ext_module_data_t *retval = lt_ext_module_data_new(sizeof (lt_ext_default_data_t),
							      _lt_ext_default_destroy_data);

	if (retval) {
		lt_ext_default_data_t *data = (lt_ext_default_data_t *)retval;

		data->tags = g_string_new(NULL);
	}

	return retval;
}

static gboolean
_lt_ext_default_precheck_tag(lt_ext_module_data_t  *data,
			     const lt_tag_t        *tag,
			     GError               **error)
{
	return TRUE;
}

static gboolean
_lt_ext_default_parse_tag(lt_ext_module_data_t  *data,
			  const gchar           *subtag,
			  GError               **error)
{
	lt_ext_default_data_t *d = (lt_ext_default_data_t *)data;

	if (d->tags->len > 0)
		g_string_append_printf(d->tags, "-%s", subtag);
	else
		g_string_append(d->tags, subtag);

	return TRUE;
}

static gchar *
_lt_ext_default_get_tag(lt_ext_module_data_t *data)
{
	lt_ext_default_data_t *d = (lt_ext_default_data_t *)data;

	return g_strdup(d->tags->str);
}

static gboolean
_lt_ext_default_validate_tag(lt_ext_module_data_t *data)
{
	return TRUE;
}

static void
_lt_ext_eaw_destroy_data(gpointer data)
{
}

static lt_ext_module_data_t *
_lt_ext_eaw_create_data(void)
{
	lt_ext_module_data_t *retval = lt_ext_module_data_new(sizeof (lt_ext_module_data_t),
							      _lt_ext_eaw_destroy_data);

	return retval;
}

static gboolean
_lt_ext_eaw_precheck_tag(lt_ext_module_data_t  *data,
			 const lt_tag_t        *tag,
			 GError               **error)
{
	/* not allowed to process any extensions */

	return FALSE;
}

static gboolean
_lt_ext_eaw_parse_tag(lt_ext_module_data_t  *data,
		      const gchar           *subtag,
		      GError               **error)
{
	/* not allowed to add any tags */

	return FALSE;
}

static gchar *
_lt_ext_eaw_get_tag(lt_ext_module_data_t *data)
{
	return g_strdup("");
}

static gboolean
_lt_ext_eaw_validate_tag(lt_ext_module_data_t *data)
{
	return TRUE;
}

static gboolean
lt_ext_module_load(lt_ext_module_t *module)
{
	gchar *filename = g_strdup_printf("liblangtag-ext-%s." G_MODULE_SUFFIX,
					  module->name);
	gchar **path_list, *s, *path = NULL, *fullname = NULL;
	const gchar *env = g_getenv("LANGTAG_EXT_MODULE_PATH");
	gint i;
	gboolean retval = FALSE;
	gsize len;

	if (!env) {
		path_list = g_strsplit(
#ifdef GNOME_ENABLE_DEBUG
			BUILDDIR G_DIR_SEPARATOR_S "liblangtag" G_DIR_SEPARATOR_S "extensions" G_SEARCHPATH_SEPARATOR_S
			BUILDDIR G_DIR_SEPARATOR_S "liblangtag" G_DIR_SEPARATOR_S "extensions" G_DIR_SEPARATOR_S ".libs" G_SEARCHPATH_SEPARATOR_S
#endif
			LANGTAG_EXT_MODULE_PATH,
			G_SEARCHPATH_SEPARATOR_S,
			-1);
	} else {
		path_list = g_strsplit(env, G_SEARCHPATH_SEPARATOR_S, -1);
	}

	for (i = 0; path_list[i] != NULL && !retval; i++) {
		s = path_list[i];

		while (*s && g_ascii_isspace(*s))
			s++;
		len = strlen(s);
		while (len > 0 && g_ascii_isspace(s[len - 1]))
			len--;
		g_free(path);
		path = g_strndup(s, len);
		if (path[0] != 0) {
			g_free(fullname);
			fullname = g_build_filename(path, filename, NULL);
			module->module = g_module_open(fullname,
						       G_MODULE_BIND_LAZY|G_MODULE_BIND_LOCAL);
			if (module->module) {
				gpointer func;

				lt_mem_add_ref(&module->parent, module->module,
					       (lt_destroy_func_t)g_module_close);
				g_module_symbol(module->module,
						"module_get_version",
						&func);
				if (!func) {
					g_warning(g_module_error());
					break;
				}
				if (((lt_ext_module_version_func_t)func)() != LT_EXT_MODULE_VERSION) {
					g_warning("`%s' isn't satisfied the required module version.",
						  filename);
					break;
				}
				g_module_symbol(module->module,
						"module_get_funcs",
						&func);
				if (!func) {
					g_warning(g_module_error());
					break;
				}
				if (!(module->funcs = ((lt_ext_module_get_funcs_func_t)func)())) {
					g_warning("No function table for `%s'",
						  filename);
					break;
				}
				g_log(G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
				      "Loading the external extension handler module: %s",
				      fullname);
				retval = TRUE;
			}
		}
	}
	if (!retval)
		g_warning("No such modules: %s", module->name);

	g_free(fullname);
	g_free(path);
	g_free(filename);
	g_strfreev(path_list);

	return retval;
}

static lt_ext_module_t *
lt_ext_module_new_with_data(const gchar                 *name,
			    const lt_ext_module_funcs_t *funcs)
{
	lt_ext_module_t *retval;

	g_return_val_if_fail (name != NULL, NULL);
	g_return_val_if_fail (funcs != NULL, NULL);

	retval = lt_mem_alloc_object(sizeof (lt_ext_module_t));
	if (retval) {
		retval->name = g_strdup(name);
		lt_mem_add_ref(&retval->parent, retval->name,
			       (lt_destroy_func_t)g_free);
		retval->funcs = funcs;

		g_log(G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG,
		      "Loading the internal extension handler: %s",
		      name);
	}

	return retval;
}

/*< protected >*/
gboolean
lt_ext_module_validate_singleton(gchar singleton)
{
	return (singleton >= '0' && singleton <= '9') ||
		(singleton >= 'A' && singleton <= 'W') ||
		(singleton >= 'Y' && singleton <= 'Z') ||
		(singleton >= 'a' && singleton <= 'w') ||
		(singleton >= 'y' && singleton <= 'z') ||
		singleton == ' ' ||
		singleton == '*';
}

gint
lt_ext_module_singleton_char_to_int(gchar singleton_c)
{
	gint retval = -1;

	if (!lt_ext_module_validate_singleton(singleton_c))
		g_print("XXXXXXXXx: %c\n", singleton_c);
	g_return_val_if_fail (lt_ext_module_validate_singleton(singleton_c), -1);

	if (singleton_c >= '0' && singleton_c <= '9') {
		retval = singleton_c - '0';
	} else if ((singleton_c >= 'a' && singleton_c <= 'z') ||
		   (singleton_c >= 'A' && singleton_c <= 'Z')) {
		retval = g_ascii_tolower(singleton_c) - 'a' + 10;
	} else if (singleton_c == ' ') {
		retval = LT_MAX_EXT_MODULES - 2;
	} else if (singleton_c == '*') {
		retval = LT_MAX_EXT_MODULES - 1;
	}

	return retval;
}

gchar
lt_ext_module_singleton_int_to_char(gint singleton)
{
	gchar retval;

	g_return_val_if_fail (singleton >= 0, 0);
	g_return_val_if_fail (singleton < LT_MAX_EXT_MODULES, 0);

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
lt_ext_module_new(const gchar *name)
{
	lt_ext_module_t *retval;

	g_return_val_if_fail (name != NULL, NULL);

	retval = lt_mem_alloc_object(sizeof (lt_ext_module_t));

	if (retval) {
		gchar *filename = g_path_get_basename(name), *module = NULL;
		static const gchar *prefix = "liblangtag-ext-";
		static gsize prefix_len = 0;
		gchar singleton_c;
		gint singleton;

		if (prefix_len == 0)
			prefix_len = strlen(prefix);

		if (strncmp(filename, prefix, prefix_len) == 0) {
			gsize len = strlen(&filename[prefix_len]);
			gsize suffix_len = strlen(G_MODULE_SUFFIX) + 1;

			if (len > suffix_len &&
			    g_strcmp0(&filename[prefix_len + len - suffix_len], "." G_MODULE_SUFFIX) == 0) {
				module = g_strndup(&filename[prefix_len], len - suffix_len);
				module[len - suffix_len] = 0;
			}
		}
		if (!module)
			module = g_strdup(filename);
		retval->name = module;
		lt_mem_add_ref(&retval->parent, retval->name,
			       (lt_destroy_func_t)g_free);

		g_free(filename);

		if (!lt_ext_module_load(retval)) {
			lt_ext_module_unref(retval);
			return NULL;
		}
		singleton_c = lt_ext_module_get_singleton(retval);
		if (singleton_c == ' ' ||
		    singleton_c == '*') {
			g_warning("Not allowed to override the internal handlers for special singleton.");
			lt_ext_module_unref(retval);
			return NULL;
		}
		singleton = lt_ext_module_singleton_char_to_int(singleton_c);
		if (singleton < 0) {
			g_warning("Invalid singleton: `%c' - `%s'",
				  singleton_c, 
				  retval->name);
			lt_ext_module_unref(retval);
			return NULL;
		}
		if (__lt_ext_modules[singleton]) {
			g_warning("Duplicate extension module: %s",
				  retval->name);
			lt_ext_module_unref(retval);
			return NULL;
		}
		__lt_ext_modules[singleton] = retval;
		lt_mem_add_weak_pointer(&retval->parent,
					(gpointer *)&__lt_ext_modules[singleton]);
	}

	return retval;
}

lt_ext_module_t *
lt_ext_module_lookup(gchar singleton_c)
{
	gint singleton = lt_ext_module_singleton_char_to_int(singleton_c);

	g_return_val_if_fail (singleton >= 0, NULL);
	g_return_val_if_fail (__lt_ext_module_initialized, NULL);

	if (!__lt_ext_modules[singleton])
		return lt_ext_module_ref(__lt_ext_default_handler);

	return lt_ext_module_ref(__lt_ext_modules[singleton]);
}

const gchar *
lt_ext_module_get_name(lt_ext_module_t *module)
{
	g_return_val_if_fail (module != NULL, NULL);

	return module->name;
}

gchar
lt_ext_module_get_singleton(lt_ext_module_t *module)
{
	g_return_val_if_fail (module != NULL, 0);
	g_return_val_if_fail (module->funcs != NULL, 0);
	g_return_val_if_fail (module->funcs->get_singleton != NULL, 0);

	return module->funcs->get_singleton();
}

lt_ext_module_data_t *
lt_ext_module_create_data(lt_ext_module_t *module)
{
	g_return_val_if_fail (module != NULL, NULL);
	g_return_val_if_fail (module->funcs != NULL, NULL);
	g_return_val_if_fail (module->funcs->create_data != NULL, NULL);

	return module->funcs->create_data();
}

gboolean
lt_ext_module_parse_tag(lt_ext_module_t       *module,
			lt_ext_module_data_t  *data,
			const gchar           *subtag,
			GError               **error)
{
	g_return_val_if_fail (module != NULL, FALSE);
	g_return_val_if_fail (data != NULL, FALSE);
	g_return_val_if_fail (subtag != NULL, FALSE);
	g_return_val_if_fail (module->funcs != NULL, FALSE);
	g_return_val_if_fail (module->funcs->parse_tag != NULL, FALSE);

	return module->funcs->parse_tag(data, subtag, error);
}

gchar *
lt_ext_module_get_tag(lt_ext_module_t      *module,
		      lt_ext_module_data_t *data)
{
	g_return_val_if_fail (module != NULL, NULL);
	g_return_val_if_fail (data != NULL, NULL);
	g_return_val_if_fail (module->funcs != NULL, NULL);
	g_return_val_if_fail (module->funcs->get_tag != NULL, NULL);

	return module->funcs->get_tag(data);
}

gboolean
lt_ext_module_validate_tag(lt_ext_module_t      *module,
			   lt_ext_module_data_t *data)
{
	g_return_val_if_fail (module != NULL, FALSE);
	g_return_val_if_fail (data != NULL, FALSE);
	g_return_val_if_fail (module->funcs != NULL, FALSE);
	g_return_val_if_fail (module->funcs->validate_tag != NULL, FALSE);

	return module->funcs->validate_tag(data);
}

gboolean
lt_ext_module_precheck_tag(lt_ext_module_t       *module,
			   lt_ext_module_data_t  *data,
			   const lt_tag_t        *tag,
			   GError               **error)
{
	GError *err = NULL;
	gboolean retval;

	g_return_val_if_fail (module != NULL, FALSE);
	g_return_val_if_fail (data != NULL, FALSE);
	g_return_val_if_fail (module->funcs != NULL, FALSE);
	g_return_val_if_fail (module->funcs->precheck_tag != NULL, FALSE);

	retval = module->funcs->precheck_tag(data, tag, &err);
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
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
#ifdef ENABLE_GMODULE
	const gchar *env = g_getenv("LANGTAG_EXT_MODULE_PATH");
	gchar **path_list;
	gint i;
	gsize suffix_len = strlen(G_MODULE_SUFFIX) + 1;

	if (__lt_ext_module_initialized)
		return;
	if (!env) {
		path_list = g_strsplit(
#ifdef GNOME_ENABLE_DEBUG
			BUILDDIR G_DIR_SEPARATOR_S "liblangtag" G_DIR_SEPARATOR_S "extensions" G_SEARCHPATH_SEPARATOR_S
			BUILDDIR G_DIR_SEPARATOR_S "liblangtag" G_DIR_SEPARATOR_S "extensions" G_DIR_SEPARATOR_S ".libs" G_SEARCHPATH_SEPARATOR_S
#endif
			LANGTAG_EXT_MODULE_PATH,
			G_SEARCHPATH_SEPARATOR_S,
			-1);
	} else {
		path_list = g_strsplit(env, G_SEARCHPATH_SEPARATOR_S, -1);
	}
	for (i = 0; path_list[i] != NULL; i++) {
		DIR *dir;

		dir = opendir(path_list[i]);
		if (dir) {
			struct dirent dent, *dresult;
			gsize len;

			while (1) {
				if (readdir_r(dir, &dent, &dresult) || dresult == NULL)
					break;

				len = strlen(dent.d_name);
				if (len > suffix_len &&
				    g_strcmp0(&dent.d_name[len - suffix_len],
					      "." G_MODULE_SUFFIX) == 0) {
					lt_ext_module_new(dent.d_name);
				}
			}
			closedir(dir);
		}
	}
	g_strfreev(path_list);
#endif /* ENABLE_GMODULE */
	__lt_ext_default_handler = lt_ext_module_new_with_data("default",
							       &__default_funcs);
	lt_mem_add_weak_pointer(&__lt_ext_default_handler->parent,
				(gpointer *)&__lt_ext_default_handler);
	__lt_ext_modules[LT_MAX_EXT_MODULES - 2] = lt_ext_module_new_with_data("empty",
									       &__empty_and_wildcard_funcs);
	lt_mem_add_weak_pointer(&__lt_ext_modules[LT_MAX_EXT_MODULES - 2]->parent,
				(gpointer *)&__lt_ext_modules[LT_MAX_EXT_MODULES - 2]);
	__lt_ext_modules[LT_MAX_EXT_MODULES - 1] = lt_ext_module_new_with_data("wildcard",
									   &__empty_and_wildcard_funcs);
	lt_mem_add_weak_pointer(&__lt_ext_modules[LT_MAX_EXT_MODULES - 1]->parent,
				(gpointer *)&__lt_ext_modules[LT_MAX_EXT_MODULES - 1]);
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
	gint i;

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
	g_return_val_if_fail (module != NULL, NULL);

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
