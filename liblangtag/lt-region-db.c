/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-region-db.c
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
#include "lt-division.h"
#include "lt-division-private.h"
#include "lt-error.h"
#include "lt-mem.h"
#include "lt-region.h"
#include "lt-region-private.h"
#include "lt-region-db.h"


struct _lt_region_db_t {
	lt_mem_t    parent;
	GHashTable *region_entries;
	GHashTable *region_codes;
	GHashTable *division_entries;
	GHashTable *division_lists;
};

typedef gboolean (* lt_xpath_func_t) (lt_region_db_t  *region,
				      xmlDocPtr        doc,
				      GError         **error);

/*< private >*/
static void
_lt_region_db_division_list_free(GList *list)
{
	GList *l = list;

	for (; l != NULL; l = g_list_next(l)) {
		lt_division_unref(l->data);
	}
	g_list_free(list);
}

static gboolean
_lt_region_db_parse(lt_region_db_t   *region,
		    const gchar      *filename,
		    lt_xpath_func_t   func,
		    GError          **error)
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
	retval = func(region, doc, &err);

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
_lt_region_db_parse_3166(lt_region_db_t  *region,
			 xmlDocPtr        doc,
			 GError         **error)
{
	xmlXPathContextPtr xctxt = xmlXPathNewContext(doc);
	xmlXPathObjectPtr xobj = NULL;
	gboolean retval = TRUE;
	int i, n;
	GError *err = NULL;

	if (!xctxt) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/iso_3166_entries/iso_3166_entry", xctxt);
	if (!xobj) {
		g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "No valid elements for %s",
			    doc->name);
		goto bail;
	}
	n = xmlXPathNodeSetGetLength(xobj->nodesetval);

	for (i = 0; i < n; i++) {
		xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, i);
		xmlChar *p;
		lt_region_t *le;

		if (!ent) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		le = lt_region_create(LT_REGION_ENTRY_3166);
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_region_t.");
			goto bail;
		}
		p = xmlGetProp(ent, (const xmlChar *)"alpha_2_code");
		lt_region_set_code(le, LT_REGION_CODE_ALPHA_2,
				   (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_codes,
				     (gchar *)lt_region_get_code(le, LT_REGION_CODE_ALPHA_2),
				     lt_region_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"alpha_3_code");
		lt_region_set_code(le, LT_REGION_CODE_ALPHA_3,
				   (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_codes,
				     (gchar *)lt_region_get_code(le, LT_REGION_CODE_ALPHA_3),
				     lt_region_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"numeric_code");
		lt_region_set_code(le, LT_REGION_CODE_NUMERIC,
				   (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_codes,
				     (gchar *)lt_region_get_code(le, LT_REGION_CODE_NUMERIC),
				     lt_region_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"name");
		lt_region_set_name(le, (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_entries,
				     (gchar *)lt_region_get_name(le),
				     lt_region_ref(le));

		lt_region_unref(le);
	}
  bail:
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
		retval = FALSE;
	}
	if (xobj)
		xmlXPathFreeObject(xobj);
	if (xctxt)
		xmlXPathFreeContext(xctxt);

	return retval;
}

static gboolean
_lt_region_db_parse_3166_3(lt_region_db_t  *region,
			   xmlDocPtr        doc,
			   GError         **error)
{
	xmlXPathContextPtr xctxt = xmlXPathNewContext(doc);
	xmlXPathObjectPtr xobj = NULL;
	gboolean retval = TRUE;
	int i, n;
	GError *err = NULL;

	if (!xctxt) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/iso_3166_entries/iso_3166_3_entry", xctxt);
	if (!xobj) {
		g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "No valid elements for %s",
			    doc->name);
		goto bail;
	}
	n = xmlXPathNodeSetGetLength(xobj->nodesetval);

	for (i = 0; i < n; i++) {
		xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, i);
		xmlChar *p;
		lt_region_t *le;

		if (!ent) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		le = lt_region_create(LT_REGION_ENTRY_3166_3);
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_region_t.");
			goto bail;
		}
		p = xmlGetProp(ent, (const xmlChar *)"alpha_4_code");
		lt_region_set_code(le, LT_REGION_CODE_ALPHA_4,
				   (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_codes,
				     (gchar *)lt_region_get_code(le, LT_REGION_CODE_ALPHA_4),
				     lt_region_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"alpha_3_code");
		lt_region_set_code(le, LT_REGION_CODE_ALPHA_3,
				   (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_codes,
				     (gchar *)lt_region_get_code(le, LT_REGION_CODE_ALPHA_3),
				     lt_region_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"numeric_code");
		if (p) {
			lt_region_set_code(le, LT_REGION_CODE_NUMERIC,
					   (const gchar *)p);
			xmlFree(p);
			g_hash_table_replace(region->region_codes,
					     (gchar *)lt_region_get_code(le, LT_REGION_CODE_NUMERIC),
					     lt_region_ref(le));
		}
		p = xmlGetProp(ent, (const xmlChar *)"names");
		lt_region_set_name(le, (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_entries,
				     (gchar *)lt_region_get_name(le),
				     lt_region_ref(le));

		lt_region_unref(le);
	}
  bail:
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
		retval = FALSE;
	}
	if (xobj)
		xmlXPathFreeObject(xobj);
	if (xctxt)
		xmlXPathFreeContext(xctxt);

	return retval;
}

static gboolean
_lt_region_db_parse_3166_all(lt_region_db_t  *region,
			     xmlDocPtr        doc,
			     GError         **error)
{
	if (!_lt_region_db_parse_3166(region, doc, error))
		return FALSE;
	if (!_lt_region_db_parse_3166_3(region, doc, error))
		return FALSE;

	return TRUE;
}

static gboolean
_lt_region_db_parse_3166_2(lt_region_db_t  *region,
			   xmlDocPtr        doc,
			   GError         **error)
{
	xmlXPathContextPtr xctxt = xmlXPathNewContext(doc);
	xmlXPathObjectPtr xobj = NULL;
	xmlChar *country_code = NULL, *division_type = NULL;
	gboolean retval = TRUE;
	int i, n;
	GError *err = NULL;

	if (!xctxt) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/iso_3166_2_entries/iso_3166_country[iso_3166_subset/iso_3166_2_entry]", xctxt);
	if (!xobj) {
		g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "No valid elements for %s",
			    doc->name);
		goto bail;
	}
	n = xmlXPathNodeSetGetLength(xobj->nodesetval);

	for (i = 0; i < n; i++) {
		xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, i);
		xmlNodePtr node, enode;
		xmlChar *p;
		lt_division_t *le;
		GList *l, *ll;

		if (!ent) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		if (country_code)
			xmlFree(country_code);
		country_code = xmlGetProp(ent, (const xmlChar *)"code");
		node = ent->children;
		while (node != NULL) {
			if (xmlStrcmp(node->name, (const xmlChar *)"text") == 0 ||
			    xmlStrcmp(node->name, (const xmlChar *)"comment") == 0) {
				/* ignore it */
			} else if (xmlStrcmp(node->name, (const xmlChar *)"iso_3166_subset") != 0) {
				g_warning("Invalid node under iso_3166_country: %s", node->name);
			} else {
				if (division_type)
					xmlFree(division_type);
				division_type = xmlGetProp(node, (const xmlChar *)"type");
				enode = node->children;
				while (enode != NULL) {
					if (xmlStrcmp(enode->name, (const xmlChar *)"text") == 0 ||
					    xmlStrcmp(enode->name, (const xmlChar *)"comment") == 0) {
						/* ignore it */
					} else if (xmlStrcmp(enode->name, (const xmlChar *)"iso_3166_2_entry") != 0) {
						g_warning("Invalid node under iso_3166_subset: %s",
							  enode->name);
					} else {
						le = lt_division_create();
						if (!le) {
							g_set_error(&err, LT_ERROR, LT_ERR_OOM,
								    "Unable to create an instance of lt_division_t.");
							goto bail;
						}
						lt_division_set(le, LT_DIVISION_COUNTRY_CODE,
								(const gchar *)country_code);
						lt_division_set(le, LT_DIVISION_TYPE,
								(const gchar *)division_type);
						p = xmlGetProp(enode, (const xmlChar *)"code");
						lt_division_set(le, LT_DIVISION_CODE,
								(const gchar *)p);
						xmlFree(p);
						p = xmlGetProp(enode, (const xmlChar *)"name");
						lt_division_set(le, LT_DIVISION_NAME,
								(const gchar *)p);
						xmlFree(p);
						ll = l = g_hash_table_lookup(region->division_lists,
									     (const gchar *)country_code);
						l = g_list_append(l, lt_division_ref(le));
						if (!ll)
							g_hash_table_replace(region->division_lists,
									     (gchar *)lt_division_get_country_code(le),
									     l);
						g_hash_table_replace(region->division_entries,
								     (gchar *)lt_division_get_code(le),
								     lt_division_ref(le));
						g_hash_table_replace(region->division_entries,
								     (gchar *)lt_division_get_name(le),
								     lt_division_ref(le));
						lt_division_unref(le);
					}
					enode = enode->next;
				}
			}
			node = node->next;
		}
	}

  bail:
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
		retval = FALSE;
	}
	if (country_code)
		xmlFree(country_code);
	if (division_type)
		xmlFree(division_type);
	if (xobj)
		xmlXPathFreeObject(xobj);
	if (xctxt)
		xmlXPathFreeContext(xctxt);

	return retval;
}

static gboolean
lt_region_db_parse(lt_region_db_t  *region,
		   GError         **error)
{
	gchar *iso3166, *iso3166_2;
	gboolean retval;

	g_return_val_if_fail (region != NULL, FALSE);

	iso3166 = g_build_filename(ISO_PREFIX, "iso_3166.xml", NULL);
	iso3166_2 = g_build_filename(ISO_PREFIX, "iso_3166_2.xml", NULL);

	if (!(retval = _lt_region_db_parse(region, iso3166,
					   _lt_region_db_parse_3166_all,
					   error)))
		goto bail;
	if (!(retval = _lt_region_db_parse(region, iso3166_2,
					   _lt_region_db_parse_3166_2,
					   error)))
		goto bail;

  bail:
	g_free(iso3166);
	g_free(iso3166_2);

	return retval;
}

/*< public >*/
lt_region_db_t *
lt_region_db_new(void)
{
	lt_region_db_t *retval = lt_mem_alloc_object(sizeof (lt_region_db_t));

	if (retval) {
		GError *err = NULL;

		retval->region_entries = g_hash_table_new_full(g_str_hash,
							       g_str_equal,
							       NULL,
							       (GDestroyNotify)lt_region_unref);
		lt_mem_add_ref(&retval->parent, retval->region_entries,
			       (lt_destroy_func_t)g_hash_table_destroy);
		retval->region_codes = g_hash_table_new_full(g_str_hash,
							     g_str_equal,
							     NULL,
							     (GDestroyNotify)lt_region_unref);
		lt_mem_add_ref(&retval->parent, retval->region_codes,
			       (lt_destroy_func_t)g_hash_table_destroy);
		retval->division_entries = g_hash_table_new_full(g_str_hash,
								 g_str_equal,
								 NULL,
								 (GDestroyNotify)lt_division_unref);
		lt_mem_add_ref(&retval->parent, retval->division_entries,
			       (lt_destroy_func_t)g_hash_table_destroy);
		retval->division_lists = g_hash_table_new_full(g_str_hash,
							       g_str_equal,
							       NULL,
							       (GDestroyNotify)_lt_region_db_division_list_free);
		lt_mem_add_ref(&retval->parent, retval->division_lists,
			       (lt_destroy_func_t)g_hash_table_destroy);

		lt_region_db_parse(retval, &err);
		if (err) {
			g_printerr(err->message);
			lt_region_db_unref(retval);
			retval = NULL;
			g_error_free(err);
		}
	}

	return retval;
}

lt_region_db_t *
lt_region_db_ref(lt_region_db_t *region)
{
	g_return_val_if_fail (region != NULL, NULL);

	return lt_mem_ref(&region->parent);
}

void
lt_region_db_unref(lt_region_db_t *region)
{
	if (region)
		lt_mem_unref(&region->parent);
}

GList *
lt_region_db_get_regions(lt_region_db_t *region)
{
	g_return_val_if_fail (region != NULL, NULL);

	return g_hash_table_get_keys(region->region_entries);
}

lt_region_t *
lt_region_db_lookup_region_from_language(lt_region_db_t *region,
					 const gchar    *language)
{
	lt_region_t *retval;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (language != NULL, NULL);

	retval = g_hash_table_lookup(region->region_entries, language);
	if (retval)
		return lt_region_ref(retval);

	return NULL;
}

lt_region_t *
lt_region_db_lookup_region_from_code(lt_region_db_t *region,
				     const gchar    *code)
{
	lt_region_t *retval;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (code != NULL, NULL);

	retval = g_hash_table_lookup(region->region_codes, code);
	if (retval)
		return lt_region_ref(retval);

	return NULL;
}

GList *
lt_region_db_get_division_type(lt_region_db_t *region,
			       const gchar    *country_code)
{
	GList *l, *ll, *retval = NULL;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (country_code != NULL, NULL);

	l = g_hash_table_lookup(region->division_lists, country_code);
	for (ll = l; ll != NULL; ll = g_list_next(ll)) {
		lt_division_t *le = ll->data;
		const gchar *type = lt_division_get_type(le);

		if (!g_list_find_custom(retval, type, (GCompareFunc)g_strcmp0))
			retval = g_list_append(retval, (gchar *)type);
	}

	return retval;
}

GList *
lt_region_db_get_divisions(lt_region_db_t *region,
			   const gchar *country_code,
			   const gchar *division_type)
{
	GList *l, *ll, *retval = NULL;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (country_code != NULL, NULL);
	g_return_val_if_fail (division_type != NULL, NULL);

	l = g_hash_table_lookup(region->division_lists, country_code);
	for (ll = l; ll != NULL; ll = g_list_next(ll)) {
		lt_division_t *le = ll->data;
		const gchar *type = lt_division_get_type(le);

		if (g_strcmp0(type, division_type) == 0) {
			retval = g_list_append(retval, lt_division_ref(le));
		}
	}

	return retval;
}

lt_division_t *
lt_region_db_lookup_division(lt_region_db_t *regiondb,
			     const gchar    *name_or_code)
{
	lt_division_t *retval;

	g_return_val_if_fail (regiondb != NULL, NULL);
	g_return_val_if_fail (name_or_code != NULL, NULL);

	retval = g_hash_table_lookup(regiondb->division_entries, name_or_code);

	if (retval)
		return lt_division_ref(retval);

	return NULL;
}
