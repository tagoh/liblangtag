/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-redundant-db.c
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
#include "lt-redundant.h"
#include "lt-redundant-private.h"
#include "lt-mem.h"
#include "lt-utils.h"
#include "lt-xml.h"
#include "lt-redundant-db.h"


/**
 * SECTION: lt-redundant-db
 * @Short_Description: An interface to access Redundant Database
 * @Title: Database - Redundant
 *
 * This class provides an interface to access Redundant database.
 * which were mostly made redundant by the advent of either RFC 4646
 * or RFC 5646.
 */
struct _lt_redundant_db_t {
	lt_mem_t    parent;
	lt_xml_t   *xml;
	GHashTable *redundant_entries;
};

/*< private >*/
static gboolean
lt_redundant_db_parse(lt_redundant_db_t  *redundantdb,
		      GError            **error)
{
	gboolean retval = TRUE;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	GError *err = NULL;
	int i, n;

	g_return_val_if_fail (redundantdb != NULL, FALSE);

	doc = lt_xml_get_subtag_registry(redundantdb->xml);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/registry/redundant", xctxt);
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
		xmlChar *tag = NULL, *desc = NULL, *preferred = NULL;
		lt_redundant_t *le = NULL;
		gchar *s;

		if (!ent) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		cnode = ent->children;
		while (cnode != NULL) {
			if (xmlStrcmp(cnode->name, (const xmlChar *)"tag") == 0) {
				if (tag) {
					g_warning("Duplicate tag element in redundant: previous value was '%s'",
						  tag);
				} else {
					tag = xmlNodeGetContent(cnode);
				}
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"added") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"text") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"deprecated") == 0) {
				/* ignore it */
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"description") == 0) {
				/* wonder if many descriptions helps something. or is it a bug? */
				if (!desc)
					desc = xmlNodeGetContent(cnode);
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"preferred-value") == 0) {
				if (preferred) {
					g_warning("Duplicate preferred-value element in redundant: previous value was '%s'",
						  preferred);
				} else {
					preferred = xmlNodeGetContent(cnode);
				}
			} else {
				g_warning("Unknown node under /registry/redundant: %s", cnode->name);
			}
			cnode = cnode->next;
		}
		if (!tag) {
			g_warning("No tag node: description = '%s', preferred-value = '%s'",
				  desc, preferred);
			goto bail1;
		}
		if (!desc) {
			g_warning("No description node: tag = '%s', preferred-value = '%s'",
				  tag, preferred);
			goto bail1;
		}
		le = lt_redundant_create();
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_redundant_t.");
			goto bail1;
		}
		lt_redundant_set_tag(le, (const gchar *)tag);
		lt_redundant_set_name(le, (const gchar *)desc);
		if (preferred)
			lt_redundant_set_preferred_tag(le, (const gchar *)preferred);

		s = g_strdup(lt_redundant_get_tag(le));
		g_hash_table_replace(redundantdb->redundant_entries,
				     lt_strlower(s),
				     lt_redundant_ref(le));
	  bail1:
		if (tag)
			xmlFree(tag);
		if (desc)
			xmlFree(desc);
		if (preferred)
			xmlFree(preferred);
		lt_redundant_unref(le);
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
 * lt_redundant_db_new:
 *
 * Create a new instance of a #lt_redundant_db_t.
 *
 * Returns: (transfer full): a new instance of #lt_redundant_db_t.
 */
lt_redundant_db_t *
lt_redundant_db_new(void)
{
	lt_redundant_db_t *retval = lt_mem_alloc_object(sizeof (lt_redundant_db_t));

	if (retval) {
		GError *err = NULL;

		retval->redundant_entries = g_hash_table_new_full(g_str_hash,
								  g_str_equal,
								  g_free,
								  (GDestroyNotify)lt_redundant_unref);
		lt_mem_add_ref(&retval->parent, retval->redundant_entries,
			       (lt_destroy_func_t)g_hash_table_destroy);

		retval->xml = lt_xml_new();
		if (!retval->xml) {
			lt_redundant_db_unref(retval);
			retval = NULL;
			goto bail;
		}
		lt_mem_add_ref(&retval->parent, retval->xml,
			       (lt_destroy_func_t)lt_xml_unref);

		lt_redundant_db_parse(retval, &err);
		if (err) {
			g_printerr(err->message);
			lt_redundant_db_unref(retval);
			retval = NULL;
			g_error_free(err);
		}
	}
  bail:

	return retval;
}

/**
 * lt_redundant_db_ref:
 * @redundantdb: a #lt_redundant_db_t.
 *
 * Increases the reference count of @redundantdb.
 *
 * Returns: (transfer none): the same @redundantdb object.
 */
lt_redundant_db_t *
lt_redundant_db_ref(lt_redundant_db_t *redundantdb)
{
	g_return_val_if_fail (redundantdb != NULL, NULL);

	return lt_mem_ref(&redundantdb->parent);
}

/**
 * lt_redundant_db_unref:
 * @redundantdb: a #lt_redundant_db_t.
 *
 * Decreases the reference count of @redundantdb. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_redundant_db_unref(lt_redundant_db_t *redundantdb)
{
	if (redundantdb)
		lt_mem_unref(&redundantdb->parent);
}

/**
 * lt_redundant_db_lookup:
 * @redundantdb: a #lt_redundant_db_t.
 * @tag: a tag name to lookup.
 *
 * Lookup @lt_redundant_t if @tag is valid and registered into the database.
 *
 * Returns: (transfer full): a #lt_redundant_t that meets with @tag.
 *                           otherwise %NULL.
 */
lt_redundant_t *
lt_redundant_db_lookup(lt_redundant_db_t *redundantdb,
		       const gchar       *tag)
{
	lt_redundant_t *retval;
	gchar *s;

	g_return_val_if_fail (redundantdb != NULL, NULL);
	g_return_val_if_fail (tag != NULL, NULL);

	s = g_strdup(tag);
	retval = g_hash_table_lookup(redundantdb->redundant_entries,
				     lt_strlower(s));
	g_free(s);
	if (retval)
		return lt_redundant_ref(retval);

	return NULL;
}
