/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-script-db.c
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

#include <libxml/xpath.h>
#include "lt-error.h"
#include "lt-mem.h"
#include "lt-utils.h"
#include "lt-xml.h"
#include "lt-script-private.h"
#include "lt-script-db.h"


/**
 * SECTION:lt-script-db
 * @Short_Description: An interface to access Script Database
 * @Title: Database - Script
 *
 * This class provides an interface to access Script Database. which has been
 * registered as ISO 15924.
 */
struct _lt_script_db_t {
	lt_mem_t    parent;
	lt_xml_t   *xml;
	GHashTable *script_entries;
};

/*< private >*/
static gboolean
lt_script_db_parse(lt_script_db_t  *scriptdb,
		   GError         **error)
{
	gboolean retval = TRUE;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	GError *err = NULL;
	int i, n;

	g_return_val_if_fail (scriptdb != NULL, FALSE);

	doc = lt_xml_get_subtag_registry(scriptdb->xml);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/registry/script", xctxt);
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
		xmlChar *subtag = NULL, *desc = NULL;
		lt_script_t *le = NULL;
		gchar *s;

		if (!ent) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		cnode = ent->children;
		while (cnode != NULL) {
			if (xmlStrcmp(cnode->name, (const xmlChar *)"subtag") == 0) {
				if (subtag) {
					g_warning("Duplicate subtag element in script: previous value was '%s'",
						  subtag);
				} else {
					subtag = xmlNodeGetContent(cnode);
				}
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"added") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"text") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"comments") == 0) {
				/* ignore it */
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"description") == 0) {
				/* wonder if many descriptions helps something. or is it a bug? */
				if (!desc)
					desc = xmlNodeGetContent(cnode);
			} else {
				g_warning("Unknown node under /registry/script: %s", cnode->name);
			}
			cnode = cnode->next;
		}
		if (!subtag) {
			g_warning("No subtag node: description = '%s'",
				  desc);
			goto bail1;
		}
		if (!desc) {
			g_warning("No description node: subtag = '%s'",
				  subtag);
			goto bail1;
		}
		le = lt_script_create();
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_script_t.");
			goto bail1;
		}
		lt_script_set_tag(le, (const gchar *)subtag);
		lt_script_set_name(le, (const gchar *)desc);

		s = g_strdup(lt_script_get_tag(le));
		g_hash_table_replace(scriptdb->script_entries,
				     lt_strlower(s),
				     lt_script_ref(le));
	  bail1:
		if (subtag)
			xmlFree(subtag);
		if (desc)
			xmlFree(desc);
		lt_script_unref(le);
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
 * lt_script_db_new:
 *
 * Create a new instance of a #lt_script_db_t.
 *
 * Returns: (transfer full): a new instance of #lt_script_db_t.
 */
lt_script_db_t *
lt_script_db_new(void)
{
	lt_script_db_t *retval = lt_mem_alloc_object(sizeof (lt_script_db_t));

	if (retval) {
		GError *err = NULL;
		lt_script_t *le;

		retval->script_entries = g_hash_table_new_full(g_str_hash,
							       g_str_equal,
							       g_free,
							       (GDestroyNotify)lt_script_unref);
		lt_mem_add_ref(&retval->parent, retval->script_entries,
			       (lt_destroy_func_t)g_hash_table_destroy);

		le = lt_script_create();
		lt_script_set_tag(le, "*");
		lt_script_set_name(le, "Wildcard entry");
		g_hash_table_replace(retval->script_entries,
				     g_strdup(lt_script_get_tag(le)),
				     le);
		le = lt_script_create();
		lt_script_set_tag(le, "");
		lt_script_set_name(le, "Empty entry");
		g_hash_table_replace(retval->script_entries,
				     g_strdup(lt_script_get_tag(le)),
				     le);

		retval->xml = lt_xml_new();
		if (!retval->xml) {
			lt_script_db_unref(retval);
			retval = NULL;
			goto bail;
		}
		lt_mem_add_ref(&retval->parent, retval->xml,
			       (lt_destroy_func_t)lt_xml_unref);

		lt_script_db_parse(retval, &err);
		if (err) {
			g_printerr(err->message);
			lt_script_db_unref(retval);
			retval = NULL;
			g_error_free(err);
		}
	}
  bail:

	return retval;
}

/**
 * lt_script_db_ref:
 * @scriptdb: a #lt_script_db_t.
 *
 * Increases the reference count of @scriptdb.
 *
 * Returns: (transfer none): the same @scriptdb object.
 */
lt_script_db_t *
lt_script_db_ref(lt_script_db_t *scriptdb)
{
	g_return_val_if_fail (scriptdb != NULL, NULL);

	return lt_mem_ref(&scriptdb->parent);
}

/**
 * lt_script_db_unref:
 * @scriptdb: a #lt_script_db_t.
 *
 * Decreases the reference count of @scriptdb. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_script_db_unref(lt_script_db_t *scriptdb)
{
	if (scriptdb)
		lt_mem_unref(&scriptdb->parent);
}

/**
 * lt_script_db_lookup:
 * @scriptdb: a #lt_script_db_t.
 * @subtag: a subtag name to lookup.
 *
 * Lookup @lt_script_t if @subtag is valid and registered into the database.
 *
 * Returns: (transfer full): a #lt_script_t that meets with @subtag.
 *                           otherwise %NULL.
 */
lt_script_t *
lt_script_db_lookup(lt_script_db_t *scriptdb,
		    const gchar    *subtag)
{
	lt_script_t *retval;
	gchar *s;

	g_return_val_if_fail (scriptdb != NULL, NULL);
	g_return_val_if_fail (subtag != NULL, NULL);

	s = g_strdup(subtag);
	retval = g_hash_table_lookup(scriptdb->script_entries,
				     lt_strlower(s));
	g_free(s);
	if (retval)
		return lt_script_ref(retval);

	return NULL;
}
