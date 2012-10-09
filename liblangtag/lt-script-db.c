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

#include <stdlib.h>
#include <string.h>
#include <libxml/xpath.h>
#include "lt-error.h"
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-trie.h"
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
	lt_mem_t   parent;
	lt_xml_t  *xml;
	lt_trie_t *script_entries;
};

/*< private >*/
static lt_bool_t
lt_script_db_parse(lt_script_db_t  *scriptdb,
		   lt_error_t     **error)
{
	lt_bool_t retval = TRUE;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	lt_error_t *err = NULL;
	int i, n;

	lt_return_val_if_fail (scriptdb != NULL, FALSE);

	doc = lt_xml_get_subtag_registry(scriptdb->xml);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		lt_error_set(&err, LT_ERR_OOM,
			     "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/registry/script", xctxt);
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
		xmlChar *subtag = NULL, *desc = NULL;
		lt_script_t *le = NULL;
		char *s;

		if (!ent) {
			lt_error_set(&err, LT_ERR_FAIL_ON_XML,
				     "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		cnode = ent->children;
		while (cnode != NULL) {
			if (xmlStrcmp(cnode->name, (const xmlChar *)"subtag") == 0) {
				if (subtag) {
					lt_warning("Duplicate subtag element in script: previous value was '%s'",
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
				lt_warning("Unknown node under /registry/script: %s", cnode->name);
			}
			cnode = cnode->next;
		}
		if (!subtag) {
			lt_warning("No subtag node: description = '%s'",
				   desc);
			goto bail1;
		}
		if (!desc) {
			lt_warning("No description node: subtag = '%s'",
				   subtag);
			goto bail1;
		}
		le = lt_script_create();
		if (!le) {
			lt_error_set(&err, LT_ERR_OOM,
				     "Unable to create an instance of lt_script_t.");
			goto bail1;
		}
		lt_script_set_tag(le, (const char *)subtag);
		lt_script_set_name(le, (const char *)desc);

		s = strdup(lt_script_get_tag(le));
		lt_trie_replace(scriptdb->script_entries,
				lt_strlower(s),
				lt_script_ref(le),
				(lt_destroy_func_t)lt_script_unref);
		free(s);
	  bail1:
		if (subtag)
			xmlFree(subtag);
		if (desc)
			xmlFree(desc);
		lt_script_unref(le);
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
		lt_error_t *err = NULL;
		lt_script_t *le;

		retval->script_entries = lt_trie_new();
		lt_mem_add_ref(&retval->parent, retval->script_entries,
			       (lt_destroy_func_t)lt_trie_unref);

		le = lt_script_create();
		lt_script_set_tag(le, "*");
		lt_script_set_name(le, "Wildcard entry");
		lt_trie_replace(retval->script_entries,
				lt_script_get_tag(le),
				le,
				(lt_destroy_func_t)lt_script_unref);
		le = lt_script_create();
		lt_script_set_tag(le, "");
		lt_script_set_name(le, "Empty entry");
		lt_trie_replace(retval->script_entries,
				lt_script_get_tag(le),
				le,
				(lt_destroy_func_t)lt_script_unref);

		retval->xml = lt_xml_new();
		if (!retval->xml) {
			lt_script_db_unref(retval);
			retval = NULL;
			goto bail;
		}
		lt_mem_add_ref(&retval->parent, retval->xml,
			       (lt_destroy_func_t)lt_xml_unref);

		lt_script_db_parse(retval, &err);
		if (lt_error_is_set(err, LT_ERR_ANY)) {
			lt_error_print(err, LT_ERR_ANY);
			lt_script_db_unref(retval);
			retval = NULL;
			lt_error_unref(err);
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
	lt_return_val_if_fail (scriptdb != NULL, NULL);

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
		    const char     *subtag)
{
	lt_script_t *retval;
	char *s;

	lt_return_val_if_fail (scriptdb != NULL, NULL);
	lt_return_val_if_fail (subtag != NULL, NULL);

	s = strdup(subtag);
	retval = lt_trie_lookup(scriptdb->script_entries,
				lt_strlower(s));
	free(s);
	if (retval)
		return lt_script_ref(retval);

	return NULL;
}
