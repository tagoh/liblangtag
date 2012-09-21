/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-tag-private.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_TAG_PRIVATE_H__
#define __LT_TAG_PRIVATE_H__

#include "lt-macros.h"
#include "lt-tag.h"

LT_BEGIN_DECLS

enum _lt_tag_state_t {
	STATE_NONE = 0,
	STATE_LANG,
	STATE_PRE_EXTLANG,
	STATE_EXTLANG,
	STATE_PRE_SCRIPT,
	STATE_SCRIPT,
	STATE_PRE_REGION,
	STATE_REGION,
	STATE_PRE_VARIANT,
	STATE_VARIANT,
	STATE_PRE_EXTENSION,
	STATE_EXTENSION,
	STATE_IN_EXTENSION,
	STATE_EXTENSIONTOKEN,
	STATE_IN_EXTENSIONTOKEN,
	STATE_EXTENSIONTOKEN2,
	STATE_PRE_PRIVATEUSE,
	STATE_PRIVATEUSE,
	STATE_IN_PRIVATEUSE,
	STATE_PRIVATEUSETOKEN,
	STATE_IN_PRIVATEUSETOKEN,
	STATE_PRIVATEUSETOKEN2,
	STATE_END
};

typedef enum _lt_tag_state_t	lt_tag_state_t;

lt_tag_state_t lt_tag_parse_wildcard(lt_tag_t     *tag,
				     const char   *tag_string,
				     GError      **error);

LT_END_DECLS

#endif /* __LT_TAG_PRIVATE_H__ */
