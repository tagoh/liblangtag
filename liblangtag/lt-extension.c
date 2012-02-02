/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extension.c
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

#include "lt-error.h"
#include "lt-mem.h"
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
	GString              *cached_tag;
	lt_ext_module_t      *module;
	gint                  singleton;
	lt_ext_module_data_t *extensions[LT_MAX_EXT_MODULES + 1];
};

/*< private >*/
static void
_lt_extension_gstring_free(GString *string)
{
	g_string_free(string, TRUE);
}

/*< protected >*/
lt_extension_t *
lt_extension_create(void)
{
	lt_extension_t *retval = lt_mem_alloc_object(sizeof (lt_extension_t));

	if (retval) {
		retval->cached_tag = g_string_new(NULL);
		lt_mem_add_ref(&retval->parent, retval->cached_tag,
			       (lt_destroy_func_t)_lt_extension_gstring_free);
	}

	return retval;
}

gboolean
lt_extension_has_singleton(lt_extension_t *extension,
			   gchar           singleton_c)
{
	gint singleton;

	g_return_val_if_fail (extension != NULL, FALSE);

	singleton = lt_ext_module_singleton_char_to_int(singleton_c);
	if (singleton < 0)
		return FALSE;

	return extension->extensions[singleton] != NULL;
}

gboolean
lt_extension_add_singleton(lt_extension_t  *extension,
			   gchar            singleton_c,
			   const lt_tag_t  *tag,
			   GError         **error)
{
	gint singleton = lt_ext_module_singleton_char_to_int(singleton_c);
	lt_ext_module_t *m;
	lt_ext_module_data_t *d;
	GError *err = NULL;

	g_return_val_if_fail (extension != NULL, FALSE);
	g_return_val_if_fail (singleton_c != 'X' && singleton_c != 'x', FALSE);
	g_return_val_if_fail (!lt_extension_has_singleton(extension, singleton_c), FALSE);
	g_return_val_if_fail (singleton >= 0, FALSE);

	m = lt_ext_module_lookup(singleton_c);
	d = lt_ext_module_create_data(m);
	if (!d) {
		lt_ext_module_unref(m);
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of lt_ext_module_data_t.");

		goto bail;
	}
	if (tag && !lt_ext_module_precheck_tag(m, d, tag, &err)) {
		lt_ext_module_data_unref(d);
		lt_ext_module_unref(m);

		goto bail;
	}
	if (extension->module)
		lt_mem_remove_ref(&extension->parent, extension->module);
	extension->module = m;
	lt_mem_add_ref(&extension->parent, extension->module,
		       (lt_destroy_func_t)lt_ext_module_unref);
	extension->extensions[singleton] = d;
	lt_mem_add_ref(&extension->parent, extension->extensions[singleton],
		       (lt_destroy_func_t)lt_ext_module_data_unref);
	extension->singleton = singleton;

	if (extension->cached_tag->len > 0)
		g_string_append_printf(extension->cached_tag, "-%c", singleton_c);
	else
		g_string_append_c(extension->cached_tag, singleton_c);

  bail:
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);

		return FALSE;
	}

	return TRUE;
}

gboolean
lt_extension_add_tag(lt_extension_t  *extension,
		     const gchar     *subtag,
		     GError         **error)
{
	gboolean retval;
	GError *err = NULL;

	g_return_val_if_fail (extension != NULL, FALSE);
	g_return_val_if_fail (subtag != NULL, FALSE);
	g_return_val_if_fail (extension->module != NULL, FALSE);
	g_return_val_if_fail (extension->extensions[extension->singleton] != NULL, FALSE);

	retval = lt_ext_module_parse_tag(extension->module,
					 extension->extensions[extension->singleton],
					 subtag,
					 &err);
	if (retval)
		g_string_append_printf(extension->cached_tag, "-%s", subtag);

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

void
lt_extension_cancel_tag(lt_extension_t *extension)
{
	g_return_if_fail (extension != NULL);

	if (extension->module && extension->extensions[extension->singleton]) {
		gchar **tags, singleton[4];
		GList *l = NULL, *ll;
		gint i;

		lt_mem_remove_ref(&extension->parent, extension->module);
		extension->module = NULL;
		lt_mem_remove_ref(&extension->parent, extension->extensions[extension->singleton]);
		extension->extensions[extension->singleton] = NULL;

		tags = g_strsplit(extension->cached_tag->str, "-", -1);
		for (i = 0; tags[i] != NULL; i++) {
			l = g_list_append(l, tags[i]);
		}
		singleton[0] = lt_ext_module_singleton_int_to_char(extension->singleton);
		singleton[1] = 0;
		g_string_truncate(extension->cached_tag, 0);
		for (ll = l; ll != NULL; ll = g_list_next(ll)) {
			if (g_strcmp0(ll->data, singleton) == 0) {
				if (ll->prev)
					ll->prev->next = NULL;
				ll->prev = NULL;
				g_list_free(ll);
				break;
			}
			if (extension->cached_tag->len > 0)
				g_string_append_printf(extension->cached_tag, "-%s", (gchar *)ll->data);
			else
				g_string_append(extension->cached_tag, ll->data);
		}
		g_list_free(l);
		g_strfreev(tags);
	}
}

lt_extension_t *
lt_extension_copy(lt_extension_t *extension)
{
	lt_extension_t *retval;
	gint i;

	g_return_val_if_fail (extension != NULL, NULL);

	retval = lt_extension_create();
	if (retval) {
		g_string_append(retval->cached_tag, extension->cached_tag->str);
		if (extension->module) {
			retval->module = lt_ext_module_ref(extension->module);
			lt_mem_add_ref(&retval->parent, retval->module,
				       (lt_destroy_func_t)lt_ext_module_unref);
			retval->singleton = extension->singleton;
			for (i = 0; i < LT_MAX_EXT_MODULES; i++) {
				if (extension->extensions[i])
					retval->extensions[i] = lt_ext_module_data_ref(extension->extensions[i]);
			}
		}
	}

	return retval;
}

gboolean
lt_extension_validate_state(lt_extension_t *extension)
{
	gboolean retval = TRUE;

	g_return_val_if_fail (extension != NULL, FALSE);

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
	g_return_val_if_fail (extension != NULL, NULL);

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
const gchar *
lt_extension_get_tag(lt_extension_t *extension)
{
	g_return_val_if_fail (extension != NULL, NULL);

	return extension->cached_tag->str;
}

/**
 * lt_extension_get_canonicalized_tag:
 * @extension: a #lt_extension_t.
 *
 * Generate a canonicalized tag.
 *
 * Returns: a string. this must be freed.
 */
gchar *
lt_extension_get_canonicalized_tag(lt_extension_t *extension)
{
	GString *string;
	gint i;
	gchar c, *s;
	lt_ext_module_t *m;

	g_return_val_if_fail (extension != NULL, NULL);

	string = g_string_new(NULL);

	for (i = 0; i < LT_MAX_EXT_MODULES; i++) {
		if (extension->extensions[i]) {
			if (string->len > 0)
				g_string_append_c(string, '-');
			c = lt_ext_module_singleton_int_to_char(i);
			g_string_append_c(string, c);
			if (c != ' ' && c != '*') {
				m = lt_ext_module_lookup(c);
				if (m) {
					s = lt_ext_module_get_tag(m, extension->extensions[i]);
					g_string_append_printf(string, "-%s", s);
					g_free(s);
					lt_ext_module_unref(m);
				} else {
					g_warning("Unable to obtain the certain module instance: singleton = '%c", c);
					break;
				}
			}
		}
	}

	return g_string_free(string, FALSE);
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
	gint i;
	gchar c;
	gchar *s;
	lt_ext_module_t *m;

	g_return_if_fail (extension != NULL);

	g_print("Extensions:\n");
	for (i = 0; i < LT_MAX_EXT_MODULES; i++) {
		if (extension->extensions[i]) {
			c = lt_ext_module_singleton_int_to_char(i);
			if (c == ' ') {
				g_print(" '' [empty]\n");
			} else if (c == '*') {
				g_print(" '*' [wildcard]\n");
			} else {
				m = lt_ext_module_lookup(c);
				if (m) {
					s = lt_ext_module_get_tag(m, extension->extensions[i]);
					g_print("  %c-%s\n", c, s);
					g_free(s);
					lt_ext_module_unref(m);
				} else {
					g_print("  [failed to obtain the module instance: singleton = '%c', data = %p]\n",
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
gboolean
lt_extension_compare(const lt_extension_t *v1,
		     const lt_extension_t *v2)
{
	gint i;
	gchar *s1 = NULL, *s2 = NULL;
	lt_ext_module_t *m = NULL;
	gboolean retval = TRUE;

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
		g_free(s1);
		g_free(s2);
		s1 = lt_ext_module_get_tag(m, v1->extensions[i]);
		s2 = lt_ext_module_get_tag(m, v2->extensions[i]);
		if (g_strcmp0(s1, s2)) {
			retval = FALSE;
			break;
		}
	}

	if (m)
		lt_ext_module_unref(m);
	g_free(s1);
	g_free(s2);

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
gboolean
lt_extension_truncate(lt_extension_t  *extension)
{
	gint i;
	gboolean retval = FALSE;

	g_return_val_if_fail (extension != NULL, FALSE);

	for (i = LT_MAX_EXT_MODULES - 1; i >= 0; i--) {
		if (extension->extensions[i]) {
			lt_mem_remove_ref(&extension->parent, extension->extensions[i]);
			extension->extensions[i] = NULL;
			retval = TRUE;
			break;
		}
	}

	return retval;
}
