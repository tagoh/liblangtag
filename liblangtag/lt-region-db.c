/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-region-db.c
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
#include "lt-iter-private.h"
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-trie.h"
#include "lt-utils.h"
#include "lt-xml.h"
#include "lt-region.h"
#include "lt-region-private.h"
#include "lt-region-db.h"


/**
 * SECTION:lt-region-db
 * @Short_Description: An interface to access Region Database
 * @Title: Database - Region
 *
 * This class provides an interface to access Region database. which has been
 * registered as ISO 3166-1 and UN M.49 code.
 */
struct _lt_region_db_t {
	lt_iter_tmpl_t  parent;
	lt_xml_t       *xml;
	lt_trie_t      *region_entries;
};
typedef struct _lt_region_db_iter_t {
	lt_iter_t  parent;
	lt_iter_t *iter;
} lt_region_db_iter_t;

/*< private >*/
static lt_bool_t
lt_region_db_parse(lt_region_db_t  *regiondb,
		   lt_error_t     **error)
{
	lt_bool_t retval = TRUE;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	lt_error_t *err = NULL;
	int i, n;

	lt_return_val_if_fail (regiondb != NULL, FALSE);

	doc = lt_xml_get_subtag_registry(regiondb->xml);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		lt_error_set(&err, LT_ERR_OOM,
			     "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/registry/region", xctxt);
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
		xmlChar *subtag = NULL, *desc = NULL, *preferred = NULL;
		lt_region_t *le = NULL;
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
					lt_warning("Duplicate subtag element in region: previous value was '%s'",
						   subtag);
				} else {
					subtag = xmlNodeGetContent(cnode);
				}
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"added") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"text") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"deprecated") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"comments") == 0) {
				/* ignore it */
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"description") == 0) {
				/* wonder if many descriptions helps something. or is it a bug? */
				if (!desc)
					desc = xmlNodeGetContent(cnode);
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"preferred-value") == 0) {
				if (preferred) {
					lt_warning("Duplicate preferred-value element in region: previous value was '%s'",
						   preferred);
				} else {
					preferred = xmlNodeGetContent(cnode);
				}
			} else {
				lt_warning("Unknown node under /registry/region: %s", cnode->name);
			}
			cnode = cnode->next;
		}
		if (!subtag) {
			lt_warning("No subtag node: description = '%s', preferred-value = '%s'",
				   desc, preferred);
			goto bail1;
		}
		if (!desc) {
			lt_warning("No description node: subtag = '%s', preferred-value = '%s'",
				   subtag, preferred);
			goto bail1;
		}
		le = lt_region_create();
		if (!le) {
			lt_error_set(&err, LT_ERR_OOM,
				     "Unable to create an instance of lt_region_t.");
			goto bail1;
		}
		lt_region_set_tag(le, (const char *)subtag);
		lt_region_set_name(le, (const char *)desc);
		if (preferred)
			lt_region_set_preferred_tag(le, (const char *)preferred);

		s = strdup(lt_region_get_tag(le));
		lt_trie_replace(regiondb->region_entries,
				lt_strlower(s),
				lt_region_ref(le),
				(lt_destroy_func_t)lt_region_unref);
		free(s);
	  bail1:
		if (subtag)
			xmlFree(subtag);
		if (desc)
			xmlFree(desc);
		if (preferred)
			xmlFree(preferred);
		lt_region_unref(le);
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

static lt_iter_t *
_lt_region_db_iter_init(lt_iter_tmpl_t *tmpl)
{
	lt_region_db_iter_t *retval;
	lt_region_db_t *db = (lt_region_db_t *)tmpl;

	retval = malloc(sizeof (lt_region_db_iter_t));
	if (retval) {
		retval->iter = lt_iter_init((lt_iter_tmpl_t *)db->region_entries);
		if (!retval->iter) {
			free(retval);
			retval = NULL;
		}
	}

	return &retval->parent;
}

static void
_lt_region_db_iter_fini(lt_iter_t *iter)
{
	lt_region_db_iter_t *db_iter = (lt_region_db_iter_t *)iter;

	lt_iter_finish(db_iter->iter);
}

static lt_bool_t
_lt_region_db_iter_next(lt_iter_t    *iter,
			lt_pointer_t *key,
			lt_pointer_t *val)
{
	lt_region_db_iter_t *db_iter = (lt_region_db_iter_t *)iter;

	return lt_iter_next(db_iter->iter, key, val);
}

/*< public >*/
/**
 * lt_region_db_new:
 *
 * Create a new instance of a #lt_region_db_t.
 *
 * Returns: (transfer full): a new instance of #lt_region_db_t.
 */
lt_region_db_t *
lt_region_db_new(void)
{
	lt_region_db_t *retval = lt_mem_alloc_object(sizeof (lt_region_db_t));

	if (retval) {
		lt_error_t *err = NULL;
		lt_region_t *le;

		LT_ITER_TMPL_INIT (&retval->parent, _lt_region_db);

		retval->region_entries = lt_trie_new();
		lt_mem_add_ref((lt_mem_t *)retval, retval->region_entries,
			       (lt_destroy_func_t)lt_trie_unref);

		le = lt_region_create();
		lt_region_set_tag(le, "*");
		lt_region_set_name(le, "Wildcard entry");
		lt_trie_replace(retval->region_entries,
				lt_region_get_tag(le),
				le,
				(lt_destroy_func_t)lt_region_unref);
		le = lt_region_create();
		lt_region_set_tag(le, "");
		lt_region_set_name(le, "Empty entry");
		lt_trie_replace(retval->region_entries,
				lt_region_get_tag(le),
				le,
				(lt_destroy_func_t)lt_region_unref);

		retval->xml = lt_xml_new();
		if (!retval->xml) {
			lt_region_db_unref(retval);
			retval = NULL;
			goto bail;
		}
		lt_mem_add_ref((lt_mem_t *)retval, retval->xml,
			       (lt_destroy_func_t)lt_xml_unref);

		lt_region_db_parse(retval, &err);
		if (lt_error_is_set(err, LT_ERR_ANY)) {
			lt_error_print(err, LT_ERR_ANY);
			lt_region_db_unref(retval);
			retval = NULL;
			lt_error_unref(err);
		}
	}
  bail:

	return retval;
}

/**
 * lt_region_db_ref:
 * @regiondb: a #lt_region_db_t.
 *
 * Increases the reference count of @regiondb.
 *
 * Returns: (transfer none): the same @regiondb object.
 */
lt_region_db_t *
lt_region_db_ref(lt_region_db_t *regiondb)
{
	lt_return_val_if_fail (regiondb != NULL, NULL);

	return lt_mem_ref((lt_mem_t *)regiondb);
}

/**
 * lt_region_db_unref:
 * @regiondb: a #lt_region_db_t.
 *
 * Decreases the reference count of @regiondb. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_region_db_unref(lt_region_db_t *regiondb)
{
	if (regiondb)
		lt_mem_unref((lt_mem_t *)regiondb);
}

/**
 * lt_region_db_lookup:
 * @regiondb: a #lt_region_db_t.
 * @language_or_code: a region code to lookup.
 *
 * Lookup @lt_region_t if @language_or_code is valid and registered into
 * the database.
 *
 * Returns: (transfer full): a #lt_region_t that meets with @language_or_code.
 *                           otherwise %NULL.
 */
lt_region_t *
lt_region_db_lookup(lt_region_db_t *regiondb,
		    const char     *language_or_code)
{
	lt_region_t *retval;
	char *s;

	lt_return_val_if_fail (regiondb != NULL, NULL);
	lt_return_val_if_fail (language_or_code != NULL, NULL);

	s = strdup(language_or_code);
	retval = lt_trie_lookup(regiondb->region_entries,
				lt_strlower(s));
	free(s);
	if (retval)
		return lt_region_ref(retval);

	return NULL;
}
