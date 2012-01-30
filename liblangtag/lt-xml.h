/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-xml.h
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
#ifndef __LT_XML_H__
#define __LT_XML_H__

#include <glib.h>
#include <libxml/tree.h>

G_BEGIN_DECLS

typedef struct _lt_xml_t	lt_xml_t;

lt_xml_t        *lt_xml_new                (void);
lt_xml_t        *lt_xml_ref                (lt_xml_t *xml);
void             lt_xml_unref              (lt_xml_t *xml);
const xmlDocPtr  lt_xml_get_subtag_registry(lt_xml_t *xml);

G_END_DECLS

#endif /* __LT_XML_H__ */
