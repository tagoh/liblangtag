/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-messages.h
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
 *   http://cgit.freedesktop.org/hieroglyph/tree/hieroglyph/hgmessages.h
 */
#ifndef __LT_MESSAGES_H__
#define __LT_MESSAGES_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdarg.h>
#include "lt-macros.h"

LT_BEGIN_DECLS

typedef enum _lt_message_type_t		lt_message_type_t;
typedef enum _lt_message_flags_t	lt_message_flags_t;
typedef enum _lt_message_category_t	lt_message_category_t;
typedef void (* lt_message_func_t)	(lt_message_type_t      type,
					 lt_message_flags_t     flags,
					 lt_message_category_t  category,
					 const char            *message,
					 lt_pointer_t           user_data);

enum _lt_message_type_t {
	LT_MSG_0 = 0,
	LT_MSG_FATAL,
	LT_MSG_CRITICAL,
	LT_MSG_WARNING,
	LT_MSG_INFO,
	LT_MSG_DEBUG,
	LT_MSG_END
};
enum _lt_message_flags_t {
	LT_MSG_FLAG_NONE	= 0,
	LT_MSG_FLAG_NO_LINEFEED	= (1 << 0),
	LT_MSG_FLAG_NO_PREFIX	= (1 << 1),
	LT_MSG_FLAG_END
};
enum _lt_message_category_t {
	LT_MSGCAT_0 = 0,
	LT_MSGCAT_DEBUG,	/* 1 */
	LT_MSGCAT_TRACE,	/* 2 */
	LT_MSGCAT_MODULE,	/* 4 */
	LT_MSGCAT_END
};


lt_message_func_t lt_message_set_default_handler(lt_message_func_t      func,
                                                 lt_pointer_t           user_data);
lt_message_func_t lt_message_set_handler        (lt_message_type_t      type,
                                                 lt_message_func_t      func,
                                                 lt_pointer_t           user_data);
lt_bool_t         lt_message_is_enabled         (lt_message_category_t  category);
void              lt_message_printf             (lt_message_type_t      type,
						 lt_message_flags_t     flags,
                                                 lt_message_category_t  category,
                                                 const char            *format,
						 ...) LT_GNUC_PRINTF(4, 5);
void              lt_message_vprintf            (lt_message_type_t      type,
						 lt_message_flags_t     flags,
                                                 lt_message_category_t  category,
                                                 const char            *format,
                                                 va_list                args);
void              lt_return_if_fail_warning     (const char            *pretty_function,
						 const char            *expression);


/* gcc-2.95.x supports both gnu style and ISO varargs, but if -ansi
 * is passed ISO vararg support is turned off, and there is no work
 * around to turn it on, so we unconditionally turn it off.
 */
#if __GNUC__ == 2 && __GNUC_MINOR__ == 95
#  undef LT_HAVE_ISO_VARARGS
#endif

#ifdef LT_HAVE_ISO_VARARGS
/* for(;;) ; so that GCC knows that control doesn't go past lt_fatal().
 * Put space before ending semicolon to avoid C++ build warnings.
 */
#define lt_fatal(...)					\
	LT_STMT_START {					\
		lt_message_printf(LT_MSG_FATAL,		\
				  LT_MSG_FLAG_NONE,	\
				  0,			\
				  __VA_ARGS__);		\
		for (;;) ;				\
	} LT_STMT_END
#define lt_critical(...)			\
	lt_message_printf(LT_MSG_CRITICAL,	\
			  LT_MSG_FLAG_NONE,	\
			  0,			\
			  __VA_ARGS__)
#define lt_warning(...)				\
	lt_message_printf(LT_MSG_WARNING,	\
			  LT_MSG_FLAG_NONE,	\
			  0,			\
			  __VA_ARGS__)
#define lt_info(...)				\
	lt_message_printf(LT_MSG_INFO,		\
			  LT_MSG_FLAG_NONE,	\
			  0,			\
			  __VA_ARGS__)
#define lt_debug(_c_,...)			\
	lt_message_printf(LT_MSG_DEBUG,		\
			  LT_MSG_FLAG_NONE,	\
			  (_c_),		\
			  __VA_ARGS__)
#define lt_debug0(_c_,_f_,...)			\
	lt_message_printf(LT_MSG_DEBUG,		\
			  (_f_),		\
			  (_c_),		\
			  __VA_ARGS__)

#elif defined(LT_HAVE_GNUC_VARARGS)

#define lt_fatal(format...)				\
	LT_STMT_START {					\
		lt_message_printf(LT_MSG_FATAL,		\
				  LT_MSG_FLAG_NONE,	\
				  0,			\
				  format);		\
		for (;;) ;				\
	} LT_STMT_END
#define lt_critical(format...)			\
	lt_message_printf(LT_MSG_CRITICAL,	\
			  LT_MSG_FLAG_NONE,	\
			  0,			\
			  format)
#define lt_warning(format...)			\
	lt_message_printf(LT_MSG_WARNING,	\
			  LT_MSG_FLAG_NONE,	\
			  0,			\
			  format)
#define lt_info(format...)			\
	lt_message_printf(LT_MSG_INFO,		\
			  LT_MSG_FLAG_NONE,	\
			  0,			\
			  format)
#define lt_debug(_c_,format...)			\
	lt_message_printf(LT_MSG_DEBUG,		\
			  LT_MSG_FLAG_NONE,	\
			  (_c_),		\
			  format)
#define lt_debug0(_c_,_f_,format...)		\
	lt_message_printf(LT_MSG_DEBUG,		\
			  (_f_),		\
			  (_c_),		\
			  format)
#else
static void
lt_fatal(const char *format,
	 ...)
{
	va_list args;

	va_start(args, format);
	lt_message_vprintf(LT_MSG_FATAL, LT_MSG_FLAG_NONE, 0, format, args);
	va_end(args);

	for (;;) ;
}
static void
lt_critical(const char *format,
	    ...)
{
	va_list args;

	va_start(args, format);
	lt_message_vprintf(LT_MSG_CRITICAL, LT_MSG_FLAG_NONE, 0, format, args);
	va_end(args);
}
static void
lt_warning(const char *format,
	   ...)
{
	va_list args;

	va_start(args, format);
	lt_message_vprintf(LT_MSG_WARNING, LT_MSG_FLAG_NONE, 0, format, args);
	va_end(args);
}
static void
lt_info(const char *format,
	...)
{
	va_list args;

	va_start(args, format);
	lt_message_vprintf(LT_MSG_INFO, LT_MSG_FLAG_NONE, 0, format, args);
	va_end(args);
}
static void
lt_debug(lt_message_category_t  category,
	 const char            *format,
	 ...)
{
	va_list args;

	va_start(args, format);
	lt_message_vprintf(LT_MSG_DEBUG, LT_MSG_FLAG_NONE, category, format, args);
	va_end(args);
}
static void
lt_debug0(lt_message_category_t  category,
	  lt_message_flags_t     flags,
	  const char            *format,
	  ...)
{
	va_list args;

	va_start(args, format);
	lt_message_vprintf(LT_MSG_DEBUG, flags, category, format, args);
	va_end(args);
}
#endif

#ifdef __GNUC__
#define _lt_return_after_eval_if_fail(__expr__,__eval__)		\
	LT_STMT_START {							\
		if (LT_LIKELY (__expr__)) {				\
		} else {						\
			lt_return_if_fail_warning(__PRETTY_FUNCTION__,	\
						  #__expr__);		\
			__eval__;					\
			return;						\
		}							\
	} LT_STMT_END
#define _lt_return_val_after_eval_if_fail(__expr__,__val__,__eval__)	\
	LT_STMT_START {							\
		if (LT_LIKELY (__expr__)) {				\
		} else {						\
			lt_return_if_fail_warning(__PRETTY_FUNCTION__,	\
						  #__expr__);		\
			__eval__;					\
			return (__val__);				\
		}							\
	} LT_STMT_END
#define lt_assert(__expr__)						\
	LT_STMT_START {							\
		if (LT_LIKELY (__expr__)) {				\
		} else {						\
			lt_fatal("%s: assertion `%s' failed", __PRETTY_FUNCTION__, \
				 #__expr__);				\
		}							\
	} LT_STMT_END
#else /* !__GNUC__ */
#define _lt_return_after_eval_if_fail(__expr__,__eval__)		\
	LT_STMT_START {							\
		if (__expr__) {						\
		} else {						\
			lt_critical("file %s: line %d: assertion `%s' failed", \
				    __FILE__,				\
				    __LINE__,				\
				    #__expr__);				\
			__eval__;					\
			return;						\
		}							\
	} LT_STMT_END
#define _lt_return_val_after_eval_if_fail(__expr__,__val__,__eval__)	\
	LT_STMT_START {							\
		if (__expr__) {						\
		} else {						\
			lt_critical("file %s: line %d: assertion `%s' failed", \
				    __FILE__,				\
				    __LINE__,				\
				    #__expr__);				\
			__eval__;					\
			return (__val__);				\
		}							\
	} LT_STMT_END
#define lt_assert(__expr__)						\
	LT_STMT_START {							\
		if (__expr__) {						\
		} else {						\
			lt_fatal("file %s: line %d: assertion `%s' failed", \
				 __FILE__,				\
				 __LINE__,				\
				 #__expr__);				\
		}							\
	} LT_STMT_END
#endif /* __GNUC__ */

#define lt_return_if_fail(__expr__)				\
	_lt_return_after_eval_if_fail(__expr__,{})
#define lt_return_val_if_fail(__expr__,__val__)			\
	_lt_return_val_after_eval_if_fail(__expr__,__val__,{})
#define lt_return_after_eval_if_fail(__expr__,__eval__)		\
	_lt_return_after_eval_if_fail(__expr__,__eval__)
#define lt_return_val_after_eval_if_fail(__expr__,__val__,__eval__)	\
	_lt_return_val_after_eval_if_fail(__expr__,__val__,__eval__)
#define lt_warn_if_reached()						\
	lt_message_printf(LT_MSG_WARNING, LT_MSG_FLAG_NONE, 0,		\
			  "(%s:%d): %s: code should not be reached",	\
			  __FILE__, __LINE__, __PRETTY_FUNCTION__)

LT_END_DECLS

#endif /* __LT_MESSAGES_H__ */
