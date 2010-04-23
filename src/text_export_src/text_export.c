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

/* global variables */
char *Program_Name;
int Verbose = 0;
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
    const class_t *classinfo;
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
	fprintf(stderr, "Syntax: %s -c rip.config [-v] [-h hostname] [-P port] [-u user] [-p password] database\n", Program_Name);
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
	while ((opt = getopt(argc, argv, "vh:u:p:P:c:")) != -1) {
		switch (opt) {
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
	int i;
	char fname[256];

	class_names = DF_get_class_names();
	for (num_classes = 0; class_names[num_classes]; num_classes++);

	tmp = calloc(num_classes, sizeof(struct class));

	for (i=0; i<num_classes; i++) {
		tmp[i].name = class_names[i];
		tmp[i].classinfo = class_lookup(class_names[i]);

		assert(strlen(tmp[i].name) < 200);
		strcpy(fname, "db.");
		strcat(fname, tmp[i].name);
		tmp[i].fp = fopen(fname, "w+");
		if (tmp[i].fp == NULL) {
			fprintf(stderr, "%s: error opening file \"%s\": %s\n", Program_Name, fname, strerror(errno));
			exit(1);
		}
		num_files++;

        strcpy(fname, "dummy.");
        strcat(fname, tmp[i].name);
        tmp[i].dummy_fp = fopen(fname, "w+");
        if (tmp[i].dummy_fp == NULL) {
            fprintf(stderr, "%s: error opening file \"%s\": %s\n", Program_Name, fname, strerror(errno));
            exit(1);
        }
        num_files++;
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

    /* we are deliberately not using last.object_type field because we don't trust it */
	for (i=0; i<num_classes; i++) {
        if (strcmp(c[i].name, object) == 0) {
			*p = ':';
			dump_fputs(object, c[i].fp);
			dump_putc('\n', c[i].fp);

            char *dummy = PM_dummify_object(object);
            if (dummy) {
                dump_fputs(dummy, c[i].dummy_fp);
                dump_putc('\n', c[i].dummy_fp);
                free(dummy);
            } else {
                //fprintf(stderr, "%s: The following object failed to dummify:\n\n%s\n", Program_Name, object);
                /*UT_alarm_operator("ERROR: text_export failed objects", "The following object failed to dummify:\n\n%s\n", Program_Name, object); */

                /* omit object if dummification failed
                 * the idea is that we should still produce the dumps, it's critical, but at the same time,
                 * spam the operator so that the object will get fixed at some point
                 * agoston, 2008-01-29 */
                /* exit(1) */
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
	SQ_connection_t *sql = NULL, *sql2 = NULL;
	SQ_result_set_t *rs = NULL, *rs2 = NULL;
	SQ_row_t *row = NULL, *row2 = NULL;
	int num_classes;
	struct class *classes;
	int num_object = 0, num_queries = 0;
	time_t start, end;
	const struct tm *now;
	char buf[1024];
	double runtime;
	int i, h, m, s;
	long first_serial, last_serial;
	FILE *serial_file;
	LG_context_t *ctx, *null_ctx;

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
		printf(" rip.config: %s\n", rip_conf);
	}

	/* init logging. we also create a null_ctx as we don't need log output on stdout from the
	 * dummify_object() call */
	ctx = LG_ctx_new();
	LG_ctx_add_appender(ctx, LG_app_get_file_info_dump(stdout));

	null_ctx = LG_ctx_new();

	/* initialise required modules */
	SQ_init(ctx);
	UT_init(ctx);
	ca_init(rip_conf);
	PM_init(null_ctx);

	/* turn off stdout buffering */
	setbuf(stdout, NULL);

	/* connect to the server */
	if (Verbose) {
		printf("\n");
		printf("Connecting to server...\n");
	}

	/* Open 2 connections: one for going through the last table, and one to query serial_ids
	 * This is required as joining serials table would result in a shared-mode row-level lock on serials,
	 * which is also locked by a LOCK TABLES command by dbupdate, thus stalling dbupdate processes for the
	 * period of the dump, which is unacceptable - agoston, 2008-02-18 */
	if (SQ_try_connection(&sql, db.hostname, db.port, db.database, db.user, db.password)) {
		fprintf(stderr, "%s: error connecting to database; %s\n", Program_Name, SQ_error(sql));
		exit(1);
	}

	if (SQ_try_connection(&sql2, db.hostname, db.port, db.database, db.user, db.password)) {
		fprintf(stderr, "%s: error connecting to database; %s\n", Program_Name, SQ_error(sql));
		exit(1);
	}

	if (Verbose) {
		printf("Connected.\n");
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
		strftime(buf, sizeof(buf), "Dump starting... %Y-%m-%d %H:%M:%S %Z", now);
		puts(buf);
	}

	/* Collect the serials we are going to dump - at this point, we close today's serials, any new
	 * update will make its way into tomorrow's dump */
	PM_get_minmax_serial(sql, &first_serial, &last_serial);

    if (Verbose) {
		printf("Min serial: %ld\nMax serial:%ld\n\n", first_serial, last_serial);
		printf("Dumping objects (each . marks 10000 object dumped):\n");
    }

    /* first emit placeholder objects */
    for (i = 0; i < num_classes; i++)
    {
        if (classes[i].classinfo->dummify_type == DUMMIFY_PLACEHOLDER)
        {
            sprintf(buf, "SELECT object FROM last WHERE pkey = '%s'", classes[i].classinfo->dummify_singleton);
            if (SQ_execute_query(sql, buf, &rs) != 0)
            {
                fprintf(stderr, "%s: error executing placeholder query; %s\n", Program_Name, SQ_error(sql));
                exit(1);
            }

            /* decide if we want to skip this object */
            if ((row = SQ_row_next(rs)))
            {
                char *ph_obj;
                if (!(ph_obj = SQ_get_column_string_nocopy(rs, row, 0)))
                {
                    fprintf(stderr, "%s: error getting placeholder object; %s\n", Program_Name, SQ_error(sql));
                    exit(1);
                }


                dump_fputs(ph_obj, classes[i].dummy_fp);
                dump_putc('\n', classes[i].dummy_fp);
            }

            if (rs) SQ_free_result(rs);
            rs = NULL;
        }
    }

    /* roll back to free all locks */
    if (SQ_execute_query(sql, "ROLLBACK", NULL)) {
        fprintf(stderr, "%s: error executing ROLLBACK; %s\n", Program_Name, SQ_error(sql));
        exit(1);
    }

    /* Outer query, crawling through the last table */
	sprintf(buf, "SELECT last.object, last.object_id, last.sequence_id FROM last "
		"WHERE last.thread_id=0 AND last.object_type<>100 AND last.object<>\"\"");

	if (SQ_execute_query_nostore(sql, buf, &rs) != 0) {
		fprintf(stderr, "%s: error with query; %s\n", Program_Name, SQ_error(sql));
		exit(1);
	}

	/* For each row, look up entry in serials table using the second sql connection,
	 * and do a ROLLBACK to release all shared-mode locks put on by the queries */
	while ((row = SQ_row_next(rs)) != NULL) {
		long object_id, sequence_id;
		char *object;
		int skip = 1;

		if (!(object = SQ_get_column_string_nocopy(rs, row, 0))) {
			fprintf(stderr, "Error: NULL object returned (#%d): %s\n", num_object, SQ_error(sql));
			exit(1);
		}

		if (SQ_get_column_int(rs, row, 1, &object_id) || SQ_get_column_int(rs, row, 2, &sequence_id)) {
			fprintf(stderr, "Error: Couldn't read object_id and sequence_id (#%d): %s\n", num_object, SQ_error(sql));
			exit(1);
		}

		/* lookup serial_id */
		sprintf(buf, "SELECT serial_id FROM serials WHERE object_id = %ld AND sequence_id = %ld", object_id, sequence_id);
		if (SQ_execute_query(sql2, buf, &rs2) != 0) {
			fprintf(stderr, "%s: error executing internal query; %s\n", Program_Name, SQ_error(sql));
			exit(1);
		}

		/* decide if we want to skip this object */
		if ((row2 = SQ_row_next(rs2))) {
			long serial_id;
			if (SQ_get_column_int(rs2, row2, 0, &serial_id)) {
				fprintf(stderr, "%s: error getting serial_id; %s\n", Program_Name, SQ_error(sql));
				exit(1);
			}

			skip = (serial_id > last_serial);

		} else {	/* zero serials - buggy DB, emit object */
			skip = 0;
		}

		if (rs2) SQ_free_result(rs2);
		rs2 = NULL;

		/* emit object if no skip */
		if (!skip) {
			output_object(object, classes, num_classes);
			num_object++;

			if (Verbose && !(num_object % 10000)) {
				printf(".");
			}
		}

		/* rollback */
		num_queries++;
		if (num_queries > 100) {
			num_queries = 0;

			/* release locks */
			if (SQ_execute_query(sql2, "ROLLBACK", NULL)) {
				fprintf(stderr, "%s: error executing ROLLBACK; %s\n", Program_Name, SQ_error(sql2));
				exit(1);
			}
		}

	}

	if (rs) SQ_free_result(rs);
	rs = NULL;

	/* close SQL connections, releasing locks on server side */
	SQ_close_connection(sql);
	SQ_close_connection(sql2);

    /* close the files */
    for (i = 0; i < num_classes; i++) {
        if (fclose(classes[i].fp) || fclose(classes[i].dummy_fp)) {
				perror("fclose");
				exit(1);
        }
    }

    if (Verbose) printf("\n\n");

	/* create last serial file */
	serial_file = fopen("CURRENTSERIAL", "w+");
	if (!serial_file) {
		perror("Error opening CURRENTSERIAL");
		exit(1);
	}

	fprintf(serial_file, "%ld\n", last_serial);
	fclose(serial_file);

	if (Verbose) {
		printf("Dumped last serial value (%ld)\n", last_serial);
	}

	/* close up shop */
	time(&end);

	/* output final results, if any */
	if (Verbose) {
		now = localtime(&end);
		strftime(buf, sizeof(buf), "Dump finished... %Y-%m-%d %H:%M:%S %Z", now);
		puts(buf);
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

