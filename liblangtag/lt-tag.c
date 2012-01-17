/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-tag.c
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include "lt-database.h"
#include "lt-error.h"
#include "lt-mem.h"
#include "lt-utils.h"
#include "lt-tag.h"
#include "lt-tag-private.h"


typedef struct _lt_tag_scanner_t {
	lt_mem_t  parent;
	gchar    *string;
	gsize     length;
	gsize     position;
} lt_tag_scanner_t;

struct _lt_tag_t {
	lt_mem_t            parent;
	gint32              wildcard_map;
	gchar              *tag_string;
	lt_lang_t          *language;
	lt_extlang_t       *extlang;
	lt_script_t        *script;
	lt_region_t        *region;
	GList              *variants;
	GList              *extensions;
	GString            *privateuse;
	lt_grandfathered_t *grandfathered;
};

/*< private >*/
static void
_lt_tag_gstring_free(GString *string)
{
	g_string_free(string, TRUE);
}

static gboolean
_lt_tag_gstring_compare(const GString *v1,
			const GString *v2)
{
	gboolean retval = FALSE;
	gchar *s1, *s2;

	if (v1 == v2)
		return TRUE;

	s1 = v1 ? lt_strlower(g_strdup(v1->str)) : NULL;
	s2 = v2 ? lt_strlower(g_strdup(v2->str)) : NULL;

	if (g_strcmp0(s1, "*") == 0 ||
	    g_strcmp0(s2, "*") == 0) {
		retval = TRUE;
		goto bail;
	}

	retval = g_strcmp0(s1, s2) == 0;
  bail:
	g_free(s1);
	g_free(s2);

	return retval;
}

static void
_lt_tag_variants_list_free(GList *list)
{
	GList *l;

	for (l = list; l != NULL; l = g_list_next(l)) {
		lt_variant_unref(l->data);
	}
	g_list_free(list);
}

static void
_lt_tag_extensions_list_free(GList *list)
{
	GList *l;

	for (l = list; l != NULL; l = g_list_next(l)) {
		g_string_free(l->data, TRUE);
	}
	g_list_free(list);
}

static gint
_lt_tag_extensions_singleton_compare(gconstpointer v1,
				     gconstpointer v2)
{
	const GString *s1 = v1;
	const gchar *s2 = v2;

	return s1->str[0] - s2[0];
}

static gint
_lt_tag_extensions_compare(gconstpointer v1,
			   gconstpointer v2)
{
	const GString *s1 = v1, *s2 = v2;

	return g_strcmp0(s1->str, s2->str);
}

static lt_tag_scanner_t *
lt_tag_scanner_new(const gchar *tag)
{
	lt_tag_scanner_t *retval = lt_mem_alloc_object(sizeof (lt_tag_scanner_t));

	if (retval) {
		retval->string = g_strdup(tag);
		lt_mem_add_ref(&retval->parent, retval->string,
			       (lt_destroy_func_t)g_free);
		retval->length = strlen(tag);
	}

	return retval;
}

static void
lt_tag_scanner_unref(lt_tag_scanner_t *scanner)
{
	if (scanner)
		lt_mem_unref(&scanner->parent);
}

static gboolean
lt_tag_scanner_get_token(lt_tag_scanner_t  *scanner,
			 gchar            **retval,
			 gsize             *length,
			 GError           **error)
{
	GString *string = NULL;
	gchar c;
	GError *err = NULL;

	g_return_val_if_fail (scanner != NULL, FALSE);

	if (scanner->position >= scanner->length) {
		g_set_error(&err, LT_ERROR, LT_ERR_EOT,
			    "No more tokens in buffer");
		goto bail;
	}

	string = g_string_new(NULL);
	while (scanner->position < scanner->length) {
		c = scanner->string[scanner->position++];
		if (c == 0) {
			if (string->len == 0) {
				g_set_error(&err, LT_ERROR, LT_ERR_EOT,
					    "No more tokens in buffer");
			}
			scanner->position--;
			break;
		}
		if (c == '*') {
			if (string->len > 0) {
				g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					    "Invalid wildcard: positon = %" G_GSIZE_FORMAT,
					    scanner->position - 1);
				break;
			}
		} else if (!g_ascii_isalnum(c) && c != '-' && c != 0) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
				    "Invalid character for tag: '%c'", c);
			break;
		}
		g_string_append_c(string, c);

		if (c == '-' ||
		    c == '*')
			break;
		if (scanner->string[scanner->position] == '-' ||
		    scanner->string[scanner->position] == 0)
			break;
	}
  bail:
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
		if (string)
			g_string_free(string, TRUE);
		*retval = NULL;
		*length = 0;

		return FALSE;
	}

	*length = string->len;
	*retval = g_string_free(string, FALSE);

	return TRUE;
}

static gboolean
lt_tag_scanner_is_eof(lt_tag_scanner_t *scanner)
{
	g_return_val_if_fail (scanner != NULL, TRUE);
	g_return_val_if_fail (scanner->position <= scanner->length, TRUE);

	return scanner->string[scanner->position] == 0 ||
		scanner->position >= scanner->length;
}

static gint
_lt_tag_variant_compare(gconstpointer a,
			gconstpointer b)
{
	return (gulong)a - (gulong)b;
}

static void
lt_tag_fill_wildcard(lt_tag_t       *tag,
		     lt_tag_state_t  begin,
		     lt_tag_state_t  end)
{
	lt_tag_state_t i;
	lt_lang_db_t *langdb;
	lt_extlang_db_t *extlangdb;
	lt_script_db_t *scriptdb;
	lt_region_db_t *regiondb;
	lt_variant_db_t *variantdb;

	for (i = begin; i < end; i++) {
		tag->wildcard_map |= (1 << (i - 1));
		switch (i) {
		    case STATE_LANG:
			    langdb = lt_db_get_lang();
			    tag->language = lt_lang_db_lookup(langdb, "*");
			    lt_mem_add_ref(&tag->parent, tag->language,
					   (lt_destroy_func_t)lt_lang_unref);
			    lt_lang_db_unref(langdb);
			    break;
		    case STATE_EXTLANG:
			    extlangdb = lt_db_get_extlang();
			    tag->extlang = lt_extlang_db_lookup(extlangdb, "*");
			    lt_mem_add_ref(&tag->parent, tag->extlang,
					   (lt_destroy_func_t)lt_extlang_unref);
			    lt_extlang_db_unref(extlangdb);
			    break;
		    case STATE_SCRIPT:
			    scriptdb = lt_db_get_script();
			    tag->script = lt_script_db_lookup(scriptdb, "*");
			    lt_mem_add_ref(&tag->parent, tag->script,
					   (lt_destroy_func_t)lt_script_unref);
			    lt_script_db_unref(scriptdb);
			    break;
		    case STATE_REGION:
			    regiondb = lt_db_get_region();
			    tag->region = lt_region_db_lookup(regiondb, "*");
			    lt_mem_add_ref(&tag->parent, tag->region,
					   (lt_destroy_func_t)lt_region_unref);
			    lt_region_db_unref(regiondb);
			    break;
		    case STATE_VARIANT:
			    variantdb = lt_db_get_variant();
			    tag->variants = g_list_append(tag->variants,
							  lt_variant_db_lookup(variantdb, "*"));
			    lt_mem_add_ref(&tag->parent, tag->variants,
					   (lt_destroy_func_t)_lt_tag_variants_list_free);
			    lt_variant_db_unref(variantdb);
			    break;
		    case STATE_EXTENSION:
			    tag->extensions = g_list_append(tag->extensions,
							    g_string_new("*"));
			    lt_mem_add_ref(&tag->parent, tag->extensions,
					   (lt_destroy_func_t)_lt_tag_extensions_list_free);
			    break;
		    case STATE_PRIVATEUSE:
			    g_string_truncate(tag->privateuse, 0);
			    g_string_append(tag->privateuse, "*");
			    break;
		    default:
			    break;
		}
	}
}

static gboolean
lt_tag_parse_prestate(lt_tag_t        *tag,
		      const gchar     *token,
		      gsize            length,
		      lt_tag_state_t  *state,
		      GError         **error)
{
	gboolean retval = TRUE;

	if (g_strcmp0(token, "-") == 0) {
		switch (*state) {
		    case STATE_PRE_EXTLANG:
			    *state = STATE_EXTLANG;
			    break;
		    case STATE_PRE_SCRIPT:
			    *state = STATE_SCRIPT;
			    break;
		    case STATE_PRE_REGION:
			    *state = STATE_REGION;
			    break;
		    case STATE_PRE_VARIANT:
			    *state = STATE_VARIANT;
			    break;
		    case STATE_PRE_EXTENSION:
			    *state = STATE_EXTENSION;
			    break;
		    case STATE_IN_EXTENSION:
			    *state = STATE_EXTENSIONTOKEN;
			    break;
		    case STATE_IN_EXTENSIONTOKEN:
			    *state = STATE_EXTENSIONTOKEN2;
			    break;
		    case STATE_PRE_PRIVATEUSE:
			    *state = STATE_PRIVATEUSE;
			    break;
		    case STATE_IN_PRIVATEUSE:
			    *state = STATE_PRIVATEUSETOKEN;
			    break;
		    case STATE_IN_PRIVATEUSETOKEN:
			    *state = STATE_PRIVATEUSETOKEN2;
			    break;
		    default:
			    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"Invalid syntax found during parsing a token: %s",
					token);
			    retval = FALSE;
			    break;
		}
	} else {
		retval = FALSE;
	}

	return retval;
}

static gboolean
lt_tag_parse_state(lt_tag_t        *tag,
		   const gchar     *token,
		   gsize            length,
		   lt_tag_state_t  *state,
		   GError         **error)
{
	gboolean retval = TRUE;
	const gchar *p;
	GList *l;

	switch (*state) {
	    case STATE_LANG:
		    if (length == 1) {
			    if (g_ascii_strcasecmp(token, "x") == 0) {
				    g_string_append(tag->privateuse, token);
				    *state = STATE_IN_PRIVATEUSE;
				    break;
			    } else {
			      invalid_tag:
				    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
						"Invalid language subtag: %s", token);
				    break;
			    }
		    } else if (length >= 2 && length <= 3) {
			    lt_lang_db_t *langdb = lt_db_get_lang();

			    /* shortest ISO 639 code */
			    tag->language = lt_lang_db_lookup(langdb, token);
			    lt_lang_db_unref(langdb);
			    if (!tag->language) {
				    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
						"Unknown ISO 639 code: %s",
						token);
				    break;
			    }
			    /* validate if it's really shortest one */
			    p = lt_lang_get_tag(tag->language);
			    if (!p || g_ascii_strcasecmp(token, p) != 0) {
				    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
						"No such language subtag: %s",
						token);
				    lt_lang_unref(tag->language);
				    tag->language = NULL;
				    break;
			    }
			    lt_mem_add_ref(&tag->parent, tag->language,
					   (lt_destroy_func_t)lt_lang_unref);
			    *state = STATE_PRE_EXTLANG;
		    } else if (length == 4) {
			    /* reserved for future use */
			    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"Reserved for future use: %s",
					token);
		    } else if (length >= 5 && length <= 8) {
			    /* registered language subtag */
			    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"XXX: registered language tag: %s",
					token);
		    } else {
			    goto invalid_tag;
		    }
		    break;
	    case STATE_EXTLANG:
		    if (length == 3) {
			    lt_extlang_db_t *extlangdb = lt_db_get_extlang();

			    tag->extlang = lt_extlang_db_lookup(extlangdb, token);
			    lt_extlang_db_unref(extlangdb);
			    if (tag->extlang) {
				    const gchar *prefix = lt_extlang_get_prefix(tag->extlang);
				    const gchar *subtag = lt_extlang_get_tag(tag->extlang);
				    const gchar *lang = lt_lang_get_better_tag(tag->language);

				    if (prefix &&
					g_ascii_strcasecmp(prefix, lang) != 0) {
					    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
							"extlang '%s' is supposed to be used with %s, but %s",
							subtag, prefix, lang);
					    lt_extlang_unref(tag->extlang);
					    tag->extlang = NULL;
				    } else {
					    lt_mem_add_ref(&tag->parent, tag->extlang,
							   (lt_destroy_func_t)lt_extlang_unref);
					    *state = STATE_PRE_SCRIPT;
				    }
				    break;
			    }
			    /* try to check something else */
		    } else {
			    /* it may be a script */
		    }
	    case STATE_SCRIPT:
		    if (length == 4) {
			    lt_script_db_t *scriptdb = lt_db_get_script();

			    tag->script = lt_script_db_lookup(scriptdb, token);
			    lt_script_db_unref(scriptdb);
			    if (tag->script) {
				    lt_mem_add_ref(&tag->parent, tag->script,
						   (lt_destroy_func_t)lt_script_unref);
				    *state = STATE_PRE_REGION;
				    break;
			    }
			    /* try to check something else */
		    } else {
			    /* it may be a region */
		    }
	    case STATE_REGION:
		    if (length == 2 ||
			(length == 3 &&
			 g_ascii_isdigit(token[0]) &&
			 g_ascii_isdigit(token[1]) &&
			 g_ascii_isdigit(token[2]))) {
			    lt_region_db_t *regiondb = lt_db_get_region();

			    tag->region = lt_region_db_lookup(regiondb, token);
			    lt_region_db_unref(regiondb);
			    if (tag->region) {
				    lt_mem_add_ref(&tag->parent, tag->region,
						   (lt_destroy_func_t)lt_region_unref);
				    *state = STATE_PRE_VARIANT;
				    break;
			    }
			    /* try to check something else */
		    } else {
			    /* it may be a variant */
		    }
	    case STATE_VARIANT:
		    if ((length >=5 && length <= 8) ||
			(length == 4 && g_ascii_isdigit(token[0]))) {
			    lt_variant_db_t *variantdb = lt_db_get_variant();
			    lt_variant_t *variant;

			    variant = lt_variant_db_lookup(variantdb, token);
			    lt_variant_db_unref(variantdb);
			    if (variant) {
				    const GList *prefixes = lt_variant_get_prefix(variant), *l;
				    gchar *langtag = lt_tag_canonicalize(tag, error);
				    GString *str_prefixes = g_string_new(NULL);
				    gboolean matched = FALSE;

				    if (error && *error) {
					    /* ignore it and fallback to the original tag string */
					    g_error_free(*error);
					    *error = NULL;
					    langtag = g_strdup(tag->tag_string);
				    }
				    for (l = prefixes; l != NULL; l = g_list_next(l)) {
					    const gchar *s = l->data;

					    if (str_prefixes->len > 0)
						    g_string_append(str_prefixes, ",");
					    g_string_append(str_prefixes, s);

					    if (g_ascii_strncasecmp(s, langtag, strlen(s)) == 0) {
						    matched = TRUE;
						    break;
					    }
				    }
				    if (prefixes && !matched) {
					    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
							"variant '%s' is supposed to be used with %s, but %s",
							token, str_prefixes->str, tag->tag_string);
					    lt_variant_unref(variant);
				    } else {
					    if (!tag->variants) {
						    tag->variants = g_list_append(tag->variants,
										  variant);
						    lt_mem_add_ref(&tag->parent, tag->variants,
								   (lt_destroy_func_t)_lt_tag_variants_list_free);
					    } else {
						    GList *prefixes = (GList *)lt_variant_get_prefix(variant);
						    const gchar *tstr;

						    if (tag->tag_string) {
							    lt_mem_remove_ref(&tag->parent, tag->tag_string);
							    tag->tag_string = NULL;
						    }
						    tstr = lt_tag_get_string(tag);
						    if (prefixes && !g_list_find_custom(prefixes, tstr, (GCompareFunc)g_strcmp0)) {
							    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
									"Variant isn't allowed for %s: %s",
									tstr,
									lt_variant_get_tag(variant));
							    lt_variant_unref(variant);
						    } else if (!prefixes && g_list_find_custom(tag->variants, variant, _lt_tag_variant_compare)) {
							    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
									"Duplicate variants: %s",
									lt_variant_get_tag(variant));
							    lt_variant_unref(variant);
						    } else {
							    tag->variants = g_list_append(tag->variants,
											  variant);
						    }
					    }
					    /* multiple variants are allowed. */
					    *state = STATE_PRE_VARIANT;
				    }
				    g_free(langtag);
				    g_string_free(str_prefixes, TRUE);
				    break;
			    }
			    /* try to check something else */
		    } else {
			    /* it may be an extension */
		    }
	    case STATE_EXTENSION:
	    extension:
		    if (length == 1 &&
			token[0] != 'x' &&
			token[0] != 'X' &&
			token[0] != '*' &&
			token[0] != '-') {
			    GString *string;
			    gboolean noref = tag->extensions == NULL;

			    if (tag->extensions && g_list_find_custom(tag->extensions, token, _lt_tag_extensions_singleton_compare)) {
				    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
						"Duplicate singleton for extension: %s", token);
			    } else {
				    string = g_string_new(token);
				    tag->extensions = g_list_append(tag->extensions, string);
				    if (noref)
					    lt_mem_add_ref(&tag->parent, tag->extensions,
							   (lt_destroy_func_t)_lt_tag_extensions_list_free);
				    *state = STATE_IN_EXTENSION;
			    }
			    break;
		    } else {
			    /* it may be a private use */
		    }
	    case STATE_PRIVATEUSE:
		    if (length == 1 && (token[0] == 'x' || token[0] == 'X')) {
			    g_string_append(tag->privateuse, token);
			    *state = STATE_IN_PRIVATEUSE;
		    } else {
			    /* No state to try */
			    retval = FALSE;
		    }
		    break;
	    case STATE_EXTENSIONTOKEN:
	    case STATE_EXTENSIONTOKEN2:
		    if (length >= 2 && length <= 8) {
			    GString *string;

			    l = g_list_last(tag->extensions);
			    string = l->data;
			    g_string_append_printf(string, "-%s", token);
			    /* try to check more extension token first */
			    *state = STATE_IN_EXTENSIONTOKEN;
		    } else {
			    if (*state == STATE_EXTENSIONTOKEN2) {
				    /* No need to destroy the previous tokens.
				     * fallback to check the extension again.
				     */
				    goto extension;
			    }
			    l = g_list_last(tag->extensions);

			    if (g_list_previous(l) == NULL) {
				    lt_mem_remove_ref(&tag->parent, tag->extensions);
				    tag->extensions = NULL;
			    } else {
				    g_string_free(l->data, TRUE);
				    tag->extensions = g_list_delete_link(tag->extensions, l);
			    }
			    /* No state to try */
			    retval = FALSE;
		    }
		    break;
	    case STATE_PRIVATEUSETOKEN:
	    case STATE_PRIVATEUSETOKEN2:
		    if (length <= 8) {
			    g_string_append_printf(tag->privateuse, "-%s", token);
			    *state = STATE_IN_PRIVATEUSETOKEN;
		    } else {
			    /* 'x'/'X' is reserved singleton for the private use subtag.
			     * so nothing to fallback to anything else.
			     */
			    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
					"Invalid tag for the private use: token = '%s'",
					token);
		    }
		    break;
	    default:
		    g_set_error(error, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
				"Unable to parse tag: %s, token = '%s' state = %d",
				tag->tag_string, token, *state);
		    break;
	}
	if (*error)
		retval = FALSE;

	return retval;
}

static gboolean
_lt_tag_parse(lt_tag_t        *tag,
	      const gchar     *langtag,
	      gboolean         allow_wildcard,
	      lt_tag_state_t  *state,
	      GError         **error)
{
	lt_tag_scanner_t *scanner = NULL;
	lt_grandfathered_db_t *grandfathereddb;
	gchar *token = NULL;
	gsize len = 0;
	GError *err = NULL;
	gboolean retval = TRUE;
	lt_tag_state_t wildcard = STATE_NONE;
	gint count = 0;

	g_return_val_if_fail (tag != NULL, FALSE);
	g_return_val_if_fail (langtag != NULL, FALSE);

	lt_tag_clear(tag);

	grandfathereddb = lt_db_get_grandfathered();
	tag->grandfathered = lt_grandfathered_db_lookup(grandfathereddb, langtag);
	lt_grandfathered_db_unref(grandfathereddb);
	if (tag->grandfathered) {
		/* no need to lookup anymore. */
		lt_mem_add_ref(&tag->parent, tag->grandfathered,
			       (lt_destroy_func_t)lt_grandfathered_unref);
		goto bail;
	}

	scanner = lt_tag_scanner_new(langtag);
	*state = STATE_LANG;
	while (!lt_tag_scanner_is_eof(scanner)) {
		if (token) {
			g_free(token);
			token = NULL;
		}
		if (!lt_tag_scanner_get_token(scanner, &token, &len, &err)) {
			if (err)
				break;
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
				    "Unrecoverable error");
			break;
		}
		count++;
		if (!token || len == 0) {
			g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
				    "No valid tokens found");
			break;
		}
		if (!lt_tag_parse_prestate(tag, token, len, state, &err)) {
			if (err)
				break;
			if (allow_wildcard && g_strcmp0(token, "*") == 0) {
				wildcard = *state;
				if (*state == STATE_LANG)
					*state += 1;
				else
					*state -= 1;
			} else {
				if (!lt_tag_parse_state(tag, token, len, state, &err))
					break;
				if (wildcard != STATE_NONE) {
					lt_tag_fill_wildcard(tag, wildcard, *state - 1);
					wildcard = STATE_NONE;
				}
			}
		}
	}
	if (wildcard != STATE_NONE) {
		lt_tag_fill_wildcard(tag, wildcard, STATE_END);
	}
	if (!err &&
	    *state != STATE_PRE_EXTLANG &&
	    *state != STATE_PRE_SCRIPT &&
	    *state != STATE_PRE_REGION &&
	    *state != STATE_PRE_VARIANT &&
	    *state != STATE_PRE_EXTENSION &&
	    *state != STATE_PRE_PRIVATEUSE &&
	    *state != STATE_IN_EXTENSIONTOKEN &&
	    *state != STATE_IN_PRIVATEUSETOKEN &&
	    *state != STATE_NONE) {
		g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
			    "Invalid tag: %s, last token = '%s', state = %d, parsed count = %d",
			    langtag, token, *state, count);
	}
  bail:
	if (tag->tag_string)
		lt_mem_remove_ref(&tag->parent, tag->tag_string);
	tag->tag_string = g_strdup(langtag);
	lt_mem_add_ref(&tag->parent, tag->tag_string,
		       (lt_destroy_func_t)g_free);
	lt_tag_scanner_unref(scanner);
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
		retval = FALSE;
	}
	g_free(token);

	return retval;
}

static gboolean
_lt_tag_match(const lt_tag_t *v1,
	      lt_tag_t       *v2,
	      lt_tag_state_t  state)
{
	g_return_val_if_fail (v1 != NULL, FALSE);
	g_return_val_if_fail (v2 != NULL, FALSE);

	if (state > STATE_EXTLANG && !v2->extlang && v1->extlang) {
		lt_extlang_db_t *db = lt_db_get_extlang();

		v2->extlang = lt_extlang_db_lookup(db, "");
		lt_mem_add_ref(&v2->parent, v2->extlang,
			       (lt_destroy_func_t)lt_extlang_unref);
		lt_extlang_db_unref(db);
	}
	if (state > STATE_SCRIPT && !v2->script && v1->script) {
		lt_script_db_t *db = lt_db_get_script();

		v2->script = lt_script_db_lookup(db, "");
		lt_mem_add_ref(&v2->parent, v2->script,
			       (lt_destroy_func_t)lt_script_unref);
		lt_script_db_unref(db);
	}
	if (state > STATE_REGION && !v2->region && v1->region) {
		lt_region_db_t *db = lt_db_get_region();

		v2->region = lt_region_db_lookup(db, "");
		lt_mem_add_ref(&v2->parent, v2->region,
			       (lt_destroy_func_t)lt_region_unref);
		lt_region_db_unref(db);
	}
	if (state > STATE_VARIANT && !v2->variants && v1->variants) {
		lt_variant_db_t *db = lt_db_get_variant();

		v2->variants = g_list_append(v2->variants, lt_variant_db_lookup(db, ""));
		lt_mem_add_ref(&v2->parent, v2->variants,
			       (lt_destroy_func_t)_lt_tag_variants_list_free);
		lt_variant_db_unref(db);
	}
	if (state > STATE_EXTENSION && !v2->extensions && v1->extensions) {
		v2->extensions = g_list_append(v2->extensions, g_string_new(""));
		lt_mem_add_ref(&v2->parent, v2->extensions,
			       (lt_destroy_func_t)_lt_tag_extensions_list_free);
	}

	return lt_tag_compare(v1, v2);
}

/*< protected >*/
lt_tag_state_t
lt_tag_parse_wildcard(lt_tag_t     *tag,
		      const gchar  *tag_string,
		      GError      **error)
{
	GError *err = NULL;
	lt_tag_state_t retval = STATE_NONE;
	gboolean ret = _lt_tag_parse(tag, tag_string, TRUE, &retval, &err);

	if (!ret && !err) {
		g_set_error(&err, LT_ERROR, LT_ERR_FAIL_ON_SCANNER,
			    "Unknown error during parsing a tag.");
	}
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
	}

	return retval;
}

/*< public >*/
lt_tag_t *
lt_tag_new(void)
{
	lt_tag_t *retval = lt_mem_alloc_object(sizeof (lt_tag_t));

	if (retval) {
		retval->privateuse = g_string_new(NULL);
		lt_mem_add_ref(&retval->parent, retval->privateuse,
			       (lt_destroy_func_t)_lt_tag_gstring_free);
	}

	return retval;
}

lt_tag_t *
lt_tag_ref(lt_tag_t *tag)
{
	g_return_val_if_fail (tag != NULL, NULL);

	return lt_mem_ref(&tag->parent);
}

void
lt_tag_unref(lt_tag_t *tag)
{
	if (tag)
		lt_mem_unref(&tag->parent);
}

void
lt_tag_clear(lt_tag_t *tag)
{
	g_return_if_fail (tag != NULL);

	if (tag->tag_string) {
		lt_mem_remove_ref(&tag->parent, tag->tag_string);
		tag->tag_string = NULL;
	}
	if (tag->language) {
		lt_mem_remove_ref(&tag->parent, tag->language);
		tag->language = NULL;
	}
	if (tag->extlang) {
		lt_mem_remove_ref(&tag->parent, tag->extlang);
		tag->extlang = NULL;
	}
	if (tag->script) {
		lt_mem_remove_ref(&tag->parent, tag->script);
		tag->script = NULL;
	}
	if (tag->region) {
		lt_mem_remove_ref(&tag->parent, tag->region);
		tag->region = NULL;
	}
	if (tag->variants) {
		lt_mem_remove_ref(&tag->parent, tag->variants);
		tag->variants = NULL;
	}
	if (tag->extensions) {
		lt_mem_remove_ref(&tag->parent, tag->extensions);
		tag->extensions = NULL;
	}
	if (tag->privateuse) {
		g_string_truncate(tag->privateuse, 0);
	}
	if (tag->grandfathered) {
		lt_mem_remove_ref(&tag->parent, tag->grandfathered);
		tag->grandfathered = NULL;
	}
}

gboolean
lt_tag_parse(lt_tag_t     *tag,
	     const gchar  *tag_string,
	     GError      **error)
{
	lt_tag_state_t state = STATE_NONE;

	return _lt_tag_parse(tag, tag_string, FALSE, &state, error);
}

const gchar *
lt_tag_get_string(lt_tag_t *tag)
{
	GString *string;
	GList *l;

	if (tag->tag_string)
		return tag->tag_string;

	string = g_string_new(NULL);
	if (tag->grandfathered)
		g_string_append(string, lt_grandfathered_get_tag(tag->grandfathered));
	else if (tag->language) {
		g_string_append(string, lt_lang_get_tag(tag->language));
		if (tag->extlang)
			g_string_append_printf(string, "-%s",
					       lt_extlang_get_tag(tag->extlang));
		if (tag->script)
			g_string_append_printf(string, "-%s",
					       lt_script_get_tag(tag->script));
		if (tag->region)
			g_string_append_printf(string, "-%s",
					       lt_region_get_tag(tag->region));
		l = tag->variants;
		while (l != NULL) {
			g_string_append_printf(string, "-%s",
					       lt_variant_get_tag(l->data));
			l = g_list_next(l);
		}
		l = tag->extensions;
		while (l != NULL) {
			GString *s = l->data;

			g_string_append_printf(string, "-%s",
					       s->str);
			l = g_list_next(l);
		}
		if (tag->privateuse && tag->privateuse->len > 0)
			g_string_append_printf(string, "-%s", tag->privateuse->str);
	} else if (tag->privateuse && tag->privateuse->len > 0) {
		g_string_append(string, tag->privateuse->str);
	}

	tag->tag_string = g_string_free(string, FALSE);
	lt_mem_add_ref(&tag->parent, tag->tag_string,
		       (lt_destroy_func_t)g_free);

	return tag->tag_string;
}

gchar *
lt_tag_canonicalize(lt_tag_t  *tag,
		    GError   **error)
{
	gchar *retval = NULL;
	GString *string = NULL;
	GError *err = NULL;
	GList *l, *ll;

	g_return_val_if_fail (tag != NULL, NULL);

	string = g_string_new(NULL);
	if (tag->grandfathered) {
		g_string_append(string, lt_grandfathered_get_better_tag(tag->grandfathered));
		goto bail1;
	}

	if (tag->language) {
		gsize len;

		g_string_append(string, lt_lang_get_better_tag(tag->language));
		if (tag->extlang) {
			const gchar *preferred = lt_extlang_get_preferred_tag(tag->extlang);

			if (preferred) {
				g_string_truncate(string, 0);
				g_string_append(string, preferred);
			} else {
				g_string_append_printf(string, "-%s",
						       lt_extlang_get_tag(tag->extlang));
			}
		}
		if (tag->script) {
			const gchar *script = lt_script_get_tag(tag->script);
			const gchar *suppress = lt_lang_get_suppress_script(tag->language);

			if (!suppress ||
			    g_ascii_strcasecmp(suppress, script))
				g_string_append_printf(string, "-%s", script);
		}
		if (tag->region) {
			g_string_append_printf(string, "-%s", lt_region_get_better_tag(tag->region));
		}
		l = tag->variants;
		len = string->len;
		while (l != NULL) {
			lt_variant_t *variant = l->data;
			const gchar *better = lt_variant_get_better_tag(variant);
			const gchar *s = lt_variant_get_tag(variant);

			if (better && g_ascii_strcasecmp(s, better) != 0) {
				/* ignore all of variants prior to this one */
				g_string_truncate(string, len);
			}
			g_string_append_printf(string, "-%s", better ? better : s);
			l = g_list_next(l);
		}
		l = g_list_copy(tag->extensions);
		l = g_list_sort(l, _lt_tag_extensions_compare);
		ll = l;
		while (ll != NULL) {
			GString *str = ll->data;

			g_string_append_printf(string, "-%s", str->str);
			ll = g_list_next(ll);
		}
		g_list_free(l);
	}
	if (tag->privateuse && tag->privateuse->len > 0) {
		g_string_append_printf(string, "%s%s",
				       string->len > 0 ? "-" : "",
				       tag->privateuse->str);
	}
	if (string->len == 0) {
		g_set_error(&err, LT_ERROR, LT_ERR_NO_TAG,
			    "No tag to convert.");
	}
  bail1:
	retval = g_string_free(string, FALSE);
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
		if (retval)
			g_free(retval);
		retval = NULL;
	}

	return retval;
}

gchar *
lt_tag_convert_to_locale(lt_tag_t  *tag,
			 GError   **error)
{
	gchar *retval = NULL;
	GString *string = NULL;
	GError *err = NULL;
	const gchar *mod = NULL;

	g_return_val_if_fail (tag != NULL, NULL);

	if (!tag->tag_string) {
		g_set_error(&err, LT_ERROR, LT_ERR_NO_TAG,
			    "No tag to convert.");
		goto bail;
	}
	string = g_string_new(NULL);
	g_string_append(string, lt_lang_get_better_tag(tag->language));
	if (tag->region)
		g_string_append_printf(string, "_%s",
				       lt_region_get_tag(tag->region));
	if (tag->script) {
		mod = lt_script_convert_to_modifier(tag->script);
		if (mod)
			g_string_append_printf(string, "@%s", mod);
	}

  bail:
	if (string)
		retval = g_string_free(string, FALSE);
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
		if (retval)
			g_free(retval);
		retval = NULL;
	}

	return retval;
}

void
lt_tag_dump(const lt_tag_t *tag)
{
	GList *l;

	g_return_if_fail (tag != NULL);

	if (tag->grandfathered) {
		lt_grandfathered_dump(tag->grandfathered);
		return;
	}
	lt_lang_dump(tag->language);
	if (tag->extlang)
		lt_extlang_dump(tag->extlang);
	if (tag->script)
		lt_script_dump(tag->script);
	if (tag->region)
		lt_region_dump(tag->region);
	l = tag->variants;
	while (l != NULL) {
		lt_variant_t *variant = l->data;

		lt_variant_dump(variant);
		l = g_list_next(l);
	}
	l = tag->extensions;
	while (l != NULL) {
		GString *ext = l->data;

		g_print("Extension: %s\n", ext->str);
		l = g_list_next(l);
	}
	if (tag->privateuse->len > 0)
		g_print("Private Use: %s\n", tag->privateuse->str);
}

gboolean
lt_tag_compare(const lt_tag_t *v1,
	       const lt_tag_t *v2)
{
	gboolean retval = TRUE;
	const GList *l1, *l2;

	g_return_val_if_fail (v1 != NULL, FALSE);
	g_return_val_if_fail (v2 != NULL, FALSE);
	g_return_val_if_fail (v1->grandfathered == NULL, FALSE);
	g_return_val_if_fail (v2->grandfathered == NULL, FALSE);

	retval &= lt_lang_compare(v1->language, v2->language);
	if (v2->extlang)
		retval &= lt_extlang_compare(v1->extlang, v2->extlang);
	if (v2->script)
		retval &= lt_script_compare(v1->script, v2->script);
	if (v2->region)
		retval &= lt_region_compare(v1->region, v2->region);
	l1 = v1->variants;
	l2 = v2->variants;
	while (l2 != NULL) {
		lt_variant_t *vv1, *vv2;

		vv1 = l1 ? l1->data : NULL;
		vv2 = l2->data;
		retval &= lt_variant_compare(vv1, vv2);
		l1 = l1 ? g_list_next(l1) : NULL;
		l2 = g_list_next(l2);
	}
	l1 = v1->extensions;
	l2 = v2->extensions;
	while (l2 != NULL) {
		GString *s1, *s2;

		s1 = l1 ? l1->data : NULL;
		s2 = l2->data;
		retval &= _lt_tag_gstring_compare(s1, s2);
		l1 = l1 ? g_list_next(l1) : NULL;
		l2 = g_list_next(l2);
	}
	if (v2->privateuse && v2->privateuse->len > 0)
		retval &= _lt_tag_gstring_compare(v1->privateuse, v2->privateuse);

	return retval;
}

gboolean
lt_tag_match(const lt_tag_t  *v1,
	     const gchar     *v2,
	     GError         **error)
{
	gboolean retval = FALSE;
	lt_tag_t *t2 = NULL;
	lt_tag_state_t state = STATE_NONE;
	GError *err = NULL;

	g_return_val_if_fail (v1 != NULL, FALSE);
	g_return_val_if_fail (v2 != NULL, FALSE);

	t2 = lt_tag_new();
	state = lt_tag_parse_wildcard(t2, v2, &err);
	if (err)
		goto bail;
	retval = _lt_tag_match(v1, t2, state);
  bail:
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
		retval = FALSE;
	}
	if (t2)
		lt_tag_unref(t2);

	return retval;
}

gchar *
lt_tag_lookup(const lt_tag_t  *tag,
	      const gchar     *pattern,
	      GError         **error)
{
	lt_tag_t *t2 = NULL;
	lt_tag_state_t state = STATE_NONE;
	GError *err = NULL;
	GList *l;
	gchar *retval = NULL;

	g_return_val_if_fail (tag != NULL, NULL);
	g_return_val_if_fail (pattern != NULL, NULL);

	t2 = lt_tag_new();
	state = lt_tag_parse_wildcard(t2, pattern, &err);
	if (err)
		goto bail;
	if (_lt_tag_match(tag, t2, state)) {
		gint32 i;

		for (i = 0; i < (STATE_END - 1); i++) {
			if (t2->wildcard_map & (1 << i)) {
				switch (i + 1) {
				    case STATE_LANG:
					    t2->language = lt_lang_ref(tag->language);
					    lt_mem_add_ref(&t2->parent, t2->language,
							   (lt_destroy_func_t)lt_lang_unref);
					    break;
				    case STATE_EXTLANG:
					    if (t2->extlang) {
						    lt_mem_remove_ref(&t2->parent, t2->extlang);
						    t2->extlang = NULL;
					    }
					    if (tag->extlang) {
						    t2->extlang = lt_extlang_ref(tag->extlang);
						    lt_mem_add_ref(&t2->parent, t2->extlang,
								   (lt_destroy_func_t)lt_extlang_unref);
					    }
					    break;
				    case STATE_SCRIPT:
					    if (t2->script) {
						    lt_mem_remove_ref(&t2->parent, t2->script);
						    t2->script = NULL;
					    }
					    if (tag->script) {
						    t2->script = lt_script_ref(tag->script);
						    lt_mem_add_ref(&t2->parent, t2->script,
								   (lt_destroy_func_t)lt_script_unref);
					    }
					    break;
				    case STATE_REGION:
					    if (t2->region) {
						    lt_mem_remove_ref(&t2->parent, t2->region);
						    t2->region = NULL;
					    }
					    if (tag->region) {
						    t2->region = lt_region_ref(tag->region);
						    lt_mem_add_ref(&t2->parent, t2->region,
								   (lt_destroy_func_t)lt_region_unref);
					    }
					    break;
				    case STATE_VARIANT:
					    if (t2->variants) {
						    lt_mem_remove_ref(&t2->parent, t2->variants);
						    t2->variants = NULL;
					    }
					    l = tag->variants;
					    while (l != NULL) {
						    t2->variants = g_list_append(t2->variants,
										 lt_variant_ref(l->data));
						    l = g_list_next(l);
					    }
					    if (t2->variants) {
						    lt_mem_add_ref(&t2->parent, t2->variants,
								   (lt_destroy_func_t)_lt_tag_variants_list_free);
					    }
					    break;
				    case STATE_EXTENSION:
				    case STATE_EXTENSIONTOKEN:
				    case STATE_EXTENSIONTOKEN2:
					    if (t2->extensions) {
						    lt_mem_remove_ref(&t2->parent, t2->extensions);
						    t2->extensions = NULL;
					    }
					    l = tag->extensions;
					    while (l != NULL) {
						    GString *s = l->data;

						    t2->extensions = g_list_append(t2->extensions,
										   g_string_new(s->str));
						    l = g_list_next(l);
					    }
					    if (t2->extensions) {
						    lt_mem_add_ref(&t2->parent, t2->extensions,
								   (lt_destroy_func_t)_lt_tag_extensions_list_free);
					    }
					    break;
				    case STATE_PRIVATEUSE:
				    case STATE_PRIVATEUSETOKEN:
				    case STATE_PRIVATEUSETOKEN2:
					    if (t2->privateuse) {
						    g_string_truncate(t2->privateuse, 0);
					    }
					    if (tag->privateuse) {
						    g_string_append(t2->privateuse, tag->privateuse->str);
					    }
					    break;
				    default:
					    break;
				}
			}
		}
		if (t2->tag_string) {
			lt_mem_remove_ref(&t2->parent, t2->tag_string);
			t2->tag_string = NULL;
		}
		retval = g_strdup(lt_tag_get_string(t2));
	}
  bail:
	if (err) {
		if (error)
			*error = g_error_copy(err);
		else
			g_warning(err->message);
		g_error_free(err);
	}
	if (t2)
		lt_tag_unref(t2);

	return retval;
}
