/***************************************
  $Revision: 1.1 $

  Surf.  surf.c - whois DB archive surfing.

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

char*    pkey;
int      instance = -1;
gboolean live = 0;
gboolean whowas = 0;

void process_arguments (int argv, char** argc) {
  if (strcmp(argc[0], "whowas") == 0 && argv != 2) {
    printf("Usage: %s <pkey>\n", argc[0]);
    exit(-1);
  }
  if (argv < 2 || argv > 4
      || (argv == 2 && strcmp(argc[1], "--live")==0) ) { 
    printf("Usage: %s [--live] <pkey> [instance]\n", argc[0]);
    exit(-1);
  }
  
  if (strcmp(argc[1], "--live") == 0) {
    live = 1;
  }
  pkey = argc[live ? 2 : 1];
  if ((!live && argv==3) || argv==4) {
    instance = atoi(argc[live ? 3 : 2]);
  }

  if (strcmp(argc[0], "whowas") == 0) {
    whowas = TRUE;
  }
}

/*
  get_operation_type_name: Given an op type in number return the name string.
*/
void get_operation_name(char *operation_name, long operation) {
  switch (operation) {
  case 1:
    strcpy(operation_name, "ADD");
    return;
  case 2:
    strcpy(operation_name, "DEL");
    return;
  case 3:
    strcpy(operation_name, "DAD");
    return;
  case 4:
    strcpy(operation_name, "UPD");
    return;
  default:
    sprintf(operation_name, "%d", operation);
  }
}

/*
  get_object_type_name: Given an object type in number return the name string.
*/
void get_object_type_name(char *object_type_name, long object_type) {
  switch (object_type) {
  case 0:
    strcpy(object_type_name, "AS-BLOCK");
    return;
  case 1:
    strcpy(object_type_name, "AS-SET");
    return;
  case 2:
    strcpy(object_type_name, "AUT-NUM");
    return;
  case 3:
    strcpy(object_type_name, "DOMAIN");
    return;
  case 4:
    strcpy(object_type_name, "INET-RTR");
    return;
  case 5:
    strcpy(object_type_name, "INET6NUM");
    return;
  case 6:
    strcpy(object_type_name, "INETNUM");
    return;
  case 7:
    strcpy(object_type_name, "KEY-CERT");
    return;
  case 8:
    strcpy(object_type_name, "LIMERICK");
    return;
  case 9:
    strcpy(object_type_name, "MNTNER");
    return;
  case 10:
    strcpy(object_type_name, "PERSON");
    return;
  case 11:
    strcpy(object_type_name, "ROLE");
    return;
  case 12:
    strcpy(object_type_name, "ROUTE");
    return;
  case 13:
    strcpy(object_type_name, "ROUTE-SET");
    return;
  case 14:
    strcpy(object_type_name, "FILTER-SET");
    return;
  case 15:
    strcpy(object_type_name, "PEER-SET");
    return;
  default:
    sprintf(object_type_name, "%d", object_type);
  }
}

long surf_real(SQ_connection_t* conn, const gchar* query, int* cont) {
  SQ_result_set_t* rs;
  SQ_row_t*        row;
  long             current_operation;
  long             operation;
  long             timestamp;
  char             type_name[20];
  long             type;
  char             operation_name[20];

  SQ_execute_query(conn, query, &rs);

  if (whowas) {
    instance = SQ_num_rows(rs) - 1;
  }
  current_operation = 0;
  while ((row = SQ_row_next(rs)) != NULL) {
    if (SQ_get_column_int(rs, row, 0, &operation) == -1) {
      operation = 1;
    }
    SQ_get_column_int(rs, row, 1, &timestamp);
    SQ_get_column_int(rs, row, 2, &type);

    if (operation == 1 && (current_operation == 1 || current_operation > 2)) {
      operation = 4;
    }
    current_operation = operation;

    get_operation_name(operation_name, operation);
    get_object_type_name(type_name, type);
    if (instance<1) {
      printf("%5d - Op: %3s Type: %12s Time: %s\n", *cont, operation_name,
	     type_name, ctime(&timestamp));
    }
    else {
      if (instance == *cont) {
	printf("%5d - Op: %3s Type: %12s Time: %s\n%s\n", *cont,
	       operation_name,
	       type_name, ctime(&timestamp),
	       SQ_get_column_string_nocopy(rs, row, 3));
      }
    }
    if (*cont) {
      *cont += 1;
    }
  }

  //SQ_query_free(rs);
  return timestamp;
}

/*
  surf: Surfs a pkey (which might include more than 1 object).


*/
void surf() {
  char             query[300];
  int              cont = 1;
  long             timestamp;

  sprintf(query,
	  "  SELECT operation, timestamp, object_type, object "
	  "    FROM archive "
	  "   WHERE pkey LIKE '%s' "
	  "ORDER BY timestamp ASC, operation DESC",
	  pkey);
  timestamp = surf_real(archive_conn, query, &cont);
  if (live) {
    printf("LIVE DATA - history\n");
    sprintf(query,
	    "  SELECT 1, timestamp, object_type, object "
	    "    FROM history "
	    "   WHERE pkey LIKE '%s' AND timestamp>%d "
	    "ORDER BY timestamp ASC",
	    pkey, timestamp);
    //printf("%s\n", query);
    cont = 0;
    surf_real(RIPE_conn, query, &cont);

    printf("LIVE DATA - last\n");
    sprintf(query,
	    "  SELECT 1, timestamp, object_type, object "
	    "    FROM last "
	    "   WHERE pkey LIKE '%s' AND timestamp>%d "
	    "ORDER BY timestamp ASC",
	    pkey, timestamp);
    //printf("%s\n", query);
    surf_real(RIPE_conn, query, &cont);
  }
}

/*
  main: Surf entry point.

  Self-documenting.
*/
int main (int argv, char** argc) {
  LG_context_t *ctx;

  process_arguments(argv, argc);
  read_configuration();
  get_db_connections();

  ctx = LG_ctx_new();
  LG_ctx_add_appender(ctx, LG_app_get_file_info_dump(stdout));
  SQ_init(ctx);


  surf();

  //close_dbs();

  return 1;
}
