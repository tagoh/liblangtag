/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extlang-db.c
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
#include "lt-extlang.h"
#include "lt-extlang-private.h"
#include "lt-mem.h"
#include "lt-utils.h"
#include "lt-xml.h"
#include "lt-extlang-db.h"


/**
 * SECTION:lt-extlang-db
 * @Short_Description: An interface to access Extlang Database
 * @Title: Database - Extlang
 *
 * This class provides an interface to access Extlang database. which has been
 * registered as ISO 639 code.
 */
struct _lt_extlang_db_t {
	lt_mem_t    parent;
	lt_xml_t   *xml;
	GHashTable *extlang_entries;
};

/*< private >*/
static gboolean
lt_extlang_db_parse(lt_extlang_db_t  *extlangdb,
		    GError          **error)
{
	gboolean retval = TRUE;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	GError *err = NULL;
	int i, n;

	g_return_val_if_fail (extlangdb != NULL, FALSE);

	doc = lt_xml_get_subtag_registry(extlangdb->xml);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/registry/extlang", xctxt);
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
		xmlChar *subtag = NULL, *desc = NULL, *macrolang = NULL, *preferred = NULL, *prefix = NULL;
		lt_extlang_t *le = NULL;
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
					g_warning("Duplicate subtag element in extlang: previous value was '%s'",
						  subtag);
				} else {
					subtag = xmlNodeGetContent(cnode);
				}
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"added") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"text") == 0) {
				/* ignore it */
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"description") == 0) {
				/* wonder if many descriptions helps something. or is it a bug? */
				if (!desc)
					desc = xmlNodeGetContent(cnode);
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"macrolanguage") == 0) {
				if (macrolang) {
					g_warning("Duplicate macrolanguage element in extlang: previous value was '%s'",
						  macrolang);
				} else {
					macrolang = xmlNodeGetContent(cnode);
				}
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"preferred-value") == 0) {
				if (preferred) {
					g_warning("Duplicate preferred-value element in extlang: previous value was '%s'",
						  preferred);
				} else {
					preferred = xmlNodeGetContent(cnode);
				}
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"prefix") == 0) {
				if (prefix) {
					g_warning("Duplicate prefix element in extlang: previous value was '%s'",
						  prefix);
				} else {
					prefix = xmlNodeGetContent(cnode);
				}
			} else {
				g_warning("Unknown node under /registry/extlang: %s", cnode->name);
			}
			cnode = cnode->next;
		}
		if (!subtag) {
			g_warning("No subtag node: description = '%s', macrolanguage = '%s', preferred-value = '%s', prefix = '%s'",
				  desc, macrolang, preferred, prefix);
			goto bail1;
		}
		if (!desc) {
			g_warning("No description node: subtag = '%s', macrolanguage = '%s', preferred-value = '%s', prefix = '%s'",
				  subtag, macrolang, preferred, prefix);
			goto bail1;
		}
		le = lt_extlang_create();
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_extlang_t.");
			goto bail1;
		}
		lt_extlang_set_tag(le, (const gchar *)subtag);
		lt_extlang_set_name(le, (const gchar *)desc);
		if (macrolang)
			lt_extlang_set_macro_language(le, (const gchar *)macrolang);
		if (preferred)
			lt_extlang_set_preferred_tag(le, (const gchar *)preferred);
		if (prefix)
			lt_extlang_add_prefix(le, (const gchar *)prefix);

		s = g_strdup(lt_extlang_get_tag(le));
		g_hash_table_replace(extlangdb->extlang_entries,
				     lt_strlower(s),
				     lt_extlang_ref(le));
	  bail1:
		if (subtag)
			xmlFree(subtag);
		if (desc)
			xmlFree(desc);
		if (macrolang)
			xmlFree(macrolang);
		if (preferred)
			xmlFree(preferred);
		if (prefix)
			xmlFree(prefix);
		lt_extlang_unref(le);
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
 * lt_extlang_db_new:
 *
 * Create a new instance of a #lt_extlang_db_t.
 *
 * Returns: (transfer full): a new instance of #lt_extlang_db_t.
 */
lt_extlang_db_t *
lt_extlang_db_new(void)
{
	lt_extlang_db_t *retval = lt_mem_alloc_object(sizeof (lt_extlang_db_t));

	if (retval) {
		GError *err = NULL;
		lt_extlang_t *le;

		retval->extlang_entries = g_hash_table_new_full(g_str_hash,
								g_str_equal,
								g_free,
								(GDestroyNotify)lt_extlang_unref);
		lt_mem_add_ref(&retval->parent, retval->extlang_entries,
			       (lt_destroy_func_t)g_hash_table_destroy);

		le = lt_extlang_create();
		lt_extlang_set_tag(le, "*");
		lt_extlang_set_name(le, "Wildcard entry");
		g_hash_table_replace(retval->extlang_entries,
				     g_strdup(lt_extlang_get_tag(le)),
				     le);
		le = lt_extlang_create();
		lt_extlang_set_tag(le, "");
		lt_extlang_set_name(le, "Empty entry");
		g_hash_table_replace(retval->extlang_entries,
				     g_strdup(lt_extlang_get_tag(le)),
				     le);

		retval->xml = lt_xml_new();
		if (!retval->xml) {
			lt_extlang_db_unref(retval);
			retval = NULL;
			goto bail;
		}
		lt_mem_add_ref(&retval->parent, retval->xml,
			       (lt_destroy_func_t)lt_xml_unref);
		lt_extlang_db_parse(retval, &err);
		if (err) {
			g_printerr(err->message);
			lt_extlang_db_unref(retval);
			retval = NULL;
			g_error_free(err);
		}
	}
  bail:

	return retval;
}

/**
 * lt_extlang_db_ref:
 * @extlangdb: a #lt_extlang_db_t.
 *
 * Increases the reference count of @extlangdb.
 *
 * Returns: (transfer none): the same @extlangdb object.
 */
lt_extlang_db_t *
lt_extlang_db_ref(lt_extlang_db_t *extlangdb)
{
	g_return_val_if_fail (extlangdb != NULL, NULL);

	return lt_mem_ref(&extlangdb->parent);
}

/**
 * lt_extlang_db_unref:
 * @extlangdb: a #lt_extlang_db_t.
 *
 * Decreases the reference count of @extlangdb. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_extlang_db_unref(lt_extlang_db_t *extlangdb)
{
	if (extlangdb)
		lt_mem_unref(&extlangdb->parent);
}

/**
 * lt_extlang_db_lookup:
 * @extlangdb: a #lt_extlang_db_t.
 * @subtag: a subtag name to lookup.
 *
 * Lookup @lt_extlang_t if @subtag is valid and registered into the database.
 *
 * Returns: (transfer full): a #lt_extlang_t that meets with @subtag.
 *                           otherwise %NULL.
 */
lt_extlang_t *
lt_extlang_db_lookup(lt_extlang_db_t *extlangdb,
		     const gchar     *subtag)
{
	lt_extlang_t *retval;
	gchar *s;

	g_return_val_if_fail (extlangdb != NULL, NULL);
	g_return_val_if_fail (subtag != NULL, NULL);

	s = g_strdup(subtag);
	retval = g_hash_table_lookup(extlangdb->extlang_entries,
				     lt_strlower(s));
	g_free(s);
	if (retval)
		return lt_extlang_ref(retval);

	return NULL;
}
