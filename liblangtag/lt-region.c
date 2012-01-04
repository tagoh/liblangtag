/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-region.c
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
#include "lt-region.h"


typedef enum _lt_region_entry_type_t {
	LT_REGION_ENTRY_3166,
	LT_REGION_ENTRY_3166_3,
	LT_REGION_ENTRY_END
} lt_region_entry_type_t;
typedef struct _lt_region_entry_t {
	lt_mem_t                parent;
	lt_region_entry_type_t  type;
	gchar                  *name;
	gchar                  *numeric_code;
	union {
		struct {
			gchar *alpha_2_code;
			gchar *alpha_3_code;
			gchar *common_name;
			gchar *official_name;
		} _3166;
		struct {
			gchar *alpha_4_code;
			gchar *alpha_3_code;
		} _3166_3;
	} iso;
} lt_region_entry_t;

typedef struct _lt_region_subset_entry_t {
	lt_mem_t  parent;
	gchar    *country_code;
	gchar    *subset_type;
	gchar    *entry_code;
	gchar    *name;
} lt_region_subset_entry_t;

struct _lt_region_t {
	lt_mem_t    parent;
	GHashTable *region_entries;
	GHashTable *region_codes;
	GHashTable *subset_entries;
	GHashTable *subset_lists;
};

typedef gboolean (* lt_xpath_func_t) (lt_region_t  *region,
				      xmlDocPtr     doc,
				      GError      **error);

/*< private >*/
static lt_region_entry_t *
lt_region_entry_new(lt_region_entry_type_t type)
{
	lt_region_entry_t *retval = lt_mem_alloc_object(sizeof (lt_region_entry_t));

	if (retval) {
		retval->type = type;
	}

	return retval;
}

static lt_region_entry_t *
lt_region_entry_ref(lt_region_entry_t *entry)
{
	g_return_val_if_fail (entry != NULL, NULL);

	return lt_mem_ref(&entry->parent);
}

static void
lt_region_entry_unref(lt_region_entry_t *entry)
{
	if (entry)
		lt_mem_unref(&entry->parent);
}

static lt_region_entry_type_t
lt_region_entry_get_type(lt_region_entry_t *entry)
{
	g_return_val_if_fail (entry != NULL, -1);

	return entry->type;
}

static void
lt_region_entry_set_name(lt_region_entry_t *entry,
			 const gchar       *name)
{
	g_return_if_fail (entry != NULL);
	g_return_if_fail (name != NULL);

	if (entry->name)
		lt_mem_remove_ref(&entry->parent, entry->name);
	entry->name = g_strdup(name);
	lt_mem_add_ref(&entry->parent, entry->name,
		       (lt_destroy_func_t)g_free);
}

static void
lt_region_entry_set_code(lt_region_entry_t *entry,
			 lt_region_code_t   code_type,
			 const gchar       *code)
{
	gchar **p;

	g_return_if_fail (entry != NULL);
	g_return_if_fail (code != NULL);

	switch (code_type) {
	    case LT_REGION_CODE_ALPHA_2:
		    g_return_if_fail (entry->type == LT_REGION_ENTRY_3166);
		    p = &entry->iso._3166.alpha_2_code;
		    break;
	    case LT_REGION_CODE_ALPHA_3:
		    if (entry->type == LT_REGION_ENTRY_3166)
			    p = &entry->iso._3166.alpha_3_code;
		    else if (entry->type == LT_REGION_ENTRY_3166_3)
			    p = &entry->iso._3166_3.alpha_3_code;
		    else {
			    g_warning("Unknown region entry type: %d\n", entry->type);
			    return;
		    }
		    break;
	    case LT_REGION_CODE_NUMERIC:
		    p = &entry->numeric_code;
		    break;
	    case LT_REGION_CODE_ALPHA_4:
		    g_return_if_fail (entry->type == LT_REGION_ENTRY_3166_3);
		    p = &entry->iso._3166_3.alpha_4_code;
		    break;
	    default:
		    g_warning("Unkonwn region code type: %d\n", code_type);
		    return;
	}
	if (*p)
		lt_mem_remove_ref(&entry->parent, *p);
	*p = g_strdup(code);
	lt_mem_add_ref(&entry->parent, *p,
		       (lt_destroy_func_t)g_free);
}

static const gchar *
lt_region_entry_get_name(const lt_region_entry_t *entry)
{
	g_return_val_if_fail (entry != NULL, NULL);

	return entry->name;
}

static const gchar *
lt_region_entry_get_code(const lt_region_entry_t *entry,
			 lt_region_code_t         code_type)
{
	const gchar *retval;

	g_return_val_if_fail (entry != NULL, NULL);

	switch (code_type) {
	    case LT_REGION_CODE_ALPHA_2:
		    g_return_val_if_fail (entry->type == LT_REGION_ENTRY_3166, NULL);
		    retval = entry->iso._3166.alpha_2_code;
		    break;
	    case LT_REGION_CODE_ALPHA_3:
		    if (entry->type == LT_REGION_ENTRY_3166)
			    retval = entry->iso._3166.alpha_3_code;
		    else if (entry->type == LT_REGION_ENTRY_3166_3)
			    retval = entry->iso._3166_3.alpha_3_code;
		    else {
			    g_warning("Unknown region entry type: %d\n", entry->type);
			    retval = NULL;
		    }
		    break;
	    case LT_REGION_CODE_NUMERIC:
		    retval = entry->numeric_code;
		    break;
	    case LT_REGION_CODE_ALPHA_4:
		    g_return_val_if_fail (entry->type == LT_REGION_ENTRY_3166_3, NULL);
		    retval = entry->iso._3166_3.alpha_4_code;
		    break;
	    default:
		    g_warning("Unkonwn region code type: %d\n", code_type);
		    retval = NULL;
		    break;
	}

	return retval;
}

static lt_region_subset_entry_t *
lt_region_subset_entry_new(void)
{
	lt_region_subset_entry_t *retval = lt_mem_alloc_object(sizeof (lt_region_subset_entry_t));

	return retval;
}

static lt_region_subset_entry_t *
lt_region_subset_entry_ref(lt_region_subset_entry_t *entry)
{
	g_return_val_if_fail (entry != NULL, NULL);

	return lt_mem_ref(&entry->parent);
}

static void
lt_region_subset_entry_unref(lt_region_subset_entry_t *entry)
{
	if (entry)
		lt_mem_unref(&entry->parent);
}

static void
lt_region_subset_entry_set(lt_region_subset_entry_t *entry,
			   lt_region_subset_vtype_t  type,
			   const gchar              *val)
{
	gchar **p;

	g_return_if_fail (entry != NULL);
	g_return_if_fail (val != NULL);

	switch (type) {
	    case LT_REGION_SUBSET_COUNTRY_CODE:
		    p = &entry->country_code;
		    break;
	    case LT_REGION_SUBSET_TYPE:
		    p = &entry->subset_type;
		    break;
	    case LT_REGION_SUBSET_ENTRY_CODE:
		    p = &entry->entry_code;
		    break;
	    case LT_REGION_SUBSET_ENTRY_NAME:
		    p = &entry->name;
		    break;
	    default:
		    g_warning("Unknown subset value type: %d", type);
		    return;
	}

	if (*p)
		lt_mem_remove_ref(&entry->parent, *p);
	*p = g_strdup(val);
	lt_mem_add_ref(&entry->parent, *p,
		       (lt_destroy_func_t)g_free);
}

static const gchar *
lt_region_subset_entry_get(lt_region_subset_entry_t *entry,
			   lt_region_subset_vtype_t  type)
{
	const gchar *retval = NULL;

	g_return_val_if_fail (entry != NULL, NULL);

	switch (type) {
	    case LT_REGION_SUBSET_COUNTRY_CODE:
		    retval = entry->country_code;
		    break;
	    case LT_REGION_SUBSET_TYPE:
		    retval = entry->subset_type;
		    break;
	    case LT_REGION_SUBSET_ENTRY_CODE:
		    retval = entry->entry_code;
		    break;
	    case LT_REGION_SUBSET_ENTRY_NAME:
		    retval = entry->name;
		    break;
	    default:
		    g_warning("Unknown subset value type: %d", type);
		    break;
	}

	return retval;
}

static void
_lt_region_subset_entry_list_free(GList *list)
{
	GList *l = list;

	for (; l != NULL; l = g_list_next(l)) {
		lt_region_subset_entry_unref(l->data);
	}
	g_list_free(list);
}

static gboolean
_lt_region_parse(lt_region_t      *region,
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
_lt_region_parse_3166(lt_region_t  *region,
		      xmlDocPtr     doc,
		      GError      **error)
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
		lt_region_entry_t *le;

		if (!ent) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		le = lt_region_entry_new(LT_REGION_ENTRY_3166);
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_region_entry_t.");
			goto bail;
		}
		p = xmlGetProp(ent, (const xmlChar *)"alpha_2_code");
		lt_region_entry_set_code(le, LT_REGION_CODE_ALPHA_2,
					 (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_codes,
				     (gchar *)lt_region_entry_get_code(le, LT_REGION_CODE_ALPHA_2),
				     lt_region_entry_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"alpha_3_code");
		lt_region_entry_set_code(le, LT_REGION_CODE_ALPHA_3,
					 (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_codes,
				     (gchar *)lt_region_entry_get_code(le, LT_REGION_CODE_ALPHA_3),
				     lt_region_entry_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"numeric_code");
		lt_region_entry_set_code(le, LT_REGION_CODE_NUMERIC,
					 (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_codes,
				     (gchar *)lt_region_entry_get_code(le, LT_REGION_CODE_NUMERIC),
				     lt_region_entry_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"name");
		lt_region_entry_set_name(le, (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_entries,
				     (gchar *)lt_region_entry_get_name(le),
				     lt_region_entry_ref(le));

		lt_region_entry_unref(le);
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
_lt_region_parse_3166_3(lt_region_t  *region,
			xmlDocPtr     doc,
			GError      **error)
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
		lt_region_entry_t *le;

		if (!ent) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		le = lt_region_entry_new(LT_REGION_ENTRY_3166_3);
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_region_entry_t.");
			goto bail;
		}
		p = xmlGetProp(ent, (const xmlChar *)"alpha_4_code");
		lt_region_entry_set_code(le, LT_REGION_CODE_ALPHA_4,
					 (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_codes,
				     (gchar *)lt_region_entry_get_code(le, LT_REGION_CODE_ALPHA_4),
				     lt_region_entry_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"alpha_3_code");
		lt_region_entry_set_code(le, LT_REGION_CODE_ALPHA_3,
					 (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_codes,
				     (gchar *)lt_region_entry_get_code(le, LT_REGION_CODE_ALPHA_3),
				     lt_region_entry_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"numeric_code");
		if (p) {
			lt_region_entry_set_code(le, LT_REGION_CODE_NUMERIC,
						 (const gchar *)p);
			xmlFree(p);
			g_hash_table_replace(region->region_codes,
					     (gchar *)lt_region_entry_get_code(le, LT_REGION_CODE_NUMERIC),
					     lt_region_entry_ref(le));
		}
		p = xmlGetProp(ent, (const xmlChar *)"names");
		lt_region_entry_set_name(le, (const gchar *)p);
		xmlFree(p);
		g_hash_table_replace(region->region_entries,
				     (gchar *)lt_region_entry_get_name(le),
				     lt_region_entry_ref(le));

		lt_region_entry_unref(le);
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
_lt_region_parse_3166_all(lt_region_t  *region,
			  xmlDocPtr     doc,
			  GError      **error)
{
	if (!_lt_region_parse_3166(region, doc, error))
		return FALSE;
	if (!_lt_region_parse_3166_3(region, doc, error))
		return FALSE;

	return TRUE;
}

static gboolean
_lt_region_parse_3166_2(lt_region_t  *region,
			xmlDocPtr     doc,
			GError      **error)
{
	xmlXPathContextPtr xctxt = xmlXPathNewContext(doc);
	xmlXPathObjectPtr xobj = NULL;
	xmlChar *country_code = NULL, *subset_type = NULL;
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
		lt_region_subset_entry_t *le;
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
				if (subset_type)
					xmlFree(subset_type);
				subset_type = xmlGetProp(node, (const xmlChar *)"type");
				enode = node->children;
				while (enode != NULL) {
					if (xmlStrcmp(enode->name, (const xmlChar *)"text") == 0 ||
					    xmlStrcmp(enode->name, (const xmlChar *)"comment") == 0) {
						/* ignore it */
					} else if (xmlStrcmp(enode->name, (const xmlChar *)"iso_3166_2_entry") != 0) {
						g_warning("Invalid node under iso_3166_subset: %s",
							  enode->name);
					} else {
						le = lt_region_subset_entry_new();
						if (!le) {
							g_set_error(&err, LT_ERROR, LT_ERR_OOM,
								    "Unable to create an instance of lt_region_subset_entry_t.");
							goto bail;
						}
						lt_region_subset_entry_set(le, LT_REGION_SUBSET_COUNTRY_CODE,
									   (const gchar *)country_code);
						lt_region_subset_entry_set(le, LT_REGION_SUBSET_TYPE,
									   (const gchar *)subset_type);
						p = xmlGetProp(enode, (const xmlChar *)"code");
						lt_region_subset_entry_set(le, LT_REGION_SUBSET_ENTRY_CODE,
									   (const gchar *)p);
						xmlFree(p);
						p = xmlGetProp(enode, (const xmlChar *)"name");
						lt_region_subset_entry_set(le, LT_REGION_SUBSET_ENTRY_NAME,
									   (const gchar *)p);
						xmlFree(p);
						ll = l = g_hash_table_lookup(region->subset_lists,
									     (const gchar *)country_code);
						l = g_list_append(l, lt_region_subset_entry_ref(le));
						if (!ll)
							g_hash_table_replace(region->subset_lists,
									     (gchar *)lt_region_subset_entry_get(le, LT_REGION_SUBSET_COUNTRY_CODE),
									     l);
						g_hash_table_replace(region->subset_entries,
								     (gchar *)lt_region_subset_entry_get(le, LT_REGION_SUBSET_ENTRY_CODE),
								     lt_region_subset_entry_ref(le));
						g_hash_table_replace(region->subset_entries,
								     (gchar *)lt_region_subset_entry_get(le, LT_REGION_SUBSET_ENTRY_NAME),
								     lt_region_subset_entry_ref(le));
						lt_region_subset_entry_unref(le);
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
	if (subset_type)
		xmlFree(subset_type);
	if (xobj)
		xmlXPathFreeObject(xobj);
	if (xctxt)
		xmlXPathFreeContext(xctxt);

	return retval;
}

static gboolean
lt_region_parse(lt_region_t  *region,
		GError      **error)
{
	gchar *iso3166, *iso3166_2;
	gboolean retval;

	g_return_val_if_fail (region != NULL, FALSE);

	iso3166 = g_build_filename(ISO_PREFIX, "iso_3166.xml", NULL);
	iso3166_2 = g_build_filename(ISO_PREFIX, "iso_3166_2.xml", NULL);

	if (!(retval = _lt_region_parse(region, iso3166,
					_lt_region_parse_3166_all,
					error)))
		goto bail;
	if (!(retval = _lt_region_parse(region, iso3166_2,
					_lt_region_parse_3166_2,
					error)))
		goto bail;

  bail:
	g_free(iso3166);
	g_free(iso3166_2);

	return retval;
}

/*< public >*/
lt_region_t *
lt_region_new(void)
{
	lt_region_t *retval = lt_mem_alloc_object(sizeof (lt_region_t));

	if (retval) {
		GError *err = NULL;

		retval->region_entries = g_hash_table_new_full(g_str_hash,
							       g_str_equal,
							       NULL,
							       (GDestroyNotify)lt_region_entry_unref);
		lt_mem_add_ref(&retval->parent, retval->region_entries,
			       (lt_destroy_func_t)g_hash_table_destroy);
		retval->region_codes = g_hash_table_new_full(g_str_hash,
							     g_str_equal,
							     NULL,
							     (GDestroyNotify)lt_region_entry_unref);
		lt_mem_add_ref(&retval->parent, retval->region_codes,
			       (lt_destroy_func_t)g_hash_table_destroy);
		retval->subset_entries = g_hash_table_new_full(g_str_hash,
							       g_str_equal,
							       NULL,
							       (GDestroyNotify)lt_region_subset_entry_unref);
		lt_mem_add_ref(&retval->parent, retval->subset_entries,
			       (lt_destroy_func_t)g_hash_table_destroy);
		retval->subset_lists = g_hash_table_new_full(g_str_hash,
							     g_str_equal,
							     NULL,
							     (GDestroyNotify)_lt_region_subset_entry_list_free);
		lt_mem_add_ref(&retval->parent, retval->subset_lists,
			       (lt_destroy_func_t)g_hash_table_destroy);

		lt_region_parse(retval, &err);
		if (err) {
			g_printerr(err->message);
			lt_region_unref(retval);
			retval = NULL;
			g_error_free(err);
		}
	}

	return retval;
}

lt_region_t *
lt_region_ref(lt_region_t *region)
{
	g_return_val_if_fail (region != NULL, NULL);

	return lt_mem_ref(&region->parent);
}

void
lt_region_unref(lt_region_t *region)
{
	if (region)
		lt_mem_unref(&region->parent);
}

GList *
lt_region_get_regions(lt_region_t *region)
{
	g_return_val_if_fail (region != NULL, NULL);

	return g_hash_table_get_keys(region->region_entries);
}

const gchar *
lt_region_lookup_code(lt_region_t      *region,
		      const gchar      *language,
		      lt_region_code_t  type)
{
	lt_region_entry_t *le;
	lt_region_entry_type_t t;
	const gchar *retval = NULL;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (language != NULL, NULL);

	le = g_hash_table_lookup(region->region_entries, language);
	if (le) {
		t = lt_region_entry_get_type(le);
		switch (t) {
		    case LT_REGION_ENTRY_3166:
			    if (type == LT_REGION_CODE_ALPHA_4)
				    return NULL;
			    break;
		    case LT_REGION_ENTRY_3166_3:
			    if (type == LT_REGION_CODE_ALPHA_2)
				    return NULL;
			    break;
		    default:
			    break;
		}
		retval = lt_region_entry_get_code(le, type);
	}

	return retval;
}

const gchar *
lt_region_lookup_region(lt_region_t *region,
			const gchar *code)
{
	lt_region_entry_t *le;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (code != NULL, NULL);

	le = g_hash_table_lookup(region->region_codes, code);
	if (le)
		return lt_region_entry_get_name(le);

	return NULL;
}

GList *
lt_region_get_subset_type(lt_region_t *region,
			  const gchar *country_code)
{
	GList *l, *ll, *retval = NULL;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (country_code != NULL, NULL);

	l = g_hash_table_lookup(region->subset_lists, country_code);
	for (ll = l; ll != NULL; ll = g_list_next(ll)) {
		lt_region_subset_entry_t *le = ll->data;
		const gchar *type = lt_region_subset_entry_get(le, LT_REGION_SUBSET_TYPE);

		if (!g_list_find_custom(retval, type, (GCompareFunc)g_strcmp0))
			retval = g_list_append(retval, (gchar *)type);
	}

	return retval;
}

GList *
lt_region_get_subset_names(lt_region_t *region,
			   const gchar *country_code,
			   const gchar *subset_name)
{
	GList *l, *ll, *retval = NULL;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (country_code != NULL, NULL);

	l = g_hash_table_lookup(region->subset_lists, country_code);
	for (ll = l; ll != NULL; ll = g_list_next(ll)) {
		lt_region_subset_entry_t *le = ll->data;
		const gchar *type = lt_region_subset_entry_get(le, LT_REGION_SUBSET_TYPE);

		if (g_strcmp0(type, subset_name) == 0) {
			const gchar *name = lt_region_subset_entry_get(le, LT_REGION_SUBSET_ENTRY_NAME);

			if (!g_list_find_custom(retval, name, (GCompareFunc)g_strcmp0))
				retval = g_list_append(retval, (gchar *)name);
		}
	}

	return retval;
}

GList *
lt_region_get_subset_codes(lt_region_t *region,
			   const gchar *country_code,
			   const gchar *subset_name)
{
	GList *l, *ll, *retval = NULL;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (country_code != NULL, NULL);

	l = g_hash_table_lookup(region->subset_lists, country_code);
	for (ll = l; ll != NULL; ll = g_list_next(ll)) {
		lt_region_subset_entry_t *le = ll->data;
		const gchar *type = lt_region_subset_entry_get(le, LT_REGION_SUBSET_TYPE);

		if (g_strcmp0(type, subset_name) == 0) {
			const gchar *code = lt_region_subset_entry_get(le, LT_REGION_SUBSET_ENTRY_CODE);

			if (!g_list_find_custom(retval, code, (GCompareFunc)g_strcmp0))
				retval = g_list_append(retval, (gchar *)code);
		}
	}

	return retval;
}

const gchar *
lt_region_lookup_subset_code(lt_region_t *region,
			     const gchar *subset_name)
{
	lt_region_subset_entry_t *le;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (subset_name != NULL, NULL);

	le = g_hash_table_lookup(region->subset_entries, subset_name);
	if (le)
		return lt_region_subset_entry_get(le, LT_REGION_SUBSET_ENTRY_CODE);

	return NULL;
}

const gchar *
lt_region_lookup_subset_name(lt_region_t *region,
			     const gchar *subset_code)
{
	lt_region_subset_entry_t *le;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (subset_code != NULL, NULL);

	le = g_hash_table_lookup(region->subset_entries, subset_code);
	if (le)
		return lt_region_subset_entry_get(le, LT_REGION_SUBSET_ENTRY_NAME);

	return NULL;
}
