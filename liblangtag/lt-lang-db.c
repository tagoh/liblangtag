/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-lang-db.c
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

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include "lt-error.h"
#include "lt-mem.h"
#include "lt-lang-private.h"
#include "lt-lang-db.h"


struct _lt_lang_db_t {
	lt_mem_t    parent;
	GHashTable *languages;
	GHashTable *lang_codes;
};

typedef gboolean (* lt_xpath_func_t) (lt_lang_db_t          *lang,
				      xmlDocPtr              doc,
				      lt_lang_db_options_t   options,
				      GError               **error);

/*< private >*/
static lt_lang_db_options_t
_lt_lang_db_options_get_scope_from_string(const xmlChar *str)
{
	static const gchar *scopes[] = {
		"I", "M", "C", "D", "R", "S", NULL
	};
	int i;

	for (i = 0; scopes[i] != NULL; i++) {
		if (g_strcmp0(scopes[i], (const gchar *)str) == 0)
			return i + LT_LANG_DB_SCOPE_BEGIN;
	}

	return 0;
}

static lt_lang_db_options_t
_lt_lang_db_options_get_type_from_string(const xmlChar *str)
{
	static const gchar *types[] = {
		"L", "E", "A", "H", "C", NULL
	};
	int i;

	for (i = 0; types[i] != NULL; i++) {
		if (g_strcmp0(types[i], (const gchar *)str) == 0)
			return i + LT_LANG_DB_TYPE_BEGIN;
	}

	/* XXX: what is the type "S" ? */

	return 0;
}

static gboolean
_lt_lang_db_parse(lt_lang_db_t          *lang,
		  const gchar           *filename,
		  lt_lang_db_options_t   options,
		  lt_xpath_func_t        func,
		  GError               **error)
{
	xmlParserCtxtPtr xmlparser = xmlNewParserCtxt();
	xmlDocPtr doc = NULL;
	GError *err = NULL;
	gboolean retval = TRUE;

	if (!xmlparser) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlParserCtxt.");
		goto bail;
	}
	doc = xmlCtxtReadFile(xmlparser, filename, "UTF-8", 0);
	if (!doc) {
		g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "Unable to read the xml file: %s",
			    filename);
		goto bail;
	}
	retval = func(lang, doc, options, &err);

  bail:
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
		retval = FALSE;
	}
	if (doc)
		xmlFreeDoc(doc);
	if (xmlparser)
		xmlFreeParserCtxt(xmlparser);

	xmlCleanupParser();

	return retval;
}

static gboolean
_lt_lang_db_parse_639(lt_lang_db_t          *lang,
		      xmlDocPtr              doc,
		      lt_lang_db_options_t   options,
		      GError               **error)
{
	xmlXPathContextPtr xctxt = xmlXPathNewContext(doc);
	xmlXPathObjectPtr xobj = NULL;
	gboolean retval = TRUE;
	int i, n;

	if (!xctxt) {
		g_set_error(error, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		retval = FALSE;
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/iso_639_entries/iso_639_entry", xctxt);
	if (!xobj) {
		g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "No valid elements for %s",
			    doc->name);
		retval = FALSE;
		goto bail;
	}
	n = xmlXPathNodeSetGetLength(xobj->nodesetval);

	for (i = 0; i < n; i++) {
		xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, i);
		xmlChar *p;
		lt_lang_t *le;

		if (!ent) {
			g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		le = lt_lang_create(LT_LANG_639_2);
		if (!le) {
			g_set_error(error, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_langt.");
			goto bail;
		}
		p = xmlGetProp(ent, (const xmlChar *)"name");
		lt_lang_set_name(le, (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(lang->languages,
				     (gchar *)lt_lang_get_name(le),
				     lt_lang_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"iso_639_1_code");
		if (p) {
			lt_lang_set_code(le,
					 LT_LANG_CODE_1,
					 (const gchar *)p);
			xmlFree(p);
			g_hash_table_replace(lang->lang_codes,
					     (gchar *)lt_lang_get_code(le, LT_LANG_CODE_1),
					     lt_lang_ref(le));
		}
		if (options & LT_LANG_DB_READ_BIBLIOGRAPHIC) {
			p = xmlGetProp(ent, (const xmlChar *)"iso_639_2B_code");
			lt_lang_set_code(le,
					 LT_LANG_CODE_2B,
					 (const gchar *)p);
			xmlFree(p);
			g_hash_table_replace(lang->lang_codes,
					     (gchar *)lt_lang_get_code(le, LT_LANG_CODE_2B),
					     lt_lang_ref(le));
		}
		if (options & LT_LANG_DB_READ_TERMINOLOGY) {
			p = xmlGetProp(ent, (const xmlChar *)"iso_639_2T_code");
			lt_lang_set_code(le,
					 LT_LANG_CODE_2T,
					 (const gchar *)p);
			xmlFree(p);
			g_hash_table_replace(lang->lang_codes,
					     (gchar *)lt_lang_get_code(le, LT_LANG_CODE_2T),
					     lt_lang_ref(le));
		}
		lt_lang_unref(le);
	}
  bail:
	if (xobj)
		xmlXPathFreeObject(xobj);
	if (xctxt)
		xmlXPathFreeContext(xctxt);

	return retval;
}

static gboolean
_lt_lang_db_parse_639_3(lt_lang_db_t          *lang,
			xmlDocPtr              doc,
			lt_lang_db_options_t   options,
			GError               **error)
{
	xmlXPathContextPtr xctxt = xmlXPathNewContext(doc);
	xmlXPathObjectPtr xobj = NULL;
	gboolean retval = TRUE;
	int i, n;

	if (!xctxt) {
		g_set_error(error, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		retval = FALSE;
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/iso_639_3_entries/iso_639_3_entry", xctxt);
	if (!xobj) {
		g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "No valid elements for %s",
			    doc->name);
		retval = FALSE;
		goto bail;
	}
	n = xmlXPathNodeSetGetLength(xobj->nodesetval);

	for (i = 0; i < n; i++) {
		xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, i);
		xmlChar *p, *type, *scope;
		lt_lang_t *le;
		lt_lang_db_options_t oscope, otype;

		if (!ent) {
			g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		scope = xmlGetProp(ent, (const xmlChar *)"scope");
		type = xmlGetProp(ent, (const xmlChar *)"type");
		oscope = _lt_lang_db_options_get_scope_from_string(scope);
		otype = _lt_lang_db_options_get_type_from_string(type);
		xmlFree(scope);
		xmlFree(type);
		if ((options & (oscope|otype))) {
			le = lt_lang_create(LT_LANG_639_3);
			if (!lang) {
				g_set_error(error, LT_ERROR, LT_ERR_OOM,
					    "Unable to create an instance of lt_lang_t.");
				goto bail;
			}
			p = xmlGetProp(ent, (const xmlChar *)"name");
			lt_lang_set_name(le, (const gchar *)p);
			xmlFree(p);
			g_hash_table_replace(lang->languages,
					     (gchar *)lt_lang_get_name(le),
					     lt_lang_ref(le));
			p = xmlGetProp(ent, (const xmlChar *)"id");
			lt_lang_set_code(le,
					 LT_LANG_CODE_ID,
					 (const gchar *)p);
			xmlFree(p);
			g_hash_table_replace(lang->lang_codes,
					     (gchar *)lt_lang_get_code(le, LT_LANG_CODE_ID),
					     lt_lang_ref(le));
			p = xmlGetProp(ent, (const xmlChar *)"part1_code");
			if (p) {
				lt_lang_set_code(le,
						 LT_LANG_CODE_PART1,
						 (const gchar *)p);
				xmlFree(p);
				g_hash_table_replace(lang->lang_codes,
						     (gchar *)lt_lang_get_code(le, LT_LANG_CODE_PART1),
						     lt_lang_ref(le));
			}
			p = xmlGetProp(ent, (const xmlChar *)"part2_code");
			if (p) {
				lt_lang_set_code(le,
						 LT_LANG_CODE_PART2,
						 (const gchar *)p);
				xmlFree(p);
				g_hash_table_replace(lang->lang_codes,
						     (gchar *)lt_lang_get_code(le, LT_LANG_CODE_PART2),
						     lt_lang_ref(le));
			}
			lt_lang_unref(le);
		}
	}
  bail:
	if (xobj)
		xmlXPathFreeObject(xobj);
	if (xctxt)
		xmlXPathFreeContext(xctxt);

	return retval;
}

static gboolean
lt_lang_db_parse(lt_lang_db_t          *lang,
		 lt_lang_db_options_t   options,
		 GError               **error)
{
	gchar *iso639, *iso639_3;
	gboolean retval;

	g_return_val_if_fail (lang != NULL, FALSE);

	iso639 = g_build_filename(ISO_PREFIX, "iso_639.xml", NULL);
	iso639_3 = g_build_filename(ISO_PREFIX, "iso_639_3.xml", NULL);

	if (!(retval = _lt_lang_db_parse(lang, iso639, options,
				      _lt_lang_db_parse_639,
				      error)))
		goto bail;
	if (!(retval = _lt_lang_db_parse(lang, iso639_3, options,
				      _lt_lang_db_parse_639_3,
				      error)))
		goto bail;

  bail:
	g_free(iso639);
	g_free(iso639_3);

	return retval;
}

/*< public >*/
lt_lang_db_t *
lt_lang_db_new(lt_lang_db_options_t options)
{
	lt_lang_db_t *retval = lt_mem_alloc_object(sizeof (lt_lang_db_t));

	if (retval) {
		GError *err = NULL;

		retval->languages = g_hash_table_new_full(g_str_hash,
							  g_str_equal,
							  NULL,
							  (GDestroyNotify)lt_lang_unref);
		lt_mem_add_ref(&retval->parent, retval->languages,
			       (lt_destroy_func_t)g_hash_table_destroy);
		retval->lang_codes = g_hash_table_new_full(g_str_hash,
							   g_str_equal,
							   NULL,
							   (GDestroyNotify)lt_lang_unref);
		lt_mem_add_ref(&retval->parent, retval->lang_codes,
			       (lt_destroy_func_t)g_hash_table_destroy);

		lt_lang_db_parse(retval, options, &err);
		if (err) {
			g_printerr(err->message);
			lt_lang_db_unref(retval);
			retval = NULL;
			g_error_free(err);
		}
	}

	return retval;
}

lt_lang_db_t *
lt_lang_db_ref(lt_lang_db_t *lang)
{
	g_return_val_if_fail (lang != NULL, NULL);

	return lt_mem_ref(&lang->parent);
}

void
lt_lang_db_unref(lt_lang_db_t *lang)
{
	if (lang)
		lt_mem_unref(&lang->parent);
}

GList *
lt_lang_db_get_languages(lt_lang_db_t *lang)
{
	g_return_val_if_fail (lang != NULL, NULL);

	return g_hash_table_get_keys(lang->languages);
}

lt_lang_t *
lt_lang_db_lookup_from_code(lt_lang_db_t *lang,
			    const gchar  *code)
{
	lt_lang_t *retval;

	g_return_val_if_fail (lang != NULL, NULL);
	g_return_val_if_fail (code != NULL, NULL);

	retval = g_hash_table_lookup(lang->lang_codes, code);
	if (retval)
		return lt_lang_ref(retval);

	return NULL;
}

lt_lang_t *
lt_lang_db_lookup_from_language(lt_lang_db_t *lang,
				const gchar  *language)
{
	lt_lang_t *retval;

	g_return_val_if_fail (lang != NULL, NULL);
	g_return_val_if_fail (language != NULL, NULL);

	retval = g_hash_table_lookup(lang->languages, language);
	if (retval)
		return lt_lang_ref(retval);

	return NULL;
}
