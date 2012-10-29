/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-grandfathered-db.c
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

#include <string.h>
#include <libxml/xpath.h>
#include "lt-error.h"
#include "lt-grandfathered.h"
#include "lt-grandfathered-private.h"
#include "lt-iter-private.h"
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-trie.h"
#include "lt-utils.h"
#include "lt-xml.h"
#include "lt-grandfathered-db.h"


/**
 * SECTION: lt-grandfathered-db
 * @Short_Description: An interface to access Grandfathered Database
 * @Title: Database - Grandfathered
 *
 * This class provides an interface to access Grandfathered database.
 * which has been registered under RFC 3066 and mostly deprecated.
 */
struct _lt_grandfathered_db_t {
	lt_iter_tmpl_t  parent;
	lt_xml_t       *xml;
	lt_trie_t      *grandfathered_entries;
};
typedef struct _lt_grandfathered_db_iter_t {
	lt_iter_t  parent;
	lt_iter_t *iter;
} lt_grandfathered_db_iter_t;

/*< private >*/
static lt_bool_t
lt_grandfathered_db_parse(lt_grandfathered_db_t  *grandfathereddb,
			  lt_error_t            **error)
{
	lt_bool_t retval = TRUE;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	lt_error_t *err = NULL;
	int i, n;

	lt_return_val_if_fail (grandfathereddb != NULL, FALSE);

	doc = lt_xml_get_subtag_registry(grandfathereddb->xml);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		lt_error_set(&err, LT_ERR_OOM,
			     "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/registry/grandfathered", xctxt);
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
		lt_grandfathered_t *le = NULL;
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
					lt_warning("Duplicate tag element in grandfathered: previous value was '%s'",
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
					lt_warning("Duplicate preferred-value element in grandfathered: previous value was '%s'",
						   preferred);
				} else {
					preferred = xmlNodeGetContent(cnode);
				}
			} else {
				lt_warning("Unknown node under /registry/grandfathered: %s", cnode->name);
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
		le = lt_grandfathered_create();
		if (!le) {
			lt_error_set(&err, LT_ERR_OOM,
				     "Unable to create an instance of lt_grandfathered_t.");
			goto bail1;
		}
		lt_grandfathered_set_tag(le, (const char *)tag);
		lt_grandfathered_set_name(le, (const char *)desc);
		if (preferred)
			lt_grandfathered_set_preferred_tag(le, (const char *)preferred);

		s = strdup(lt_grandfathered_get_tag(le));
		lt_trie_replace(grandfathereddb->grandfathered_entries,
				lt_strlower(s),
				lt_grandfathered_ref(le),
				(lt_destroy_func_t)lt_grandfathered_unref);
		free(s);
	  bail1:
		if (tag)
			xmlFree(tag);
		if (desc)
			xmlFree(desc);
		if (preferred)
			xmlFree(preferred);
		lt_grandfathered_unref(le);
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
_lt_grandfathered_db_iter_init(lt_iter_tmpl_t *tmpl)
{
	lt_grandfathered_db_iter_t *retval;
	lt_grandfathered_db_t *db = (lt_grandfathered_db_t *)tmpl;

	retval = malloc(sizeof (lt_grandfathered_db_iter_t));
	if (retval) {
		retval->iter = lt_iter_init((lt_iter_tmpl_t *)db->grandfathered_entries);
		if (!retval->iter) {
			free(retval);
			retval = NULL;
		}
	}

	return &retval->parent;
}

static void
_lt_grandfathered_db_iter_fini(lt_iter_t *iter)
{
	lt_grandfathered_db_iter_t *db_iter = (lt_grandfathered_db_iter_t *)iter;

	lt_iter_finish(db_iter->iter);
}

static lt_bool_t
_lt_grandfathered_db_iter_next(lt_iter_t    *iter,
			       lt_pointer_t *key,
			       lt_pointer_t *val)
{
	lt_grandfathered_db_iter_t *db_iter = (lt_grandfathered_db_iter_t *)iter;

	return lt_iter_next(db_iter->iter, key, val);
}

/*< public >*/
/**
 * lt_grandfathered_db_new:
 *
 * Create a new instance of a #lt_grandfathered_db_t.
 *
 * Returns: (transfer full): a new instance of #lt_grandfathered_db_t.
 */
lt_grandfathered_db_t *
lt_grandfathered_db_new(void)
{
	lt_grandfathered_db_t *retval = lt_mem_alloc_object(sizeof (lt_grandfathered_db_t));

	if (retval) {
		lt_error_t *err = NULL;

		LT_ITER_TMPL_INIT (&retval->parent, _lt_grandfathered_db);

		retval->grandfathered_entries = lt_trie_new();
		lt_mem_add_ref((lt_mem_t *)retval, retval->grandfathered_entries,
			       (lt_destroy_func_t)lt_trie_unref);

		retval->xml = lt_xml_new();
		if (!retval->xml) {
			lt_grandfathered_db_unref(retval);
			retval = NULL;
			goto bail;
		}
		lt_mem_add_ref((lt_mem_t *)retval, retval->xml,
			       (lt_destroy_func_t)lt_xml_unref);

		lt_grandfathered_db_parse(retval, &err);
		if (lt_error_is_set(err, LT_ERR_ANY)) {
			lt_error_print(err, LT_ERR_ANY);
			lt_grandfathered_db_unref(retval);
			retval = NULL;
			lt_error_unref(err);
		}
	}
  bail:

	return retval;
}

/**
 * lt_grandfathered_db_ref:
 * @grandfathereddb: a #lt_grandfathered_db_t.
 *
 * Increases the reference count of @grandfathereddb.
 *
 * Returns: (transfer none): the same @grandfathereddb object.
 */
lt_grandfathered_db_t *
lt_grandfathered_db_ref(lt_grandfathered_db_t *grandfathereddb)
{
	lt_return_val_if_fail (grandfathereddb != NULL, NULL);

	return lt_mem_ref((lt_mem_t *)grandfathereddb);
}

/**
 * lt_grandfathered_db_unref:
 * @grandfathereddb: a #lt_grandfathered_db_t.
 *
 * Decreases the reference count of @grandfathereddb. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_grandfathered_db_unref(lt_grandfathered_db_t *grandfathereddb)
{
	if (grandfathereddb)
		lt_mem_unref((lt_mem_t *)grandfathereddb);
}

/**
 * lt_grandfathered_db_lookup:
 * @grandfathereddb: a #lt_grandfathered_db_t.
 * @tag: a tag name to lookup.
 *
 * Lookup @lt_grandfathered_t if @tag is valid and registered into the database.
 *
 * Returns: (transfer full): a #lt_grandfathered_t that meets with @tag.
 *                           otherwise %NULL.
 */
lt_grandfathered_t *
lt_grandfathered_db_lookup(lt_grandfathered_db_t *grandfathereddb,
			   const char            *tag)
{
	lt_grandfathered_t *retval;
	char *s;

	lt_return_val_if_fail (grandfathereddb != NULL, NULL);
	lt_return_val_if_fail (tag != NULL, NULL);

	s = strdup(tag);
	retval = lt_trie_lookup(grandfathereddb->grandfathered_entries,
				lt_strlower(s));
	free(s);
	if (retval)
		return lt_grandfathered_ref(retval);

	return NULL;
}
