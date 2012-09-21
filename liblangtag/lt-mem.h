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

#include <glib.h>

G_BEGIN_DECLS

typedef struct _lt_mem_t		lt_mem_t;

typedef void (* lt_destroy_func_t)	(gpointer data);

struct _lt_mem_t {
	volatile guint  ref_count;
	gsize           size;
	GHashTable     *refs;
	GList          *weak_pointers;
};

gpointer lt_mem_alloc_object       (gsize              size);
gpointer lt_mem_ref                (lt_mem_t          *object);
void     lt_mem_unref              (lt_mem_t          *object);
void     lt_mem_add_ref            (lt_mem_t          *object,
                                    gpointer           p,
                                    lt_destroy_func_t  func);
void     lt_mem_remove_ref         (lt_mem_t          *object,
                                    gpointer           p);
void     lt_mem_delete_ref         (lt_mem_t          *object,
                                    gpointer           p);
void     lt_mem_add_weak_pointer   (lt_mem_t          *object,
                                    gpointer          *p);
void     lt_mem_remove_weak_pointer(lt_mem_t          *object,
                                    gpointer          *p);

/* utility functions */
void lt_mem_gstring_free(GString *string);

G_END_DECLS

#endif /* __LT_MEM_H__ */
