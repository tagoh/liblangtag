/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extlang.c
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

#include "lt-mem.h"
#include "lt-extlang.h"
#include "lt-extlang-private.h"


/**
 * SECTION:lt-extlang
 * @Short_Description: A container class for Extlang
 * @Title: Container - Extlang
 *
 * This container class provides a data access to Extlang entry.
 */
struct _lt_extlang_t {
	lt_mem_t  parent;
	gchar    *tag;
	gchar    *description;
	gchar    *macrolanguage;
	gchar    *preferred_tag;
	gchar    *prefix;
};

/*< private >*/

/*< protected >*/
lt_extlang_t *
lt_extlang_create(void)
{
	lt_extlang_t *retval;

	retval = lt_mem_alloc_object(sizeof (lt_extlang_t));

	return retval;
}

void
lt_extlang_set_tag(lt_extlang_t *extlang,
		   const gchar  *subtag)
{
	g_return_if_fail (extlang != NULL);
	g_return_if_fail (subtag != NULL);

	if (extlang->tag)
		lt_mem_remove_ref(&extlang->parent, extlang->tag);
	extlang->tag = g_strdup(subtag);
	lt_mem_add_ref(&extlang->parent, extlang->tag,
		       (lt_destroy_func_t)g_free);
}

void
lt_extlang_set_preferred_tag(lt_extlang_t *extlang,
			     const gchar  *subtag)
{
	g_return_if_fail (extlang != NULL);
	g_return_if_fail (subtag != NULL);

	if (extlang->preferred_tag)
		lt_mem_remove_ref(&extlang->parent, extlang->preferred_tag);
	extlang->preferred_tag = g_strdup(subtag);
	lt_mem_add_ref(&extlang->parent, extlang->preferred_tag,
		       (lt_destroy_func_t)g_free);
}

void
lt_extlang_set_name(lt_extlang_t *extlang,
		    const gchar  *description)
{
	g_return_if_fail (extlang != NULL);
	g_return_if_fail (description != NULL);

	if (extlang->description)
		lt_mem_remove_ref(&extlang->parent, extlang->description);
	extlang->description = g_strdup(description);
	lt_mem_add_ref(&extlang->parent, extlang->description,
		       (lt_destroy_func_t)g_free);
}

void
lt_extlang_set_macro_language(lt_extlang_t *extlang,
			      const gchar  *macrolanguage)
{
	g_return_if_fail (extlang != NULL);
	g_return_if_fail (macrolanguage != NULL);

	if (extlang->macrolanguage)
		lt_mem_remove_ref(&extlang->parent, extlang->macrolanguage);
	extlang->macrolanguage = g_strdup(macrolanguage);
	lt_mem_add_ref(&extlang->parent, extlang->macrolanguage,
		       (lt_destroy_func_t)g_free);
}

void
lt_extlang_add_prefix(lt_extlang_t *extlang,
		      const gchar  *prefix)
{
	g_return_if_fail (extlang != NULL);
	g_return_if_fail (prefix != NULL);

	if (extlang->prefix)
		lt_mem_remove_ref(&extlang->parent, extlang->prefix);
	extlang->prefix = g_strdup(prefix);
	lt_mem_add_ref(&extlang->parent, extlang->prefix,
		       (lt_destroy_func_t)g_free);
}

/*< public >*/
/**
 * lt_extlang_ref:
 * @extlang: a #lt_extlang_t.
 *
 * Increases the reference count of @extlang.
 *
 * Returns: (transfer none): the same @extlang object.
 */
lt_extlang_t *
lt_extlang_ref(lt_extlang_t *extlang)
{
	g_return_val_if_fail (extlang != NULL, NULL);

	return lt_mem_ref(&extlang->parent);
}

/**
 * lt_extlang_unref:
 * @extlang: a #lt_extlang_t.
 *
 * Decreases the reference count of @extlang. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_extlang_unref(lt_extlang_t *extlang)
{
	if (extlang)
		lt_mem_unref(&extlang->parent);
}

/**
 * lt_extlang_get_tag:
 * @extlang: a #lt_extlang_t.
 *
 * Obtains the subtag that is registered as ISO 639 code.
 *
 * Returns: a subtag name.
 */
const gchar *
lt_extlang_get_tag(const lt_extlang_t *extlang)
{
	g_return_val_if_fail (extlang != NULL, NULL);

	return extlang->tag;
}

/**
 * lt_extlang_get_preferred_tag:
 * @extlang: a #lt_extlang_t.
 *
 * Obtains the preferred-value. this is available only when the subtag is
 * marked as deprecated.
 *
 * Returns: a preferred-value for the subtag or %NULL.
 */
const gchar *
lt_extlang_get_preferred_tag(const lt_extlang_t *extlang)
{
	g_return_val_if_fail (extlang != NULL, NULL);

	return extlang->preferred_tag;
}

/**
 * lt_extlang_get_name:
 * @extlang: a #lt_extlang_t.
 *
 * Obtains the description of the subtag.
 *
 * Returns: a description string.
 */
const gchar *
lt_extlang_get_name(const lt_extlang_t *extlang)
{
	g_return_val_if_fail (extlang != NULL, NULL);

	return extlang->description;
}

/**
 * lt_extlang_get_macro_language:
 * @extlang: a #lt_extlang_t.
 *
 * Obtains the macrolanguage being assigned for the subtag.
 * This is available only when the subtag is registered as the macrolanguage
 * in ISO 639-3.
 *
 * Returns: a macrolanguage name or %NULL.
 */
const gchar *
lt_extlang_get_macro_language(const lt_extlang_t *extlang)
{
	g_return_val_if_fail (extlang != NULL, NULL);

	return extlang->macrolanguage;
}

/**
 * lt_extlang_get_prefix:
 * @extlang: a #lt_extlang_t.
 *
 * Obtains the prefix being assigned to the subtag.
 * This is available only when the subtag has a particular seqnence of
 * subgtags that form a meaningful tag with the subtag.
 *
 * Returns: a prefix string or %NULL.
 */
const gchar *
lt_extlang_get_prefix(const lt_extlang_t *extlang)
{
	g_return_val_if_fail (extlang != NULL, NULL);

	return extlang->prefix;
}

/**
 * lt_extlang_dump:
 * @extlang: a #lt_extlang_t.
 *
 * Dumps the container information to the standard output.
 */
void
lt_extlang_dump(const lt_extlang_t *extlang)
{
	const gchar *macrolang = lt_extlang_get_macro_language(extlang);
	const gchar *preferred = lt_extlang_get_preferred_tag(extlang);
	const gchar *prefix = lt_extlang_get_prefix(extlang);
	GString *string = g_string_new(NULL);

	if (macrolang) {
		if (string->len == 0)
			g_string_append(string, " (");
		g_string_append_printf(string, "macrolanguage: %s",
				       macrolang);
	}
	if (preferred) {
		if (string->len == 0)
			g_string_append(string, " (");
		else
			g_string_append(string, ", ");
		g_string_append_printf(string, "preferred-value: %s",
				       preferred);
	}
	if (prefix) {
		if (string->len == 0)
			g_string_append(string, " (");
		else
			g_string_append(string, ", ");
		g_string_append_printf(string, "prefix: %s",
				       prefix);
	}
	if (string->len > 0)
		g_string_append(string, ")");
	g_print("Extlang: %s [%s]%s\n",
		lt_extlang_get_tag(extlang),
		lt_extlang_get_name(extlang),
		string->str);
	g_string_free(string, TRUE);
}

/**
 * lt_extlang_compare:
 * @v1: a #lt_extlang_t.
 * @v2: a #lt_extlang_t.
 *
 * Compares @v1 and @v2 is the same object or not.
 *
 * Returns: %TRUE if it's the same, otherwise %FALSE.
 */
gboolean
lt_extlang_compare(const lt_extlang_t *v1,
		   const lt_extlang_t *v2)
{
	const gchar *s1, *s2;

	if (v1 == v2)
		return TRUE;

	s1 = v1 ? lt_extlang_get_tag(v1) : NULL;
	s2 = v2 ? lt_extlang_get_tag(v2) : NULL;

	if (g_strcmp0(s1, "*") == 0 ||
	    g_strcmp0(s2, "*") == 0)
		return TRUE;

	return g_strcmp0(s1, s2) == 0;
}
