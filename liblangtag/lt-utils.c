/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-utils.c
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

#if HAVE_VASPRINTF
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#if HAVE_MEMORY_H
#include <memory.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#endif
#include "lt-messages.h"
#include "lt-utils.h"


/*< private >*/

/*< public >*/
int
lt_strcmp0(const char *v1,
	   const char *v2)
{
	if (!v1)
		return -(v1 != v2);
	if (!v2)
		return v1 != v2;
	return strcmp(v1, v2);
}

int
lt_strcasecmp(const char *s1,
	      const char *s2)
{
	char c1, c2;

	lt_return_val_if_fail (s1 != NULL, 0);
	lt_return_val_if_fail (s2 != NULL, 0);

	while (*s1 && *s2) {
		c1 = tolower((int)*s1);
		c2 = tolower((int)*s2);
		if (c1 != c2)
			return (c1 - c2);
		s1++;
		s2++;
	}

	return (*s1 - *s2);
}

int
lt_strncasecmp(const char *s1,
	       const char *s2,
	       size_t      len)
{
	char c1, c2;

	lt_return_val_if_fail (s1 != NULL, 0);
	lt_return_val_if_fail (s2 != NULL, 0);

	while (len && *s1 && *s2) {
		len--;
		c1 = tolower((int)*s1);
		c2 = tolower((int)*s2);
		if (c1 != c2)
			return (c1 - c2);
		s1++;
		s2++;
	}

	if (len)
		return (*s1 - *s2);
	else
		return 0;
}

char *
lt_strlower(char *string)
{
	size_t len;
	char *p = string;

	lt_return_val_if_fail (string != NULL, NULL);

	len = strlen(string);
	while (len) {
		*p = tolower((int)*p);
		p++;
		len--;
	}

	return string;
}

char *
lt_strdup_printf(const char *format,
		 ...)
{
	va_list ap;
	char *retval;

	va_start(ap, format);

	retval = lt_strdup_vprintf(format, ap);

	va_end(ap);

	return retval;
}

char *
lt_strdup_vprintf(const char *format,
		  va_list     args)
{
	char *retval;

	lt_return_val_if_fail (format != NULL, NULL);

#if HAVE_VASPRINTF
	if (vasprintf(&retval, format, args) < 0) {
		retval = NULL;
	}
#elif LT_HAVE_C99_VSNPRINTF
	LT_STMT_START {
		char c;
		va_list ap;
		int size;

		va_copy(ap, args);

		size = vsnprintf(&c, 1, format, ap) + 1;

		va_end(ap);

		retval = malloc(sizeof (char) * size);
		if (retval) {
			vsprintf(retval, format, args);
		}
	} LT_STMT_END;
#elif HAVE_VSNPRINTF
	LT_STMT_START {
		va_list ap;
		int size = 1024, n;
		char *p;

		retval = malloc(size);
		if (!retval)
			return NULL;

		while (1) {
			va_copy(ap, args);

			n = vsnprintf(retval, size, format, ap);

			va_end(ap);

			if (n > -1 && n < size)
				return retval;

			if (n > -1)
				size = n + 1;
			else
				size *= 2;

			p = realloc(retval, size);
			if (!p) {
				free(retval);
				retval = NULL;
				break;
			}
			retval = p;
		}
	} LT_STMT_END;
#else
#error no vsnprintf function implemented.
#endif

	return retval;
}

char *
lt_strndup(const char *s,
	   size_t      n)
{
#if HAVE_STRNDUP
	return strndup(s, n);
#else
	const char *p = s;
	char *retval;
	size_t i = 0;

	while (p && *p && i < n) {
		p++;
		i++;
	}
	retval = malloc(i + 1);
	if (!retval)
		return NULL;
	retval[i] = 0;

	return memcpy(retval, s, i);
#endif
}
