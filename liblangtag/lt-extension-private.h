/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-extension-private.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_EXTENSION_PRIVATE_H__
#define __LT_EXTENSION_PRIVATE_H__

#include "lt-macros.h"
#include "lt-ext-module.h"
#include "lt-extension.h"

LT_BEGIN_DECLS

lt_extension_t *lt_extension_create        (void);
lt_extension_t *lt_extension_copy          (lt_extension_t  *extension);
lt_bool_t       lt_extension_has_singleton (lt_extension_t  *extension,
                                            char             singleton_c);
lt_bool_t       lt_extension_add_singleton (lt_extension_t  *extension,
					    char             singleton_c,
					    const lt_tag_t  *tag,
					    lt_error_t     **error);
lt_bool_t       lt_extension_add_tag       (lt_extension_t  *extension,
                                            const char      *subtag,
                                            lt_error_t     **error);
void            lt_extension_cancel_tag    (lt_extension_t  *extension);
lt_bool_t       lt_extension_validate_state(lt_extension_t  *extension);

LT_END_DECLS

#endif /* __LT_EXTENSION_PRIVATE_H__ */
