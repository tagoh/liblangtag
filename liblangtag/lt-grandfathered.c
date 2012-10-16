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

#include <stdlib.h>
#include <string.h>
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-string.h"
#include "lt-utils.h"
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
	lt_return_if_fail (grandfathered != NULL);
	lt_return_if_fail (tag != NULL);

	if (grandfathered->tag)
		lt_mem_delete_ref(&grandfathered->parent, grandfathered->tag);
	grandfathered->tag = strdup(tag);
	lt_mem_add_ref(&grandfathered->parent, grandfathered->tag, free);
}

void
lt_grandfathered_set_name(lt_grandfathered_t *grandfathered,
			  const char         *description)
{
	lt_return_if_fail (grandfathered != NULL);
	lt_return_if_fail (description != NULL);

	if (grandfathered->description)
		lt_mem_delete_ref(&grandfathered->parent, grandfathered->description);
	grandfathered->description = strdup(description);
	lt_mem_add_ref(&grandfathered->parent, grandfathered->description, free);
}

void
lt_grandfathered_set_preferred_tag(lt_grandfathered_t *grandfathered,
				   const char         *subtag)
{
	lt_return_if_fail (grandfathered != NULL);
	lt_return_if_fail (subtag != NULL);

	if (grandfathered->preferred_tag)
		lt_mem_delete_ref(&grandfathered->parent, grandfathered->preferred_tag);
	grandfathered->preferred_tag = strdup(subtag);
	lt_mem_add_ref(&grandfathered->parent, grandfathered->preferred_tag, free);
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
	lt_return_val_if_fail (grandfathered != NULL, NULL);

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
	lt_return_val_if_fail (grandfathered != NULL, NULL);

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
	lt_return_val_if_fail (grandfathered != NULL, NULL);

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
	lt_return_val_if_fail (grandfathered != NULL, NULL);

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
	lt_string_t *string = lt_string_new(NULL);

	if (preferred) {
		if (lt_string_length(string) == 0)
			lt_string_append(string, " (");
		lt_string_append_printf(string, "preferred-value: %s",
					preferred);
	}
	if (lt_string_length(string) > 0)
		lt_string_append(string, ")");

	lt_info("Grandfathered: %s [%s]%s",
		lt_grandfathered_get_tag(grandfathered),
		lt_grandfathered_get_name(grandfathered),
		lt_string_value(string));

	lt_string_unref(string);
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
	lt_return_val_if_fail (v1 != NULL, FALSE);
	lt_return_val_if_fail (v2 != NULL, FALSE);

	if (v1 == v2)
		return TRUE;

	return lt_strcmp0(lt_grandfathered_get_tag(v1), lt_grandfathered_get_tag(v2)) == 0;
}
