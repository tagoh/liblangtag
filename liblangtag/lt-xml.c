/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-xml.c
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

#include <glib.h>
#include <libxml/parser.h>
#include "lt-error.h"
#include "lt-mem.h"
#include "lt-xml.h"


struct _lt_xml_t {
	lt_mem_t  parent;
	xmlDocPtr subtag_registry;
	xmlDocPtr cldr_bcp47_calendar;
	xmlDocPtr cldr_bcp47_collation;
	xmlDocPtr cldr_bcp47_currency;
	xmlDocPtr cldr_bcp47_number;
	xmlDocPtr cldr_bcp47_timezone;
	xmlDocPtr cldr_bcp47_variant;
};

static lt_xml_t *__xml = NULL;

G_LOCK_DEFINE_STATIC (lt_xml);

/*< private >*/
static gboolean
lt_xml_read_subtag_registry(lt_xml_t  *xml,
			    GError   **error)
{
	gchar *regfile = NULL;
	xmlParserCtxtPtr xmlparser;
	xmlDocPtr doc = NULL;
	GError *err = NULL;

	g_return_val_if_fail (xml != NULL, FALSE);

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
	xml->subtag_registry = doc;
	lt_mem_add_ref(&xml->parent, xml->subtag_registry,
		       (lt_destroy_func_t)xmlFreeDoc);

  bail:
	g_free(regfile);
	if (xmlparser)
		xmlFreeParserCtxt(xmlparser);

	xmlCleanupParser();

	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);

		return FALSE;
	}

	return TRUE;
}

static gboolean
lt_xml_read_cldr_bcp47(lt_xml_t     *xml,
		       const gchar  *filename,
		       xmlDocPtr    *doc,
		       GError      **error)
{
	gchar *regfile = NULL;
	xmlParserCtxtPtr xmlparser;
	GError *err = NULL;

	g_return_val_if_fail (xml != NULL, FALSE);

#ifdef GNOME_ENABLE_DEBUG
	regfile = g_build_filename(BUILDDIR, "data", "common", "bcp47", filename, NULL);
	if (!g_file_test(regfile, G_FILE_TEST_EXISTS)) {
		g_free(regfile);
#endif
	regfile = g_build_filename(REGDATADIR, "common", "bcp47", filename, NULL);
#ifdef GNOME_ENABLE_DEBUG
	}
#endif
	xmlparser = xmlNewParserCtxt();
	if (!xmlparser) {
		g_set_error(&err, LT_ERROR, LT_ERR_OOM,
			    "Unable to create an instance of xmlParserCtxt.");
		goto bail;
	}
	*doc = xmlCtxtReadFile(xmlparser, regfile, "UTF-8", 0);
	if (!*doc) {
		g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_XML,
			    "Unable to read the xml file: %s",
			    regfile);
		goto bail;
	}
	lt_mem_add_ref(&xml->parent, *doc,
		       (lt_destroy_func_t)xmlFreeDoc);

  bail:
	g_free(regfile);
	if (xmlparser)
		xmlFreeParserCtxt(xmlparser);

	xmlCleanupParser();

	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);

		return FALSE;
	}

	return TRUE;
}

/*< public >*/
lt_xml_t *
lt_xml_new(void)
{
	GError *err = NULL;

	G_LOCK (lt_xml);

	if (__xml) {
		G_UNLOCK (lt_xml);

		return lt_xml_ref(__xml);
	}

	__xml = lt_mem_alloc_object(sizeof (lt_xml_t));
	if (__xml) {
		lt_mem_add_weak_pointer(&__xml->parent, (gpointer *)&__xml);
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
		if (!lt_xml_read_cldr_bcp47(__xml, "variant.xml",
					    &__xml->cldr_bcp47_variant,
					    &err))
			goto bail;
	}

  bail:
	if (err) {
		g_warning(err->message);
		g_error_free(err);
		lt_xml_unref(__xml);
	}

	G_UNLOCK (lt_xml);

	return __xml;
}

lt_xml_t *
lt_xml_ref(lt_xml_t *xml)
{
	g_return_val_if_fail (xml != NULL, NULL);

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
	g_return_val_if_fail (xml != NULL, NULL);

	return xml->subtag_registry;
}

const xmlDocPtr
lt_xml_get_cldr_bcp47(lt_xml_t      *xml,
		      lt_xml_cldr_t  type)
{
	g_return_val_if_fail (xml != NULL, NULL);

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
	    case LT_XML_CLDR_BCP47_VARIANT:
		    return xml->cldr_bcp47_variant;
	    default:
		    break;
	}

	return NULL;
}
