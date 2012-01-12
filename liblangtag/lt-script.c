/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-script.c
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

#include <string.h>
#include "lt-mem.h"
#include "lt-script.h"
#include "lt-script-private.h"


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
lt_script_t *
lt_script_ref(lt_script_t *script)
{
	g_return_val_if_fail (script != NULL, NULL);

	return lt_mem_ref(&script->parent);
}

void
lt_script_unref(lt_script_t *script)
{
	if (script)
		lt_mem_unref(&script->parent);
}

const gchar *
lt_script_get_name(const lt_script_t *script)
{
	g_return_val_if_fail (script != NULL, NULL);

	return script->description;
}

const gchar *
lt_script_get_tag(const lt_script_t *script)
{
	g_return_val_if_fail (script != NULL, NULL);

	return script->tag;
}

void
lt_script_dump(const lt_script_t *script)
{
	g_print("Script: %s [%s]\n",
		lt_script_get_tag(script),
		lt_script_get_name(script));
}

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
