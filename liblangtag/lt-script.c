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
	gchar    *name;
	gchar    *alpha_4_code;
	gchar    *numeric_code;
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
		   const gchar *name)
{
	g_return_if_fail (script != NULL);
	g_return_if_fail (name != NULL);

	if (script->name)
		lt_mem_remove_ref(&script->parent, script->name);
	script->name = g_strdup(name);
	lt_mem_add_ref(&script->parent, script->name,
		       (lt_destroy_func_t)g_free);
}

void
lt_script_set_code(lt_script_t *script,
		   const gchar *code)
{
	gsize i, len;
	gboolean is_numeric_code = TRUE;

	g_return_if_fail (script != NULL);
	g_return_if_fail (code != NULL);

	len = strlen(code);
	for (i = 0; i < len; i++) {
		if (code[i] < '0' || code[i] > '9') {
			is_numeric_code = FALSE;
			break;
		}
	}
	if (is_numeric_code) {
		if (script->numeric_code)
			lt_mem_remove_ref(&script->parent, script->numeric_code);
		script->numeric_code = g_strdup(code);
		lt_mem_add_ref(&script->parent, script->numeric_code,
			       (lt_destroy_func_t)g_free);
	} else {
		if (script->alpha_4_code)
			lt_mem_remove_ref(&script->parent, script->alpha_4_code);
		script->alpha_4_code = g_strdup(code);
		lt_mem_add_ref(&script->parent, script->alpha_4_code,
			       (lt_destroy_func_t)g_free);
	}
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

	return script->name;
}

const gchar *
lt_script_get_alpha_code(const lt_script_t *script)
{
	g_return_val_if_fail (script != NULL, NULL);

	return script->alpha_4_code;
}

const gchar *
lt_script_get_numeric_code(const lt_script_t *script)
{
	g_return_val_if_fail (script != NULL, NULL);

	return script->numeric_code;
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

	p = lt_script_get_alpha_code(script);
	for (i = 0; i < len; i++) {
		if (modifiers[i].script &&
		    g_ascii_strcasecmp(p, modifiers[i].script) == 0)
			return modifiers[i].modifier;
	}

	return NULL;
}
