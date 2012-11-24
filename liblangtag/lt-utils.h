/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-utils.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_UTILS_H__
#define __LT_UTILS_H__

#include <stdarg.h>
#include "lt-macros.h"

LT_BEGIN_DECLS

int   lt_strcmp0       (const char *v1,
                        const char *v2);
int   lt_strcasecmp    (const char *s1,
			const char *s2);
int   lt_strncasecmp   (const char *s1,
			const char *s2,
			size_t      len);
char *lt_strlower      (char       *string);
char *lt_strdup_printf (const char *format,
			...) LT_GNUC_PRINTF (1, 2);
char *lt_strdup_vprintf(const char *format,
                        va_list     args);
char *lt_strndup       (const char *s,
			size_t      n);

LT_END_DECLS

#endif /* __LT_UTILS_H__ */
