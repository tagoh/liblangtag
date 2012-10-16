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

/**
 * SECTION: lt-string
 * @Short_Description: text buffers which grow automatically as text is added
 * @Title: Strings
 *
 * A #lt_string_t is an object that handles the memory management of a C
 * string.
 */
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
	lt_mem_remove_ref(&string->parent, string->string);
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

/**
 * lt_string_new:
 * @string: an initial string to set
 *
 * Creates an instance of #lt_string_t with @string.
 *
 * Returns: a new instance of #lt_string_t.
 */
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

/**
 * lt_string_ref:
 * @string: a #lt_string_t
 *
 * Increases the reference count of @string.
 *
 * Returns: (transfer none): the same @string object.
 */
lt_string_t *
lt_string_ref(lt_string_t *string)
{
	lt_return_val_if_fail (string != NULL, NULL);

	return lt_mem_ref(&string->parent);
}

/**
 * lt_string_unref:
 * @string: a #lt_string_t
 *
 * Decreases the reference count of @string. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_string_unref(lt_string_t *string)
{
	if (string)
		lt_mem_unref(&string->parent);
}

/**
 * lt_string_free:
 * @string: a #lt_string_t
 * @free_segment: if %TRUE, the actual character data is freed as well
 *
 * Frees the memory allocated for the #lt_string_t.
 * If @free_segment is %TRUE it also frees the character data.  If
 * it's %FALSE, the caller gains ownership of the buffer and must
 * free it after use with free().
 *
 * Returns: the character data of @string
 *          (i.e. %NULL if @free_segment is %TRUE)
 */
char *
lt_string_free(lt_string_t *string,
	       lt_bool_t    free_segment)
{
	char *retval = NULL;

	if (!free_segment) {
		lt_mem_remove_ref(&string->parent, string->string);
		retval = string->string;
	}
	lt_string_unref(string);

	return retval;
}

/**
 * lt_string_length:
 * @string: a #lt_string_t
 *
 * Returns the number of characters in buffer for @string.
 *
 * Returns: the number of characters
 */
size_t
lt_string_length(const lt_string_t *string)
{
	lt_return_val_if_fail (string != NULL, 0);

	return string->len;
}

/**
 * lt_string_value:
 * @string: a #lt_string_t
 *
 * Returns the buffer in @string.
 *
 * Returns: a string which @string has.
 */
const char *
lt_string_value(const lt_string_t *string)
{
	lt_return_val_if_fail (string != NULL, NULL);

	return string->string;
}

/**
 * lt_string_truncate:
 * @string: a #lt_string_t
 * @len: the number of characters to be truncated from the buffer.
 *
 * Truncates the characters in the buffer according to @len. if @len is
 * a negative, how many characters is truncated will be calculated from
 * current size. i.e. if the buffer contains "abc", and @len is -1,
 * the buffer will be "ab" after this call.
 *
 * Returns: (transfer none): the same @string object.
 */
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

/**
 * lt_string_clear:
 * @string: a #lt_string_t
 *
 * Clean up the buffer in @string.
 */
void
lt_string_clear(lt_string_t *string)
{
	lt_string_truncate(string, 0);
}

/**
 * lt_string_append_c:
 * @string: a #lt_string_t
 * @c: the byte to append onto the end of @string
 *
 * Adds a byte onto the end of a #lt_string_t, expanding
 * it if necessary.
 *
 * Returns: (transfer none): the same @string object.
 */
lt_string_t *
lt_string_append_c(lt_string_t *string,
		   char         c)
{
	lt_return_val_if_fail (string != NULL, NULL);

	if ((string->len + 2) >= string->allocated_len) {
		if (!_lt_string_expand(string, 1))
			return string;
	}
	string->string[string->len++] = c;
	string->string[string->len] = 0;

	return string;
}

/**
 * lt_string_append:
 * @string: a #lt_string_t
 * @str: the string to append onto the end of @string
 *
 * Adds a string onto the end of a #lt_string_t, expanding
 * it if necessary.
 *
 * Returns: (transfer none): the same @string object
 */
lt_string_t *
lt_string_append(lt_string_t *string,
		 const char  *str)
{
	size_t len;

	lt_return_val_if_fail (string != NULL, NULL);
	lt_return_val_if_fail (str != NULL, string);

	len = strlen(str);
	if ((string->len + len + 1) >= string->allocated_len) {
		if (!_lt_string_expand(string, len))
			return string;
	}
	strncpy(&string->string[string->len], str, len);
	string->len += len;
	string->string[string->len] = 0;

	return string;
}

/**
 * lt_string_append_filename:
 * @string: a #lt_string_t
 * @path: the string to append onto the end of @string as a file path
 * @...: a %NULL-terminated list of strings to append onto the end of @string as a file path
 *
 * Adds a string onto the end of a #lt_string_t as a file path.
 *
 * Returns: (transfer none): the same @string object
 */
lt_string_t *
lt_string_append_filename(lt_string_t *string,
			  const char  *path,
			  ...)
{
	va_list ap;
	const char *p;

	lt_return_val_if_fail (string != NULL, NULL);
	lt_return_val_if_fail (path != NULL, string);

	if (lt_string_length(string) == 0 && path[0] != LT_DIR_SEPARATOR)
		lt_string_append(string, LT_DIR_SEPARATOR_S);

	va_start(ap, path);
	p = path;
	while (p) {
		if (lt_string_length(string) > 0 && lt_string_at(string, -1) != LT_DIR_SEPARATOR)
			lt_string_append(string, LT_DIR_SEPARATOR_S);
		lt_string_append(string, p);
		p = (const char *)va_arg(ap, const char *);
	}

	return string;
}

/**
 * lt_string_append_printf:
 * @string: a #lt_string_t
 * @format: the string format. See the printf() documentation
 * @...: the parameters to insert into the format string
 *
 * Appends a formatted string onto the end of a #lt_string_t.
 * This is similar to the standard sprintf() function,
 * except that the text is appended to the #lt_string_t.
 */
void
lt_string_append_printf(lt_string_t *string,
			const char  *format,
			...)
{
	va_list ap;
	char *str;

	lt_return_if_fail (string != NULL);
	lt_return_if_fail (format != NULL);

	va_start(ap, format);
	str = lt_strdup_vprintf(format, ap);
	lt_string_append(string, str);
	free(str);

	va_end(ap);
}

/**
 * lt_string_replace_c:
 * @string: a #lt_string_t
 * @pos: position in @string where replacement should happen
 * @c: the byte to replace
 *
 * Replaces a character in @string at @pos.
 *
 * Returns: (transfer none): the same @string object
 */
lt_string_t *
lt_string_replace_c(lt_string_t *string,
		    size_t       pos,
		    char         c)
{
	lt_return_val_if_fail (string != NULL, NULL);
	lt_return_val_if_fail (pos < string->len, string);
	lt_return_val_if_fail (pos > 0, string);

	string->string[pos] = c;

	return string;
}

/**
 * lt_string_at:
 * @string: a #lt_string_t
 * @pos: position in @string where to obtain the byte
 *
 * Obtain a byte in a #lt_string_t at @pos. If @pos is a negative,
 * the position is calculated from current size. i.e. if the buffer
 * contains "abc", and @pos is -1, this will returns 'c' then.
 *
 * Returns: the byte in @string at @pos
 */
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
