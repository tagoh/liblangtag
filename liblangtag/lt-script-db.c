/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-script-db.c
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
#include "lt-script-private.h"
#include "lt-script-db.h"


struct _lt_script_db_t {
	lt_mem_t    parent;
	GHashTable *entries;
};

/*< private >*/
static gboolean
lt_script_db_parse(lt_script_db_t  *script,
		   GError         **error)
{
	gchar *iso15924;
	xmlParserCtxtPtr xmlparser = xmlNewParserCtxt();
	xmlDocPtr doc = NULL;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	gboolean retval = TRUE;
	GError *err = NULL;
	int i, n;

	g_return_val_if_fail (script != NULL, FALSE);

	iso15924 = g_build_filename(ISO_PREFIX, "iso_15924.xml", NULL);

	if (!xmlparser) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlParserCtxt.");
		goto bail;
	}
	doc = xmlCtxtReadFile(xmlparser, iso15924, "UTF-8", 0);
	if (!doc) {
		g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "Unable to read the xml file: %s",
			    iso15924);
		goto bail;
	}
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/iso_15924_entries/iso_15924_entry", xctxt);
	if (!xobj) {
		g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "No valid elements for %s",
			    doc->name);
		goto bail;
	}
	n = xmlXPathNodeSetGetLength(xobj->nodesetval);

	for (i = 0; i < n; i++) {
		xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, i);
		xmlChar *p;
		lt_script_t *le;
		gchar *s;

		if (!ent) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		le = lt_script_create();
		if (!le) {
			g_set_error(&err, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of lt_script_t.");
			goto bail;
		}
		p = xmlGetProp(ent, (const xmlChar *)"name");
		lt_script_set_name(le, (const gchar *)p);
		if (g_strcmp0((const gchar *)p, lt_script_get_name(le)) != 0) {
			g_warning("buggy 'name' entry in %s: %s",
				  iso15924, p);
			xmlFree(p);
			goto bail1;
		}
		xmlFree(p);
		s = g_strdup(lt_script_get_name(le));
		g_hash_table_replace(script->entries,
				     lt_strlower(s),
				     lt_script_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"alpha_4_code");
		lt_script_set_code(le, (const gchar *)p);
		if (g_strcmp0((const gchar *)p, lt_script_get_alpha_code(le)) != 0) {
			g_warning("buggy 'alpha_4_code' entry for %s in %s: %s",
				  lt_script_get_name(le),
				  iso15924, p);
			xmlFree(p);
			goto bail1;
		}
		xmlFree(p);
		s = g_strdup(lt_script_get_alpha_code(le));
		g_hash_table_replace(script->entries,
				     lt_strlower(s),
				     lt_script_ref(le));
		p = xmlGetProp(ent, (const xmlChar *)"numeric_code");
		lt_script_set_code(le, (const gchar *)p);
		if (g_strcmp0((const gchar *)p, lt_script_get_numeric_code(le)) != 0) {
			g_warning("buggy 'numeric_code' entry for %s in %s: %s",
				  lt_script_get_name(le),
				  iso15924, p);
			xmlFree(p);
			goto bail1;
		}
		xmlFree(p);
		g_hash_table_replace(script->entries,
				     g_strdup(lt_script_get_numeric_code(le)),
				     lt_script_ref(le));
	  bail1:
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
	if (doc)
		xmlFreeDoc(doc);
	if (xmlparser)
		xmlFreeParserCtxt(xmlparser);
	g_free(iso15924);

	xmlCleanupParser();

	return retval;
}

/*< public >*/
lt_script_db_t *
lt_script_db_new(void)
{
	lt_script_db_t *retval = lt_mem_alloc_object(sizeof (lt_script_db_t));

	if (retval) {
		GError *err = NULL;

		retval->entries = g_hash_table_new_full(g_str_hash,
							g_str_equal,
							g_free,
							(GDestroyNotify)lt_script_unref);
		lt_mem_add_ref(&retval->parent, retval->entries,
			       (lt_destroy_func_t)g_hash_table_destroy);

		lt_script_db_parse(retval, &err);
		if (err) {
			g_printerr(err->message);
			lt_script_db_unref(retval);
			retval = NULL;
			g_error_free(err);
		}
	}

	return retval;
}

lt_script_db_t *
lt_script_db_ref(lt_script_db_t *script)
{
	g_return_val_if_fail (script != NULL, NULL);

	return lt_mem_ref(&script->parent);
}

void
lt_script_db_unref(lt_script_db_t *script)
{
	if (script)
		lt_mem_unref(&script->parent);
}

GList *
lt_script_db_get_scripts(lt_script_db_t *script)
{
	GHashTableIter iter;
	gpointer key, val;
	GList *retval = NULL;

	g_return_val_if_fail (script != NULL, NULL);

	g_hash_table_iter_init(&iter, script->entries);
	while (g_hash_table_iter_next(&iter, &key, &val)) {
		lt_script_t *le = val;

		retval = g_list_append(retval, (gpointer)lt_script_get_name(le));
	}

	return retval;
}

lt_script_t *
lt_script_db_lookup(lt_script_db_t *script,
		    const gchar    *script_name_or_alpha_code_or_num_code)
{
	lt_script_t *retval;
	gchar *s;

	g_return_val_if_fail (script != NULL, NULL);
	g_return_val_if_fail (script_name_or_alpha_code_or_num_code != NULL, NULL);

	s = g_strdup(script_name_or_alpha_code_or_num_code);
	retval = g_hash_table_lookup(script->entries,
				     lt_strlower(s));
	g_free(s);
	if (retval)
		return lt_script_ref(retval);

	return NULL;
}
