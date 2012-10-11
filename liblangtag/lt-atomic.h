/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-atomic.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_ATOMIC_H__
#define __LT_ATOMIC_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include "lt-messages.h"

LT_BEGIN_DECLS

LT_INLINE_FUNC int       lt_atomic_int_get         (volatile int *v);
LT_INLINE_FUNC int       lt_atomic_int_inc         (volatile int *v);
LT_INLINE_FUNC lt_bool_t lt_atomic_int_dec_and_test(volatile int *v);

#ifndef LT_HAVE_ATOMIC_BUILTINS
static pthread_mutex_t __lt_atomic_lock = PTHREAD_MUTEX_INITIALIZER;
#endif

/*< private >*/

/*< public >*/
#ifdef LT_HAVE_ATOMIC_BUILTINS
LT_INLINE_FUNC int
lt_atomic_int_get(volatile int *v)
{
	lt_return_val_if_fail (v != NULL, 0);

	__sync_synchronize();
	return *v;
}

LT_INLINE_FUNC int
lt_atomic_int_inc(volatile int *v)
{
	lt_return_val_if_fail (v != NULL, 0);

	return __sync_fetch_and_add(v, 1);
}

lt_bool_t
lt_atomic_int_dec_and_test(volatile int *v)
{
	lt_return_val_if_fail (v != NULL, FALSE);

	return __sync_fetch_and_sub(v, 1) == 1;
}

#else /* !LT_HAVE_ATOMIC_BUILTINS */
LT_INLINE_FUNC int
lt_atomic_int_get(volatile int *v)
{
	volatile int retval;

	lt_return_val_if_fail (v != NULL, 0);

	pthread_mutex_lock(&__lt_atomic_lock);
	retval = *v;
	pthread_mutex_unlock(&__lt_atomic_lock);

	return retval;
}

LT_INLINE_FUNC int
lt_atomic_int_inc(volatile int *v)
{
	int retval;

	lt_return_val_if_fail (v != NULL, 0);

	pthread_mutex_lock(&__lt_atomic_lock);
	retval = (*v)++;
	pthread_mutex_unlock(&__lt_atomic_lock);

	return retval;
}

LT_INLINE_FUNC lt_bool_t
lt_atomic_int_dec_and_test(volatile int *v)
{
	lt_bool_t retval;

	lt_return_if_fail (v != NULL, FALSE);

	pthread_mutex_lock(&__lt_atomic_lock);
	retval = --(*v) == 0;
	pthread_mutex_unlock(&__lt_atomic_lock);

	return retval;
}
#endif /* LT_HAVE_ATOMIC_BUILTINS */

LT_END_DECLS

#endif /* __LT_ATOMIC_H__ */
