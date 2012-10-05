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

#include <glib.h> /* GHashTable and mutex lock is still used here */
#include <sys/stat.h>
#include <libxml/parser.h>
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

G_LOCK_DEFINE_STATIC (lt_xml);

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

		if (!lt_string_append_filename(regfile,
					       BUILDDIR, "data", "language-subtag-reegistry.xml", NULL)) {
			lt_error_set(&err, LT_ERR_OOM, "Unable to allocate a memory");
			goto bail;
		}
		if (stat(lt_string_value(regfile), &st) == -1) {
			lt_string_clear(regfile);
#endif
	if (!lt_string_append_filename(regfile,
				       lt_db_get_datadir(),
				       "language-subtag-registry.xml", NULL)) {
		lt_error_set(&err, LT_ERR_OOM, "Unable to allocate a memory");
		goto bail;
	}
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

		if (!lt_string_append_filename(regfile,
					       SRCDIR, "data", "common", "bcp47", filename, NULL)) {
			lt_error_set(&err, LT_ERR_OOM, "Unable to allocate a memory");
			goto bail;
		}
		if (stat(lt_string_value(regfile), &st) == -1) {
			lt_string_clear(regfile);
#endif
	if (!lt_string_append_filename(regfile,
				       lt_db_get_datadir(),
				       "common", "bcp47", filename, NULL)) {
		lt_error_set(&err, LT_ERR_OOM, "Unable to allocate a memory");
		goto bail;
	}
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

		if (!lt_string_append_filename(regfile,
					       SRCDIR, "data", "common", "supplemental", filename, NULL)) {
			lt_error_set(&err, LT_ERR_OOM, "Unable to allocate a memory");
			goto bail;
		}
		if (stat(lt_string_value(regfile), &st) == -1) {
			lt_string_clear(regfile);
#endif
	if (!lt_string_append_filename(regfile,
				       lt_db_get_datadir(),
				       "common", "supplemental", filename, NULL)) {
		lt_error_set(&err, LT_ERR_OOM, "Unable to allocate a memory");
		goto bail;
	}
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

/*< public >*/
lt_xml_t *
lt_xml_new(void)
{
	lt_error_t *err = NULL;

	G_LOCK (lt_xml);

	if (__xml) {
		G_UNLOCK (lt_xml);

		return lt_xml_ref(__xml);
	}

	__xml = lt_mem_alloc_object(sizeof (lt_xml_t));
	if (__xml) {
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

	G_UNLOCK (lt_xml);

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
