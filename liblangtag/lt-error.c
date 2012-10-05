/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-error.c
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h> /* XXX: just shut up GHashTable dependency in lt-mem.h */
#include <execinfo.h>
#include <stdlib.h>
#include "lt-list.h"
#include "lt-mem.h"
#include "lt-messages.h"
#include "lt-utils.h"
#include "lt-error.h"

struct _lt_error_t {
	lt_mem_t   parent;
	lt_list_t *data;
};
typedef struct _lt_error_data_t {
	lt_mem_t          parent;
	lt_error_type_t   type;
	char             *message;
	char            **traces;
	size_t            stack_size;
} lt_error_data_t;

/**
 * SECTION:lt-error
 * @Short_Description: Error handling
 * @Title: Error
 *
 * This section describes the error handling in this library.
 */
/*< private >*/
lt_error_t *
lt_error_new(void)
{
	return lt_mem_alloc_object(sizeof (lt_error_t));
}

/*< public >*/
lt_error_t *
lt_error_ref(lt_error_t *error)
{
	lt_return_val_if_fail (error != NULL, NULL);

	return lt_mem_ref(&error->parent);
}

void
lt_error_unref(lt_error_t *error)
{
	if (error)
		lt_mem_unref(&error->parent);
}

lt_error_t *
lt_error_set(lt_error_t      **error,
	     lt_error_type_t   type,
	     const char       *message,
	     ...)
{
	va_list ap;
	void *traces[1024];
	lt_error_data_t *d = lt_mem_alloc_object(sizeof (lt_error_data_t));
	int size;
	lt_bool_t allocated;

	lt_return_val_if_fail (error != NULL, NULL);

	if (!d)
		goto bail0;
	if (!*error)
		*error = lt_error_new();
	if (!*error)
		goto bail0;

	d->type = type;
	va_start(ap, message);
	d->message = lt_strdup_vprintf(message, ap);
	va_end(ap);

	size = backtrace(traces, 1024);
	if (size > 0)
		d->traces = backtrace_symbols(traces, size);
	d->stack_size = size;

	lt_mem_add_ref(&d->parent, d->message, free);
	lt_mem_add_ref(&d->parent, d->traces, free);

	allocated = (*error)->data == NULL;
	(*error)->data = lt_list_append((*error)->data, d, (lt_destroy_func_t)lt_mem_unref);
	if (allocated)
		lt_mem_add_ref(&(*error)->parent,
			       (*error)->data,
			       (lt_destroy_func_t)lt_list_free);

	return *error;
  bail0:
	lt_critical("Out of memory");

	return *error;
}

void
lt_error_clear(lt_error_t *error)
{
	if (error) {
		lt_mem_remove_ref(&error->parent, error->data);
		error->data = NULL;
	}
}

lt_bool_t
lt_error_is_set(lt_error_t      *error,
		lt_error_type_t  type)
{
	if (type == LT_ERR_ANY) {
		return error && error->data;
	} else {
		if (error && error->data) {
			lt_list_t *l;

			for (l = error->data; l != NULL; l = lt_list_next(l)) {
				lt_error_data_t *d = lt_list_value(l);

				if (d->type == type)
					return TRUE;
			}
		}
	}

	return FALSE;
}

void
lt_error_print(lt_error_t      *error,
	       lt_error_type_t  type)
{
	lt_list_t *l;

	if (lt_error_is_set(error, type)) {
		lt_warning("Error raised:");
		for (l = error->data; l != NULL; l = lt_list_next(l)) {
			lt_error_data_t *d = lt_list_value(l);
			int i;

			if (type == LT_ERR_ANY || type == d->type) {
				lt_warning("  %s", d->message);
				if (d->stack_size > 0) {
					lt_warning("  Backtraces:");
				} else {
					lt_warning("  No backtraces");
				}
				for (i = 1; i < d->stack_size; i++) {
					lt_warning("    %d. %s", i - 1, d->traces[i]);
				}
			}
		}
	}
}
