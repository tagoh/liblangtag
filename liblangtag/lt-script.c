/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-script.c
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

#include <string.h>
#include "lt-mem.h"
#include "lt-script.h"
#include "lt-script-private.h"


/**
 * SECTION: lt-script
 * @Short_Description: A container class for Script subtag
 * @Title: Container - Script
 *
 * This container class provides a data access to Script subtag entry.
 */
struct _lt_script_t {
	lt_mem_t  parent;
	gchar    *tag;
	gchar    *description;
};


/*< private >*/

/*< protected >*/
lt_script_t *
lt_script_create(void)
{
	lt_script_t *retval = lt_mem_alloc_object(sizeof (lt_script_t));

	return retval;
}

void
lt_script_set_name(lt_script_t *script,
		   const gchar *description)
{
	g_return_if_fail (script != NULL);
	g_return_if_fail (description != NULL);

	if (script->description)
		lt_mem_remove_ref(&script->parent, script->description);
	script->description = g_strdup(description);
	lt_mem_add_ref(&script->parent, script->description,
		       (lt_destroy_func_t)g_free);
}

void
lt_script_set_tag(lt_script_t *script,
		  const gchar *subtag)
{
	g_return_if_fail (script != NULL);
	g_return_if_fail (subtag != NULL);

	if (script->tag)
		lt_mem_remove_ref(&script->parent, script->tag);
	script->tag = g_strdup(subtag);
	lt_mem_add_ref(&script->parent, script->tag,
		       (lt_destroy_func_t)g_free);
}

/*< public >*/
/**
 * lt_script_ref:
 * @script: a #lt_script_t.
 *
 * Increases the reference count of @script.
 *
 * Returns: (transfer none): the same @script object.
 */
lt_script_t *
lt_script_ref(lt_script_t *script)
{
	g_return_val_if_fail (script != NULL, NULL);

	return lt_mem_ref(&script->parent);
}

/**
 * lt_script_unref:
 * @script: a #lt_script_t.
 *
 * Decreases the reference count of @script. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_script_unref(lt_script_t *script)
{
	if (script)
		lt_mem_unref(&script->parent);
}

/**
 * lt_script_get_name:
 * @script: a #lt_script_t.
 *
 * Obtains the description of the subtag.
 *
 * Returns: a description string.
 */
const gchar *
lt_script_get_name(const lt_script_t *script)
{
	g_return_val_if_fail (script != NULL, NULL);

	return script->description;
}

/**
 * lt_script_get_tag:
 * @script: a #lt_script_t.
 *
 * Obtains the tag name.
 *
 * Returns: a tag string.
 */
const gchar *
lt_script_get_tag(const lt_script_t *script)
{
	g_return_val_if_fail (script != NULL, NULL);

	return script->tag;
}

/**
 * lt_script_dump:
 * @script: a #lt_script_t.
 *
 * Dumps the container information to the standard output.
 */
void
lt_script_dump(const lt_script_t *script)
{
	g_print("Script: %s [%s]\n",
		lt_script_get_tag(script),
		lt_script_get_name(script));
}

/**
 * lt_script_convert_to_modifier:
 * @script: a #lt_script_t.
 *
 * Convert the script subtag to the locale variant modifier.
 *
 * Returns: a modifier string or %NULL.
 */
const gchar *
lt_script_convert_to_modifier(const lt_script_t *script)
{
	const gchar *p;
	static const struct {
		gchar *modifier;
		gchar *script;
	} modifiers[] = {
		{"abegede", NULL},
		{"cyrillic", "Cyrl"},
		{"cyrillic", "Cyrs"},
		{"devanagari", "Deva"},
		{"euro", NULL},
		{"iqtelif", NULL},
		{"latin", "Latf"},
		{"latin", "Latg"},
		{"latin", "Latn"},
		{"saaho", NULL},
		{NULL, NULL}
	};
	static gsize len = G_N_ELEMENTS(modifiers), i;

	g_return_val_if_fail (script != NULL, NULL);

	p = lt_script_get_tag(script);
	for (i = 0; i < len; i++) {
		if (modifiers[i].script &&
		    g_ascii_strcasecmp(p, modifiers[i].script) == 0)
			return modifiers[i].modifier;
	}

	return NULL;
}

/**
 * lt_script_compare:
 * @v1: a #lt_script_t.
 * @v2: a #lt_script_t.
 *
 * Compare if @v1 and @v2 is the same object or not.
 *
 * Returns: %TRUE if it's the same, otherwise %FALSE.
 */
gboolean
lt_script_compare(const lt_script_t *v1,
		  const lt_script_t *v2)
{
	const gchar *s1, *s2;

	if (v1 == v2)
		return TRUE;

	s1 = v1 ? lt_script_get_tag(v1) : NULL;
	s2 = v2 ? lt_script_get_tag(v2) : NULL;

	if (g_strcmp0(s1, "*") == 0 ||
	    g_strcmp0(s2, "*") == 0)
		return TRUE;

	return g_strcmp0(s1, s2) == 0;
}
