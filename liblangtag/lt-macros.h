/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-macros.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#if !defined (__LANGTAG_H__INSIDE) && !defined (__LANGTAG_COMPILATION)
#error "Only <liblangtag/langtag.h> can be included directly."
#endif

#include <sys/param.h>

#ifndef __LT_MACROS_H__
#define __LT_MACROS_H__

/**
 * SECTION:lt-macros
 * @Short_Description: macros used in liblangtag
 * @Title: Miscellaneous Macros
 *
 * These macros provide more specialized features which are not needed so often
 * by application programmers.
 */

/* Guard C code in headers, while including them from C++ */
/**
 * LT_BEGIN_DECLS:
 *
 * Used (along with #LT_END_DECLS) to bracket header files. If the
 * compiler in use is a C++ compiler, adds <literal>extern "C"</literal>
 * around the header.
 */
/**
 * LT_END_DECLS:
 *
 * Used (along with #LT_BEGIN_DECLS) to bracket header files. If the
 * compiler in use is a C++ compiler, adds <literal>extern "C"</literal>
 * around the header.
 */
#ifdef __cplusplus
#  define LT_BEGIN_DECLS	extern "C" {
#  define LT_END_DECLS		}
#else
#  define LT_BEGIN_DECLS
#  define LT_END_DECLS
#endif

/* statement wrappers */
/**
 * LT_STMT_START:
 *
 * Used within multi-statement macros so that they can be used in places where
 * only on statement is expected by the compiler.
 */
/**
 * LT_STMT_END:
 *
 * Used within multi-statement macros so that they can be used in places where
 * only on statement is expected by the compiler.
 */
#if !(defined (LT_STMT_START) && defined (LT_STMT_END))
#  define LT_STMT_START		do
#  define LT_STMT_END		while (0)
#endif

/* inline wrapper */
/* inlining hassle. for compilers thta don't allow the 'inline' keyword,
 * mostly because of strict ANSI C compliance or dumbness, we try to fall
 * back to either '__inline__' or '__inline'.
 * LT_CAN_INLINE is defined in hgconfig.h if the compiler seems to be
 * actually *capable* to do function inlining, in which case inline
 * function bodies do make sense. we also define LT_INLINE_FUNC to properly
 * export the function prototypes if no inlining can be performed.
 */
#if defined (LT_HAVE_INLINE) && defined (__GNUC__) && defined (__STRICT_ANSI__)
#  undef inline
#  define inline	__inline__
#elif !defined (LT_HAVE_INLINE)
#  undef inline
#  if defined (LT_HAVE___INLINE__)
#    define inline __inline__
#  elif defined (LT_HAVE___INLINE)
#    define inline __inline
#  else /* !inline && !__inline__ && !__inline */
#    define inline /* don't inline, then */
#  endif
#endif
/**
 * LT_INLINE_FUNC:
 *
 * This macro is used to export function prototypes so they can be linked
 * with an external version when no inlining is performed.
 */
#if defined (__GNUC__)
#  define LT_INLINE_FUNC	static __inline __attribute__ ((unused))
#elif defined (LT_CAN_INLINE)
#  define LT_INLINE_FUNC	static inline
#else /* can't inline */
#  define LT_INLINE_FUNC
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#  define LT_GNUC_PRINTF(format_idx, arg_idx)	\
	__attribute__((__format__ (__printf__, format_idx, arg_idx)))
#else /* !__GNUC__ */
#  define LT_GNUC_PRINTF(format_idx, arg_idx)
#endif
#if __GNUC__ >= 4
#  define LT_GNUC_NULL_TERMINATED		\
	__attribute__((__sentinel__))
#else /* !__GNUC__ */
#  define LT_GNUC_NULL_TERMINATED
#endif

/*
 * The LT_LIKELY and LT_UNLIKELY macros let the programmer give hints to
 * the compiler about the expected result of an expression. Some compilers
 * can use this information for optimizations.
 *
 * The _LT_BOOLEAN_EXPR macro is intended to trigger a gcc warning when
 * putting assignments in lt_return_if_fail()
 */
#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#  define _LT_BOOLEAN_EXPR(_e_)				\
	__extension__ ({				\
			int __bool_var__;		\
			if (_e_)			\
				__bool_var__ = 1;	\
			else				\
				__bool_var__ = 0;	\
			__bool_var__;			\
		})
#  define LT_LIKELY(_e_)	(__builtin_expect (_LT_BOOLEAN_EXPR (_e_), 1))
#  define LT_UNLIKELY(_e_)	(__builtin_expect (_LT_BOOLEAN_EXPR (_e_), 0))
#else
#  define LT_LIKELY(_e_)	(_e_)
#  define LT_UNLIKELY(_e_)	(_e_)
#endif

/* boolean */
#ifndef FALSE
#  define FALSE	(0)
#endif
#ifndef TRUE
#  define TRUE	(!FALSE)
#endif

/* Macros for path separator */
#ifdef _WIN32
#  define LT_DIR_SEPARATOR_S		"\\"
#  define LT_DIR_SEPARATOR		'\\'
#  define LT_SEARCHPATH_SEPARATOR_S	";"
#  define LT_SEARCHPATH_SEPARATOR	';'
#else
#  define LT_DIR_SEPARATOR_S		"/"
#  define LT_DIR_SEPARATOR		'/'
#  define LT_SEARCHPATH_SEPARATOR_S	":"
#  define LT_SEARCHPATH_SEPARATOR	':'
#endif

/* Macros for min/max */
#ifdef MAX
#  define LT_MAX		MAX
#else
#  define LT_MAX(a,b)		(((a)>(b))?(a):(b))
#endif
#ifdef MIN
#  define LT_MIN		MIN
#else
#  define LT_MIN(a,b)		(((a)<(b))?(a):(b))
#endif

/* maybe 512 should be enough */
#define LT_PATH_MAX	512

/* Macros to adjust an alignment */
#define LT_ALIGNED_TO(_x_,_y_)		(((_x_) + (_y_) - 1) & ~((_y_) - 1))
#define LT_ALIGNED_TO_POINTER(_x_)	LT_ALIGNED_TO ((_x_), ALIGNOF_VOID_P)

/* Macro to count the number of elements in an array. */
#define LT_N_ELEMENTS(_x_)		(sizeof (_x_) / sizeof ((_x_)[0]))

/* Debugging macro */
#if (defined (__i386__) || defined (__x86_64__)) && defined (__GNUC__) && __GNUC__ >= 2
#  define LT_BREAKPOINT()						\
	LT_STMT_START {__asm__ __volatile__ ("int $03");} LT_STMT_END
#elif (defined (_MSC_VER) || defined (__DMC__)) && defined (_M_IX86)
#  define LT_BREAKPOINT()				\
	LT_STMT_START {__asm int 3h} LT_STMT_END
#elif defined (_MSC_VER)
#  define LT_BREAKPOINT()				\
	LT_STMT_START {__debugbreak();} LT_STMT_END
#elif defined (__alpha__) && !defined(__osf__) && defined (__GNUC__) && __GNUC__ >= 2
#  define LT_BREAKPOINT()						\
	LT_STMT_START {__asm__ __volatile__ ("bpt");} LT_STMT_END
#else
#  define LT_BREAKPOINT()				\
	LT_STMT_START {raise(SIGTRAP);} LT_STMT_END
#endif

LT_BEGIN_DECLS

typedef void *		lt_pointer_t;
typedef int		lt_bool_t;
typedef lt_pointer_t (* lt_copy_func_t)	(lt_pointer_t data);
typedef void (* lt_destroy_func_t)	(lt_pointer_t data);
typedef int (* lt_compare_func_t)	(const lt_pointer_t v1,
					 const lt_pointer_t v2);

LT_END_DECLS

#endif /* __LT_MACROS_H__ */
