/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * main.h
 * Copyright (C) 2007-2012 Akira TAGOH
 * Copyright (C) 2008-2012 Red Hat, Inc. All rights reserved.
 * 
 * Authors:
 *   Akira TAGOH  <tagoh@redhat.com>
 * 
 * You may distribute under the terms of either the GNU
 * Lesser General Public License or the Mozilla Public
 * License, as specified in the README file.
 */
#ifndef __TEST_MAIN_H__
#define __TEST_MAIN_H__

#include <check.h>
#include <liblangtag/langtag.h>

LT_BEGIN_DECLS

#define TESTER_ERROR	tester_get_error_quark()
#define TDEF(fn)	START_TEST (test_ ## fn)
#define TEND		END_TEST
#define T(fn)		tcase_add_test(tc, test_ ## fn)
#define TNUL(obj)	fail_unless((obj) != NULL, "Failed to create an object")


void   setup           (void);
void   teardown        (void);
Suite *tester_suite    (void);
void   tester_pop_error(void);

LT_END_DECLS

#endif /* __TEST_MAIN_H__ */
