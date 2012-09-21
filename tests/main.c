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
#include <glib-object.h>
#include "main.h"

extern Suite *tester_suite(void);

static GLogFunc old_logger = NULL;
static GError *error = NULL;
G_LOCK_DEFINE_STATIC(err);

/*
 * Private functions
 */
static void
logger(const gchar    *log_domain,
       GLogLevelFlags  log_level,
       const gchar    *message,
       gpointer        user_data)
{
	gchar *prev = NULL;

	G_LOCK (err);

	if (error) {
		prev = g_strdup_printf("\n    %s", error->message);
		g_error_free(error);
		error = NULL;
	}
	g_set_error(&error, TESTER_ERROR, log_level,
		    "%s%s", message,
		    (prev ? prev : ""));
	g_free(prev);

	G_UNLOCK (err);
}

static void
init(int    argc,
     char **argv)
{
	old_logger = g_log_set_default_handler(logger, NULL);
}

static void
fini(void)
{
	if (old_logger)
		g_log_set_default_handler(old_logger, NULL);
}

/*
 * Public functions
 */
GQuark
tester_get_error_quark(void)
{
	GQuark quark = 0;

	if (!quark)
		quark = g_quark_from_static_string("tester-error");

	return quark;
}

gchar *
tester_pop_error(void)
{
	gchar *retval = NULL;

	if (error) {
		retval = g_strdup(error->message);
		g_clear_error(&error);
	}

	return retval;
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
