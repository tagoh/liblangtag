/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-string.c
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

#include <stdlib.h>
#include <string.h>
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-utils.h"
#include "lt-string.h"

#define LT_STRING_SIZE	128

struct _lt_string_t {
	lt_mem_t  parent;
	char     *string;
	size_t    len;
	size_t    allocated_len;
};

lt_bool_t _lt_string_expand(lt_string_t *string,
			    size_t       size);

/*< private >*/
lt_bool_t
_lt_string_expand(lt_string_t *string,
		  size_t       size)
{
	string->allocated_len += LT_ALIGNED_TO_POINTER (size + LT_STRING_SIZE);
	lt_mem_delete_ref(&string->parent, string->string);
	string->string = realloc(string->string, string->allocated_len);
	if (!string->string) {
		string->len = 0;
		string->allocated_len = 0;

		return FALSE;
	}
	lt_mem_add_ref(&string->parent, string->string, free);

	return TRUE;
}

/*< protected >*/

/*< public >*/
lt_string_t *
lt_string_new(const char *string)
{
	lt_string_t *retval = lt_mem_alloc_object(sizeof (lt_string_t));

	if (retval) {
		retval->len = string ? strlen(string) : 0;
		retval->allocated_len = LT_ALIGNED_TO_POINTER (retval->len + LT_STRING_SIZE);
		retval->string = malloc(retval->allocated_len);
		if (!retval->string) {
			lt_mem_unref(&retval->parent);
			return NULL;
		}
		if (string)
			strcpy(retval->string, string);
		else
			retval->string[retval->len] = 0;
		lt_mem_add_ref(&retval->parent, retval->string, free);
	}

	return retval;
}

lt_string_t *
lt_string_ref(lt_string_t *string)
{
	lt_return_val_if_fail (string != NULL, NULL);

	return lt_mem_ref(&string->parent);
}

void
lt_string_unref(lt_string_t *string)
{
	if (string)
		lt_mem_unref(&string->parent);
}

char *
lt_string_free(lt_string_t *string,
	       lt_bool_t    free_segment)
{
	char *retval = NULL;

	if (!free_segment) {
		lt_mem_delete_ref(&string->parent, string->string);
		retval = string->string;
	}
	lt_string_unref(string);

	return retval;
}

size_t
lt_string_length(const lt_string_t *string)
{
	lt_return_val_if_fail (string != NULL, 0);

	return string->len;
}

const char *
lt_string_value(const lt_string_t *string)
{
	lt_return_val_if_fail (string != NULL, NULL);

	return string->string;
}

lt_string_t *
lt_string_truncate(lt_string_t *string,
		   ssize_t      len)
{
	lt_return_val_if_fail (string != NULL, NULL);

	if (len < 0)
		len = string->len + len;
	len = LT_MAX (len, 0);
	string->len = LT_MIN (len, string->len);
	string->string[string->len] = 0;

	return string;
}

void
lt_string_clear(lt_string_t *string)
{
	lt_string_truncate(string, 0);
}

lt_bool_t
lt_string_append_c(lt_string_t *string,
		   char         c)
{
	lt_return_val_if_fail (string != NULL, FALSE);

	if ((string->len + 2) >= string->allocated_len) {
		if (!_lt_string_expand(string, 1))
			return FALSE;
	}
	string->string[string->len++] = c;
	string->string[string->len] = 0;

	return TRUE;
}

lt_bool_t
lt_string_append(lt_string_t *string,
		 const char  *str)
{
	size_t len;

	lt_return_val_if_fail (string != NULL, FALSE);
	lt_return_val_if_fail (str != NULL, FALSE);

	len = strlen(str);
	if ((string->len + len + 1) >= string->allocated_len) {
		if (!_lt_string_expand(string, len))
			return FALSE;
	}
	strncpy(&string->string[string->len], str, len);
	string->len += len;
	string->string[string->len] = 0;

	return TRUE;
}

lt_bool_t
lt_string_append_filename(lt_string_t *string,
			  const char  *path,
			  ...)
{
	lt_bool_t retval = FALSE;
	va_list ap;
	const char *p;

	lt_return_val_if_fail (string != NULL, FALSE);
	lt_return_val_if_fail (path != NULL, FALSE);

	if (lt_string_length(string) == 0)
		retval |= !lt_string_append(string, LT_DIR_SEPARATOR_S);

	va_start(ap, path);
	p = path;
	while (p && !retval) {
		if (lt_string_at(string, -1) != LT_DIR_SEPARATOR)
			retval |= !lt_string_append(string, LT_DIR_SEPARATOR_S);
		retval |= !lt_string_append(string, p);
		p = (const char *)va_arg(ap, const char *);
	}

	return !retval;
}

lt_bool_t
lt_string_append_printf(lt_string_t *string,
			const char  *format,
			...)
{
	va_list ap;
	char *str;
	lt_bool_t retval;

	lt_return_val_if_fail (string != NULL, FALSE);
	lt_return_val_if_fail (format != NULL, FALSE);

	va_start(ap, format);
	str = lt_strdup_vprintf(format, ap);
	retval = lt_string_append(string, str);
	free(str);

	va_end(ap);

	return retval;
}

lt_string_t *
lt_string_replace_c(lt_string_t *string,
		    size_t       pos,
		    char         c)
{
	lt_return_val_if_fail (string != NULL, NULL);
	lt_return_val_if_fail (pos < string->len, NULL);

	string->string[pos] = c;

	return string;
}

char
lt_string_at(lt_string_t *string,
	     ssize_t      pos)
{
	lt_return_val_if_fail (string != NULL, 0);

	if (pos < 0)
		pos = string->len + pos;
	pos = LT_MAX (pos, 0);
	pos = LT_MIN (pos, string->len);

	return string->string[pos];
}
