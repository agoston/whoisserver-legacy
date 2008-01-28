/* text_export - output RIP database into text files of RPSL objects */

/*
 Copyright (c) 2000                       RIPE NCC


 All Rights Reserved

 Permission to use, copy, modify, and distribute this software and its
 documentation for any purpose and without fee is hereby granted,
 provided that the above copyright notice appear in all copies and that
 both that copyright notice and this permission notice appear in
 supporting documentation, and that the name of the author not be
 used in advertising or publicity pertaining to distribution of the
 software without specific, written prior permission.

 THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
 AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: text_export.c,v 1.13 2004/04/14 15:19:15 engin Exp $ */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#include "rip.h"
#include "mysql_driver.h"
#include "defs.h"
#include "memwrap.h"
#include "query_instructions.h"
#include "lg.h"

/* global variables */
char *Program_Name;
int Verbose = 0;
int Use_Transactions = 0;
long long Amt_Dumped = 0;
int Dump_Short = 0;

/* structure holding information used to connect to SQL server */
struct database_info {
	char *hostname;
	int port;
	char *database;
	char *user;
	char *password;
};

/* structure for class */
struct class {
	char *name;
	FILE *fp;
	char *short_name;
	FILE *short_fp;
};

/* output functions which track amount of data written */
#define dump_putc(c,stream)  (Amt_Dumped++, putc((c),(stream)))
void dump_fputs(const char *s, FILE *stream) {
	Amt_Dumped += fputs(s, stream);
}

/* extracts the program name from command-line arguments */
void get_program_name(int argc, char **argv) {
	char *p;

	if (argc <= 0) {
		fprintf(stderr, "ERROR: unable to determine program name\n");
		exit(1);
	}

	p = strrchr(argv[0], '/');
	if (p == NULL) {
		Program_Name = argv[0];
	} else {
		Program_Name = p+1;
	}
}

void syntax() {
	fprintf(stderr,
	"Syntax: %s [-t] [-v] [-h hostname] [-P port] [-u user] [-p password] [-s]"
	"database\n", Program_Name);
}

/* exits on error */
void parse_options(int argc, char **argv, struct database_info *db, int *dump_short) {
	int opt;

	/* defaults */
	db->hostname = "localhost";
	db->port = 0;
	db->user = "";
	db->password = "";
	*dump_short = 0;

	/* parse command line arguments with the ever-handy getopt() */
	while ((opt = getopt(argc, argv, "tvh:u:p:P:s")) != -1) {
		switch (opt) {
			case 't':
				Use_Transactions = 1;
				break;
			case 'v':
				Verbose++;
				break;
			case 'h':
				db->hostname = optarg;
				break;
			case 'P':
				db->port = atoi(optarg);
				break;
			case 'u':
				db->user = optarg;
				break;
			case 'p':
				db->password = optarg;
				break;
			case 's':
				*dump_short = 1;
				break;
			case '?':
				syntax();
				exit(1);
		}
	}

	/* check remaining argument count */
	argc -= optind;
	argv += optind;
	if (argc != 1) {
		syntax();
		exit(1);
	}

	/* remaining argument should be database name */
	db->database = argv[0];
}

int load_classes(struct class **c) {
	char* const*class_names;
	int num_classes;
	struct class *tmp;
	int i;
	char fname[64];

	class_names = DF_get_class_names();
	num_classes = 0;
	while (class_names[num_classes] != NULL) {
		num_classes++;
	}
	tmp = UT_malloc(sizeof(struct class) * num_classes);

	for (i=0; i<num_classes; i++) {
		tmp[i].name = class_names[i];

		assert(strlen(tmp[i].name) < 60);
		strcpy(fname, "db.");
		strcat(fname, tmp[i].name);
		tmp[i].fp = fopen(fname, "w+");
		if (tmp[i].fp == NULL) {
			fprintf(stderr, "%s: error opening file \"%s\"\n",
			Program_Name, fname);
			exit(1);
		}

		if (Dump_Short) {
			tmp[i].short_name = DF_get_class_code(DF_class_name2type(class_names[i]));
			assert(strlen(tmp[i].short_name) < 60);
			strcpy(fname, "db.");
			strcat(fname, tmp[i].short_name);
			tmp[i].short_fp = fopen(fname, "w+");
			if (tmp[i].short_fp == NULL) {
				fprintf(stderr, "%s: error opening file \"%s\"\n",
				Program_Name, fname);
				exit(1);
			}
		} else {
			tmp[i].short_name = NULL;
			tmp[i].short_fp = NULL;
		}
	}

	*c = tmp;
	return num_classes;
}

/* outputs the object to the appropriate files */
void output_object(char *object, struct class *c, int num_classes) {
	char *p;
	int i;

	p = strchr(object, ':');
	if (p == NULL) {
		fprintf(stderr,
		"%s: format error in database; no colon in object, \"%s\"\n",
		Program_Name, object);
		exit(1);
	}
	*p = '\0';

	for (i=0; i<num_classes; i++) {
		if (strcmp(c[i].name, object) == 0) {
			*p = ':';
			dump_fputs(object, c[i].fp);
			dump_putc('\n', c[i].fp);
			if (Dump_Short) {
				p = QI_fast_output(object);
				dump_fputs(p, c[i].short_fp);
				free(p);
				dump_putc('\n', c[i].short_fp);
			}
			return;
		}
	}

	fprintf(stderr, "%s: unknown class of object found, \"%s\"\n",
	Program_Name, object);
	exit(1);
}

void seperate_time_t(int n, int *hours, int *minutes, int *seconds) {
	*hours = n / (60 * 60);
	n %= (60 * 60);
	*minutes = n / 60;
	n %= 60;
	*seconds = n;
}

/* program entry point */
int main(int argc, char **argv) {
	struct database_info db;
	int ret;
	SQ_connection_t *sql;
	int num_classes;
	struct class *classes;
	SQ_result_set_t *rs;
	SQ_result_set_t *rs_begin; /* only for BEGIN statement  */
	SQ_result_set_t *rs_commit; /* only for COMMIT statement */
	SQ_row_t *row;
	int num_object;
	time_t start, end;
	const struct tm *now;
	char tmp[64];
	double runtime;
	int h, m, s;

	/* initialise SQ to log */
	LG_context_t *ctx;
	ctx = LG_ctx_new();
	LG_ctx_add_appender(ctx, LG_app_get_file_info_dump(stdout));
	SQ_init(ctx);

	/* turn off output buffering (why isn't this the default?) */
	setbuf(stdout, NULL);

	/* record our program's name for any future use */
	get_program_name(argc, argv);

	/* parse our command line */
	parse_options(argc, argv, &db, &Dump_Short);
	if (Verbose) {
		printf("\n");
		printf("Command line options\n");
		printf("   database: %s\n", db.database);
		printf("   hostname: %s\n", db.hostname);
		if (db.port) {
			printf("       port: %d\n", db.port);
		} else {
			printf("       port: <default>\n");
		}
		printf("       user: %s\n", (*db.user == '\0') ? "<default>" : db.user);
		printf("   password: <hidden>\n");
		printf(" dump short: %s\n", Dump_Short ? "yes" : "no");
		printf("use transactions: %s\n", Use_Transactions ? "yes" : "no");
	}

	/* connect to the server */

	if (Verbose) {
		printf("\n");
		printf("Connecting to server...\n");
	}

	ret = SQ_try_connection(&sql, db.hostname, db.port, db.database, db.user, db.password);
	if (!NOERR(ret)) {
		fprintf(stderr, "%s: error connecting to database; %s\n", Program_Name,
		SQ_error(sql));
		exit(1);
	}

	if (Verbose) {
		printf("Connected.\n");
	}

	if (Use_Transactions) {
		if (Verbose) {
			printf("\n");
			printf("BEGINning transaction...\n");
		}
		if (SQ_execute_query(sql, "BEGIN", &rs_begin) != 0) {
			fprintf(stderr, "%s: error with BEGIN transaction; %s\n", Program_Name,
			SQ_error(sql));
			exit(1);

		}
	}

	/* submit our query */
	if (Verbose) {
		printf("\n");
		printf("Submitting query...\n");
	}

	if (SQ_execute_query_nostore(sql, "SELECT object FROM last "
		"WHERE (thread_id=0) AND (object_type<>100) AND (object<>\"\")", &rs) != 0) {
		fprintf(stderr, "%s: error with query; %s\n", Program_Name,
		SQ_error(sql));
		exit(1);
	}

	if (Verbose) {
		printf("Submitted.\n");
	}

	/* initialize our class information (also creates output files) */
	num_classes = load_classes(&classes);

	/* record start time */
	time(&start);

	if (Verbose) {
		printf("\n");
		printf("Loaded %d classes\n", num_classes);
		printf("Created %d output files\n", Dump_Short ? (num_classes * 2) : num_classes);
		printf("\n");
		now = localtime(&start);
		strftime(tmp, sizeof(tmp), "Dump starting... %Y-%m-%d %H:%M:%S %Z", now);
		puts(tmp);
	}

	/* read our MySQL data */
	num_object = 0;
	while ((row = SQ_row_next(rs)) != NULL) {
		char *object = SQ_get_column_string_nocopy(rs, row, 0);
		output_object(object, classes, num_classes);
		num_object++;
	}

	if (Use_Transactions) {
		if (Verbose) {
			printf("\n");
			printf("COMMITting transaction...\n\n");
		}
		if (SQ_execute_query(sql, "COMMIT", &rs_commit) != 0) {
			fprintf(stderr, "%s: error with COMMIT transaction; %s\n", Program_Name,
			SQ_error(sql));
			exit(1);

		}
	}

	/* close up shop */
	time(&end);

	/* output final results, if any */
	if (Verbose) {
		now = localtime(&end);
		strftime(tmp, sizeof(tmp), "Dump finished... %Y-%m-%d %H:%M:%S %Z", now);
		puts(tmp);
		runtime = difftime(end, start);
		if (runtime <= 0.0)
			runtime = 1.0;
		seperate_time_t((int)(runtime + 0.5), &h, &m, &s);
		printf("Wrote %d objects in %d:%02d:%02d\n", num_object, h, m, s);
		printf(" %12.2f objects/second\n", num_object/runtime);
		printf(" %12.2f byte/second\n", Amt_Dumped/runtime);
		printf("\n");
	}

	/* assume that 0 outputs is an error */
	if (num_object == 0) {
		fprintf(stderr, "No objects dumped, exiting with error\n");
		exit(1);
	}

	return 0;
}

