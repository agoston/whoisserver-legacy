/***************************************
  $Revision: 1.1 $

  Backup.  backup.c - whois archive DB backup.

  Status: NOT REVIEWED, NOT TESTED, NOT COMPLETE

  Implementation by: Tiago Antao

  ******************/ /******************
  Copyright (c) 2002               RIPE NCC
 
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
  ***************************************/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <rip.h>
#include "miniconf.h"
#include "dbsupport.h"
#include "aconf.h"

long begin_time = 0;  //begin_time is really begin_time-1

/*
  process_arguments: processes command-line arguments.
*/
void process_arguments(int argv, char** argc) {
  if (argv != 1) {
    printf("Usage: %s \n", argc[0]);
    exit(1);
  }

}

/*
 mark_pkey_position: updates the table of pkey positions

 The mark is the end_time variable.
*/
void mark_pkey_position() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  char             pkey[100];
  char             ins_upd[200];
  char             sel[200];

  //clumsy design courtesy of mysql bugs - should have a distinct
  //and no, I cannot use nostore
  sprintf(sel, 
	  "SELECT pkey FROM archive WHERE timestamp>%d",
	  begin_time);
  SQ_execute_query(archive_conn, sel, &rs);
  //printf("%s\n", sel);

  while ((row = SQ_row_next(rs)) != NULL) {
    prepare_string_attribute(SQ_get_column_string_nocopy(rs, row, 0), pkey);

    sprintf(ins_upd,
	   "INSERT INTO pkey_position(pkey,position) VALUES ('%s', %ld)",
	    pkey, begin_time + 1);
    //printf("%s\n", ins_upd);
    if (SQ_execute_query(archive_conn, ins_upd, NULL) != 0) {
      sprintf(ins_upd,
	     "UPDATE pkey_position SET position=%ld WHERE pkey='%s'",
	     begin_time, pkey);
      SQ_execute_query(archive_conn, ins_upd, NULL);
    }
  }
  SQ_free_result(rs);
}

/*
 export_archive: exports current archiving to dump

 Exports current archive and table with previous references.

 The name of the archive is determined from the timestamps from the
 current archiving operation.

 Also sets timestamp on backup table.

 mysqldump is used and has to be on PATH!
*/
void export_archive() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  char*            cmax;
  char             dump_command[2000];
  char             ts_upd[100];

  SQ_execute_query(archive_conn,
		   "SELECT max(timestamp) from archive", &rs);
  row = SQ_row_next(rs);

  if (row == NULL) {
    printf("Problems accessing the archive2 table");
    exit(1);
  }
  cmax = SQ_get_column_string_nocopy(rs, row, 0);
  if (cmax == NULL) {
    //printf("Its empty\n");
    SQ_free_result(rs);
    exit(0);
  }
  sprintf(dump_command,
	  "mysqldump -q -t -h%s -u%s \"-w timestamp>%ld\" -p%s %s archive > %ld-%s.dump",
	  host2, user2, begin_time, pass2, db2, begin_time, cmax);
  if (system(dump_command) != 0) {
    printf("mysqldump failed on archive\n");
    exit(1);
  }
  sprintf(dump_command,
	  "mysqldump -t -h%s -u%s -p%s %s pkey_position > %ld-%s+pos.dump",
	  host2, user2, pass2, db2, begin_time, cmax);
  if (system(dump_command) != 0) {
    printf("mysqldump failed on pkey_position\n");
    exit(1);
  }

  sprintf(ts_upd, "INSERT INTO dump VALUES (%s)", cmax);
  if (SQ_execute_query(archive_conn, ts_upd, NULL) != 0) {
    printf("Problems inserting new dump timestamp\n");
    exit(1);
  }
  sprintf(ts_upd, "DELETE FROM dump WHERE timestamp <%s", cmax);
  if (SQ_execute_query(archive_conn, ts_upd, NULL) != 0) {
    printf("Problems deleting old dump timestamp\n");
    exit(1);
  }

  SQ_free_result(rs);
}

void get_begin_time() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  char             select_query[100];

  sprintf(select_query, "SELECT max(timestamp) FROM dump");

  //printf("%s\n", select_query);
  SQ_execute_query(archive_conn, select_query, &rs);

  row = SQ_row_next(rs);
  if (row == NULL) {
    printf("Could not get timestamp\n");
    exit(1);
  }

  SQ_get_column_int(rs, row, 0, &begin_time);
  if (begin_time <0) {
    begin_time=0;
  }
}


/*
  main: Dump entry point

  This function is reasonably self-commenting.
  Just note that pk_position functions generate a table of
  previous references of pkeys.
*/
int main (int argv, char** argc) {
  process_arguments(argv, argc);
  read_configuration();
  get_db_connections();

  get_begin_time();
  mark_pkey_position();
  export_archive();
  return 0;
}
