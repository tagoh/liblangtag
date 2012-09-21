/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-script-private.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_SCRIPT_PRIVATE_H__
#define __LT_SCRIPT_PRIVATE_H__

#include "lt-macros.h"
#include "lt-script.h"

LT_BEGIN_DECLS

lt_script_t *lt_script_create  (void);
void         lt_script_set_name(lt_script_t *script,
                                const char  *description);
void         lt_script_set_tag (lt_script_t *script,
                                const char  *subtag);

LT_END_DECLS

#endif /* __LT_SCRIPT_PRIVATE_H__ */
