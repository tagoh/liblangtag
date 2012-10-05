/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * lt-mem.h
 * Copyright (C) 2011-2012 Akira TAGOH
 * 
 * Authors:
 *   Akira TAGOH  <akira@tagoh.org>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __LT_MEM_H__
#define __LT_MEM_H__

#include "lt-macros.h"
#include "lt-list.h"

LT_BEGIN_DECLS

typedef struct _lt_mem_t		lt_mem_t;

struct _lt_mem_t {
	volatile unsigned int  ref_count;
	size_t                 size;
	GHashTable            *refs;
	lt_list_t             *weak_pointers;
};

lt_pointer_t lt_mem_alloc_object       (size_t             size);
lt_pointer_t lt_mem_ref                (lt_mem_t          *object);
void         lt_mem_unref              (lt_mem_t          *object);
void         lt_mem_add_ref            (lt_mem_t          *object,
                                        lt_pointer_t       p,
                                        lt_destroy_func_t  func);
void         lt_mem_remove_ref         (lt_mem_t          *object,
                                        lt_pointer_t       p);
void         lt_mem_delete_ref         (lt_mem_t          *object,
                                        lt_pointer_t       p);
void         lt_mem_add_weak_pointer   (lt_mem_t          *object,
                                        lt_pointer_t      *p);
void         lt_mem_remove_weak_pointer(lt_mem_t          *object,
                                        lt_pointer_t      *p);

LT_END_DECLS

#endif /* __LT_MEM_H__ */
