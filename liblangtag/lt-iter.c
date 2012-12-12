/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-iter.c
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

#include <stdlib.h>
#include "lt-messages.h"
#include "lt-iter-private.h"

#define MAGIC_CODE	0xB1C023FF

/*< private >*/

/*< protected >*/
void
lt_iter_tmpl_init(lt_iter_tmpl_t *tmpl)
{
	lt_return_if_fail (tmpl != NULL);

	tmpl->magic_code = MAGIC_CODE;
}

/*< public >*/
lt_iter_t *
lt_iter_ref(lt_iter_t *iter)
{
	lt_iter_tmpl_t *tmpl;

	lt_return_val_if_fail (iter != NULL, NULL);

	tmpl = iter->target;

	return lt_mem_ref(&tmpl->parent);
}

void
lt_iter_unref(lt_iter_t *iter)
{
	lt_iter_tmpl_t *tmpl = iter->target;

	if (iter)
		lt_mem_unref(&tmpl->parent);
}

lt_iter_t *
lt_iter_init(lt_iter_tmpl_t *tmpl)
{
	lt_iter_t *retval;

	lt_return_val_if_fail (tmpl != NULL, NULL);
	lt_return_val_if_fail (tmpl->magic_code == MAGIC_CODE, NULL);
	lt_return_val_if_fail (tmpl->init != NULL, NULL);

	retval = tmpl->init(tmpl);
	if (retval)
		retval->target = tmpl;

	return retval;
}

void
lt_iter_finish(lt_iter_t *iter)
{
	lt_return_if_fail (iter != NULL);
	lt_return_if_fail (iter->target != NULL);

	iter->target->fini(iter);

	free(iter);
}

lt_bool_t
lt_iter_next(lt_iter_t    *iter,
	     lt_pointer_t *key,
	     lt_pointer_t *val)
{
	lt_return_val_if_fail (iter != NULL, FALSE);
	lt_return_val_if_fail (iter->target != NULL, FALSE);
	lt_return_val_if_fail (iter->target->next != NULL, FALSE);

	return iter->target->next(iter, key, val);
}
