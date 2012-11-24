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

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif
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

/**
 * lt_error_ref:
 * @error: a #lt_error_t
 *
 * Inscreases the reference count of @error.
 *
 * Returns: (transfer none): the same @error object.
 */
lt_error_t *
lt_error_ref(lt_error_t *error)
{
	lt_return_val_if_fail (error != NULL, NULL);

	return lt_mem_ref(&error->parent);
}

/**
 * lt_error_unref:
 * @error: a #lt_error_t
 *
 * Decreases the reference count of @error. when its reference count
 * drops to 0, the object is finalized (i.e. its memory is freed).
 */
void
lt_error_unref(lt_error_t *error)
{
	if (error)
		lt_mem_unref(&error->parent);
}

/**
 * lt_error_set:
 * @error: a return location for a #lt_error_t
 * @type: a #lt_error_type_t
 * @message: the string format to output the error messages
 * @...: the parameters to insert into the format string
 *
 * Sets the error into @error according to the given parameters.
 *
 * Returns: an instance of #lt_error_t
 */
lt_error_t *
lt_error_set(lt_error_t      **error,
	     lt_error_type_t   type,
	     const char       *message,
	     ...)
{
	va_list ap;
#if HAVE_BACKTRACE
	void *traces[1024];
#endif
	lt_error_data_t *d = lt_mem_alloc_object(sizeof (lt_error_data_t));
	int size = 0;
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

#if HAVE_BACKTRACE
	size = backtrace(traces, 1024);
	if (size > 0)
		d->traces = backtrace_symbols(traces, size);
#else
	d->traces = NULL;
#endif
	d->stack_size = size;

	lt_mem_add_ref(&d->parent, d->message, free);
	if (d->traces)
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

/**
 * lt_error_clear:
 * @error: a #lt_error_t
 *
 * Clean up all of the errors in @error.
 */
void
lt_error_clear(lt_error_t *error)
{
	if (error) {
		lt_mem_delete_ref(&error->parent, error->data);
		error->data = NULL;
	}
}

/**
 * lt_error_is_set:
 * @error: a #lt_error_t
 * @type: a #lt_error_type_t
 *
 * Checks if @error contains @type of errors. if #LT_ERR_ANY is set to @type,
 * all the types of the errors are targeted. otherwise the result is filtered
 * out by @type.
 *
 * Returns: %TRUE if any, otherwise %FALSE
 */
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

/**
 * lt_error_print:
 * @error: a #lt_error_t
 * @type: a #lt_error_type_t
 *
 * Output the error messages in @error according to @type.
 */
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
