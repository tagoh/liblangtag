/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-variant-db.c
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

#include <libxml/xpath.h>
#include "lt-error.h"
#include "lt-variant.h"
#include "lt-variant-private.h"
#include "lt-mem.h"
#include "lt-utils.h"
#include "lt-xml.h"
#include "lt-variant-db.h"


/**
 * SECTION: lt-variant-db
 * @Short_Description: An interface to access Variant Database
 * @Title: Database - Variant
 *
 * This class provides an interface to access Variant database. which has been
 * registered with IANA.
 */
struct _lt_variant_db_t {
	lt_mem_t    parent;
	lt_xml_t   *xml;
	GHashTable *variant_entries;
};

/*< private >*/
static gboolean
lt_variant_db_parse(lt_variant_db_t  *variantdb,
		    GError          **error)
{
	gboolean retval = TRUE;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	GError *err = NULL;
	int i, n;

	g_return_val_if_fail (variantdb != NULL, FALSE);

	doc = lt_xml_get_subtag_registry(variantdb->xml);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/registry/variant", xctxt);
	if (!xobj) {
		g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "No valid elements for %s",
			    doc->name);
		goto bail;
	}
	n = xmlXPathNodeSetGetLength(xobj->nodesetval);

	for (i = 0; i < n; i++) {
		xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, i);
		xmlNodePtr cnode;
		xmlChar *subtag = NULL, *desc = NULL, *preferred = NULL;
		lt_variant_t *le = NULL;
		gchar *s;
		GList *prefix_list = NULL, *l;

		if (!ent) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		cnode = ent->children;
		while (cnode != NULL) {
			if (xmlStrcmp(cnode->name, (const xmlChar *)"subtag") == 0) {
				if (subtag) {
					g_warning("Duplicate subtag element in variant: previous value was '%s'",
						  subtag);
				} else {
					subtag = xmlNodeGetContent(cnode);
				}
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"added") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"text") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"comments") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"deprecated") == 0) {
				/* ignore it */
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"description") == 0) {
				/* wonder if many descriptions helps something or is it a bug? */
				if (!desc)
					desc = xmlNodeGetContent(cnode);
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"prefix") == 0) {
				prefix_list = g_list_append(prefix_list,
							    xmlNodeGetContent(cnode));
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"preferred-value") == 0) {
				if (preferred) {
					g_warning("Duplicate preferred-value element in variant: previous value was '%s'",
						  preferred);
				} else {
					preferred = xmlNodeGetContent(cnode);
				}
			} else {
				g_warning("Unknown node under /registry/variant: %s", cnode->name);
			}
			cnode = cnode->next;
		}
		if (!subtag) {
			g_warning("No subtag node: description = '%s', prefix = '%s', preferred-value = '%s'",
				  desc, prefix_list ? (gchar *)prefix_list->data : "N/A", preferred);
			goto bail1;
		}
		if (!desc) {
			g_warning("No description node: subtag = '%s', prefix = '%s', preferred-value = '%s'",
				  subtag, prefix_list ? (gchar *)prefix_list->data : "N/A", preferred);
			goto bail1;
		}
		le = lt_variant_create();
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_variant_t.");
			goto bail1;
		}
		lt_variant_set_tag(le, (const gchar *)subtag);
		lt_variant_set_name(le, (const gchar *)desc);
		for (l = prefix_list; l != NULL; l = g_list_next(l)) {
			lt_variant_add_prefix(le, l->data);
			xmlFree(l->data);
		}
		g_list_free(prefix_list);
		if (preferred)
			lt_variant_set_preferred_tag(le, (const gchar *)preferred);

		s = g_strdup(lt_variant_get_tag(le));
		g_hash_table_replace(variantdb->variant_entries,
				     lt_strlower(s),
				     lt_variant_ref(le));
	  bail1:
		if (subtag)
			xmlFree(subtag);
		if (desc)
			xmlFree(desc);
		if (preferred)
			xmlFree(preferred);
		lt_variant_unref(le);
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

/*< public >*/
/**
 * lt_variant_db_new:
 *
 * Create a new instance of a #lt_variant_db_t.
 *
 * Returns: (transfer full): a new instance of #lt_variant_db_t.
 */
lt_variant_db_t *
lt_variant_db_new(void)
{
	lt_variant_db_t *retval = lt_mem_alloc_object(sizeof (lt_variant_db_t));

	if (retval) {
		GError *err = NULL;
		lt_variant_t *le;

		retval->variant_entries = g_hash_table_new_full(g_str_hash,
								g_str_equal,
								g_free,
								(GDestroyNotify)lt_variant_unref);
		lt_mem_add_ref(&retval->parent, retval->variant_entries,
			       (lt_destroy_func_t)g_hash_table_destroy);

		le = lt_variant_create();
		lt_variant_set_tag(le, "*");
		lt_variant_set_name(le, "Wildcard entry");
		g_hash_table_replace(retval->variant_entries,
				     g_strdup(lt_variant_get_tag(le)),
				     le);
		le = lt_variant_create();
		lt_variant_set_tag(le, "");
		lt_variant_set_name(le, "Empty entry");
		g_hash_table_replace(retval->variant_entries,
				     g_strdup(lt_variant_get_tag(le)),
				     le);

		retval->xml = lt_xml_new();
		if (!retval->xml) {
			lt_variant_db_unref(retval);
			retval = NULL;
			goto bail;
		}
		lt_mem_add_ref(&retval->parent, retval->xml,
			       (lt_destroy_func_t)lt_xml_unref);

		lt_variant_db_parse(retval, &err);
		if (err) {
			g_printerr(err->message);
			lt_variant_db_unref(retval);
			retval = NULL;
			g_error_free(err);
		}
	}
  bail:

	return retval;
}

/**
 * lt_variant_db_ref:
 * @variantdb: a #lt_variant_db_t.
 *
 * Increases the reference count of @variantdb.
 *
 * Returns: (transfer none): the same @variantdb object.
 */
lt_variant_db_t *
lt_variant_db_ref(lt_variant_db_t *variantdb)
{
	g_return_val_if_fail (variantdb != NULL, NULL);

	return lt_mem_ref(&variantdb->parent);
}

/**
 * lt_variant_db_unref:
 * @variantdb: a #lt_variant_db_t.
 *
 * Decreases the reference count of @variantdb. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_variant_db_unref(lt_variant_db_t *variantdb)
{
	if (variantdb)
		lt_mem_unref(&variantdb->parent);
}

/**
 * lt_variant_db_lookup:
 * @variantdb: a #lt_variant_db_t.
 * @subtag: a subtag name to lookup.
 *
 * Lookup @lt_variant_t if @subtag is valid and registered into the database.
 *
 * Returns: (transfer full): a #lt_variant_t that meets with @subtag.
 *                           otherwise %NULL.
 */
lt_variant_t *
lt_variant_db_lookup(lt_variant_db_t *variantdb,
		     const gchar     *subtag)
{
	lt_variant_t *retval;
	gchar *s;

	g_return_val_if_fail (variantdb != NULL, NULL);
	g_return_val_if_fail (subtag != NULL, NULL);

	s = g_strdup(subtag);
	retval = g_hash_table_lookup(variantdb->variant_entries,
				     lt_strlower(s));
	g_free(s);
	if (retval)
		return lt_variant_ref(retval);

	return NULL;
}
