/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-ldml-t.c
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
#include "lt-ext-module.h"
#include "lt-list.h"
#include "lt-messages.h"
#include "lt-string.h"
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
	lt_list_t             *fields;
} lt_ext_ldml_t_data_t;

/*< private >*/
static lt_bool_t
_lt_ext_ldml_t_lookup_type(lt_ext_ldml_t_data_t  *data,
			   const char            *subtag,
			   lt_error_t           **error)
{
	lt_xml_t *xml = NULL;
	xmlDocPtr doc;
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	int i, n;
	char key[4], *xpath_string = NULL;
	lt_list_t *l;
	lt_string_t *s;
	lt_bool_t retval = FALSE;

	l = lt_list_last(data->fields);
	if (l == NULL) {
		lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
			     "Invalid internal state. failed to find a key container.");
		goto bail;
	}
	s = lt_list_value(l);
	strncpy(key, lt_string_value(s), 2);
	key[2] = 0;

	xml = lt_xml_new();
	doc = lt_xml_get_cldr(xml, LT_XML_CLDR_BCP47_TRANSFORM);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		lt_error_set(error, LT_ERR_OOM,
			     "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xpath_string = lt_strdup_printf("/ldmlBCP47/keyword/key[@extension = 't' and @name = '%s']", key);
	xobj = xmlXPathEvalExpression((const xmlChar *)xpath_string, xctxt);
	if (!xobj) {
		lt_error_set(error, LT_ERR_FAIL_ON_XML,
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
			lt_error_set(error, LT_ERR_FAIL_ON_XML,
				     "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		cnode = ent->children;
		while (cnode != NULL) {
			if (xmlStrcmp(cnode->name, (const xmlChar *)"type") == 0) {
				name = xmlGetProp(cnode, (const xmlChar *)"name");
				if (name && lt_strcasecmp((const char *)name, subtag) == 0) {
					retval = TRUE;
					xmlFree(name);
					goto bail;
				}
				xmlFree(name);
			} else if (xmlStrcmp(cnode->name, (const xmlChar *)"text") == 0) {
				/* ignore */
			} else {
				lt_warning("Unknown node under /ldmlBCP47/keyword/key: %s", cnode->name);
			}
			cnode = cnode->next;
		}
	}
  bail:
	free(xpath_string);
	if (xobj)
		xmlXPathFreeObject(xobj);
	if (xctxt)
		xmlXPathFreeContext(xctxt);
	if (xml)
		lt_xml_unref(xml);

	return retval;
}

static lt_bool_t
_lt_ext_ldml_t_lookup_key(lt_ext_ldml_t_data_t  *data,
			  const char            *subtag,
			  lt_error_t           **error)
{
	int i, n;
	lt_xml_t *xml = lt_xml_new();
	lt_bool_t retval = FALSE;
	xmlDocPtr doc = lt_xml_get_cldr(xml, LT_XML_CLDR_BCP47_TRANSFORM);
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	xmlChar *name = NULL;

	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		lt_error_set(error, LT_ERR_OOM,
			     "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/ldmlBCP47/keyword/key[@extension = 't']", xctxt);
	if (!xobj) {
		lt_error_set(error, LT_ERR_FAIL_ON_XML,
			     "No valid elements for %s",
			     doc->name);
		goto bail;
	}
	n = xmlXPathNodeSetGetLength(xobj->nodesetval);

	for (i = 0; i < n; i++) {
		xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, i);

		if (!ent) {
			lt_error_set(error, LT_ERR_FAIL_ON_XML,
				     "Unable to obtain the xml node via XPath.");
			goto bail;
		}
		if (name)
			xmlFree(name);
		name = xmlGetProp(ent, (const xmlChar *)"name");
		if (lt_strcasecmp((const char *)name, subtag) == 0) {
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
_lt_ext_ldml_t_destroy_data(lt_pointer_t data)
{
	lt_ext_ldml_t_data_t *d = (lt_ext_ldml_t_data_t *)data;

	lt_tag_unref(d->tag);
	lt_list_free(d->fields);
}

static char
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

static lt_bool_t
_lt_ext_ldml_t_precheck_tag(lt_ext_module_data_t  *data,
			    const lt_tag_t        *tag,
			    lt_error_t           **error)
{
	if (lt_tag_get_grandfathered(tag)) {
		lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
			     "Grandfathered tags aren't allowed to have LDML.");
		return FALSE;
	}
	if (lt_tag_get_extlang(tag)) {
		lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
			     "Extlang tags aren't allowed to have LDML.");
		return FALSE;
	}

	return TRUE;
}

static lt_bool_t
_lt_ext_ldml_t_parse_tag(lt_ext_module_data_t  *data,
			 const char            *subtag,
			 lt_error_t           **error)
{
	lt_ext_ldml_t_data_t *d = (lt_ext_ldml_t_data_t *)data;
	lt_bool_t retval = TRUE;
	size_t len = strlen(subtag);
	const lt_string_t *s;

	if (d->state != STATE_FIELD && d->state != STATE_FIELD2) {
		if (_lt_ext_ldml_t_lookup_key(d, subtag, error)) {
			goto setup_field_key;
		} else if (lt_error_is_set(*error, LT_ERR_ANY)) {
			goto bail;
		}
	}
	switch (d->state) {
	    case STATE_NONE:
		    if (!lt_tag_parse(d->tag, subtag, error))
			    break;
		    if (lt_tag_get_language(d->tag) == NULL) {
			    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
					 "No such language subtag: %s",
					 subtag);
			    break;
		    }
		    d->state = STATE_LANG;
		    break;
	    case STATE_LANG:
		    if (!lt_tag_parse_with_extra_token(d->tag, subtag, error))
			    break;
		    if (lt_tag_get_extlang(d->tag)) {
			    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
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
		    if (!lt_tag_parse_with_extra_token(d->tag, subtag, error))
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
		    if (!lt_tag_parse_with_extra_token(d->tag, subtag, error))
			    break;
	    check_variant:
		    if (lt_tag_get_extension(d->tag)) {
			    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
					 "Extension tag isn't allowed");
			    break;
		    } else if ((s = lt_tag_get_privateuse(d->tag)) && lt_string_length(s) > 0) {
			    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
					 "PrivateUse tag isn't allowed");
			    break;
		    } else if (lt_tag_get_variants(d->tag)) {
			    d->state = STATE_VARIANT;
		    } else {
			    lt_warn_if_reached();
		    }
		    break;
	    case STATE_FIELD2:
		    if (!lt_tag_get_language(d->tag)) {
			    if (_lt_ext_ldml_t_lookup_key(d, subtag, error)) {
			      setup_field_key:
				    d->state = STATE_FIELD;
				    d->fields = lt_list_append(d->fields,
							       lt_string_new(subtag),
							       (lt_destroy_func_t)lt_string_unref);
				    break;
			    } else if (lt_error_is_set(*error, LT_ERR_ANY)) {
				    break;
			    }
		    }
	    case STATE_FIELD:
		    if (len >= 3 && len <= 8) {
			    lt_list_t *l;
			    lt_string_t *s;

			    if (!_lt_ext_ldml_t_lookup_type(d, subtag, error)) {
				    if (!lt_error_is_set(*error, LT_ERR_ANY)) {
					    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
							 "Unknown -t- extension type: %s", subtag);
				    }
				    break;
			    }
			    l = lt_list_last(d->fields);
			    if (l == NULL) {
				    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
						 "Invalid internal state. failed to find a key container.");
				    break;
			    }
			    s = lt_list_value(l);
			    lt_string_append_printf(s, "-%s", subtag);
			    d->state = STATE_FIELD2;
		    } else {
			    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
					 "Invalid syntax: expected to see an field value, but `%s'", subtag);
		    }
		    break;
	    default:
		    lt_warn_if_reached();
		    break;
	}
  bail:
	if (lt_error_is_set(*error, LT_ERR_ANY))
		retval = FALSE;

	return retval;
}

static char *
_lt_ext_ldml_t_get_tag(lt_ext_module_data_t *data)
{
	lt_ext_ldml_t_data_t *d = (lt_ext_ldml_t_data_t *)data;
	lt_string_t *s = lt_string_new(lt_tag_get_string(d->tag));
	lt_list_t *l;

	if (d->fields) {
		for (l = d->fields; l != NULL; l = lt_list_next(l)) {
			const lt_string_t *t = lt_list_value(l);
			char *ts = strdup(lt_string_value(t));

			if (lt_string_length(s) > 0)
				lt_string_append_c(s, '-');
			if (lt_string_length(t) == 2) {
				/* XXX: do we need to auto-complete the clipped type here? */
			}
			lt_string_append(s, lt_strlower(ts));
			free(ts);
		}
	}

	return lt_string_free(s, FALSE);
}

static lt_bool_t
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
int
module_get_version(void)
{
	return LT_EXT_MODULE_VERSION;
}

const lt_ext_module_funcs_t *
module_get_funcs(void)
{
	return &__funcs;
}
