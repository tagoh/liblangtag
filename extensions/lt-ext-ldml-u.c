/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-ext-ldml-u.c
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

#include <stdint.h>
#include <string.h>
#include <libxml/xpath.h>
#include "lt-error.h"
#include "lt-ext-module.h"
#include "lt-list.h"
#include "lt-messages.h"
#include "lt-string.h"
#include "lt-utils.h"
#include "lt-xml.h"


typedef enum _lt_ext_ldml_u_state_t {
	STATE_NONE = 0,
	STATE_ATTRIBUTE,
	STATE_KEY,
	STATE_TYPE,
	STATE_END
} lt_ext_ldml_u_state_t;
typedef struct _lt_ext_ldml_u_data_t {
	lt_ext_module_data_t   parent;
	lt_xml_cldr_t          current_type;
	lt_ext_ldml_u_state_t  state;
	lt_list_t             *attributes;
	lt_list_t             *tags;
} lt_ext_ldml_u_data_t;

int                          LT_MODULE_SYMBOL (get_version) (void);
const lt_ext_module_funcs_t *LT_MODULE_SYMBOL (get_funcs) (void);

/*< private >*/
static int
_lt_ext_ldml_u_sort_attributes(const lt_pointer_t a,
			       const lt_pointer_t b)
{
	return lt_strcasecmp(a, b);
}

static int
_lt_ext_ldml_u_sort_tags(const lt_pointer_t a,
			 const lt_pointer_t b)
{
	const lt_string_t *s1 = a, *s2 = b;

	return lt_strcasecmp(lt_string_value(s1), lt_string_value(s2));
}

static lt_bool_t
_lt_ext_ldml_u_lookup_type(lt_ext_ldml_u_data_t  *data,
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

	lt_return_val_if_fail (data->current_type > 0, FALSE);

	l = lt_list_last(data->tags);
	if (l == NULL) {
		lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
			     "Invalid internal state. failed to find a key container.");
		goto bail;
	}
	s = lt_list_value(l);
	strncpy(key, lt_string_value(s), 2);
	key[2] = 0;

	xml = lt_xml_new();
	doc = lt_xml_get_cldr(xml, data->current_type);
	xctxt = xmlXPathNewContext(doc);
	if (!xctxt) {
		lt_error_set(error, LT_ERR_OOM,
			     "Unable to create an instance of xmlXPathContextPtr.");
		goto bail;
	}
	xpath_string = lt_strdup_printf("/ldmlBCP47/keyword/key[@name = '%s']", key);
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
				} else if (lt_strcmp0((const char *)name, "CODEPOINTS") == 0) {
					size_t len = strlen(subtag), j;
					static const char *hexdigit = "0123456789abcdefABCDEF";
					char *p;
					uint64_t x;

					/* an exception to deal with the unicode code point. */
					if (len >= 4 && len <= 6) {
						for (j = 0; j < len; j++) {
							if (!strchr(hexdigit, subtag[j]))
								goto bail2;
						}
						x = strtoull(subtag, &p, 16);
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
_lt_ext_ldml_u_lookup_key(lt_ext_ldml_u_data_t  *data,
			  const char            *subtag,
			  lt_error_t           **error)
{
	int i, j, n;
	lt_xml_t *xml = lt_xml_new();
	lt_bool_t retval = FALSE;

	for (i = LT_XML_CLDR_BCP47_BEGIN; i <= LT_XML_CLDR_BCP47_END; i++) {
		xmlDocPtr doc = lt_xml_get_cldr(xml, i);
		xmlXPathContextPtr xctxt = NULL;
		xmlXPathObjectPtr xobj = NULL;

		xctxt = xmlXPathNewContext(doc);
		if (!xctxt) {
			lt_error_set(error, LT_ERR_OOM,
				     "Unable to create an instance of xmlXPathContextPtr.");
			goto bail1;
		}
		xobj = xmlXPathEvalExpression((const xmlChar *)"/ldmlBCP47/keyword/key", xctxt);
		if (!xobj) {
			lt_error_set(error, LT_ERR_FAIL_ON_XML,
				     "No valid elements for %s",
				     doc->name);
			goto bail1;
		}
		n = xmlXPathNodeSetGetLength(xobj->nodesetval);

		for (j = 0; j < n; j++) {
			xmlNodePtr ent = xmlXPathNodeSetItem(xobj->nodesetval, j);
			xmlChar *name;

			if (!ent) {
				lt_error_set(error, LT_ERR_FAIL_ON_XML,
					     "Unable to obtain the xml node via XPath.");
				goto bail1;
			}
			name = xmlGetProp(ent, (const xmlChar *)"name");
			if (lt_strcasecmp((const char *)name, subtag) == 0) {
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
		if (lt_error_is_set(*error, LT_ERR_ANY) || retval)
			goto bail;
	}
	lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
		     "Invalid key for -u- extension: %s",
		     subtag);
  bail:
	lt_xml_unref(xml);

	return *error == NULL;
}

static void
_lt_ext_ldml_u_destroy_data(lt_pointer_t data)
{
	lt_ext_ldml_u_data_t *d = (lt_ext_ldml_u_data_t *)data;

	if (d->attributes)
		lt_list_free(d->attributes);
	if (d->tags)
		lt_list_free(d->tags);
}

static char
_lt_ext_ldml_u_get_singleton(void)
{
	return 'u';
}

static lt_ext_module_data_t *
_lt_ext_ldml_u_create_data(void)
{
	lt_ext_module_data_t *retval = lt_ext_module_data_new(sizeof (lt_ext_ldml_u_data_t),
							      _lt_ext_ldml_u_destroy_data);

	if (retval) {
		lt_ext_ldml_u_data_t *d = (lt_ext_ldml_u_data_t *)retval;

		d->state = STATE_NONE;
		d->current_type = LT_XML_CLDR_BEGIN;
	}

	return retval;
}

static lt_bool_t
_lt_ext_ldml_u_precheck_tag(lt_ext_module_data_t  *data,
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
_lt_ext_ldml_u_parse_tag(lt_ext_module_data_t  *data,
			 const char            *subtag,
			 lt_error_t           **error)
{
	lt_ext_ldml_u_data_t *d = (lt_ext_ldml_u_data_t *)data;
	lt_bool_t retval = TRUE;
	size_t len = strlen(subtag);

  restate:
	switch (d->state) {
	    case STATE_NONE:
		    if (len >= 3 && len <= 8) {
			    d->state = STATE_ATTRIBUTE;
		    } else if (len == 2) {
			    d->state = STATE_KEY;
		    } else {
			    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
					 "Invalid syntax: expected to see an attribute or a key, but `%s'", subtag);
			    break;
		    }
		    goto restate;
	    case STATE_ATTRIBUTE:
		    if (len >= 3 && len <= 8) {
			    d->attributes = lt_list_append(d->attributes,
							   strdup(subtag),
							   free);
			    /* next words may be still an attribute. keep the state */
		    } else {
			    /* it may be a key */
			    d->state = STATE_KEY;
			    goto restate;
		    }
		    break;
	    case STATE_KEY:
		    if (len != 2) {
			    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
					 "Invalid syntax: expected to see a key, but `%s'", subtag);
			    break;
		    }
		    _lt_ext_ldml_u_lookup_key(d, subtag, error);
		    d->tags = lt_list_append(d->tags,
					     lt_string_new(subtag),
					     (lt_destroy_func_t)lt_string_unref);
		    break;
	    case STATE_TYPE:
		    if (len >= 3 && len <= 8) {
			    lt_list_t *l;
			    lt_string_t *s;

			    if (!_lt_ext_ldml_u_lookup_type(d, subtag, error)) {
				    if (!lt_error_is_set(*error, LT_ERR_ANY))
					    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
							 "Unknown -u- extension type: %s", subtag);
				    break;
			    }
			    l = lt_list_last(d->tags);
			    if (l == NULL) {
				    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
						 "Invalid internal state. failed to find a key container.");
				    break;
			    }
			    s = lt_list_value(l);
			    lt_string_append_printf(s, "-%s", subtag);
		    } else if (len == 2) {
			    d->state = STATE_KEY;
			    goto restate;
		    } else {
			    lt_error_set(error, LT_ERR_FAIL_ON_SCANNER,
					 "Invalid syntax: expected to see a type or a key, but `%s'", subtag);
			    break;
		    }
		    break;
	    default:
		    lt_warn_if_reached();
		    break;
	}

	if (lt_error_is_set(*error, LT_ERR_ANY))
		retval = FALSE;

	return retval;
}

static char *
_lt_ext_ldml_u_get_tag(lt_ext_module_data_t *data)
{
	lt_ext_ldml_u_data_t *d = (lt_ext_ldml_u_data_t *)data;
	lt_string_t *s = lt_string_new(NULL);
	lt_list_t *l;

	if (d->attributes) {
		d->attributes = lt_list_sort(d->attributes, _lt_ext_ldml_u_sort_attributes);
		for (l = d->attributes; l != NULL; l = lt_list_next(l)) {
			const char *a = lt_list_value(l);

			if (lt_string_length(s) > 0)
				lt_string_append_c(s, '-');
			lt_string_append(s, a);
		}
	}
	if (d->tags) {
		d->tags = lt_list_sort(d->tags, _lt_ext_ldml_u_sort_tags);
		for (l = d->tags; l != NULL; l = lt_list_next(l)) {
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
_lt_ext_ldml_u_validate_tag(lt_ext_module_data_t *data)
{
	return TRUE;
}

static const lt_ext_module_funcs_t __funcs = {
	_lt_ext_ldml_u_get_singleton,
	_lt_ext_ldml_u_create_data,
	_lt_ext_ldml_u_precheck_tag,
	_lt_ext_ldml_u_parse_tag,
	_lt_ext_ldml_u_get_tag,
	_lt_ext_ldml_u_validate_tag,
};

/*< public >*/
int
LT_MODULE_SYMBOL (get_version) (void)
{
	return LT_EXT_MODULE_VERSION;
}

const lt_ext_module_funcs_t *
LT_MODULE_SYMBOL (get_funcs) (void)
{
	return &__funcs;
}
