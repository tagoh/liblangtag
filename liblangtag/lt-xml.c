/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-xml.c
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

#include <pthread.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include "lt-error.h"
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-database.h"
#include "lt-string.h"
#include "lt-xml.h"


struct _lt_xml_t {
	lt_mem_t  parent;
	xmlDocPtr subtag_registry;
	xmlDocPtr cldr_bcp47_calendar;
	xmlDocPtr cldr_bcp47_collation;
	xmlDocPtr cldr_bcp47_currency;
	xmlDocPtr cldr_bcp47_number;
	xmlDocPtr cldr_bcp47_timezone;
	xmlDocPtr cldr_bcp47_transform;
	xmlDocPtr cldr_bcp47_variant;
	xmlDocPtr cldr_supplemental_likelysubtags;
};

static lt_xml_t *__xml = NULL;
static pthread_mutex_t __lt_xml_lock = PTHREAD_MUTEX_INITIALIZER;

/*< private >*/
static lt_bool_t
lt_xml_read_subtag_registry(lt_xml_t  *xml,
			    lt_error_t   **error)
{
	lt_string_t *regfile;
	xmlParserCtxtPtr xmlparser = NULL;
	xmlDocPtr doc = NULL;
	lt_error_t *err = NULL;

	lt_return_val_if_fail (xml != NULL, FALSE);

	regfile = lt_string_new(NULL);
#ifdef GNOME_ENABLE_DEBUG
	LT_STMT_START {
		struct stat st;

		lt_string_append_filename(regfile,
					  BUILDDIR,
					  "data", "language-subtag-reegistry.xml", NULL);
		if (stat(lt_string_value(regfile), &st) == -1) {
			lt_string_clear(regfile);
#endif
	lt_string_append_filename(regfile,
				  lt_db_get_datadir(),
				  "language-subtag-registry.xml", NULL);
#ifdef GNOME_ENABLE_DEBUG
		}
	} LT_STMT_END;
#endif
	xmlparser = xmlNewParserCtxt();
	if (!xmlparser) {
		lt_error_set(&err, LT_ERR_OOM,
			     "Unable to create an instance of xmlParserCtxt.");
		goto bail;
	}
	doc = xmlCtxtReadFile(xmlparser, lt_string_value(regfile), "UTF-8", 0);
	if (!doc) {
		lt_error_set(&err, LT_ERR_FAIL_ON_XML,
			     "Unable to read the xml file: %s",
			     lt_string_value(regfile));
		goto bail;
	}
	xml->subtag_registry = doc;
	lt_mem_add_ref(&xml->parent, xml->subtag_registry,
		       (lt_destroy_func_t)xmlFreeDoc);

  bail:
	lt_string_unref(regfile);
	if (xmlparser)
		xmlFreeParserCtxt(xmlparser);

	if (lt_error_is_set(err, LT_ERR_ANY)) {
		if (error)
			*error = lt_error_ref(err);
		else
			lt_error_print(err, LT_ERR_ANY);
		lt_error_unref(err);

		return FALSE;
	}

	return TRUE;
}

static lt_bool_t
lt_xml_read_cldr_bcp47(lt_xml_t     *xml,
		       const char   *filename,
		       xmlDocPtr    *doc,
		       lt_error_t  **error)
{
	lt_string_t *regfile;
	xmlParserCtxtPtr xmlparser = NULL;
	lt_error_t *err = NULL;

	lt_return_val_if_fail (xml != NULL, FALSE);

	regfile = lt_string_new(NULL);
#ifdef GNOME_ENABLE_DEBUG
	LT_STMT_START {
		struct stat st;

		lt_string_append_filename(regfile,
					  SRCDIR, "data", "common", "bcp47",
					  filename, NULL);
		if (stat(lt_string_value(regfile), &st) == -1) {
			lt_string_clear(regfile);
#endif
	lt_string_append_filename(regfile,
				  lt_db_get_datadir(),
				  "common", "bcp47", filename, NULL);
#ifdef GNOME_ENABLE_DEBUG
		}
	} LT_STMT_END;
#endif
	xmlparser = xmlNewParserCtxt();
	if (!xmlparser) {
		lt_error_set(&err, LT_ERR_OOM,
			     "Unable to create an instance of xmlParserCtxt.");
		goto bail;
	}
	*doc = xmlCtxtReadFile(xmlparser, lt_string_value(regfile), "UTF-8", 0);
	if (!*doc) {
		lt_error_set(&err, LT_ERR_FAIL_ON_XML,
			     "Unable to read the xml file: %s",
			     lt_string_value(regfile));
		goto bail;
	}
	lt_mem_add_ref(&xml->parent, *doc,
		       (lt_destroy_func_t)xmlFreeDoc);

  bail:
	lt_string_unref(regfile);
	if (xmlparser)
		xmlFreeParserCtxt(xmlparser);

	if (lt_error_is_set(err, LT_ERR_ANY)) {
		if (error)
			*error = lt_error_ref(err);
		else
			lt_error_print(err, LT_ERR_ANY);
		lt_error_unref(err);

		return FALSE;
	}

	return TRUE;
}

static lt_bool_t
lt_xml_read_cldr_supplemental(lt_xml_t     *xml,
			      const char   *filename,
			      xmlDocPtr    *doc,
			      lt_error_t  **error)
{
	lt_string_t *regfile = NULL;
	xmlParserCtxtPtr xmlparser = NULL;
	lt_error_t *err = NULL;

	lt_return_val_if_fail (xml != NULL, FALSE);

	regfile = lt_string_new(NULL);
#ifdef GNOME_ENABLE_DEBUG
	LT_STMT_START {
		struct stat st;

		lt_string_append_filename(regfile,
					  SRCDIR, "data", "common", "supplemental",
					  filename, NULL);
		if (stat(lt_string_value(regfile), &st) == -1) {
			lt_string_clear(regfile);
#endif
	lt_string_append_filename(regfile,
				  lt_db_get_datadir(),
				  "common", "supplemental", filename, NULL);
#ifdef GNOME_ENABLE_DEBUG
		}
	} LT_STMT_END;
#endif
	xmlparser = xmlNewParserCtxt();
	if (!xmlparser) {
		lt_error_set(&err, LT_ERR_OOM,
			     "Unable to create an instance of xmlParserCtxt.");
		goto bail;
	}
	*doc = xmlCtxtReadFile(xmlparser, lt_string_value(regfile), "UTF-8", 0);
	if (!*doc) {
		lt_error_set(&err, LT_ERR_FAIL_ON_XML,
			     "Unable to read the xml file: %s",
			     lt_string_value(regfile));
		goto bail;
	}
	lt_mem_add_ref(&xml->parent, *doc,
		       (lt_destroy_func_t)xmlFreeDoc);

  bail:
	lt_string_unref(regfile);
	if (xmlparser)
		xmlFreeParserCtxt(xmlparser);

	if (lt_error_is_set(err, LT_ERR_ANY)) {
		if (error)
			*error = lt_error_ref(err);
		else
			lt_error_print(err, LT_ERR_ANY);
		lt_error_unref(err);

		return FALSE;
	}

	return TRUE;
}

static lt_bool_t
_lt_xml_merge_keys(lt_xml_t    *xml,
		   xmlDocPtr    doc1,
		   xmlDocPtr    doc2,
		   lt_error_t **error)
{
	xmlXPathContextPtr xctxt = NULL;
	xmlXPathObjectPtr xobj = NULL;
	xmlNodePtr parent_node;
	int i, n;
	lt_bool_t retval = FALSE;

	xctxt = xmlXPathNewContext(doc1);
	if (!xctxt) {
		lt_error_set(error, LT_ERR_OOM,
			     "Unable to create an instance of xmlXPathContextPtr");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/ldmlBCP47/keyword", xctxt);
	if (!xobj) {
		lt_error_set(error, LT_ERR_FAIL_ON_XML,
			     "No valid elements for %s: keyword",
			     doc1->name);
		goto bail;
	}
	if ((n = xmlXPathNodeSetGetLength(xobj->nodesetval)) != 1) {
		lt_error_set(error, LT_ERR_FAIL_ON_XML,
			     "Too many keyword elements in %s: %s", doc1->name, doc2->name);
		goto bail;
	}
	parent_node = xmlXPathNodeSetItem(xobj->nodesetval, 0);
	xmlXPathFreeObject(xobj);
	xmlXPathFreeContext(xctxt);
	xobj = NULL;
	xctxt = NULL;

	xctxt = xmlXPathNewContext(doc2);
	if (!xctxt) {
		lt_error_set(error, LT_ERR_OOM,
			     "Unable to create an instance of xmlXPathContextPtr");
		goto bail;
	}
	xobj = xmlXPathEvalExpression((const xmlChar *)"/ldmlBCP47/keyword/key", xctxt);
	if (!xobj) {
		lt_error_set(error, LT_ERR_FAIL_ON_XML,
			     "No valid elements for %s: key",
			     doc2->name);
		goto bail;
	}
	n = xmlXPathNodeSetGetLength(xobj->nodesetval);
	for (i = 0; i < n; i++) {
		xmlNodePtr p = xmlCopyNode(xmlXPathNodeSetItem(xobj->nodesetval, i), 1);

		xmlAddChild(parent_node, p);
	}

	retval = TRUE;
  bail:
	if (xobj)
		xmlXPathFreeObject(xobj);
	if (xctxt)
		xmlXPathFreeContext(xctxt);
	lt_mem_remove_ref(&xml->parent, doc2);
	xmlFreeDoc(doc2);

	return retval;
}

/*< public >*/
lt_xml_t *
lt_xml_new(void)
{
	lt_error_t *err = NULL;

	pthread_mutex_lock(&__lt_xml_lock);

	if (__xml) {
		pthread_mutex_unlock(&__lt_xml_lock);

		return lt_xml_ref(__xml);
	}

	__xml = lt_mem_alloc_object(sizeof (lt_xml_t));
	if (__xml) {
		xmlDocPtr doc = NULL;

		lt_mem_add_weak_pointer(&__xml->parent, (lt_pointer_t *)&__xml);
		if (!lt_xml_read_subtag_registry(__xml, &err))
			goto bail;
		if (!lt_xml_read_cldr_bcp47(__xml, "calendar.xml",
					    &__xml->cldr_bcp47_calendar,
					    &err))
			goto bail;
		if (!lt_xml_read_cldr_bcp47(__xml, "collation.xml",
					    &__xml->cldr_bcp47_collation,
					    &err))
			goto bail;
		if (!lt_xml_read_cldr_bcp47(__xml, "currency.xml",
					    &__xml->cldr_bcp47_currency,
					    &err))
			goto bail;
		if (!lt_xml_read_cldr_bcp47(__xml, "number.xml",
					    &__xml->cldr_bcp47_number,
					    &err))
			goto bail;
		if (!lt_xml_read_cldr_bcp47(__xml, "timezone.xml",
					    &__xml->cldr_bcp47_timezone,
					    &err))
			goto bail;
		if (!lt_xml_read_cldr_bcp47(__xml, "transform.xml",
					    &__xml->cldr_bcp47_transform,
					    &err))
			goto bail;
		if (!lt_xml_read_cldr_bcp47(__xml, "transform_ime.xml",
					    &doc,
					    &err))
			goto bail;
		if (!_lt_xml_merge_keys(__xml, __xml->cldr_bcp47_transform, doc, &err))
			goto bail;
		if (!lt_xml_read_cldr_bcp47(__xml, "transform_keyboard.xml",
					    &doc,
					    &err))
			goto bail;
		if (!_lt_xml_merge_keys(__xml, __xml->cldr_bcp47_transform, doc, &err))
			goto bail;
		if (!lt_xml_read_cldr_bcp47(__xml, "transform_mt.xml",
					    &doc,
					    &err))
			goto bail;
		if (!_lt_xml_merge_keys(__xml, __xml->cldr_bcp47_transform, doc, &err))
			goto bail;
		if (!lt_xml_read_cldr_bcp47(__xml, "transform_private_use.xml",
					    &doc,
					    &err))
			goto bail;
		if (!_lt_xml_merge_keys(__xml, __xml->cldr_bcp47_transform, doc, &err))
			goto bail;
		if (!lt_xml_read_cldr_bcp47(__xml, "variant.xml",
					    &__xml->cldr_bcp47_variant,
					    &err))
			goto bail;
		if (!lt_xml_read_cldr_supplemental(__xml, "likelySubtags.xml",
						   &__xml->cldr_supplemental_likelysubtags,
						   &err))
			goto bail;
	}

  bail:
	if (lt_error_is_set(err, LT_ERR_ANY)) {
		lt_error_print(err, LT_ERR_ANY);
		lt_error_unref(err);
		lt_xml_unref(__xml);
	}

	pthread_mutex_unlock(&__lt_xml_lock);

	return __xml;
}

lt_xml_t *
lt_xml_ref(lt_xml_t *xml)
{
	lt_return_val_if_fail (xml != NULL, NULL);

	return lt_mem_ref(&xml->parent);
}

void
lt_xml_unref(lt_xml_t *xml)
{
	if (xml)
		lt_mem_unref(&xml->parent);
}

const xmlDocPtr
lt_xml_get_subtag_registry(lt_xml_t *xml)
{
	lt_return_val_if_fail (xml != NULL, NULL);

	return xml->subtag_registry;
}

const xmlDocPtr
lt_xml_get_cldr(lt_xml_t      *xml,
		lt_xml_cldr_t  type)
{
	lt_return_val_if_fail (xml != NULL, NULL);

	switch (type) {
	    case LT_XML_CLDR_BCP47_CALENDAR:
		    return xml->cldr_bcp47_calendar;
	    case LT_XML_CLDR_BCP47_COLLATION:
		    return xml->cldr_bcp47_collation;
	    case LT_XML_CLDR_BCP47_CURRENCY:
		    return xml->cldr_bcp47_currency;
	    case LT_XML_CLDR_BCP47_NUMBER:
		    return xml->cldr_bcp47_number;
	    case LT_XML_CLDR_BCP47_TIMEZONE:
		    return xml->cldr_bcp47_timezone;
	    case LT_XML_CLDR_BCP47_TRANSFORM:
		    return xml->cldr_bcp47_transform;
	    case LT_XML_CLDR_BCP47_VARIANT:
		    return xml->cldr_bcp47_variant;
	    case LT_XML_CLDR_SUPPLEMENTAL_LIKELY_SUBTAGS:
		    return xml->cldr_supplemental_likelysubtags;
	    default:
		    break;
	}

	return NULL;
}
