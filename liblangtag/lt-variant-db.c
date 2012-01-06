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

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include "lt-error.h"
#include "lt-variant.h"
#include "lt-variant-private.h"
#include "lt-mem.h"
#include "lt-utils.h"
#include "lt-variant-db.h"


struct _lt_variant_db_t {
	lt_mem_t    parent;
	GHashTable *variant_entries;
};

/*< private >*/
static gboolean
lt_variant_db_parse(lt_variant_db_t  *variantdb,
		    GError          **error)
{
	gboolean retval = TRUE;
	gchar *regfile = NULL;
	xmlParserCtxtPtr xmlparser;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	GError *err = NULL;
	int i, n;

	g_return_val_if_fail (variantdb != NULL, FALSE);

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
		xmlChar *subtag = NULL, *desc = NULL, *prefix = NULL;
		lt_variant_t *le = NULL;
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
				desc = xmlNodeGetContent(cnode);
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"prefix") == 0) {
				prefix = xmlNodeGetContent(cnode);
			} else {
				g_warning("Unknown node under /registry/variant: %s", cnode->name);
			}
			cnode = cnode->next;
		}
		if (!subtag) {
			g_warning("No subtag node: description = '%s', prefix = '%s'",
				  desc, prefix);
			goto bail1;
		}
		if (!desc) {
			g_warning("No description node: subtag = '%s', prefix = '%s'",
				  subtag, prefix);
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
		if (prefix)
			lt_variant_set_prefix(le, (const gchar *)prefix);

		s = g_strdup(lt_variant_get_tag(le));
		g_hash_table_replace(variantdb->variant_entries,
				     lt_strlower(s),
				     lt_variant_ref(le));
	  bail1:
		if (subtag)
			xmlFree(subtag);
		if (desc)
			xmlFree(desc);
		if (prefix)
			xmlFree(prefix);
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
lt_variant_db_t *
lt_variant_db_new(void)
{
	lt_variant_db_t *retval = lt_mem_alloc_object(sizeof (lt_variant_db_t));

	if (retval) {
		GError *err = NULL;

		retval->variant_entries = g_hash_table_new_full(g_str_hash,
								g_str_equal,
								g_free,
								(GDestroyNotify)lt_variant_unref);
		lt_mem_add_ref(&retval->parent, retval->variant_entries,
			       (lt_destroy_func_t)g_hash_table_destroy);

		lt_variant_db_parse(retval, &err);
		if (err) {
			g_printerr(err->message);
			lt_variant_db_unref(retval);
			retval = NULL;
			g_error_free(err);
		}
	}

	return retval;
}

lt_variant_db_t *
lt_variant_db_ref(lt_variant_db_t *variantdb)
{
	g_return_val_if_fail (variantdb != NULL, NULL);

	return lt_mem_ref(&variantdb->parent);
}

void
lt_variant_db_unref(lt_variant_db_t *variantdb)
{
	if (variantdb)
		lt_mem_unref(&variantdb->parent);
}

lt_variant_t *
lt_variant_db_lookup(lt_variant_db_t *variantdb,
		     const gchar     *subtag)
{
	lt_variant_t *retval;

	g_return_val_if_fail (variantdb != NULL, NULL);
	g_return_val_if_fail (subtag != NULL, NULL);

	retval = g_hash_table_lookup(variantdb->variant_entries, subtag);
	if (retval)
		return lt_variant_ref(retval);

	return NULL;
}
