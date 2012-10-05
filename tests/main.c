/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * main.c
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <unistd.h>
#include <check.h>
#include "langtag.h"
#include "lt-messages.h"
#include "main.h"

extern Suite *tester_suite(void);

static lt_message_func_t old_logger = NULL;
static lt_error_t *error = NULL;

/*
 * Private functions
 */
static void
logger(lt_message_type_t      type,
       lt_message_flags_t     flags,
       lt_message_category_t  category,
       const char            *message,
       lt_pointer_t           user_data)
{
	lt_error_set(&error, type, message);
}

static void
init(int    argc,
     char **argv)
{
	old_logger = lt_message_set_default_handler(logger, NULL);
}

static void
fini(void)
{
	tester_pop_error();
	if (old_logger)
		lt_message_set_default_handler(old_logger, NULL);
}

/*
 * Public functions
 */
void
tester_pop_error(void)
{
	if (lt_error_is_set(error, LT_ERR_ANY)) {
		lt_error_print(error, LT_ERR_ANY);
		lt_error_clear(error);
	}
}

int
main(int    argc,
     char **argv)
{
	int number_failed;
	Suite *s = tester_suite();
	SRunner *sr = srunner_create(s);

	init(argc, argv);

	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	fini();

	return (number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
