/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-lang.c
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
#include "lt-lang.h"
#include "lt-lang-private.h"


struct _lt_lang_t {
	lt_mem_t  parent;
	gchar    *tag;
	gchar    *description;
	gchar    *suppress_script;
	gchar    *scope;
	gchar    *macrolanguage;
	gchar    *preferred_tag;
};

/*< private >*/

/*< protected >*/
lt_lang_t *
lt_lang_create(void)
{
	lt_lang_t *retval = lt_mem_alloc_object(sizeof (lt_lang_t));

	return retval;
}

void
lt_lang_set_name(lt_lang_t   *lang,
		 const gchar *description)
{
	g_return_if_fail (lang != NULL);
	g_return_if_fail (description != NULL);

	if (lang->description)
		lt_mem_remove_ref(&lang->parent, lang->description);
	lang->description = g_strdup(description);
	lt_mem_add_ref(&lang->parent, lang->description,
		       (lt_destroy_func_t)g_free);
}

void
lt_lang_set_tag(lt_lang_t   *lang,
		const gchar *subtag)
{
	g_return_if_fail (lang != NULL);
	g_return_if_fail (subtag != NULL);

	if (lang->tag)
		lt_mem_remove_ref(&lang->parent, lang->tag);
	lang->tag = g_strdup(subtag);
	lt_mem_add_ref(&lang->parent, lang->tag,
		       (lt_destroy_func_t)g_free);
}

void
lt_lang_set_preferred_tag(lt_lang_t   *lang,
			  const gchar *subtag)
{
	g_return_if_fail (lang != NULL);
	g_return_if_fail (subtag != NULL);

	if (lang->preferred_tag)
		lt_mem_remove_ref(&lang->parent, lang->preferred_tag);
	lang->preferred_tag = g_strdup(subtag);
	lt_mem_add_ref(&lang->parent, lang->preferred_tag,
		       (lt_destroy_func_t)g_free);
}

void
lt_lang_set_suppress_script(lt_lang_t   *lang,
			    const gchar *script)
{
	g_return_if_fail (lang != NULL);
	g_return_if_fail (script != NULL);

	if (lang->suppress_script)
		lt_mem_remove_ref(&lang->parent, lang->suppress_script);
	lang->suppress_script = g_strdup(script);
	lt_mem_add_ref(&lang->parent, lang->suppress_script,
		       (lt_destroy_func_t)g_free);
}

void
lt_lang_set_macro_language(lt_lang_t   *lang,
			   const gchar *macrolanguage)
{
	g_return_if_fail (lang != NULL);
	g_return_if_fail (macrolanguage != NULL);

	if (lang->macrolanguage)
		lt_mem_remove_ref(&lang->parent, lang->macrolanguage);
	lang->macrolanguage = g_strdup(macrolanguage);
	lt_mem_add_ref(&lang->parent, lang->macrolanguage,
		       (lt_destroy_func_t)g_free);
}

void
lt_lang_set_scope(lt_lang_t   *lang,
		  const gchar *scope)
{
	g_return_if_fail (lang != NULL);
	g_return_if_fail (scope != NULL);

	if (lang->scope)
		lt_mem_remove_ref(&lang->parent, lang->scope);
	lang->scope = g_strdup(scope);
	lt_mem_add_ref(&lang->parent, lang->scope,
		       (lt_destroy_func_t)g_free);
}

/*< public >*/
lt_lang_t *
lt_lang_ref(lt_lang_t *lang)
{
	g_return_val_if_fail (lang != NULL, NULL);

	return lt_mem_ref(&lang->parent);
}

void
lt_lang_unref(lt_lang_t *lang)
{
	if (lang)
		lt_mem_unref(&lang->parent);
}

const gchar *
lt_lang_get_name(const lt_lang_t *lang)
{
	g_return_val_if_fail (lang != NULL, NULL);

	return lang->description;
}

const gchar *
lt_lang_get_better_tag(const lt_lang_t *lang)
{
	const gchar *retval = lt_lang_get_preferred_tag(lang);

	if (!retval)
		retval = lt_lang_get_tag(lang);

	return retval;
}

const gchar *
lt_lang_get_tag(const lt_lang_t *lang)
{
	g_return_val_if_fail (lang != NULL, NULL);

	return lang->tag;
}

const gchar *
lt_lang_get_preferred_tag(const lt_lang_t *lang)
{
	g_return_val_if_fail (lang != NULL, NULL);

	return lang->preferred_tag;
}

const gchar *
lt_lang_get_suppress_script(const lt_lang_t *lang)
{
	g_return_val_if_fail (lang != NULL, NULL);

	return lang->suppress_script;
}

const gchar *
lt_lang_get_macro_language(const lt_lang_t *lang)
{
	g_return_val_if_fail (lang != NULL, NULL);

	return lang->macrolanguage;
}

const gchar *
lt_lang_get_scope(const lt_lang_t *lang)
{
	g_return_val_if_fail (lang != NULL, NULL);

	return lang->scope;
}
