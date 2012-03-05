/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-tag-private.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * This library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __LT_TAG_PRIVATE_H__
#define __LT_TAG_PRIVATE_H__

#include <glib.h>
#include <lt-tag.h>

G_BEGIN_DECLS

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
				     const gchar  *tag_string,
				     GError      **error);

G_END_DECLS

#endif /* __LT_TAG_PRIVATE_H__ */
