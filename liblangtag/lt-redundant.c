/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-redundant.c
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
#include "lt-redundant.h"
#include "lt-redundant-private.h"


/**
 * SECTION: lt-redundant
 * @Short_Description: A container class for Redundant subtag
 * @Title: Container - Redundant
 *
 * This container class provides a data access to Redundant subtag entry.
 */
struct _lt_redundant_t {
	lt_mem_t  parent;
	char     *tag;
	char     *description;
	char     *preferred_tag;
};

/*< private >*/

/*< protected >*/
lt_redundant_t *
lt_redundant_create(void)
{
	lt_redundant_t *retval;

	retval = lt_mem_alloc_object(sizeof (lt_redundant_t));

	return retval;
}

void
lt_redundant_set_tag(lt_redundant_t *redundant,
		     const char     *tag)
{
	lt_return_if_fail (redundant != NULL);
	lt_return_if_fail (tag != NULL);

	if (redundant->tag)
		lt_mem_remove_ref(&redundant->parent, redundant->tag);
	redundant->tag = strdup(tag);
	lt_mem_add_ref(&redundant->parent, redundant->tag, free);
}

void
lt_redundant_set_name(lt_redundant_t *redundant,
		      const char     *description)
{
	lt_return_if_fail (redundant != NULL);
	lt_return_if_fail (description != NULL);

	if (redundant->description)
		lt_mem_remove_ref(&redundant->parent, redundant->description);
	redundant->description = strdup(description);
	lt_mem_add_ref(&redundant->parent, redundant->description, free);
}

void
lt_redundant_set_preferred_tag(lt_redundant_t *redundant,
			       const char     *subtag)
{
	lt_return_if_fail (redundant != NULL);
	lt_return_if_fail (subtag != NULL);

	if (redundant->preferred_tag)
		lt_mem_remove_ref(&redundant->parent, redundant->preferred_tag);
	redundant->preferred_tag = strdup(subtag);
	lt_mem_add_ref(&redundant->parent, redundant->preferred_tag, free);
}

/*< public >*/
/**
 * lt_redundant_ref:
 * @redundant: a #lt_redundant_t.
 *
 * Increases the reference count of @redundant.
 *
 * Returns: (transfer none): the same @redundant object.
 */
lt_redundant_t *
lt_redundant_ref(lt_redundant_t *redundant)
{
	lt_return_val_if_fail (redundant != NULL, NULL);

	return lt_mem_ref(&redundant->parent);
}

/**
 * lt_redundant_unref:
 * @redundant: a #lt_redundant_t.
 *
 * Decreases the reference count of @redundant. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_redundant_unref(lt_redundant_t *redundant)
{
	if (redundant)
		lt_mem_unref(&redundant->parent);
}

/**
 * lt_redundant_get_better_tag:
 * @redundant: a #lt_redundant_t.
 *
 * Obtains the better tag for use. this is a convenient function to get
 * the preferred-value if available.
 *
 * Returns: a tag string.
 */
const char *
lt_redundant_get_better_tag(const lt_redundant_t *redundant)
{
	const char *retval = lt_redundant_get_preferred_tag(redundant);

	if (!retval)
		retval = lt_redundant_get_tag(redundant);

	return retval;
}

/**
 * lt_redundant_get_tag:
 * @redundant: a #lt_redundant_t.
 *
 * Obtains the tag name.
 *
 * Returns: a tag string.
 */
const char *
lt_redundant_get_tag(const lt_redundant_t *redundant)
{
	lt_return_val_if_fail (redundant != NULL, NULL);

	return redundant->tag;
}

/**
 * lt_redundant_get_name:
 * @redundant: a #lt_redundant_t.
 *
 * Obtains the description of the tag.
 *
 * Returns: a description string.
 */
const char *
lt_redundant_get_name(const lt_redundant_t *redundant)
{
	lt_return_val_if_fail (redundant != NULL, NULL);

	return redundant->description;
}

/**
 * lt_redundant_get_preferred_tag:
 * @redundant: a #lt_redundant_t.
 *
 * Obtains the preferred-value. this is available only when the tag is
 * marked as deprecated.
 *
 * Returns: a preferred-value for the tag or %NULL.
 */
const char *
lt_redundant_get_preferred_tag(const lt_redundant_t *redundant)
{
	lt_return_val_if_fail (redundant != NULL, NULL);

	return redundant->preferred_tag;
}

/**
 * lt_redundant_dump:
 * @redundant: a #lt_redundant_t.
 *
 * Dumps the container information to the standard output.
 */
void
lt_redundant_dump(const lt_redundant_t *redundant)
{
	const char *preferred = lt_redundant_get_preferred_tag(redundant);
	lt_string_t *string = lt_string_new(NULL);

	if (preferred) {
		if (lt_string_length(string) == 0)
			lt_string_append(string, " (");
		lt_string_append_printf(string, "preferred-value: %s",
					preferred);
	}
	if (lt_string_length(string) > 0)
		lt_string_append(string, ")");

	lt_info("Redundant: %s [%s]%s",
		lt_redundant_get_tag(redundant),
		lt_redundant_get_name(redundant),
		lt_string_value(string));

	lt_string_unref(string);
}

/**
 * lt_redundant_compare:
 * @v1: a #lt_redundant_t.
 * @v2: a #lt_redundant_t.
 *
 * Compare if @v1 and @v2 is the same object or not.
 *
 * Returns: %TRUE if it's the same. otherwise %FALSE.
 */
lt_bool_t
lt_redundant_compare(const lt_redundant_t *v1,
		     const lt_redundant_t *v2)
{
	lt_return_val_if_fail (v1 != NULL, FALSE);
	lt_return_val_if_fail (v2 != NULL, FALSE);

	if (v1 == v2)
		return TRUE;

	return lt_strcmp0(lt_redundant_get_tag(v1), lt_redundant_get_tag(v2)) == 0;
}
