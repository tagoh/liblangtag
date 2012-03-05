/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-ldml-t.c
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
#include "lt-tag.h"
#include "lt-utils.h"
#include "lt-xml.h"


typedef enum _lt_ext_ldml_t_state_t {
	STATE_NONE = 0,
	STATE_LANG,
	STATE_SCRIPT,
	STATE_REGION,
	STATE_VARIANT,
	STATE_VARIANT2,
	STATE_FIELD,
	STATE_FIELD2,
	STATE_END
} lt_ext_ldml_t_state_t;
typedef struct _lt_ext_ldml_t_data_t {
	lt_ext_module_data_t   parent;
	lt_ext_ldml_t_state_t  state;
	lt_tag_t              *tag;
	GList                 *fields;
} lt_ext_ldml_t_data_t;

/*< private >*/
static gboolean
_lt_ext_ldml_t_lookup_type(lt_ext_ldml_t_data_t  *data,
			   const gchar           *subtag,
			   GError               **error)
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

	l = g_list_last(data->fields);
	if (l == NULL) {
		g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
			    "Invalid internal state. failed to find a key container.");
		goto bail;
	}
	s = l->data;
	strncpy(key, s->str, 2);
	key[2] = 0;

	xml = lt_xml_new();
	doc = lt_xml_get_cldr(xml, LT_XML_CLDR_BCP47_TRANSFORM);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		g_set_error(error, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xpath_string = g_strdup_printf("/ldmlBCP47/keyword/key[@extension = 't' and @name = '%s']", key);
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
_lt_ext_ldml_t_lookup_key(lt_ext_ldml_t_data_t  *data,
			  const gchar           *subtag,
			  GError               **error)
{
	gint i, n;
	lt_xml_t *xml = lt_xml_new();
	gboolean retval = FALSE;
	xmlDocPtr doc = lt_xml_get_cldr(xml, LT_XML_CLDR_BCP47_TRANSFORM);
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	xmlChar *name = NULL;

	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		g_set_error(error, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/ldmlBCP47/keyword/key[@extension = 't']", xctxt);
	if (!xobj) {
		g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "No valid elements for %s",
			    doc->name);
		goto bail;
	}
	n = xmlXPathNodeSetGetLength(xobj->nodesetval);

	for (i = 0; i < n; i++) {
		xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, i);

		if (!ent) {
			g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_XML,
				    "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		if (name)
			xmlFree(name);
		name = xmlGetProp(ent, (const xmlChar *)"name");
		if (g_ascii_strcasecmp((const gchar *)name, subtag) == 0) {
			retval = TRUE;
			break;
		}
	}
  bail:
	if (name)
		xmlFree(name);
	if (xobj)
		xmlXPathFreeObject(xobj);
	if (xctxt)
		xmlXPathFreeContext(xctxt);
	lt_xml_unref(xml);

	return retval;
}

static void
_lt_ext_ldml_t_destroy_data(gpointer data)
{
	lt_ext_ldml_t_data_t *d = (lt_ext_ldml_t_data_t *)data;

	lt_tag_unref(d->tag);
	if (d->fields) {
		GList *l;

		for (l = d->fields; l != NULL; l = g_list_next(l)) {
			g_string_free(l->data, TRUE);
		}
		g_list_free(d->fields);
	}
}

static gchar
_lt_ext_ldml_t_get_singleton(void)
{
	return 't';
}

static lt_ext_module_data_t *
_lt_ext_ldml_t_create_data(void)
{
	lt_ext_module_data_t *retval = lt_ext_module_data_new(sizeof (lt_ext_ldml_t_data_t),
							      _lt_ext_ldml_t_destroy_data);

	if (retval) {
		lt_ext_ldml_t_data_t *d = (lt_ext_ldml_t_data_t *)retval;

		d->state = STATE_NONE;
		d->tag = lt_tag_new();
	}

	return retval;
}

static gboolean
_lt_ext_ldml_t_precheck_tag(lt_ext_module_data_t  *data,
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
_lt_ext_ldml_t_parse_tag(lt_ext_module_data_t  *data,
			 const gchar           *subtag,
			 GError               **error)
{
	lt_ext_ldml_t_data_t *d = (lt_ext_ldml_t_data_t *)data;
	GError *err = NULL;
	gboolean retval = TRUE;
	gsize len = strlen(subtag);
	const GString *s;

	if (d->state != STATE_FIELD && d->state != STATE_FIELD2) {
		if (_lt_ext_ldml_t_lookup_key(d, subtag, &err)) {
			goto setup_field_key;
		} else if (err) {
			goto bail;
		}
	}
	switch (d->state) {
	    case STATE_NONE:
		    if (!lt_tag_parse(d->tag, subtag, &err))
			    break;
		    if (lt_tag_get_language(d->tag) == NULL) {
			    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"No such language subtag: %s",
					subtag);
			    break;
		    }
		    d->state = STATE_LANG;
		    break;
	    case STATE_LANG:
		    if (!lt_tag_parse_with_extra_token(d->tag, subtag, &err))
			    break;
		    if (lt_tag_get_extlang(d->tag)) {
			    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"Extlang tag isn't allowed: %s",
					subtag);
			    break;
		    } else if (lt_tag_get_script(d->tag)) {
			    d->state = STATE_SCRIPT;
		    } else {
			    goto check_region;
		    }
		    break;
	    case STATE_SCRIPT:
		    if (!lt_tag_parse_with_extra_token(d->tag, subtag, &err))
			    break;
	    check_region:
		    if (lt_tag_get_region(d->tag)) {
			    d->state = STATE_REGION;
		    } else {
			    goto check_variant;
		    }
		    break;
	    case STATE_REGION:
	    case STATE_VARIANT:
		    if (!lt_tag_parse_with_extra_token(d->tag, subtag, &err))
			    break;
	    check_variant:
		    if (lt_tag_get_extension(d->tag)) {
			    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"Extension tag isn't allowed");
			    break;
		    } else if ((s = lt_tag_get_privateuse(d->tag)) && s->len > 0) {
			    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"PrivateUse tag isn't allowed");
			    break;
		    } else if (lt_tag_get_variants(d->tag)) {
			    d->state = STATE_VARIANT;
		    } else {
			    g_warn_if_reached();
		    }
		    break;
	    case STATE_FIELD2:
		    if (!lt_tag_get_language(d->tag)) {
			    if (_lt_ext_ldml_t_lookup_key(d, subtag, &err)) {
			      setup_field_key:
				    d->state = STATE_FIELD;
				    d->fields = g_list_append(d->fields, g_string_new(subtag));
				    break;
			    } else if (err) {
				    break;
			    }
		    }
	    case STATE_FIELD:
		    if (len >= 3 && len <= 8) {
			    GList *l;
			    GString *s;

			    if (!_lt_ext_ldml_t_lookup_type(d, subtag, &err)) {
				    if (!err) {
					    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
							"Unknown -t- extension type: %s", subtag);
				    }
				    break;
			    }
			    l = g_list_last(d->fields);
			    if (l == NULL) {
				    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
						"Invalid internal state. failed to find a key container.");
				    break;
			    }
			    s = l->data;
			    g_string_append_printf(s, "-%s", subtag);
			    d->state = STATE_FIELD2;
		    } else {
			    g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"Invalid syntax: expected to see an field value, but `%s'", subtag);
		    }
		    break;
	    default:
		    g_warn_if_reached();
		    break;
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

	return retval;
}

static gchar *
_lt_ext_ldml_t_get_tag(lt_ext_module_data_t *data)
{
	lt_ext_ldml_t_data_t *d = (lt_ext_ldml_t_data_t *)data;
	GString *s = g_string_new(lt_tag_get_string(d->tag));
	GList *l;

	if (d->fields) {
		for (l = d->fields; l != NULL; l = g_list_next(l)) {
			const GString *t = l->data;
			gchar *ts = g_strdup(t->str);

			if (s->len > 0)
				g_string_append_c(s, '-');
			if (t->len == 2) {
				/* XXX: do we need to auto-complete the clipped type here? */
			}
			g_string_append(s, lt_strlower(ts));
			g_free(ts);
		}
	}

	return g_string_free(s, FALSE);
}

static gboolean
_lt_ext_ldml_t_validate_tag(lt_ext_module_data_t *data)
{
	return TRUE;
}

static const lt_ext_module_funcs_t __funcs = {
	_lt_ext_ldml_t_get_singleton,
	_lt_ext_ldml_t_create_data,
	_lt_ext_ldml_t_precheck_tag,
	_lt_ext_ldml_t_parse_tag,
	_lt_ext_ldml_t_get_tag,
	_lt_ext_ldml_t_validate_tag,
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
