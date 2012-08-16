/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * reg2xml.c
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
#include <glib.h>
#include <libxml/tree.h>
#include "lt-utils.h"

/*< private >*/
static gchar *
_drop_crlf(gchar *string)
{
	gsize len, i;

	g_return_val_if_fail (string != NULL, NULL);
	g_return_val_if_fail (string[0] != 0, NULL);

	len = strlen(string);
	for (i = len - 1; i > 0; i--) {
		if (string[i] == '\r' ||
		    string[i] == '\n') {
			string[i] = 0;
		} else {
			break;
		}
	}

	return string;
}

static gboolean
_parse(const gchar *filename,
       xmlNodePtr   root)
{
	FILE *fp;
	gchar buffer[1024];
	gboolean in_entry = FALSE;
	xmlNodePtr ent = NULL;

	if ((fp = fopen(filename, "rb")) == NULL) {
		g_printerr("Unable to open %s\n", filename);
		return FALSE;
	}
	while (1) {
		fgets(buffer, 1024, fp);
		if (feof(fp))
			break;
		_drop_crlf(buffer);
		if (g_strcmp0(buffer, "%%") == 0) {
			if (in_entry) {
				if (ent) {
					xmlAddChild(root, ent);
				}
				ent = NULL;
			}
			in_entry = TRUE;
		} else {
			if (!in_entry) {
				/* ignore it */
				continue;
			}
			if (strncmp(buffer, "Type: ", 6) == 0) {
				if (ent) {
					g_warning("Duplicate entry type: line = '%s', current type = '%s'",
						  buffer, ent->name);
				} else {
					ent = xmlNewNode(NULL, (const xmlChar *)&buffer[6]);
				}
			} else {
				if (!ent) {
					g_warning("No entry type: line = '%s'",
						  buffer);
					in_entry = FALSE;
				} else {
					gchar **tokens, *tag;
					fpos_t pos;
					gsize len;

				  multiline:
					fgetpos(fp, &pos);
					len = strlen(buffer);
					fgets(&buffer[len], 1024 - len, fp);
					if (buffer[len] == ' ') {
						gsize l, i;

						_drop_crlf(&buffer[len]);
						l = strlen(&buffer[len]);
						for (i = 1; i < l; i++) {
							if (buffer[len + i] != ' ')
								break;
						}
						if (i > 1) {
							memmove(&buffer[len + 1], &buffer[len + i], l - i);
						}
						goto multiline;
					} else {
						buffer[len] = 0;
						fsetpos(fp, &pos);
					}
					tokens = g_strsplit(buffer, ": ", 2);
					tag = g_strdup(tokens[0]);
					xmlNewChild(ent, NULL,
						    (const xmlChar *)lt_strlower(tag),
						    (const xmlChar *)tokens[1]);
					g_strfreev(tokens);
					g_free(tag);
				}
			}
		}
	}

	return TRUE;
}

static gboolean
_output_xml(const gchar *filename,
	    const gchar *out_filename)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	gboolean retval;

	doc = xmlNewDoc((const xmlChar *)"1.0");
	doc->encoding = xmlStrdup((const xmlChar *)"UTF-8");
	root = xmlNewDocNode(doc, NULL,
			     (const xmlChar *)"registry",
			     NULL);
	xmlDocSetRootElement(doc, root);

	if (!(retval = _parse(filename, root)))
		goto bail;

	xmlSaveFormatFileEnc(out_filename && out_filename[0] != '-' ? out_filename : "-",
			     doc, "UTF-8", 1);
  bail:
	xmlFreeDoc(doc);

	return retval;
}

/*< public >*/
int
main(int    argc,
     char **argv)
{
	if (argc < 3)
		return 1;

	if (!_output_xml(argv[1], argv[2]))
		return 1;

	return 0;
}
