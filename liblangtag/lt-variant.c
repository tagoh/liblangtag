/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-variant.c
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
#include "lt-variant.h"
#include "lt-variant-private.h"


struct _lt_variant_t {
	lt_mem_t  parent;
	gchar    *tag;
	gchar    *description;
	gchar    *preferred_tag;
	GList    *prefix;
};

/*< private >*/
static void
_lt_variant_prefix_free(GList *list)
{
	GList *l;

	for (l = list; l != NULL; l = g_list_next(l)) {
		g_free(l->data);
	}
	g_list_free(list);
}

/*< protected >*/
lt_variant_t *
lt_variant_create(void)
{
	lt_variant_t *retval;

	retval = lt_mem_alloc_object(sizeof (lt_variant_t));

	return retval;
}

void
lt_variant_set_tag(lt_variant_t *variant,
		   const gchar  *subtag)
{
	g_return_if_fail (variant != NULL);
	g_return_if_fail (subtag != NULL);

	if (variant->tag)
		lt_mem_remove_ref(&variant->parent, variant->tag);
	variant->tag = g_strdup(subtag);
	lt_mem_add_ref(&variant->parent, variant->tag,
		       (lt_destroy_func_t)g_free);
}

void
lt_variant_set_preferred_tag(lt_variant_t *variant,
			     const gchar  *subtag)
{
	g_return_if_fail (variant != NULL);
	g_return_if_fail (subtag != NULL);

	if (variant->preferred_tag)
		lt_mem_remove_ref(&variant->parent, variant->preferred_tag);
	variant->preferred_tag = g_strdup(subtag);
	lt_mem_add_ref(&variant->parent, variant->preferred_tag,
		       (lt_destroy_func_t)g_free);
}

void
lt_variant_set_name(lt_variant_t *variant,
		    const gchar  *description)
{
	g_return_if_fail (variant != NULL);
	g_return_if_fail (description != NULL);

	if (variant->description)
		lt_mem_remove_ref(&variant->parent, variant->description);
	variant->description = g_strdup(description);
	lt_mem_add_ref(&variant->parent, variant->description,
		       (lt_destroy_func_t)g_free);
}

void
lt_variant_add_prefix(lt_variant_t *variant,
		      const gchar  *prefix)
{
	g_return_if_fail (variant != NULL);
	g_return_if_fail (prefix != NULL);

	if (!variant->prefix) {
		variant->prefix = g_list_append(variant->prefix, g_strdup(prefix));
		lt_mem_add_ref(&variant->parent, variant->prefix,
			       (lt_destroy_func_t)_lt_variant_prefix_free);
	} else {
		variant->prefix = g_list_append(variant->prefix, g_strdup(prefix));
	}
}

/*< public >*/
lt_variant_t *
lt_variant_ref(lt_variant_t *variant)
{
	g_return_val_if_fail (variant != NULL, NULL);

	return lt_mem_ref(&variant->parent);
}

void
lt_variant_unref(lt_variant_t *variant)
{
	if (variant)
		lt_mem_unref(&variant->parent);
}

const gchar *
lt_variant_get_better_tag(const lt_variant_t *variant)
{
	const gchar *retval = lt_variant_get_preferred_tag(variant);

	if (!retval)
		retval = lt_variant_get_tag(variant);

	return retval;
}

const gchar *
lt_variant_get_tag(const lt_variant_t *variant)
{
	g_return_val_if_fail (variant != NULL, NULL);

	return variant->tag;
}

const gchar *
lt_variant_get_preferred_tag(const lt_variant_t *variant)
{
	g_return_val_if_fail (variant != NULL, NULL);

	return variant->preferred_tag;
}

const gchar *
lt_variant_get_name(const lt_variant_t *variant)
{
	g_return_val_if_fail (variant != NULL, NULL);

	return variant->description;
}

const GList *
lt_variant_get_prefix(const lt_variant_t *variant)
{
	g_return_val_if_fail (variant != NULL, NULL);

	return variant->prefix;
}

void
lt_variant_dump(const lt_variant_t *variant)
{
	GString *string = g_string_new(NULL);
	const GList *list, *l;
	const gchar *preferred = lt_variant_get_preferred_tag(variant);

	list = lt_variant_get_prefix(variant);
	for (l = list; l != NULL; l = g_list_next(l)) {
		if (string->len == 0)
			g_string_append(string, " (prefix = [");
		else
			g_string_append(string, ", ");
		g_string_append(string, (const gchar *)l->data);
	}
	if (preferred) {
		if (string->len == 0)
			g_string_append(string, " (");
		else
			g_string_append(string, ", ");
		g_string_append_printf(string, "preferred-value: %s",
				       preferred);
	}
	if (string->len > 0)
		g_string_append(string, "]");
	if (string->len > 0)
		g_string_append(string, ")");

	g_print("Variant: %s [%s]%s\n",
		lt_variant_get_tag(variant),
		lt_variant_get_name(variant),
		string->str);

	g_string_free(string, TRUE);
}

gboolean
lt_variant_compare(const lt_variant_t *v1,
		   const lt_variant_t *v2)
{
	g_return_val_if_fail (v1 != NULL, FALSE);
	g_return_val_if_fail (v2 != NULL, FALSE);

	if (v1 == v2)
		return TRUE;

	return g_strcmp0(lt_variant_get_tag(v1), lt_variant_get_tag(v2)) == 0;
}
