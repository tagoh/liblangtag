/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-messages.c
 * Copyright (C) 2006-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 *
 * Borrowed from hieroglyph:
 *   http://cgit.freedesktop.org/hieroglyph/tree/hieroglyph/hgmessages.c
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lt-messages.h"

static void _lt_message_default_handler(lt_message_type_t      type,
					lt_message_flags_t     flags,
					lt_message_category_t  category,
					const char            *message,
					lt_pointer_t           user_data);


static lt_message_func_t __lt_message_default_handler = _lt_message_default_handler;
static lt_pointer_t __lt_message_default_handler_data = NULL;
static lt_message_func_t __lt_message_handler[LT_MSG_END];
static lt_pointer_t __lt_message_handler_data[LT_MSG_END];

/*< private >*/
static char *
_lt_message_get_prefix(lt_message_type_t     type,
		       lt_message_category_t category)
{
	static const char *type_string[LT_MSG_END + 1] = {
		NULL,
		"*** ",
		"E: ",
		"W: ",
		"I: ",
		"D: ",
		NULL
	};
	static const char *category_string[LT_MSGCAT_END + 1] = {
		NULL,
		" DEBUG",
		" TRACE",
		"MODULE",
		NULL
	};
	static const char unknown_type[] = "?: ";
	static const char unknown_cat[] = "???";
	static const char no_cat[] = "";
	const char *ts, *cs;
	char *retval = NULL, *catstring = NULL;
	size_t tlen = 0, clen = 0, len;

	type = LT_MIN (type, LT_MSG_END);
	category = LT_MIN (category, LT_MSGCAT_END);
	if (type_string[type]) {
		ts = type_string[type];
	} else {
		ts = unknown_type;
	}
	tlen = strlen(ts);
	if (category_string[category]) {
		cs = category_string[category];
	} else if (category == 0) {
		cs = no_cat;
	} else {
		cs = unknown_cat;
	}
	clen = strlen(cs);
	if (clen > 0) {
		catstring = malloc(sizeof (char) * (clen + 6));
		snprintf(catstring, clen + 6, "[%s]: ", cs);
		clen = strlen(catstring);
	}
	len = tlen + clen + 1;
	retval = malloc(sizeof (char) * len);
	if (retval) {
		snprintf(retval, len, "%s%s ", ts, catstring ? catstring : "");
	}
	if (catstring)
		free(catstring);

	return retval;
}

static void
_lt_message_stacktrace(void)
{
	void *traces[1024];
	char **strings;
	int size, i;

	size = backtrace(traces, 1024);
	if (size > 0) {
		strings = backtrace_symbols(traces, size);
		lt_debug(LT_MSGCAT_TRACE, "Stacktrace:");
		/*
		 * XXX:
		 * 0.. here.
		 * 1.. _lt_message_default_handler
		 * 2.. lt_message_vprintf
		 * 3.. lt_message_printf
		 * 4.. lt_* macros
		 */
		for (i = 4; i < size; i++) {
			lt_debug(LT_MSGCAT_TRACE, "  %d. %s", i - 3, strings[i]);
		}
		free(strings);
	}
}

static void
_lt_message_default_handler(lt_message_type_t      type,
			    lt_message_flags_t     flags,
			    lt_message_category_t  category,
			    const char            *message,
			    lt_pointer_t           user_data)
{
	char *prefix = NULL;

	if (flags == 0 || (flags & LT_MSG_FLAG_NO_PREFIX) == 0)
		prefix = _lt_message_get_prefix(type, category);
	fprintf(stderr, "%s%s%s", prefix ? prefix : "", message, flags == 0 || (flags & LT_MSG_FLAG_NO_LINEFEED) == 0 ? "\n" : "");
	if (lt_message_is_enabled(LT_MSGCAT_TRACE) && category != LT_MSGCAT_TRACE)
		_lt_message_stacktrace();
	if (lt_message_is_enabled(LT_MSGCAT_DEBUG) &&
	    type != LT_MSG_DEBUG)
		LT_BREAKPOINT();

	if (prefix)
		free(prefix);
}

/*< public >*/

/**
 * lt_message_set_default_handler:
 * @func:
 * @user_data:
 *
 * FIXME
 *
 * Returns:
 */
lt_message_func_t
lt_message_set_default_handler(lt_message_func_t func,
			       lt_pointer_t      user_data)
{
	lt_message_func_t retval = __lt_message_default_handler;

	__lt_message_default_handler = func;
	__lt_message_default_handler_data = user_data;

	return retval;
}

/**
 * lt_message_set_handler:
 * @type:
 * @func:
 * @user_data:
 *
 * FIXME
 *
 * Returns:
 */
lt_message_func_t
lt_message_set_handler(lt_message_type_t type,
		       lt_message_func_t func,
		       lt_pointer_t      user_data)
{
	lt_message_func_t retval;

	if (type >= LT_MSG_END) {
		fprintf(stderr, "[BUG] invalid message type: %d\n", type);
		return NULL;
	}

	retval = __lt_message_handler[type];
	__lt_message_handler[type] = func;
	__lt_message_handler_data[type] = user_data;

	return retval;
}

/**
 * lt_message_is_masked:
 * @category:
 *
 * FIXME
 *
 * Returns:
 */
lt_bool_t
lt_message_is_enabled(lt_message_category_t category)
{
	static lt_bool_t cache = FALSE;
	static int mask = 0;
	const char *env;

	if (!cache) {
		env = getenv("LT_DEBUG");
		if (env)
			mask = atoi(env);
		cache = TRUE;
	}

	return ((1 << (category - 1)) & mask) != 0;
}

/**
 * lt_message_printf:
 * @type:
 * @category:
 * @format:
 *
 * FIXME
 */
void
lt_message_printf(lt_message_type_t      type,
		  lt_message_flags_t     flags,
		  lt_message_category_t  category,
		  const char            *format,
		  ...)
{
	va_list args;

	va_start(args, format);

	lt_message_vprintf(type, flags, category, format, args);

	va_end(args);
}

/**
 * lt_message_vprintf:
 * @type:
 * @category:
 * @format:
 * @args:
 *
 * FIXME
 */
void
lt_message_vprintf(lt_message_type_t      type,
		   lt_message_flags_t     flags,
		   lt_message_category_t  category,
		   const char            *format,
		   va_list                args)
{
	char buffer[4096];

	if (type >= LT_MSG_END) {
		fprintf(stderr, "[BUG] Invalid message type: %d\n", type);
		return;
	}
	if (category >= LT_MSGCAT_END) {
		fprintf(stderr, "[BUG] Invalid category type: %d\n", category);
		return;
	}
	if (type == LT_MSG_DEBUG) {
		if (!lt_message_is_enabled(category))
			return;
	}

	vsnprintf(buffer, 4096, format, args);
	if (__lt_message_handler[type]) {
		__lt_message_handler[type](type, flags, category, buffer, __lt_message_handler_data[type]);
	} else if (__lt_message_default_handler) {
		__lt_message_default_handler(type, flags, category, buffer, __lt_message_default_handler_data);
	}
	if (type == LT_MSG_FATAL)
		abort();
}

/**
 * lt_return_if_fail_warning:
 * @pretty_function:
 * @expression:
 *
 * FIXME
 */
void
lt_return_if_fail_warning(const char *pretty_function,
			  const char *expression)
{
	lt_message_printf(LT_MSG_CRITICAL,
			  LT_MSG_FLAG_NONE,
			  0,
			  "%s: assertion `%s' failed",
			  pretty_function,
			  expression);
}
