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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#include "rip.h"
#include "class.h"

/* defined in PM module */
extern const int PM_PRIVATE_OBJECT_TYPES[5];
                                  
/* global variables */
char *Program_Name;
int Verbose = 0;
int Use_Transactions = 0;
long long Amt_Dumped = 0;
int num_files = 0;

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
	FILE *dummy_fp;
};

/* output functions which track amount of data written */
inline void dump_putc(char c, FILE* stream) {
	Amt_Dumped++;
	putc(c, stream);
}

inline void dump_fputs(const char *s, FILE *stream) {
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
	fprintf(stderr, "Syntax: %s -c rip.config [-t] [-v] [-h hostname] [-P port] [-u user] [-p password] database\n", Program_Name);
}

/* exits on error */
void parse_options(int argc, char **argv, struct database_info *db, char **rip_conf) {
	int opt;

	/* defaults */
	db->hostname = "localhost";
	db->port = 0;
	db->user = "";
	db->password = "";

	/* parse command line arguments with the ever-handy getopt() */
	while ((opt = getopt(argc, argv, "tvh:u:p:P:c:")) != -1) {
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
			case 'c':
				*rip_conf = optarg;
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
	int i,j;
	char fname[256];

	class_names = DF_get_class_names();
	for (num_classes = 0; class_names[num_classes] != NULL; num_classes++); 

	tmp = calloc(num_classes, sizeof(struct class));

	for (i=0; i<num_classes; i++) {
		tmp[i].name = class_names[i];

		assert(strlen(tmp[i].name) < 200);
		strcpy(fname, "db.");
		strcat(fname, tmp[i].name);
		tmp[i].fp = fopen(fname, "w+");
		if (tmp[i].fp == NULL) {
			fprintf(stderr, "%s: error opening file \"%s\": %s\n", Program_Name, fname, strerror(errno));
			exit(1);
		}
		num_files++;

		/* this is kinda crap, but it's init, so performance doesn't matter,
		 * and we should not rely on the order DF_get_class_names() returns class names
		 * agoston, 2008-01-28 */
		const class_t *actclass = class_lookup(class_names[i]);
		for (j = 0; j < sizeof(PM_PRIVATE_OBJECT_TYPES)/sizeof(*PM_PRIVATE_OBJECT_TYPES); j++) {
			if (PM_PRIVATE_OBJECT_TYPES[j] == actclass->id) {
				strcpy(fname, "dummy.");
				strcat(fname, tmp[i].name);
				tmp[i].dummy_fp = fopen(fname, "w+");
				if (tmp[i].dummy_fp == NULL) {
					fprintf(stderr, "%s: error opening file \"%s\": %s\n", Program_Name, fname, strerror(errno));
					exit(1);
				}
				num_files++;
			}
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
		fprintf(stderr, "%s: format error in database; no colon in object, \"%s\"\n", Program_Name, object);
		exit(1);
	}
	*p = '\0';

	for (i=0; i<num_classes; i++) {
		if (strcmp(c[i].name, object) == 0) {
			*p = ':';
			dump_fputs(object, c[i].fp);
			dump_putc('\n', c[i].fp);
			if (c[i].dummy_fp) {
				char *dummy = PM_dummify_object(object);
				if (dummy) {
					dump_fputs(dummy, c[i].dummy_fp);
					dump_putc('\n', c[i].dummy_fp);
					free(dummy);
				} else {
					fprintf(stderr, "%s: The following object failed to dummify:\n\n%s\n", Program_Name, object);
					UT_alarm_operator("ERROR: text_export failed objects", "The following object failed to dummify:\n\n%s\n", Program_Name, object);
					/* omit object if dummification failed
					 * the idea is that we should still produce the dumps, it's critical, but at the same time,
					 * spam the operator so that the object will get fixed at some point
					 * agoston, 2008-01-29 */
					/* exit(1) */
				}
			}
			return;
		}
	}

	fprintf(stderr, "%s: unknown class of object found, \"%s\"\n", Program_Name, object);
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
	char *rip_conf = NULL;
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

	/* record our program's name for any future use */
	get_program_name(argc, argv);

	/* parse our command line */
	parse_options(argc, argv, &db, &rip_conf);
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
		printf("use transactions: %s\n", Use_Transactions ? "yes" : "no");
		printf(" rip.config: %s\n", rip_conf);
	}
	
	/* initialise required modules: LG, SQ, UT, CA, PM */
	LG_context_t *ctx;
	ctx = LG_ctx_new();
	LG_ctx_add_appender(ctx, LG_app_get_file_info_dump(stdout));
	SQ_init(ctx);
	UT_init(ctx);
	ca_init(rip_conf);
	PM_init(ctx);

	/* turn off stdout buffering */
	setbuf(stdout, NULL);

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
		printf("Created %d output files\n", num_files);
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

