/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-redundant-db.c
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

#include <glib.h> /* XXX: GHashTable dependency is still there */
#include <stdlib.h>
#include <string.h>
#include <libxml/xpath.h>
#include "lt-error.h"
#include "lt-redundant.h"
#include "lt-redundant-private.h"
#include "lt-mem.h"
#include "lt-messages.h"
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
static lt_bool_t
lt_redundant_db_parse(lt_redundant_db_t  *redundantdb,
		      lt_error_t        **error)
{
	lt_bool_t retval = TRUE;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	lt_error_t *err = NULL;
	int i, n;

	lt_return_val_if_fail (redundantdb != NULL, FALSE);

	doc = lt_xml_get_subtag_registry(redundantdb->xml);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		lt_error_set(&err, LT_ERR_OOM,
			     "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/registry/redundant", xctxt);
	if (!xobj) {
		lt_error_set(&err, LT_ERR_FAIL_ON_XML,
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
		char *s;

		if (!ent) {
			lt_error_set(&err, LT_ERR_FAIL_ON_XML,
				     "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		cnode = ent->children;
		while (cnode != NULL) {
			if (xmlStrcmp(cnode->name, (const xmlChar *)"tag") == 0) {
				if (tag) {
					lt_warning("Duplicate tag element in redundant: previous value was '%s'",
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
					lt_warning("Duplicate preferred-value element in redundant: previous value was '%s'",
						   preferred);
				} else {
					preferred = xmlNodeGetContent(cnode);
				}
			} else {
				lt_warning("Unknown node under /registry/redundant: %s", cnode->name);
			}
			cnode = cnode->next;
		}
		if (!tag) {
			lt_warning("No tag node: description = '%s', preferred-value = '%s'",
				   desc, preferred);
			goto bail1;
		}
		if (!desc) {
			lt_warning("No description node: tag = '%s', preferred-value = '%s'",
				   tag, preferred);
			goto bail1;
		}
		le = lt_redundant_create();
		if (!le) {
			lt_error_set(&err, LT_ERR_OOM,
				     "Unable to create an instance of lt_redundant_t.");
			goto bail1;
		}
		lt_redundant_set_tag(le, (const char *)tag);
		lt_redundant_set_name(le, (const char *)desc);
		if (preferred)
			lt_redundant_set_preferred_tag(le, (const char *)preferred);

		s = strdup(lt_redundant_get_tag(le));
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
	if (lt_error_is_set(err, LT_ERR_ANY)) {
		if (error)
			*error = lt_error_ref(err);
		else
			lt_error_print(err, LT_ERR_ANY);
		lt_error_unref(err);
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
		lt_error_t *err = NULL;

		retval->redundant_entries = g_hash_table_new_full(g_str_hash,
								  g_str_equal,
								  free,
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
		if (lt_error_is_set(err, LT_ERR_ANY)) {
			lt_error_print(err, LT_ERR_ANY);
			lt_redundant_db_unref(retval);
			retval = NULL;
			lt_error_unref(err);
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
	lt_return_val_if_fail (redundantdb != NULL, NULL);

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
		       const char        *tag)
{
	lt_redundant_t *retval;
	char *s;

	lt_return_val_if_fail (redundantdb != NULL, NULL);
	lt_return_val_if_fail (tag != NULL, NULL);

	s = strdup(tag);
	retval = g_hash_table_lookup(redundantdb->redundant_entries,
				     lt_strlower(s));
	free(s);
	if (retval)
		return lt_redundant_ref(retval);

	return NULL;
}
