/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-ldml.c
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

#include <string.h>
#include <libxml/xpath.h>
#include "lt-error.h"
#include "lt-ext-module.h"
#include "lt-xml.h"


typedef enum _lt_ext_ldml_state_t {
	STATE_NONE = 0,
	STATE_ATTRIBUTE,
	STATE_KEY,
	STATE_TYPE,
	STATE_END
} lt_ext_ldml_state_t;
typedef struct _lt_ext_ldml_data_t {
	lt_ext_module_data_t  parent;
	lt_xml_cldr_t         current_type;
	lt_ext_ldml_state_t   state;
	GList                *attributes;
	GList                *tags;
} lt_ext_ldml_data_t;

/*< private >*/
static gboolean
_lt_ext_ldml_lookup_type(lt_ext_ldml_data_t  *data,
			 const gchar         *subtag,
			 GError             **error)
{
	lt_xml_t *xml = NULL;
	xmlDocPtr doc;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	gint i, n;
	gchar key[4], *xpath_string = NULL;
	GList *l;
	GString *s;
	gboolean retval = FALSE;

	g_return_val_if_fail (data->current_type > 0, FALSE);

	l = g_list_last(data->tags);
	if (l == NULL) {
		g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
			    "Invalid internal state. failed to find a key container.");
		goto bail;
	}
	s = l->data;
	strncpy(key, s->str, 2);
	key[2] = 0;

	xml = lt_xml_new();
	doc = lt_xml_get_cldr_bcp47(xml, data->current_type);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		g_set_error(error, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xpath_string = g_strdup_printf("/ldmlBCP47/keyword/key[@name = '%s']", key);
	xobj = xmlXPathEvalExpression((const xmlChar *)xpath_string, xctxt);
	if (!xobj) {
		g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "No valid elements for %s: %s",
			    doc->name, xpath_string);
		goto bail;
	}
	n = xmlXPathNodeSetGetLength(xobj->nodesetval);

	for (i = 0; i < n; i++) {
		xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, i);
		xmlNodePtr cnode;
		xmlChar *name;

		if (!ent) {
			g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		cnode = ent->children;
		while (cnode != NULL) {
			if (xmlStrcmp(cnode->name, (const xmlChar *)"type") == 0) {
				name = xmlGetProp(cnode, (const xmlChar *)"name");
				if (name && g_ascii_strcasecmp((const gchar *)name, subtag) == 0) {
					retval = TRUE;
					xmlFree(name);
					goto bail;
				} else if (g_strcmp0((const gchar *)name, "CODEPOINTS") == 0) {
					gsize len = strlen(subtag), j;
					static const gchar *hexdigit = "0123456789abcdefABCDEF";
					gchar *p;
					guint64 x;

					/* an exception to deal with the unicode code point. */
					if (len >= 4 && len <= 6) {
						for (j = 0; j < len; j++) {
							if (!strchr(hexdigit, subtag[j]))
								goto bail2;
						}
						x = g_ascii_strtoull(subtag, &p, 16);
						if (p && p[0] == 0 && x <= 0x10ffff) {
							retval = TRUE;
							xmlFree(name);
							goto bail;
						}
					}
				  bail2:;
				}
				xmlFree(name);
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"text") == 0) {
				/* ignore */
			} else {
				g_warning("Unknown node under /ldmlBCP47/keyword/key: %s", cnode->name);
			}
			cnode = cnode->next;
		}
	}
  bail:
	g_free(xpath_string);
	if (xobj)
		xmlXPathFreeObject(xobj);
	if (xctxt)
		xmlXPathFreeContext(xctxt);
	if (xml)
		lt_xml_unref(xml);

	return retval;
}

static gboolean
_lt_ext_ldml_lookup_key(lt_ext_ldml_data_t  *data,
			const gchar         *subtag,
			GError             **error)
{
	gint i, j, n;
	lt_xml_t *xml = lt_xml_new();
	gboolean retval = FALSE;

	for (i = LT_XML_CLDR_BCP47_BEGIN + 1; i < LT_XML_CLDR_BCP47_END; i++) {
		xmlDocPtr doc = lt_xml_get_cldr_bcp47(xml, i);
		xmlXPathContextPtr xctxt = NULL;
		xmlXPathObjectPtr xobj = NULL;

		xctxt = xmlXPathNewContext(doc);
		if (!xctxt) {
			g_set_error(error, LT_ERROR, LT_ERR_OOM,
				    "Unable to create an instance of xmlXPathContextPtr.");
			goto bail1;
		}
		xobj = xmlXPathEvalExpression((const xmlChar *)"/ldmlBCP47/keyword/key", xctxt);
		if (!xobj) {
			g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "No valid elements for %s",
				    doc->name);
			goto bail1;
		}
		n = xmlXPathNodeSetGetLength(xobj->nodesetval);

		for (j = 0; j < n; j++) {
			xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, j);
			xmlChar *name;

			if (!ent) {
				g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_XML,
					    "Unable to obtain the xml node via XPath.");
				goto bail1;
			}
			name = xmlGetProp(ent, (const xmlChar *)"name");
			if (g_ascii_strcasecmp((const gchar *)name, subtag) == 0) {
				data->current_type = i;
				data->state = STATE_TYPE;
				xmlFree(name);
				retval = TRUE;
				goto bail1;
			}
			xmlFree(name);
		}
	  bail1:
		if (xobj)
			xmlXPathFreeObject(xobj);
		if (xctxt)
			xmlXPathFreeContext(xctxt);
		if (*error || retval)
			goto bail;
	}
	g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
		    "Invalid key for -u- extension: %s",
		    subtag);
  bail:
	lt_xml_unref(xml);

	return *error == NULL;
}

static void
_lt_ext_ldml_destroy_data(gpointer data)
{
	lt_ext_ldml_data_t *d = (lt_ext_ldml_data_t *)data;

	if (d->attributes) {
		GList *l;

		for (l = d->attributes; l != NULL; l = g_list_next(l)) {
			g_free(l->data);
		}
		g_list_free(d->attributes);
	}
	if (d->tags) {
		GList *l;

		for (l = d->tags; l != NULL; l = g_list_next(l)) {
			g_string_free(l->data, TRUE);
		}
		g_list_free(d->tags);
	}
}

static gchar
_lt_ext_ldml_get_singleton(void)
{
	return 'u';
}

static lt_ext_module_data_t *
_lt_ext_ldml_create_data(void)
{
	lt_ext_module_data_t *retval = lt_ext_module_data_new(sizeof (lt_ext_ldml_data_t),
							      _lt_ext_ldml_destroy_data);

	if (retval) {
		lt_ext_ldml_data_t *d = (lt_ext_ldml_data_t *)retval;

		d->state = STATE_NONE;
		d->current_type = LT_XML_CLDR_BCP47_BEGIN;
	}

	return retval;
}

static gboolean
_lt_ext_ldml_precheck_tag(lt_ext_module_data_t  *data,
			  const lt_tag_t        *tag,
			  GError               **error)
{
	if (lt_tag_get_grandfathered(tag)) {
		g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
			    "Grandfathered tags aren't allowed to have LDML.");
		return FALSE;
	}
	if (lt_tag_get_extlang(tag)) {
		g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
			    "Extlang tags aren't allowed to have LDML.");
		return FALSE;
	}

	return TRUE;
}

static gboolean
_lt_ext_ldml_parse_tag(lt_ext_module_data_t  *data,
		       const gchar           *subtag,
		       GError               **error)
{
	lt_ext_ldml_data_t *d = (lt_ext_ldml_data_t *)data;
	GError *err = NULL;
	gboolean retval = TRUE;
	gsize len = strlen(subtag);

  restate:
	switch (d->state) {
	    case STATE_NONE:
		    if (len >= 3 && len <= 8) {
			    d->state = STATE_ATTRIBUTE;
		    } else if (len == 2) {
			    d->state = STATE_KEY;
		    } else {
			    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"Invalid syntax: expected to see an attribute or a key, but `%s'", subtag);
			    break;
		    }
		    goto restate;
	    case STATE_ATTRIBUTE:
		    if (len >= 3 && len <= 8) {
			    d->attributes = g_list_append(d->attributes,
							  g_strdup(subtag));
			    /* next words may be still an attribute. keep the state */
		    } else {
			    /* it may be a key */
			    d->state = STATE_KEY;
			    goto restate;
		    }
		    break;
	    case STATE_KEY:
		    if (len != 2) {
			    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"Invalid syntax: expected to see a key, but `%s'", subtag);
			    break;
		    }
		    _lt_ext_ldml_lookup_key(d, subtag, &err);
		    d->tags = g_list_append(d->tags, g_string_new(subtag));
		    break;
	    case STATE_TYPE:
		    if (len >= 3 && len <= 8) {
			    GList *l;
			    GString *s;

			    if (!_lt_ext_ldml_lookup_type(d, subtag, &err)) {
				    if (!err) {
					    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
							"Unknown -u- extension type: %s", subtag);
				    }
				    break;
			    }
			    l = g_list_last(d->tags);
			    if (l == NULL) {
				    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
						"Invalid internal state. failed to find a key container.");
				    break;
			    }
			    s = l->data;
			    g_string_append_printf(s, "-%s", subtag);
		    } else if (len == 2) {
			    d->state = STATE_KEY;
			    goto restate;
		    } else {
			    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"Invalid syntax: expected to see a type or a key, but `%s'", subtag);
			    break;
		    }
		    break;
	    default:
		    g_warn_if_reached();
		    break;
	}

	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
		retval = FALSE;
	}

	return retval;
}

static gchar *
_lt_ext_ldml_get_tag(lt_ext_module_data_t *data)
{
	lt_ext_ldml_data_t *d = (lt_ext_ldml_data_t *)data;
	GString *s = g_string_new(NULL);
	GList *l;

	if (d->attributes) {
		for (l = d->attributes; l != NULL; l = g_list_next(l)) {
			const gchar *a = l->data;

			if (s->len > 0)
				g_string_append_c(s, '-');
			g_string_append(s, a);
		}
	}
	if (d->tags) {
		for (l = d->tags; l != NULL; l = g_list_next(l)) {
			const GString *t = l->data;

			if (s->len > 0)
				g_string_append_c(s, '-');
			if (t->len == 2) {
				/* XXX: do we need to auto-complete the clipped type here? */
			}
			g_string_append(s, t->str);
		}
	}

	return g_string_free(s, FALSE);
}

static gboolean
_lt_ext_ldml_validate_tag(lt_ext_module_data_t *data)
{
	return TRUE;
}

static const lt_ext_module_funcs_t __funcs = {
	_lt_ext_ldml_get_singleton,
	_lt_ext_ldml_create_data,
	_lt_ext_ldml_precheck_tag,
	_lt_ext_ldml_parse_tag,
	_lt_ext_ldml_get_tag,
	_lt_ext_ldml_validate_tag,
};

/*< public >*/
gint
module_get_version(void)
{
	return LT_EXT_MODULE_VERSION;
}

const lt_ext_module_funcs_t *
module_get_funcs(void)
{
	return &__funcs;
}
