/***************************************
  $Revision: 1.1 $

  Recreate.  recreate.c - whois DB recreation via pseudo mirror server.

  Status: NOT REVIEWED, TESTED, COMPLETE

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
#include <regex.h>
#include <rip.h>
#include "miniconf.h"
#include "dbsupport.h"
#include "aconf.h"

int  response_sock;
long end_serial = 0;
long serial_max;
long serial_min;
long requested_max;
long requested_min;
char source_name[100];
long RIPE_serial_min;
long updates_sent;

/*
  process_arguments: proccesses command-line arguments.
*/
void process_arguments(int argv, char** argc) {
  if (argv != 2 ) {
    printf("Usage: %s <serial>\n", argc[0]);
    exit(1);
  }

  end_serial = atol(argc[1]); //assume is number
  
  if (end_serial == 0) {
    printf("Usage: %s <serial>\n", argc[0]);
    exit(1);
  }
  
}

void put_delete_before(char* pkey, long timestamp, char* buffer) {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  char             query[200];
  char             ppkey[200];
  long             ts;
  char*            object;

  prepare_string_attribute(pkey,
			   ppkey);
  sprintf(query,
	  "SELECT max(timestamp) FROM archive "
	  " WHERE timestamp<%ld AND pkey='%s'",
	  timestamp, ppkey);
  SQ_execute_query(archive_conn, query, &rs);
  row = SQ_row_next(rs);
  if (row==NULL) { //very bad
    printf ("Severe error with executing: %s\n", query);
    exit(1);
  }
  SQ_get_column_int(rs, row, 0, &ts);
  SQ_free_result(rs);

  sprintf(query,
	  "SELECT object FROM archive WHERE timestamp=%ld "
	  "   AND pkey='%s' AND operation=1",
	  timestamp, pkey);
  SQ_execute_query(archive_conn, query, &rs);
  row = SQ_row_next(rs);
  if (row==NULL) { //bummer, it has to be on last of RIP DB
    SQ_free_result(rs);
    sprintf(query,
	    "SELECT object FROM last WHERE pkey='%s'",
	    pkey);
    SQ_execute_query(RIPE_conn, query, &rs);
    row = SQ_row_next(rs);
    if (row==NULL) { //very bad
      printf ("Severe error with executing: %s\n", query);
      exit(1);
    }
  }
  object = SQ_get_column_string_nocopy(rs, row, 0);
  sprintf(buffer, "DEL\n\n%s\n\n", object);
  SK_write(response_sock, buffer, strlen(buffer), NULL, NULL);
  SQ_free_result(rs);
}

/*
  get_serial_interval: gets the upper and lower bounds of available serials.
*/
void get_serial_interval() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  char             query[200];

  sprintf(query,
	  "SELECT min(serial_id), max(serial_id) "
	  "  FROM archive "
	  " WHERE serial_id<=%ld ",
	  end_serial);
  SQ_execute_query(archive_conn, query, &rs);
  row = SQ_row_next(rs);
  SQ_get_column_int(rs, row, 0, &serial_min);
  SQ_get_column_int(rs, row, 1, &serial_max);
  SQ_free_result(rs);
}

/*
  generate_load_data: generates and send data to be recreated.
*/
void generate_load_data() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  char             query[200];
  char*            object;
  long             operation;
  char*            pkey;
  long             timestamp;
  char             sbuffer[1000000]; // ridiculous but more bug proof?


  updates_sent = 0;
  sprintf(sbuffer,
	  "%%START Version: 2 %s %ld-%ld\n\n",
	  source_name, requested_min, serial_max);
  SK_write(response_sock, sbuffer, strlen(sbuffer), NULL, NULL);
  sprintf(query,
	  "  SELECT object, operation, pkey, timestamp "
	  "    FROM archive "
	  "   WHERE serial_id<=%ld AND serial_id>=%ld "
	  "     AND serial_id IS NOT NULL "
	  "ORDER BY timestamp ASC, serial_id ASC ",
	  // order because of special deletes (op=3)
	  end_serial, requested_min);
  SQ_execute_query(archive_conn, query, &rs);
  while ((row = SQ_row_next(rs)) != NULL) {
    updates_sent++;
    object = SQ_get_column_string_nocopy(rs, row, 0);
    SQ_get_column_int(rs, row, 1, &operation);
    switch (operation) {
    case 1:
      sprintf(sbuffer, "ADD\n\n%s\n\n", object);
      SK_write(response_sock, sbuffer, strlen(sbuffer), NULL, NULL);
      break;
    case 2:
      sprintf(sbuffer, "DEL\n\n%s\n\n", object);
      SK_write(response_sock, sbuffer, strlen(sbuffer), NULL, NULL);
      break;
    case 3:
      pkey = SQ_get_column_string_nocopy(rs, row, 2);
      SQ_get_column_int(rs, row, 3, &timestamp);
      put_delete_before(pkey, timestamp, sbuffer);
      sprintf(sbuffer, "ADD\n\n%s\n\n", object);
      SK_write(response_sock, sbuffer, strlen(sbuffer), NULL, NULL);
      break;
    }

  }
  SQ_free_result(rs);

  sprintf(sbuffer,"%%END %s\n\n", source_name);
  SK_write(response_sock, sbuffer, strlen(sbuffer), NULL, NULL);
}

/*
  serve_TCP: creates a socket and accepts a connection.
*/
void serve_TCP() {
  int sock;

  sock = SK_getsock(SOCK_STREAM, 24444, 128, INADDR_ANY);
  //printf("Accepting connections\n");
  response_sock = SK_accept_connection(sock);
}

/*
  fake_sources: fakes a source

  Irrelevant?
*/
void fake_sources() {
  char line[100];
  sprintf(line, "RIPE:2:Y:%ld-%ld\n\n", serial_min, serial_max);
  SK_write(response_sock, line, strlen(line), NULL, NULL);
}

/*
  get_operation: gets the operation required by the client.

  Also puts the source name in a global variable if -g requested.
*/
int get_operation() {
  char       line[201];
  regex_t    reg;
  regmatch_t match[3];

  bzero(line, 201);
  if (SK_cd_gets(&response_sock, line, 200)>=0) {
    if (strncmp(line,"-q sources", 10)==0) {
      return 0;
    }
    else {
      regcomp(&reg,"-g (.+):2:([0-9]+)-", REG_EXTENDED);
      if (regexec(&reg, line, 3, match, 0) != 0) {
	printf("match failed: %s\n", line);
	exit(1);
      }
      //printf("match OK: %s\n", line);
      bzero(source_name, 99);
      strncpy(source_name,
	      line + match[1].rm_so,
	      match[1].rm_eo - match[1].rm_so);
      requested_min = atol(line+match[2].rm_so);
      //printf("%sXX\n", line+match[2].rm_so);
      return 1;
    }
  }
  else {
    //printf("Input from client ended\n");
    exit(1);
  }
}

/*
  get_minimum_RIPE_serial: returns the minimum serial of recreate

  The minimum serial of recreate is the initial max + 1.
  Returns in a global variable.
*/
void get_minimum_RIPE_serial() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  long             result;

  SQ_execute_query(RIPE_conn,
		   "SELECT max(serial_id) FROM serials",
		   &rs);

  row = SQ_row_next(rs);

  if (SQ_get_column_int(rs, row, 0, &result) == 0) {
    RIPE_serial_min = result + 1;
  }
  else {
    RIPE_serial_min = 1;
  }

  SQ_free_result(rs);
}

/*
  wait_for_whoisd_completion: as named

  As we know the serial when we started and the number of updates sent
  we know which number should be the max on serials.
  We wait for it.
*/
void wait_for_whoisd_completion() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  long             current_serial=0;

  while (current_serial<RIPE_serial_min+updates_sent) {
    SQ_execute_query(RIPE_conn,
		     "SELECT max(serial_id) FROM serials",
		     &rs);
    
    row = SQ_row_next(rs);
    
    if (SQ_get_column_int(rs, row, 0, &current_serial) != 0) {
      current_serial = 0;
    }
    
    SQ_free_result(rs);
    sleep(1); //ALWAYS wait
  }
}

/*
  process_timestamp_add: patches the timestmap of an add
*/
void process_timestamp_add(SQ_result_set_t* rs,
			   SQ_row_t*        row,
			   long             timestamp) {
  long atlast;
  long seq_id;
  long object_id;
  char upd[200];

  SQ_get_column_int(rs, row, 0, &object_id);
  SQ_get_column_int(rs, row, 1, &seq_id);
  SQ_get_column_int(rs, row, 2, &atlast);

  if (atlast) {
    sprintf(upd,
	    "UPDATE last SET timestamp=%ld WHERE object_id=%ld",
	    timestamp, object_id);
  }
  else {
    sprintf(upd,
	    "UPDATE history "
	    "   SET timestamp=%ld "
	    " WHERE object_id=%ld AND sequence_id=%ld",
	    timestamp, object_id, seq_id);
  }
  SQ_execute_query(RIPE_conn, upd, NULL);
}

/*
  process_timestamp_delete: patches the timestamp of a delete
*/
void process_timestamp_delete(SQ_result_set_t* rs,
			      SQ_row_t*        row,
			      long             timestamp) {
  long object_id;
  char upd[200];

  SQ_get_column_int(rs, row, 0, &object_id);

  sprintf(upd,
	  "UPDATE last "
	  "   SET timestamp=%ld "
	  " WHERE object_id=%ld AND object=''",
	  timestamp, object_id);
  SQ_execute_query(RIPE_conn, upd, NULL);
}

/*
  correct_timestamps:
*/
void correct_timestamps() {
  SQ_result_set_t* RIPE_rs;
  SQ_row_t*        RIPE_row;
  char             RIPE_query[200];
  SQ_result_set_t* archive_rs;
  SQ_row_t*        archive_row;
  char             archive_query[200];
  long             operation;
  long             timestamp;

  sprintf(RIPE_query,
	  "  SELECT object_id, sequence_id, atlast "
	  "    FROM serials "
	  "   WHERE serial_id>=%ld "
	  "ORDER BY serial_id ",
	  RIPE_serial_min);
  SQ_execute_query(RIPE_conn, RIPE_query, &RIPE_rs);

  sprintf(archive_query,
	  "  SELECT timestamp, operation "
	  "    FROM archive "
	  "   WHERE serial_id<=%ld AND serial_id>=%ld "
	  "     AND serial_id IS NOT NULL "
	  "ORDER BY timestamp ASC, serial_id ASC ",
	  end_serial, requested_min);
  SQ_execute_query(archive_conn, archive_query, &archive_rs);

  while((archive_row = SQ_row_next(archive_rs)) != NULL) {
    SQ_get_column_int(archive_rs, archive_row, 0, &timestamp);
    SQ_get_column_int(archive_rs, archive_row, 1, &operation);
    if (operation==3) {
      RIPE_row = SQ_row_next(RIPE_rs);
      if (RIPE_row == NULL) {
	printf("serials are incomplete irt archive\n");
	exit(1);
      } 
      process_timestamp_delete(RIPE_rs, RIPE_row, timestamp);
      operation = 1;
    }
    RIPE_row = SQ_row_next(RIPE_rs);
    if (RIPE_row == NULL) {
      printf("serials are incomplete irt archive\n");
      exit(1);
    }
    if (operation==1) {
      process_timestamp_add(RIPE_rs, RIPE_row, timestamp);
    }
    else {
      process_timestamp_delete(RIPE_rs, RIPE_row, timestamp);
    }

  }
  RIPE_row = SQ_row_next(RIPE_rs);
  if (RIPE_row != NULL) {
    printf("archive is incomplete irt serials\n");
    exit(1);
  }

  SQ_free_result(RIPE_rs);
  SQ_free_result(archive_rs);
}

/*
  main: recreate entry point.

  Determine biggest serial on process start (on RIPE DB)
  Get serials available (on archive DB)
  Wait for TCP connection
  while(1)
   switch (whoisd request)
     0: fake_sources (-g & -q)
     1: generate_load_data
        wait for whois completion
        patch timestamps
*/
int main (int argv, char** argc) {
  process_arguments(argv, argc);
  read_configuration();
  get_db_connections();

  get_minimum_RIPE_serial();
  get_serial_interval();
  serve_TCP();
  while (1) {
    switch (get_operation()) {
    case 0:
      fake_sources();
      break;
    case 1:
      generate_load_data();
      wait_for_whoisd_completion();
      correct_timestamps();
      return 0;
    }
  }

  //close_dbs();

  return 0;
}
