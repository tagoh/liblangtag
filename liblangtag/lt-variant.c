/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-variant.c
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
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-string.h"
#include "lt-utils.h"
#include "lt-variant.h"
#include "lt-variant-private.h"


/**
 * SECTION: lt-variant
 * @Short_Description: A container class for Variant subtag
 * @Title: Container - Variant
 *
 * This container class provides a data access to Variant subtag entry.
 */
struct _lt_variant_t {
	lt_mem_t   parent;
	char      *tag;
	char      *description;
	char      *preferred_tag;
	lt_list_t *prefix;
};

/*< private >*/

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
		   const char   *subtag)
{
	lt_return_if_fail (variant != NULL);
	lt_return_if_fail (subtag != NULL);

	if (variant->tag)
		lt_mem_remove_ref(&variant->parent, variant->tag);
	variant->tag = strdup(subtag);
	lt_mem_add_ref(&variant->parent, variant->tag, free);
}

void
lt_variant_set_preferred_tag(lt_variant_t *variant,
			     const char   *subtag)
{
	lt_return_if_fail (variant != NULL);
	lt_return_if_fail (subtag != NULL);

	if (variant->preferred_tag)
		lt_mem_remove_ref(&variant->parent, variant->preferred_tag);
	variant->preferred_tag = strdup(subtag);
	lt_mem_add_ref(&variant->parent, variant->preferred_tag, free);
}

void
lt_variant_set_name(lt_variant_t *variant,
		    const char   *description)
{
	lt_return_if_fail (variant != NULL);
	lt_return_if_fail (description != NULL);

	if (variant->description)
		lt_mem_remove_ref(&variant->parent, variant->description);
	variant->description = strdup(description);
	lt_mem_add_ref(&variant->parent, variant->description, free);
}

void
lt_variant_add_prefix(lt_variant_t *variant,
		      const char   *prefix)
{
	lt_bool_t no_prefixes;

	lt_return_if_fail (variant != NULL);
	lt_return_if_fail (prefix != NULL);

	no_prefixes = variant->prefix == NULL;
	variant->prefix = lt_list_append(variant->prefix, strdup(prefix), free);
	if (no_prefixes)
		lt_mem_add_ref(&variant->parent, variant->prefix, lt_list_free);
}

/*< public >*/
/**
 * lt_variant_ref:
 * @variant: a #lt_variant_t.
 *
 * Increases the reference count of @variant.
 *
 * Returns: (transfer none): the same @variant object.
 */
lt_variant_t *
lt_variant_ref(lt_variant_t *variant)
{
	lt_return_val_if_fail (variant != NULL, NULL);

	return lt_mem_ref(&variant->parent);
}

/**
 * lt_variant_unref:
 * @variant: a #lt_variant_t.
 *
 * Decreases the reference count of @variant. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_variant_unref(lt_variant_t *variant)
{
	if (variant)
		lt_mem_unref(&variant->parent);
}

/**
 * lt_variant_get_better_tag:
 * @variant: a #lt_variant_t.
 *
 * Obtains the better tag for use. this is a convenient function to get
 * the preferred-value if available.
 *
 * Returns: a tag string.
 */
const char *
lt_variant_get_better_tag(const lt_variant_t *variant)
{
	const char *retval = lt_variant_get_preferred_tag(variant);

	if (!retval)
		retval = lt_variant_get_tag(variant);

	return retval;
}

/**
 * lt_variant_get_tag:
 * @variant: a #lt_variant_t.
 *
 * Obtains the tag name.
 *
 * Returns: a tag string.
 */
const char *
lt_variant_get_tag(const lt_variant_t *variant)
{
	lt_return_val_if_fail (variant != NULL, NULL);

	return variant->tag;
}

/**
 * lt_variant_get_preferred_tag:
 * @variant: a #lt_variant_t.
 *
 * Obtains the preferred-value. this is available only when the tag is
 * marked as deprecated.
 *
 * Returns: a preferred-value for the tag or %NULL.
 */
const char *
lt_variant_get_preferred_tag(const lt_variant_t *variant)
{
	lt_return_val_if_fail (variant != NULL, NULL);

	return variant->preferred_tag;
}

/**
 * lt_variant_get_name:
 * @variant: a #lt_variant_t.
 *
 * Obtains the description of the subtag.
 *
 * Returns: a description string.
 */
const char *
lt_variant_get_name(const lt_variant_t *variant)
{
	lt_return_val_if_fail (variant != NULL, NULL);

	return variant->description;
}

/**
 * lt_variant_get_prefix:
 * @variant: a #lt_variant_t.
 *
 * Obtains the prefix being assigned to the subtag.
 * This is available only when the subtag has any suitable sequence of
 * subtags for forming (with other subtags, as appropriate) a language
 * tag when using the variant.
 *
 * Returns: (element-type utf8) (transfer none): a #lt_list_t contains prefix
 *          strings or %NULL.
 */
const lt_list_t *
lt_variant_get_prefix(const lt_variant_t *variant)
{
	lt_return_val_if_fail (variant != NULL, NULL);

	return variant->prefix;
}

/**
 * lt_variant_dump:
 * @variant: a #lt_variant_t.
 *
 * Dumps the container information to the standard output.
 */
void
lt_variant_dump(const lt_variant_t *variant)
{
	lt_string_t *string = lt_string_new(NULL);
	const lt_list_t *list, *l;
	const char *preferred = lt_variant_get_preferred_tag(variant);

	list = lt_variant_get_prefix(variant);
	for (l = list; l != NULL; l = lt_list_next(l)) {
		if (lt_string_length(string) == 0)
			lt_string_append(string, " (prefix = [");
		else
			lt_string_append(string, ", ");
		lt_string_append(string, (const char *)lt_list_value(l));
	}
	if (lt_string_length(string) > 0)
		lt_string_append(string, "]");
	if (preferred) {
		if (lt_string_length(string) == 0)
			lt_string_append(string, " (");
		else
			lt_string_append(string, ", ");
		lt_string_append_printf(string, "preferred-value: %s",
					preferred);
	}
	if (lt_string_length(string) > 0)
		lt_string_append(string, ")");

	lt_info("Variant: %s [%s]%s",
		lt_variant_get_tag(variant),
		lt_variant_get_name(variant),
		lt_string_value(string));

	lt_string_unref(string);
}

/**
 * lt_variant_compare:
 * @v1: a #lt_variant_t.
 * @v2: a #lt_variant_t.
 *
 * Compare if @v1 and @v2 is the same object or not.
 *
 * Returns: %TRUE if it's the same, otherwise %FALSE.
 */
lt_bool_t
lt_variant_compare(const lt_variant_t *v1,
		   const lt_variant_t *v2)
{
	const char *s1, *s2;

	if (v1 == v2)
		return TRUE;

	s1 = v1 ? lt_variant_get_tag(v1) : NULL;
	s2 = v2 ? lt_variant_get_tag(v2) : NULL;

	if (lt_strcmp0(s1, "*") == 0 ||
	    lt_strcmp0(s2, "*") == 0)
		return TRUE;

	return lt_strcmp0(s1, s2) == 0;
}
