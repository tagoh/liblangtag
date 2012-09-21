/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-region.c
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
#include "lt-region.h"
#include "lt-region-private.h"


/**
 * SECTION:lt-region
 * @Short_Description: A container class for Region subtag
 * @Title: Container - Region
 *
 * This container class provides a data access to Region subtag entry.
 */
struct _lt_region_t {
	lt_mem_t  parent;
	char     *tag;
	char     *description;
	char     *preferred_tag;
};


/*< private >*/

/*< protected >*/
lt_region_t *
lt_region_create(void)
{
	lt_region_t *retval = lt_mem_alloc_object(sizeof (lt_region_t));

	return retval;
}

void
lt_region_set_name(lt_region_t *region,
		   const char  *description)
{
	g_return_if_fail (region != NULL);
	g_return_if_fail (description != NULL);

	if (region->description)
		lt_mem_remove_ref(&region->parent, region->description);
	region->description = g_strdup(description);
	lt_mem_add_ref(&region->parent, region->description,
		       (lt_destroy_func_t)g_free);
}

void
lt_region_set_tag(lt_region_t *region,
		  const char  *subtag)
{
	g_return_if_fail (region != NULL);
	g_return_if_fail (subtag != NULL);

	if (region->tag)
		lt_mem_remove_ref(&region->parent, region->tag);
	region->tag = g_strdup(subtag);
	lt_mem_add_ref(&region->parent, region->tag,
		       (lt_destroy_func_t)g_free);
}

void
lt_region_set_preferred_tag(lt_region_t *region,
			    const char  *subtag)
{
	g_return_if_fail (region != NULL);
	g_return_if_fail (subtag != NULL);

	if (region->preferred_tag)
		lt_mem_remove_ref(&region->parent, region->preferred_tag);
	region->preferred_tag = g_strdup(subtag);
	lt_mem_add_ref(&region->parent, region->preferred_tag,
		       (lt_destroy_func_t)g_free);
}

/*< public >*/
/**
 * lt_region_ref:
 * @region: a #lt_region_t.
 *
 * Increases the reference count of @region.
 *
 * Returns: (transfer none): the same @region object.
 */
lt_region_t *
lt_region_ref(lt_region_t *region)
{
	g_return_val_if_fail (region != NULL, NULL);

	return lt_mem_ref(&region->parent);
}

/**
 * lt_region_unref:
 * @region: a #lt_region_t.
 *
 * Decreases the reference count of @region. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_region_unref(lt_region_t *region)
{
	if (region)
		lt_mem_unref(&region->parent);
}

/**
 * lt_region_get_name:
 * @region: a #lt_region_t.
 *
 * Obtains the description of the subtag.
 *
 * Returns: a description string.
 */
const char *
lt_region_get_name(const lt_region_t *region)
{
	g_return_val_if_fail (region != NULL, NULL);

	return region->description;
}

/**
 * lt_region_get_better_tag:
 * @region: a #lt_region_t.
 *
 * Obtains the better tag for use. this is a convenient function to get
 * the preferred-value if available.
 *
 * Returns: a tag string.
 */
const char *
lt_region_get_better_tag(const lt_region_t *region)
{
	const char *retval = lt_region_get_preferred_tag(region);

	if (!retval)
		retval = lt_region_get_tag(region);

	return retval;
}

/**
 * lt_region_get_tag:
 * @region: a #lt_region_t.
 *
 * Obtains the tag name.
 *
 * Returns: a tag string.
 */
const char *
lt_region_get_tag(const lt_region_t *region)
{
	g_return_val_if_fail (region != NULL, NULL);

	return region->tag;
}

/**
 * lt_region_get_preferred_tag:
 * @region: a #lt_region_t.
 *
 * Obtains the preferred-value. this is available only when the tag is
 * marked as deprecated.
 *
 * Returns: a preferred-value for the tag or %NULL.
 */
const char *
lt_region_get_preferred_tag(const lt_region_t *region)
{
	g_return_val_if_fail (region != NULL, NULL);

	return region->preferred_tag;
}

/**
 * lt_region_dump:
 * @region: a #lt_region_t.
 *
 * Dumps the container information to the standard output.
 */
void
lt_region_dump(const lt_region_t *region)
{
	GString *string = g_string_new(NULL);
	const char *preferred = lt_region_get_preferred_tag(region);

	if (preferred) {
		if (string->len == 0)
			g_string_append(string, " (");
		g_string_append_printf(string, "preferred-value: %s", preferred);
	}
	if (string->len > 0)
		g_string_append(string, ")");

	g_print("Region: %s [%s]%s\n",
		lt_region_get_tag(region),
		lt_region_get_name(region),
		string->str);
	g_string_free(string, TRUE);
}

/**
 * lt_region_compare:
 * @v1: a #lt_region_t.
 * @v2: a #lt_region_t.
 *
 * Compare if @v1 and @v2 is the same object or not.
 *
 * Returns: %TRUE if it's the same, otherwise %FALSE.
 */
lt_bool_t
lt_region_compare(const lt_region_t *v1,
		  const lt_region_t *v2)
{
	const char *s1, *s2;

	if (v1 == v2)
		return TRUE;

	s1 = v1 ? lt_region_get_tag(v1) : NULL;
	s2 = v2 ? lt_region_get_tag(v2) : NULL;

	if (g_strcmp0(s1, "*") == 0 ||
	    g_strcmp0(s2, "*") == 0)
		return TRUE;

	return g_strcmp0(s1, s2) == 0;
}
