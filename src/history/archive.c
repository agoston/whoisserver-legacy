/***************************************
  $Revision: 1.1 $

  Archive.  archive.c - whois DB archiving.

  Status: REVIEWED, TESTED, COMPLETE

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

long begin_time = 0;
long end_time;

/*
  process_arguments: processes command-line arguments.
*/
void process_arguments(int argv, char** argc) {
  if (argv != 3 && argv != 1) {
    printf("Usage: %s [-begin <ts>]\n", argc[0]);
    exit(1);
  }

  if (argv == 3) {
    if (strcmp(argc[1], "-begin") == 0) {
      begin_time = atol(argc[2]); //assume is number
    }
    else {
      //begin makes no sense, probably should be removed (or used only
      //with begin 0
      printf("Usage: %s [-begin <ts>]\n", argc[0]);
      exit(1);
    }
  }
}

//thread_id!!!
//check errors
/*
  archive_table: archives a table

  Archives a table (last or history).
  
  For each record (observing timestamps) on to archive  table
    Reads record, writes record to archive2.
*/
void archive_table(char* table) {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  char             select_query[300];
  char*            insert_query;
  char*            object_buffer;
  char             pkey_buffer[300];
  int              object_size;
  char             object_id[50];
  char             sequence_id[50];
  char             operation[50];
  long             cont;

  cont = 0;
  
  //printf("%s\n", table);
  sprintf(select_query,
	  "SELECT object_id, sequence_id, pkey,"
	  "       timestamp, object_type, object"
	  "  FROM %s "
	  " WHERE timestamp>=%ld AND timestamp<%ld AND pkey<>''",
	  table, begin_time-200000, end_time);
  //printf("%s\n", select_query);
  //200000 - arbitrary value to give slack for delays in updates
  SQ_execute_query_nostore(RIPE_conn, select_query, &rs);


  while ((row = SQ_row_next(rs)) != NULL) {
    //printf(".");
    object_size   = strlen(SQ_get_column_string_nocopy(rs,row,5));
    object_buffer = UT_malloc(object_size*2 + 1); //worst case
    insert_query  = UT_malloc(object_size*2 + 300);

    prepare_string_attribute(SQ_get_column_string_nocopy(rs,row,5),
			     object_buffer);
    prepare_string_attribute(SQ_get_column_string_nocopy(rs,row,2),
			     pkey_buffer);
    check_null(rs, row, 0, object_id);
    check_null(rs, row, 1, sequence_id);
    //check_null(rs, row, 3, operation);
    cont ++;
    sprintf(insert_query,
	    "INSERT INTO archive2(object_id, pkey, object_type, operation, "
	    "timestamp, sequence_id, object) "
	    "VALUES (%s,'%s', %s, 1, %s, %s,'%s')\0",
	    object_id,
	    pkey_buffer,
	    SQ_get_column_string_nocopy(rs,row,4),
	    //operation,
	    SQ_get_column_string_nocopy(rs,row,3),
	    sequence_id,
	    object_buffer
	   );
    if (SQ_execute_query(archive_conn, insert_query, NULL) != 0) {
      //printf("'%s' Failed!\n", insert_query);
      exit(1);
    }

    UT_free(object_buffer);
    UT_free(insert_query);
  }
  //printf(" %d\n", cont);
  SQ_free_result(rs);

  //printf ("Archived %ld\n", cont);

}


/*
  patch_serial: patches serials into archive2
*/
void patch_serial(long object_id, long sequence, long serial) {
  char update_query[100];

  sprintf(update_query,
	  "UPDATE archive2 "
	  "   SET serial_id = %ld "
	  " WHERE object_id = %ld AND sequence_id = %ld ",
	  serial, object_id, sequence);

  //printf("%s\n", update_query);
  SQ_execute_query(archive_conn, update_query, NULL);
}

/*
  patch_pseudo_delete: patches pseudo delete operation
*/
void patch_pseudo_delete(long object_id, long sequence) {
  char update_query[100];

  sprintf(update_query,
	  "UPDATE archive2 "
	  "   SET operation = 3 "
	  " WHERE object_id = %ld AND sequence_id = %ld ",
	  object_id, sequence);

  //printf("%s\n", update_query);
  SQ_execute_query(archive_conn, update_query, NULL);
}

/*
*/
void patch_delete_object(long object_id, long sequence) {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  SQ_row_t*        next;
  long             serial;
  char*            object;
  char*            clean_object;
  char             select_query[200];
  char*            update_query;


  if (sequence == 0) {
    return;
  }
  sprintf(select_query,
	  "SELECT object FROM history WHERE object_id=%ld AND sequence_id=%ld",
	  object_id, sequence);

  //printf("%s\n", select_query);
  SQ_execute_query(RIPE_conn, select_query, &rs);

  row = SQ_row_next(rs);
  if (row == NULL) {
    //printf("Could not patch delete (SELECT) for oid=%ld seq=%ld\n",
    //  object_id, sequence);
    SQ_free_result(rs);
    return;
  }

  object = SQ_get_column_string_nocopy(rs, row, 0);
  update_query = UT_malloc(strlen(object)*2 + 300);
  clean_object = UT_malloc(strlen(object)*2 + 5); // this is lame
  prepare_string_attribute(object, clean_object);
  sprintf(update_query,
	  "UPDATE archive2 "
	  "   SET object='%s', operation=2 "
	  " WHERE object_id=%d AND sequence_id=0",
	  clean_object, object_id);
  UT_free(clean_object);
  //printf("%s\n", update_query);
  if (SQ_execute_query(archive_conn, update_query, NULL) != 0) {
    //printf("Could not patch delete (UPDATE) for oid=%ld seq=%ld\n",
    //  object_id, sequence);
    exit(1);
  }
  UT_free(update_query);
  SQ_free_result(rs);
}

/*
  patch_serials: patches serials into archive

    SELECT serial_id, operation, object_id, sequence_id
      FROM serials
  ORDER BY object_id, sequence_id, operation DESC
*/
void patch_serials() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  SQ_row_t*        next;
  long             serial;
  long             sequence;
  long             operation;
  long             object_id;
  long             next_serial;
  long             next_object_id;


  SQ_execute_query(RIPE_conn,
		   "  SELECT serial_id, operation, object_id, "
		   "         sequence_id "
		   "    FROM serials "
		   "ORDER BY object_id, sequence_id, operation DESC",
		   &rs);

  row = SQ_row_next(rs);
  while (row != NULL) {
    SQ_get_column_int(rs, row, 0, &serial);
    SQ_get_column_int(rs, row, 1, &operation);
    SQ_get_column_int(rs, row, 2, &object_id);
    SQ_get_column_int(rs, row, 3, &sequence);

    next = SQ_row_next(rs);


    if (object_id != 0) {
      // Deletes
      if (operation == 2) {
	if (next == NULL) {
	  patch_delete_object(object_id, sequence);
	  patch_serial(object_id, 0, serial);
	}
	else {
	  SQ_get_column_int(rs, next, 2, &next_object_id);
	  SQ_get_column_int(rs, next, 0, &next_serial);
	  if (next_object_id == object_id  &&
	      next_serial == serial + 1) {
	    patch_pseudo_delete(object_id, sequence+1);
	  }
	  else {
	    patch_delete_object(object_id, sequence);
	    patch_serial(object_id, 0, serial);
	  }
	}
      }
      else {
	patch_serial(object_id, sequence, serial);
      }
    }

    row = next;
  }

  SQ_free_result(rs);

  /* // TBR removed
     SQ_execute_query(archive_conn,
     " UPDATE archive2 "
     "   SET operation=2 "
     " WHERE object='' ",
     NULL);
  */
}

void compute_begin_time() {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  long             max_archived;

  if (begin_time==0) {
    SQ_execute_query(archive_conn,
		     "SELECT max(timestamp) FROM archive",
		     &rs);
    row = SQ_row_next(rs);

    if ((row != NULL) && 
	(SQ_get_column_int(rs, row, 0, &max_archived)==0)) {
      begin_time = max_archived + 1;
    }
    SQ_free_result(rs);
  }
}

/*
  archive2: archives

  Self-documenting? No...

  The general philosophy is: Copy verbatim the content of last and
  history tables (at least the ones that are not on the current archive)
  and only then patch in the serials.
  The rational for this strategy is: It should be assured in as much as
  possible that no records on last and history are lost, that would be
  very serious, as such the code on that part should be simple.
  The serial code can be a little bit more complex, it should not fail,
  but if it has a bug it is less serious than losing last/history.
*/
void archive2() {
  if (SQ_execute_query(RIPE_conn,
		       "LOCK TABLES history READ, serials READ, last READ",
		       NULL) != 0) {
    //printf("Could not lock tables for reading:\n%s\n", SQ_error(RIPE_conn));
    exit(1);
  }

  compute_begin_time();
  //printf("Archiving last\n");
  archive_table("last");
  //printf("Archiving history\n");
  archive_table("history");
  //printf("Patching serials/deletes\n");
  patch_serials();

  SQ_execute_query(RIPE_conn, "UNLOCK TABLES", NULL);

}

/*
  delete_from: simple function to make code writing more clean

  Should it be on a separate place?
*/
void delete_from(char *from) {
  char delete[200];

  sprintf(delete, "DELETE FROM %s", from);
  SQ_execute_query(archive_conn, delete, NULL);
}

/*
  update: simple function to make code writing more clean

  Should it be on a separate place?
*/
void update(char* table, char* set, char* where) {
  char update[200];

  sprintf(update, "UPDATE %s SET %s", table, set);
  SQ_execute_query(archive_conn, update, NULL);
}

/*
  insert_select: simple function to make code writing more clean

  Should it be on a separate place?
*/
void insert_select(char* to, char *from, char* distinct) {
  char insert[200];

  sprintf(insert, "INSERT INTO %s SELECT %s * FROM %s", to, distinct, from);
  SQ_execute_query(archive_conn, insert, NULL);
}

long count(char *query) {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  long             result;

  result = 0;

  if (SQ_execute_query(archive_conn, query, &rs) != 0) {
    //printf("'%s' failed\n", query);
    exit(1);
  }

  row = SQ_row_next(rs);

  if (row == NULL) {
    printf("count for '%s' failed\n", query);
    exit(1);
  }
  SQ_get_column_int(rs, row, 0, &result);

  SQ_free_result(rs);

  return result;
}

void clean_repetitions() {
  long max_serial;
  char delete[200];

  max_serial = count("SELECT MAX(serial_id) FROM archive");
  sprintf(delete, "DELETE FROM archive2 where serial_id<=%d", max_serial);
  SQ_execute_query(archive_conn, delete, NULL);
}
/*
  main: Archive entry point

  This function is reasonably self-commenting.
  Also the previous hour is not archived to avoid concurrency problems
  with updates.
  Also it is important to archive last _before_ of history because of
  deletes that might occur during the process (repetitions are ok, they
  will be removed)

  A first execution over a normal database will be sloooow.
*/
int main (int argv, char** argc) {
  LG_context_t *ctx;

  process_arguments(argv, argc);
  read_configuration();
  end_time = time(NULL) - 3600;
  get_db_connections();

  ctx = LG_ctx_new();
  LG_ctx_add_appender(ctx, LG_app_get_file_info_dump(stdout));
  
  SQ_init(ctx);

  //printf("Erasing archive2\n");
  delete_from("archive2");
  //printf("Beginning archive\n");
  archive2();
  //printf("Inserting archive2 into archive\n");
  update("archive2", "object_id = NULL", "");
  clean_repetitions();
  SQ_execute_query(archive_conn,
    "delete from archive2 where serial_id is null", NULL);

  //printf("Inserting archive2 into archive\n");
  insert_select("archive", "archive2", "");
  //printf("delete from archive2\n");
  //delete_from("archive2");

  //close_dbs();

  //printf("Ending sucessful execution of archive\n");
  return 0;
}
