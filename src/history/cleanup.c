/***************************************
  $Revision: 1.1 $

  Cleanup.  cleanup.c - whois DB clean up.

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
#include "rip.h"
#include "miniconf.h"
#include "dbsupport.h"
#include "aconf.h"

long max_serial = 0;
long end_time = 0;
int innodb = 0;

void process_arguments(int argv, char** argc) {
  if (argv != 1 && (argv != 2 || strcmp(argc[1], "--innodb") != 0)) {
    printf("Usage: %s [--innodb]\n", argc[0]);
    exit(1);
  }
  if (argv == 2) {
    innodb = 1;
  }
}

/*
  get_max_serial: gets max serial that can be deleted.

  This is somewhat tricky because of deletes in last. A join cannot be
  made safely, so:
  when determining the serial via timestamp:
    when joining with history ignore serials with op=DELETE
    when joining with last consider only object='' and ignore sequence.

  returns the max of the 2 joins.
*/
void get_max_serial() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  char             query[1000];
  long             last_serial;

  sprintf(query,
	  "SELECT max(serials.serial_id) "
	  "  FROM serials, history "
	  " WHERE serials.object_id=history.object_id "
	  "   AND serials.sequence_id=history.sequence_id "
	  "   AND timestamp<%ld "
	  "   AND operation=1",
	  end_time);
  SQ_execute_query(RIPE_conn, query, &rs);
  row = SQ_row_next(rs);
  if (row != NULL) {
    if (SQ_get_column_int(rs, row, 0, &max_serial) != 0) {
      max_serial = 0;
    }
  }
  SQ_free_result(rs);

  sprintf(query,
	  "SELECT max(serials.serial_id) "
	  "  FROM serials, last "
	  " WHERE last.object_id=serials.object_id "
	  //"   AND s.sequence_id=h.sequence_id "
	  "   AND timestamp<=%ld "
	  "   AND object=''",
	  end_time);
  SQ_execute_query(RIPE_conn, query, &rs);
  row = SQ_row_next(rs);
  if (row != NULL) {
    if (SQ_get_column_int(rs, row, 0, &last_serial) == 0) {
      if (last_serial>max_serial) {
	max_serial = last_serial;
      }
    }
  }
  //printf ("max serial: %ld\n", max_serial);
  SQ_free_result(rs);


  //If all serials are to be deleted then preserve 1 for mirror ref
  sprintf(query,
	  "SELECT max(serial_id) "
	  "  FROM serials ");
  SQ_execute_query(RIPE_conn, query, &rs);
  row = SQ_row_next(rs);
  if (row != NULL) {
    if (SQ_get_column_int(rs, row, 0, &last_serial) == 0) {
      if (last_serial == max_serial) {
	max_serial = max_serial - 1;
      }
    }
  }
  //printf ("max serial: %ld\n", max_serial);
  SQ_free_result(rs);
}

/*
  delete_serials: delete serials referencing old operations.

  max_serial was previously computed on get_max_serial
 */
void delete_serials() {
  char delete_query[200];

  sprintf(delete_query,
	  "DELETE FROM serials "
	  " WHERE serial_id<=%ld",
	  max_serial);
  SQ_execute_query(RIPE_conn, delete_query, NULL);
}

/*
  cleanup_history: cleans the history table.

  A blind cleanup cannot be done as a record with an old timestamp might
  be referenced in a newer delete (a delete needs the last object incarnation).
  If it is not referenced on an existing serial (serial cleanup should be
  done _before_ history cleanup) then it can be deleted.


  SELECT FROM history LEFT JOIN serials WHERE timestamp<=?
    if record has serial_id NULL then delete it from history.
*/
void cleanup_history() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  char             query[300];
  char             delete_query[300];
  long             object_id;
  long             sequence_id;

  sprintf(query,
	  "SELECT history.object_id, history.sequence_id, serials.serial_id "
	  "  FROM history LEFT JOIN serials USING(object_id, sequence_id) "
	  " WHERE history.timestamp<=%ld",
	  end_time);
  SQ_execute_query(RIPE_conn, query, &rs);
  //printf("%s Q: %s\n", SQ_error(RIPE_conn), query);
  while ((row = SQ_row_next(rs)) != NULL) {
    //printf(".\n");
    if (SQ_get_column_string_nocopy(rs, row, 2) == NULL) {
      SQ_get_column_int(rs, row, 0, &object_id);
      SQ_get_column_int(rs, row, 1, &sequence_id);
      sprintf(delete_query,
	      "DELETE "
	      "  FROM history"
	      " WHERE object_id = %ld AND sequence_id = %ld",
	      object_id, sequence_id);
      //printf("DH: %s\n", delete_query);
      SQ_execute_query(RIPE_conn, delete_query, NULL);
    }
  }

  SQ_free_result(rs);
}

/*
  delete_last: deletes empty objects from last which are older than desired
*/
void delete_last() {
  char delete_last[200];
  sprintf(delete_last,
	  "DELETE FROM last WHERE timestamp<=%ld AND object=''",
	  end_time);
  SQ_execute_query(RIPE_conn, delete_last, NULL);
}

/*
  set_end_timestamp: get max timestamp from archive to use it as limit.
*/
void set_end_timestamp() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;

  if (SQ_execute_query(archive_conn,
		       "SELECT max(timestamp) FROM archive",
		       &rs) != 0 ) {
    printf("Could not determine maximum timestamp on archive\n");
    exit(1);
  }

  row = SQ_row_next(rs);

  if (row == NULL) {
    printf("Could not determine maximum timestamp on archive\n");
    exit(1);
  }

  if (SQ_get_column_int(rs, row, 0, &end_time) != 0) {
    printf("Could not determine maximum timestamp on archive\n");
    exit(1);
  }

  //hard-coded less 1 week
  end_time = end_time - 60*60*24*7;
}

/*
  main: cleanup entry point

  self-documenting (see pseudo-code on iii).

  get_max_serial prepares variables to be used by delete_serials.
*/
int main (int argv, char** argc) {
  LG_context_t *ctx;

  process_arguments(argv, argc);
  read_configuration();
  get_db_connections();

  ctx = LG_ctx_new();
  LG_ctx_add_appender(ctx, LG_app_get_file_info_dump(stdout));
  SQ_init(ctx);

  printf("Beginning cleanup");
  //printf("locking tables\n");
  if (innodb) {
    if (SQ_execute_query(RIPE_conn, "BEGIN", NULL) != 0) {
      printf("Could not begin transaction:\n%s\n", SQ_error(RIPE_conn));
      return 1;
    }
  }
  else {
    if (SQ_execute_query(RIPE_conn,
			 "LOCK TABLES history WRITE, serials WRITE, "
			 "            last WRITE ", NULL) != 0) {
      printf("Could not lock tables for writing:\n%s\n", SQ_error(RIPE_conn));
      return 1;
    }
  }
  //printf("tables locked\n");

  set_end_timestamp();
  //printf("ets: %ld\n", end_time);
  //printf("get_max_serial\n");
  get_max_serial();
  //printf("delete serials\n");
  delete_serials();
  //printf("cleanup history\n");
  cleanup_history();
  //printf("cleanup last\n");
  delete_last();

  if (innodb) {
    if (SQ_execute_query(RIPE_conn, "COMMIT", NULL) != 0) {
      printf("Could not commit transaction:\n%s\n", SQ_error(RIPE_conn));
      return 1;
    }
  }
  else {
    SQ_execute_query(RIPE_conn, "UNLOCK TABLES", NULL);
  }
  //close_dbs();
  printf("Ending sucessful cleanup");
  return 0;
}

