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

#include <glib.h>

G_BEGIN_DECLS

/* maybe 512 should be enough */
#define LT_PATH_MAX	512

gchar *lt_strlower(gchar *string);

G_END_DECLS

#endif /* __LT_UTILS_H__ */

