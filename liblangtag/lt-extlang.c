/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extlang.c
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

#include <glib.h> /* just shut up GHashTable dependency in lt-mem.h */
#include <stdlib.h>
#include <string.h>
#include "lt-macros.h"
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-string.h"
#include "lt-utils.h"
#include "lt-extlang.h"
#include "lt-extlang-private.h"


/**
 * SECTION:lt-extlang
 * @Short_Description: A container class for Extlang subtag
 * @Title: Container - Extlang
 *
 * This container class provides a data access to Extlang subtag entry.
 */
struct _lt_extlang_t {
	lt_mem_t  parent;
	char     *tag;
	char     *description;
	char     *macrolanguage;
	char     *preferred_tag;
	char     *prefix;
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
		   const char   *subtag)
{
	lt_return_if_fail (extlang != NULL);
	lt_return_if_fail (subtag != NULL);

	if (extlang->tag)
		lt_mem_remove_ref(&extlang->parent, extlang->tag);
	extlang->tag = strdup(subtag);
	lt_mem_add_ref(&extlang->parent, extlang->tag, free);
}

void
lt_extlang_set_preferred_tag(lt_extlang_t *extlang,
			     const char   *subtag)
{
	lt_return_if_fail (extlang != NULL);
	lt_return_if_fail (subtag != NULL);

	if (extlang->preferred_tag)
		lt_mem_remove_ref(&extlang->parent, extlang->preferred_tag);
	extlang->preferred_tag = strdup(subtag);
	lt_mem_add_ref(&extlang->parent, extlang->preferred_tag, free);
}

void
lt_extlang_set_name(lt_extlang_t *extlang,
		    const char   *description)
{
	lt_return_if_fail (extlang != NULL);
	lt_return_if_fail (description != NULL);

	if (extlang->description)
		lt_mem_remove_ref(&extlang->parent, extlang->description);
	extlang->description = strdup(description);
	lt_mem_add_ref(&extlang->parent, extlang->description, free);
}

void
lt_extlang_set_macro_language(lt_extlang_t *extlang,
			      const char   *macrolanguage)
{
	lt_return_if_fail (extlang != NULL);
	lt_return_if_fail (macrolanguage != NULL);

	if (extlang->macrolanguage)
		lt_mem_remove_ref(&extlang->parent, extlang->macrolanguage);
	extlang->macrolanguage = strdup(macrolanguage);
	lt_mem_add_ref(&extlang->parent, extlang->macrolanguage, free);
}

void
lt_extlang_add_prefix(lt_extlang_t *extlang,
		      const char   *prefix)
{
	lt_return_if_fail (extlang != NULL);
	lt_return_if_fail (prefix != NULL);

	if (extlang->prefix)
		lt_mem_remove_ref(&extlang->parent, extlang->prefix);
	extlang->prefix = strdup(prefix);
	lt_mem_add_ref(&extlang->parent, extlang->prefix, free);
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
	lt_return_val_if_fail (extlang != NULL, NULL);

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
const char *
lt_extlang_get_tag(const lt_extlang_t *extlang)
{
	lt_return_val_if_fail (extlang != NULL, NULL);

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
const char *
lt_extlang_get_preferred_tag(const lt_extlang_t *extlang)
{
	lt_return_val_if_fail (extlang != NULL, NULL);

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
const char *
lt_extlang_get_name(const lt_extlang_t *extlang)
{
	lt_return_val_if_fail (extlang != NULL, NULL);

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
 * Returns: a macrolanguage string or %NULL.
 */
const char *
lt_extlang_get_macro_language(const lt_extlang_t *extlang)
{
	lt_return_val_if_fail (extlang != NULL, NULL);

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
const char *
lt_extlang_get_prefix(const lt_extlang_t *extlang)
{
	lt_return_val_if_fail (extlang != NULL, NULL);

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
	const char *macrolang = lt_extlang_get_macro_language(extlang);
	const char *preferred = lt_extlang_get_preferred_tag(extlang);
	const char *prefix = lt_extlang_get_prefix(extlang);
	lt_string_t *string = lt_string_new(NULL);

	if (macrolang) {
		if (lt_string_length(string) == 0)
			lt_string_append(string, " (");
		lt_string_append_printf(string, "macrolanguage: %s",
					macrolang);
	}
	if (preferred) {
		if (lt_string_length(string) == 0)
			lt_string_append(string, " (");
		else
			lt_string_append(string, ", ");
		lt_string_append_printf(string, "preferred-value: %s",
					preferred);
	}
	if (prefix) {
		if (lt_string_length(string) == 0)
			lt_string_append(string, " (");
		else
			lt_string_append(string, ", ");
		lt_string_append_printf(string, "prefix: %s",
					prefix);
	}
	if (lt_string_length(string) > 0)
		lt_string_append(string, ")");
	lt_info("Extlang: %s [%s]%s",
		lt_extlang_get_tag(extlang),
		lt_extlang_get_name(extlang),
		lt_string_value(string));

	lt_string_unref(string);
}

/**
 * lt_extlang_compare:
 * @v1: a #lt_extlang_t.
 * @v2: a #lt_extlang_t.
 *
 * Compares if @v1 and @v2 is the same object or not.
 *
 * Returns: %TRUE if it's the same, otherwise %FALSE.
 */
lt_bool_t
lt_extlang_compare(const lt_extlang_t *v1,
		   const lt_extlang_t *v2)
{
	const char *s1, *s2;

	if (v1 == v2)
		return TRUE;

	s1 = v1 ? lt_extlang_get_tag(v1) : NULL;
	s2 = v2 ? lt_extlang_get_tag(v2) : NULL;

	if (lt_strcmp0(s1, "*") == 0 ||
	    lt_strcmp0(s2, "*") == 0)
		return TRUE;

	return lt_strcmp0(s1, s2) == 0;
}
