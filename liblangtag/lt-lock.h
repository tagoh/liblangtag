/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-lock.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_LOCK_H__
#define __LT_LOCK_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_PTHREAD
#include <pthread.h>
#elif _WIN32
#include <windows.h>
#endif
#include "lt-macros.h"

LT_BEGIN_DECLS

#define LT_LOCK_DEFINE_STATIC(v)	static LT_LOCK_DEFINE(v)
#define LT_LOCK_NAME(v)			__lt_ ## name ## _lock

#if HAVE_PTHREAD
#define LT_LOCK_DEFINE(v)		pthread_mutex_t LT_LOCK_NAME (v) = PTHREAD_MUTEX_INITIALIZER
#define LT_LOCK(v)			pthread_mutex_lock(&LT_LOCK_NAME (v))
#define LT_UNLOCK(v)			pthread_mutex_unlock(&LT_LOCK_NAME (v))
#elif _WIN32
#define LT_LOCK_DEFINE(v)		HANDLE LT_LOCK_NAME (v)
#define LT_LOCK(v)			LT_LOCK_NAME (v) = CreateMutex(NULL, FALSE, NULL)
#define LT_UNLOCK(v)			ReleaseMutex(LT_LOCK_NAME (v))
#else
#error No Mutex Lock available
#endif

LT_END_DECLS

#endif /* __LT_LOCK_H__ */
