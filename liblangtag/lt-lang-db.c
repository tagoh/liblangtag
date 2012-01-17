/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-lang-db.c
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
#include "lt-lang-private.h"
#include "lt-lang-db.h"


/**
 * SECTION: lt-lang-db
 * @Short_Description: An interface to access Language Database
 * @Title: Database - Language
 *
 * This class provides an interface to access Language database. which has been
 * registered as ISO 639 code.
 */
struct _lt_lang_db_t {
	lt_mem_t    parent;
	GHashTable *lang_entries;
};

/*< private >*/
static gboolean
lt_lang_db_parse(lt_lang_db_t  *langdb,
		 GError       **error)
{
	gboolean retval = TRUE;
	gchar *regfile = NULL;
	xmlParserCtxtPtr xmlparser;
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	GError *err = NULL;
	int i, n;

	g_return_val_if_fail (langdb != NULL, FALSE);

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
	xobj = xmlXPathEvalExpression((const xmlChar *)"/registry/language", xctxt);
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
		xmlChar *subtag = NULL, *desc = NULL, *scope = NULL, *macrolang = NULL;
		xmlChar *preferred = NULL, *suppress = NULL;
		lt_lang_t *le = NULL;
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
					g_warning("Duplicate subtag element in language: previous value was '%s'",
						  subtag);
				} else {
					subtag = xmlNodeGetContent(cnode);
				}
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"added") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"deprecated") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"text") == 0 ||
				   xmlStrcmp(cnode->name, (const xmlChar *)"comments") == 0) {
				/* ignore it */
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"description") == 0) {
				/* wonder if many descriptions helps something. or is it a bug? */
				if (!desc)
					desc = xmlNodeGetContent(cnode);
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"scope") == 0) {
				if (scope) {
					g_warning("Duplicate scope element in language: previous value was '%s'",
						  scope);
				} else {
					scope = xmlNodeGetContent(cnode);
				}
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"macrolanguage") == 0) {
				if (macrolang) {
					g_warning("Duplicate macrolanguage element in language: previous value was '%s'",
						  macrolang);
				} else {
					macrolang = xmlNodeGetContent(cnode);
				}
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"preferred-value") == 0) {
				if (preferred) {
					g_warning("Duplicate preferred-value element in language: previous value was '%s'",
						  preferred);
				} else {
					preferred = xmlNodeGetContent(cnode);
				}
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"suppress-script") == 0) {
				if (suppress) {
					g_warning("Duplicate suppress-script element in language: previous value was '%s'",
						  suppress);
				} else {
					suppress = xmlNodeGetContent(cnode);
				}
			} else {
				g_warning("Unknown node under /registry/language: %s", cnode->name);
			}
			cnode = cnode->next;
		}
		if (!subtag) {
			g_warning("No subtag node: description = '%s', scope = '%s', macrolanguage = '%s'",
				  desc, scope, macrolang);
			goto bail1;
		}
		if (!desc) {
			g_warning("No description node: subtag = '%s', scope = '%s', macrolanguage = '%s'",
				  subtag, scope, macrolang);
			goto bail1;
		}
		le = lt_lang_create();
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_lang_t.");
			goto bail1;
		}
		lt_lang_set_tag(le, (const gchar *)subtag);
		lt_lang_set_name(le, (const gchar *)desc);
		if (scope)
			lt_lang_set_scope(le, (const gchar *)scope);
		if (macrolang)
			lt_lang_set_macro_language(le, (const gchar *)macrolang);
		if (preferred)
			lt_lang_set_preferred_tag(le, (const gchar *)preferred);
		if (suppress)
			lt_lang_set_suppress_script(le, (const gchar *)suppress);

		s = g_strdup(lt_lang_get_tag(le));
		g_hash_table_replace(langdb->lang_entries,
				     lt_strlower(s),
				     lt_lang_ref(le));
	  bail1:
		if (subtag)
			xmlFree(subtag);
		if (desc)
			xmlFree(desc);
		if (scope)
			xmlFree(scope);
		if (macrolang)
			xmlFree(macrolang);
		if (preferred)
			xmlFree(preferred);
		if (suppress)
			xmlFree(suppress);
		lt_lang_unref(le);
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
/**
 * lt_lang_db_new:
 *
 * Create a new instance of a #lt_lang_db_t.
 *
 * Returns: (transfer full): a new instance of #lt_lang_db_t.
 */
lt_lang_db_t *
lt_lang_db_new(void)
{
	lt_lang_db_t *retval = lt_mem_alloc_object(sizeof (lt_lang_db_t));

	if (retval) {
		GError *err = NULL;
		lt_lang_t *le;

		retval->lang_entries = g_hash_table_new_full(g_str_hash,
							     g_str_equal,
							     g_free,
							     (GDestroyNotify)lt_lang_unref);
		lt_mem_add_ref(&retval->parent, retval->lang_entries,
			       (lt_destroy_func_t)g_hash_table_destroy);

		le = lt_lang_create();
		lt_lang_set_tag(le, "*");
		lt_lang_set_name(le, "Wildcard entry");
		g_hash_table_replace(retval->lang_entries,
				     g_strdup(lt_lang_get_tag(le)),
				     le);

		lt_lang_db_parse(retval, &err);
		if (err) {
			g_printerr(err->message);
			lt_lang_db_unref(retval);
			retval = NULL;
			g_error_free(err);
		}
	}

	return retval;
}

/**
 * lt_lang_db_ref:
 * @langdb: a #lt_lang_db_t.
 *
 * Increases the reference count of @langdb.
 *
 * Returns: (transfer none): the same @langdb object.
 */
lt_lang_db_t *
lt_lang_db_ref(lt_lang_db_t *langdb)
{
	g_return_val_if_fail (langdb != NULL, NULL);

	return lt_mem_ref(&langdb->parent);
}

/**
 * lt_lang_db_unref:
 * @langdb: a #lt_lang_db_t.
 *
 * Decreases the reference count of @langdb. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_lang_db_unref(lt_lang_db_t *langdb)
{
	if (langdb)
		lt_mem_unref(&langdb->parent);
}

/**
 * lt_lang_db_lookup:
 * @langdb: a #lt_lang_db_t.
 * @subtag: a subtag name to lookup.
 *
 * Lookup @lt_lang_t if @subtag is valid and registered into the database.
 *
 * Returns: (transfer full): a #lt_lang_t that meets with @subtag.
 */
lt_lang_t *
lt_lang_db_lookup(lt_lang_db_t *langdb,
		  const gchar  *subtag)
{
	lt_lang_t *retval;
	gchar *s;

	g_return_val_if_fail (langdb != NULL, NULL);
	g_return_val_if_fail (subtag != NULL, NULL);

	s = g_strdup(subtag);
	retval = g_hash_table_lookup(langdb->lang_entries, lt_strlower(s));
	g_free(s);
	if (retval)
		return lt_lang_ref(retval);

	return NULL;
}
