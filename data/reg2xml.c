/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * reg2xml.c
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
#include <libxml/tree.h>
#include "lt-messages.h"
#include "lt-utils.h"

/*< private >*/
static char *
_drop_crlf(char *string)
{
	size_t len, i;

	lt_return_val_if_fail (string != NULL, NULL);
	lt_return_val_if_fail (string[0] != 0, NULL);

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

static lt_bool_t
_parse(const char *filename,
       xmlNodePtr  root)
{
	FILE *fp;
	char buffer[1024];
	lt_bool_t in_entry = FALSE;
	xmlNodePtr ent = NULL;

	if ((fp = fopen(filename, "rb")) == NULL) {
		lt_critical("Unable to open %s", filename);
		return FALSE;
	}
	while (1) {
		fgets(buffer, 1024, fp);
		if (feof(fp))
			break;
		_drop_crlf(buffer);
		if (lt_strcmp0(buffer, "%%") == 0) {
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
					lt_warning("Duplicate entry type: line = '%s', current type = '%s'",
						   buffer, ent->name);
				} else {
					ent = xmlNewNode(NULL, (const xmlChar *)&buffer[6]);
				}
			} else {
				if (!ent) {
					lt_warning("No entry type: line = '%s'",
						   buffer);
					in_entry = FALSE;
				} else {
					char *token, *tag;
					fpos_t pos;
					size_t len;

				  multiline:
					fgetpos(fp, &pos);
					len = strlen(buffer);
					fgets(&buffer[len], 1024 - len, fp);
					if (buffer[len] == ' ') {
						size_t l, i;

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
					token = strstr(buffer, ": ");
					tag = strndup(buffer, token - buffer);
					token += 2;
					xmlNewChild(ent, NULL,
						    (const xmlChar *)lt_strlower(tag),
						    (const xmlChar *)token);
					free(tag);
				}
			}
		}
	}

	return TRUE;
}

static lt_bool_t
_output_xml(const char *filename,
	    const char *out_filename)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	lt_bool_t retval;

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
