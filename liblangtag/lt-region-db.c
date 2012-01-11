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
#include "lt-error.h"
#include "lt-mem.h"
#include "lt-utils.h"
#include "lt-region.h"
#include "lt-region-private.h"
#include "lt-region-db.h"


struct _lt_region_db_t {
	lt_mem_t    parent;
	GHashTable *region_entries;
	GHashTable *region_codes;
};


/*< private >*/
static gboolean
lt_region_db_parse(lt_region_db_t  *regiondb,
		   GError         **error)
{
	gboolean retval = TRUE;
	gchar *regfile = NULL;
	xmlParserCtxtPtr xmlparser;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	GError *err = NULL;
	int i, n;

	g_return_val_if_fail (regiondb != NULL, FALSE);

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
	xobj = xmlXPathEvalExpression((const xmlChar *)"/registry/region", xctxt);
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
		lt_region_t *le = NULL;
		gchar *s;

		if (!ent) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		cnode = ent->children;
		while (cnode != NULL) {
			if (xmlStrcmp(cnode->name, (const xmlChar *)"subtag") == 0) {
				subtag = xmlNodeGetContent(cnode);
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"added") == 0) {
				/* ignore it */
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"description") == 0) {
				/* wonder if many descriptions helps something. or is it a bug? */
				if (desc)
					xmlFree(desc);
				desc = xmlNodeGetContent(cnode);
			} else {
				g_warning("Unknown node under /registry/region: %s", cnode->name);
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
		le = lt_region_create();
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_region_t.");
			goto bail1;
		}
		lt_region_set_tag(le, (const gchar *)subtag);
		lt_region_set_name(le, (const gchar *)desc);

		s = g_strdup(lt_region_get_tag(le));
		g_hash_table_replace(regiondb->region_entries,
				     lt_strlower(s),
				     lt_region_ref(le));
	  bail1:
		if (subtag)
			xmlFree(subtag);
		if (desc)
			xmlFree(desc);
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
							     g_free,
							     (GDestroyNotify)lt_region_unref);
		lt_mem_add_ref(&retval->parent, retval->region_codes,
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

lt_region_t *
lt_region_db_lookup(lt_region_db_t *region,
		    const gchar    *language_or_code)
{
	lt_region_t *retval;

	g_return_val_if_fail (region != NULL, NULL);
	g_return_val_if_fail (language_or_code != NULL, NULL);

	retval = g_hash_table_lookup(region->region_entries, language_or_code);
	if (retval)
		return lt_region_ref(retval);

	return NULL;
}
