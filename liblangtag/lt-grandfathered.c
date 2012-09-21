/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-grandfathered.c
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

#include "lt-mem.h"
#include "lt-grandfathered.h"
#include "lt-grandfathered-private.h"


/**
 * SECTION: lt-grandfathered
 * @Short_Description: A container class for Grandfathered subtag
 * @Title: Container - Grandfathered
 *
 * This container class provides a data access to Grandfathered subtag entry.
 */
struct _lt_grandfathered_t {
	lt_mem_t  parent;
	char     *tag;
	char     *description;
	char     *preferred_tag;
};

/*< private >*/

/*< protected >*/
lt_grandfathered_t *
lt_grandfathered_create(void)
{
	lt_grandfathered_t *retval;

	retval = lt_mem_alloc_object(sizeof (lt_grandfathered_t));

	return retval;
}

void
lt_grandfathered_set_tag(lt_grandfathered_t *grandfathered,
			 const char         *tag)
{
	g_return_if_fail (grandfathered != NULL);
	g_return_if_fail (tag != NULL);

	if (grandfathered->tag)
		lt_mem_remove_ref(&grandfathered->parent, grandfathered->tag);
	grandfathered->tag = g_strdup(tag);
	lt_mem_add_ref(&grandfathered->parent, grandfathered->tag,
		       (lt_destroy_func_t)g_free);
}

void
lt_grandfathered_set_name(lt_grandfathered_t *grandfathered,
			  const char         *description)
{
	g_return_if_fail (grandfathered != NULL);
	g_return_if_fail (description != NULL);

	if (grandfathered->description)
		lt_mem_remove_ref(&grandfathered->parent, grandfathered->description);
	grandfathered->description = g_strdup(description);
	lt_mem_add_ref(&grandfathered->parent, grandfathered->description,
		       (lt_destroy_func_t)g_free);
}

void
lt_grandfathered_set_preferred_tag(lt_grandfathered_t *grandfathered,
				   const char         *subtag)
{
	g_return_if_fail (grandfathered != NULL);
	g_return_if_fail (subtag != NULL);

	if (grandfathered->preferred_tag)
		lt_mem_remove_ref(&grandfathered->parent, grandfathered->preferred_tag);
	grandfathered->preferred_tag = g_strdup(subtag);
	lt_mem_add_ref(&grandfathered->parent, grandfathered->preferred_tag,
		       (lt_destroy_func_t)g_free);
}

/*< public >*/
/**
 * lt_grandfathered_ref:
 * @grandfathered: a #lt_grandfathered_t.
 *
 * Increases the reference count of @grandfathered.
 *
 * Returns: (transfer none): the same @grandfathered object.
 */
lt_grandfathered_t *
lt_grandfathered_ref(lt_grandfathered_t *grandfathered)
{
	g_return_val_if_fail (grandfathered != NULL, NULL);

	return lt_mem_ref(&grandfathered->parent);
}

/**
 * lt_grandfathered_unref:
 * @grandfathered: a #lt_grandfathered_t.
 *
 * Decreases the reference count of @grandfathered. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_grandfathered_unref(lt_grandfathered_t *grandfathered)
{
	if (grandfathered)
		lt_mem_unref(&grandfathered->parent);
}

/**
 * lt_grandfathered_get_better_tag:
 * @grandfathered: a #lt_grandfathered_t.
 *
 * Obtains the better tag for use. this is a convenient function to get
 * the preferred-value if available.
 *
 * Returns: a tag string.
 */
const char *
lt_grandfathered_get_better_tag(const lt_grandfathered_t *grandfathered)
{
	const char *retval = lt_grandfathered_get_preferred_tag(grandfathered);

	if (!retval)
		retval = lt_grandfathered_get_tag(grandfathered);

	return retval;
}

/**
 * lt_grandfathered_get_tag:
 * @grandfathered: a #lt_grandfathered_t.
 *
 * Obtains the tag name.
 *
 * Returns: a tag string.
 */
const char *
lt_grandfathered_get_tag(const lt_grandfathered_t *grandfathered)
{
	g_return_val_if_fail (grandfathered != NULL, NULL);

	return grandfathered->tag;
}

/**
 * lt_grandfathered_get_name:
 * @grandfathered: a #lt_grandfathered_t.
 *
 * Obtains the description of the tag.
 *
 * Returns: a description string.
 */
const char *
lt_grandfathered_get_name(const lt_grandfathered_t *grandfathered)
{
	g_return_val_if_fail (grandfathered != NULL, NULL);

	return grandfathered->description;
}

/**
 * lt_grandfathered_get_preferred_tag:
 * @grandfathered: a #lt_grandfathered_t.
 *
 * Obtains the preferred-value. this is available only when the tag is
 * marked as deprecated.
 *
 * Returns: a preferred-value for the tag or %NULL.
 */
const char *
lt_grandfathered_get_preferred_tag(const lt_grandfathered_t *grandfathered)
{
	g_return_val_if_fail (grandfathered != NULL, NULL);

	return grandfathered->preferred_tag;
}

/**
 * lt_grandfathered_dump:
 * @grandfathered: a #lt_grandfathered_t.
 *
 * Dumps the container information to the standard output.
 */
void
lt_grandfathered_dump(const lt_grandfathered_t *grandfathered)
{
	const char *preferred = lt_grandfathered_get_preferred_tag(grandfathered);
	GString *string = g_string_new(NULL);

	if (preferred) {
		if (string->len == 0)
			g_string_append(string, " (");
		g_string_append_printf(string, "preferred-value: %s",
				       preferred);
	}
	if (string->len > 0)
		g_string_append(string, ")");

	g_print("Grandfathered: %s [%s]%s\n",
		lt_grandfathered_get_tag(grandfathered),
		lt_grandfathered_get_name(grandfathered),
		string->str);
	g_string_free(string, TRUE);
}

/**
 * lt_grandfathered_compare:
 * @v1: a #lt_grandfathered_t.
 * @v2: a #lt_grandfathered_t.
 *
 * Compare if @v1 and @v2 is the same object or not.
 *
 * Returns: %TRUE if it's the same. otherwise %FALSE.
 */
lt_bool_t
lt_grandfathered_compare(const lt_grandfathered_t *v1,
			 const lt_grandfathered_t *v2)
{
	g_return_val_if_fail (v1 != NULL, FALSE);
	g_return_val_if_fail (v2 != NULL, FALSE);

	if (v1 == v2)
		return TRUE;

	return g_strcmp0(lt_grandfathered_get_tag(v1), lt_grandfathered_get_tag(v2)) == 0;
}
