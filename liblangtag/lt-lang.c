/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-lang.c
 * Copyright (C) 2012 Akira TAGOH
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
#include "lt-lang-db.h"
#include "lt-lang.h"
#include "lt-lang-private.h"


struct _lt_lang_t {
	lt_mem_t              parent;
	lt_lang_entry_type_t  type;
	gchar                *name;
	union {
		struct {
			gchar *_2B_code;
			gchar *_2T_code;
			gchar *_1_code;
		} _639_2;
		struct {
			gchar *part2_code;
			gchar *id;
			gchar *part1_code;
		} _639_3;
	} iso;
};

/*< private >*/

/*< protected >*/
lt_lang_t *
lt_lang_create(lt_lang_entry_type_t type)
{
	lt_lang_t *retval = lt_mem_alloc_object(sizeof (lt_lang_t));

	if (retval) {
		retval->type = type;
	}

	return retval;
}

void
lt_lang_set_name(lt_lang_t   *lang,
		 const gchar *name)
{
	g_return_if_fail (lang != NULL);
	g_return_if_fail (name != NULL);

	if (lang->name)
		lt_mem_remove_ref(&lang->parent, lang->name);
	lang->name = g_strdup(name);
	lt_mem_add_ref(&lang->parent, lang->name,
		       (lt_destroy_func_t)g_free);
}

void
lt_lang_set_code(lt_lang_t      *lang,
		 lt_lang_code_t  code_type,
		 const gchar    *code)
{
	gchar **p;

	g_return_if_fail (lang != NULL);
	g_return_if_fail (code != NULL);

	switch (code_type) {
	    case LT_LANG_CODE_1:
		    if (lang->type == LT_LANG_639_2)
			    p = &lang->iso._639_2._1_code;
		    else if (lang->type == LT_LANG_639_3)
			    p = &lang->iso._639_3.part1_code;
		    else {
			    g_warning("Unknown lang entry type: %d\n", lang->type);
			    return;
		    }
		    break;
	    case LT_LANG_CODE_2B:
		    if (lang->type == LT_LANG_639_2)
			    p = &lang->iso._639_2._2B_code;
		    else if (lang->type == LT_LANG_639_3)
			    p = &lang->iso._639_3.part2_code;
		    else {
			    g_warning("Unknown lang entry type: %d\n", lang->type);
			    return;
		    }
		    break;
	    case LT_LANG_CODE_2T:
		    if (lang->type == LT_LANG_639_2)
			    p = &lang->iso._639_2._2T_code;
		    else if (lang->type == LT_LANG_639_3)
			    p = &lang->iso._639_3.id;
		    else {
			    g_warning("Unknown lang entry type: %d\n", lang->type);
			    return;
		    }
		    break;
	    default:
		    g_warning("Unkonwn lang code type: %d\n", code_type);
		    return;
	}
	if (*p)
		lt_mem_remove_ref(&lang->parent, *p);
	*p = g_strdup(code);
	lt_mem_add_ref(&lang->parent, *p,
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

	return lang->name;
}

const gchar *
lt_lang_get_code(const lt_lang_t *lang,
		 lt_lang_code_t   code_type)
{
	const gchar *retval;

	g_return_val_if_fail (lang != NULL, NULL);

	switch (code_type) {
	    case LT_LANG_CODE_1:
		    if (lang->type == LT_LANG_639_2)
			    retval = lang->iso._639_2._1_code;
		    else if (lang->type == LT_LANG_639_3)
			    retval = lang->iso._639_3.part1_code;
		    else {
			    g_warning("Unknown lang entry type: %d\n", lang->type);
			    retval = NULL;
		    }
		    break;
	    case LT_LANG_CODE_2B:
		    if (lang->type == LT_LANG_639_2)
			    retval = lang->iso._639_2._2B_code;
		    else if (lang->type == LT_LANG_639_3)
			    retval = lang->iso._639_3.part2_code;
		    else {
			    g_warning("Unknown lang entry type: %d\n", lang->type);
			    retval = NULL;
		    }
		    break;
	    case LT_LANG_CODE_2T:
		    if (lang->type == LT_LANG_639_2)
			    retval = lang->iso._639_2._2T_code;
		    else if (lang->type == LT_LANG_639_3)
			    retval = lang->iso._639_3.id;
		    else {
			    g_warning("Unknown lang entry type: %d\n", lang->type);
			    retval = NULL;
		    }
		    break;
	    default:
		    g_warning("Unknown lang code type: %d", code_type);
		    retval = NULL;
	}

	return retval;
}
