/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-grandfathered-db.c
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
#include "lt-grandfathered.h"
#include "lt-grandfathered-private.h"
#include "lt-mem.h"
#include "lt-utils.h"
#include "lt-grandfathered-db.h"


struct _lt_grandfathered_db_t {
	lt_mem_t    parent;
	GHashTable *grandfathered_entries;
};

/*< private >*/
static gboolean
lt_grandfathered_db_parse(lt_grandfathered_db_t  *grandfathereddb,
			  GError                **error)
{
	gboolean retval = TRUE;
	gchar *regfile = NULL;
	xmlParserCtxtPtr xmlparser;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	GError *err = NULL;
	int i, n;

	g_return_val_if_fail (grandfathereddb != NULL, FALSE);

#ifdef GNOME_ENABLE_DEBUG
	regfile = g_build_filename(BUILDDIR, "data", "language-subtag-registry.xml", NULL);
	if (!g_file_test(regfile, G_FILE_TEST_EXISTS)) {
		g_free(regfile);
#endif
	regfile = g_build_filename(REGDATADIR, "language-subtag-registry.xml", NULL);
#ifdef GNOME_ENABLE_DEBUG
	}
#endif
	xmlparser = xmlNewParserCtxt();
	if (!xmlparser) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlParserCtxt.");
		goto bail;
	}
	doc = xmlCtxtReadFile(xmlparser, regfile, "UTF-8", 0);
	if (!doc) {
		g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "Unable to read the xml file: %s",
			    regfile);
		goto bail;
	}
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/registry/grandfathered", xctxt);
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
		lt_grandfathered_t *le = NULL;
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
					g_warning("Duplicate tag element in grandfathered: previous value was '%s'",
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
					g_warning("Duplicate preferred-value element in grandfathered: previous value was '%s'",
						  preferred);
				} else {
					preferred = xmlNodeGetContent(cnode);
				}
			} else {
				g_warning("Unknown node under /registry/grandfathered: %s", cnode->name);
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
		le = lt_grandfathered_create();
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_grandfathered_t.");
			goto bail1;
		}
		lt_grandfathered_set_tag(le, (const gchar *)tag);
		lt_grandfathered_set_name(le, (const gchar *)desc);
		if (preferred)
			lt_grandfathered_set_preferred_tag(le, (const gchar *)preferred);

		s = g_strdup(lt_grandfathered_get_tag(le));
		g_hash_table_replace(grandfathereddb->grandfathered_entries,
				     lt_strlower(s),
				     lt_grandfathered_ref(le));
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
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
		retval = FALSE;
	}
	g_free(regfile);

	if (xobj)
		xmlXPathFreeObject(xobj);
	if (xctxt)
		xmlXPathFreeContext(xctxt);
	if (doc)
		xmlFreeDoc(doc);
	if (xmlparser)
		xmlFreeParserCtxt(xmlparser);

	xmlCleanupParser();

	return retval;
}

/*< public >*/
lt_grandfathered_db_t *
lt_grandfathered_db_new(void)
{
	lt_grandfathered_db_t *retval = lt_mem_alloc_object(sizeof (lt_grandfathered_db_t));

	if (retval) {
		GError *err = NULL;

		retval->grandfathered_entries = g_hash_table_new_full(g_str_hash,
								      g_str_equal,
								      g_free,
								      (GDestroyNotify)lt_grandfathered_unref);
		lt_mem_add_ref(&retval->parent, retval->grandfathered_entries,
			       (lt_destroy_func_t)g_hash_table_destroy);

		lt_grandfathered_db_parse(retval, &err);
		if (err) {
			g_printerr(err->message);
			lt_grandfathered_db_unref(retval);
			retval = NULL;
			g_error_free(err);
		}
	}

	return retval;
}

lt_grandfathered_db_t *
lt_grandfathered_db_ref(lt_grandfathered_db_t *grandfathereddb)
{
	g_return_val_if_fail (grandfathereddb != NULL, NULL);

	return lt_mem_ref(&grandfathereddb->parent);
}

void
lt_grandfathered_db_unref(lt_grandfathered_db_t *grandfathereddb)
{
	if (grandfathereddb)
		lt_mem_unref(&grandfathereddb->parent);
}

lt_grandfathered_t *
lt_grandfathered_db_lookup(lt_grandfathered_db_t *grandfathereddb,
			   const gchar           *tag)
{
	lt_grandfathered_t *retval;
	gchar *s;

	g_return_val_if_fail (grandfathereddb != NULL, NULL);
	g_return_val_if_fail (tag != NULL, NULL);

	s = g_strdup(tag);
	retval = g_hash_table_lookup(grandfathereddb->grandfathered_entries,
				     lt_strlower(s));
	g_free(s);
	if (retval)
		return lt_grandfathered_ref(retval);

	return NULL;
}
