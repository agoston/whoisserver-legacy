/***************************************
  $Revision: 1.3 $

  SQL module (sq) - this is a MySQL implementation of the SQL module.

  Status: NOT REVUED, TESTED

  ******************/ /******************
  Filename            : mysql_driver.c
  Authors             : ottrey@ripe.net
                        marek@ripe.net
  OSs Tested          : Solaris 7 / sun4u / sparc
  ******************/ /******************
  Copyright (c) 1999                              RIPE NCC
 
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

#include "rip.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <strings.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
#include <glib.h>

static LG_context_t *sq_context;

void SQ_init (LG_context_t *ctx) {
  sq_context = ctx;
}

/* 
Description:

  Connect to the the MySQL database, returning an error if unsuccessful.

Arguments:

  SQ_connection_t **conn; used to return pointer to connection structure

  const char *host; database server host to connect to, may be NULL or 
    "localhost", in which case Unix sockets may be used

  unsigned int port; port to connect to database server on, may be 0 to use  
    default

  const char *db; name of database to use, may be NULL

  const char *user; name of user to connect as, if NULL then the current Unix 
    user login is used

  const char *password; password to send, may be NULL to not use a password

Returns:
  
  SQ_OK on success

  SQ_CTCONN on error; the exact reason may be determined by using SQ_error() 
    on the value returned in *conn - this structure should be properly via
    SQ_close_connection(), even on error

Notes:

  Most parameters are passed straight through to the MySQL connect function,
  so the MySQL documentation should be checked for current meaning.
*/

int 
SQ_try_connection (SQ_connection_t **conn, const char *host,
                   unsigned int port, const char *db,
                   const char *user, const char *password)
{
    SQ_connection_t *res;
    
    *conn = mysql_init(NULL);
    dieif(*conn == NULL);  /* XXX SK - need to call "out of memory handler" */

    mysql_options(*conn, MYSQL_READ_DEFAULT_GROUP, "client");

    res = mysql_real_connect(*conn, host, user, password, db, port, NULL, 0);
    if (res == NULL) {
        return SQ_CTCONN;
    } else {
        return SQ_OK;
    }
}

/* SQ_get_connection() */
/*++++++++++++++++++++++++++++++++++++++
  Get a connection to the database.

  const char *host
  
  unsigned int port

  const char *db
  
  const char *user
  
  const char *password
   
  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_init">mysql_init()</A>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_real_connect">mysql_real_connect()</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
SQ_connection_t *SQ_get_connection(const char *host, unsigned int port, const char *db, const char *user, const char *password) {

  SQ_connection_t *sql_connection;
  int res;
  unsigned try = 0;

  /* XXX MB.
     This is really kludgy! 
     For some (unknown yet) reason, sometimes the connection does not
     work the first time. So we try up to 3 times here, and give up only
     then.

     Check the logfiles for warnings, especially with newer mysql version,
     like 3.23. The problem may or may not go away.

     SK - I added a sleep() to avoid crushing the poor server.
  */

  for (;;) {
    /* try to connect */
    res = SQ_try_connection(&sql_connection, host, port, db, user, password);

    /* on success, return our result */
    if (NOERR(res)) {
        return sql_connection;
    }
    else {
      
      /* if we've tried enough, exit with error */
      if (try >= 3) {
        LG_log(sq_context, LG_SEVERE, " %s; %s", db, 
		  sql_connection ? SQ_error(sql_connection) : "-?");
        die;
      }

      /* otherwise, prepare to try again */
      LG_log(sq_context, LG_ERROR, " %s; %s", db, 
		sql_connection ? SQ_error(sql_connection) : "-?");

      if (try > 0) {
        sleep(try);
      }
      try++;
      
      if( sql_connection ) {
	SQ_close_connection(sql_connection);
      }
    }
  }/* for(;;) */
} /* SQ_get_connection() */

/* SQ_execute_query() */
/*++++++++++++++++++++++++++++++++++++++
  Execute the sql query.

  SQ_connection_t *sql_connection Connection to database.
  
  const char *query SQL query.

  SQ_result_set_t *result ptr to the structure to hold result. 
  May be NULL if no result is needed.

  Returns: 
    0 if the query was successful.
    Non-zero if an error occured.
  
  More:
  +html+ <PRE>
  Authors:
        ottrey, andrei, marek
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_query">mysql_query()</A>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_use_result">mysql_use_result()</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
int SQ_execute_query(SQ_connection_t *sql_connection, const char *query, SQ_result_set_t **result_ptr) {
	int err;
	SQ_result_set_t *result;

//	there's no need to measure every f***ing query
//	float seconds;
//	ut_timer_t start_time, stop_time;
//	UT_timeget(&start_time);

	err = mysql_query(sql_connection, query);

	/* log the time and result of the query */
	if (err == 0) {
		result = mysql_store_result(sql_connection);

//	there's no need to measure every f***ing query
//		UT_timeget(&stop_time);
//		seconds = UT_timediff( &start_time, &stop_time);
//
//		LG_log(sq_context, LG_DEBUG, "spent %.2f sec; got %d rows from [%s: %s]", seconds,
//		        SQ_get_affected_rows(sql_connection), sql_connection->db, query);

		if (result_ptr)
			*result_ptr=result;
		else if (result)
			mysql_free_result(result);
		return (0);
	} else
		return (-1);

} /* SQ_execute_query() */

/* 
Description:
 
    Performs identially to SQ_execute_query(), except that it does not read the
    entire query into memory.

Notes:

    No data may be written to the table until the entire result set is read,
    so this should only be used in cases where:

    1. an unacceptably large amount of memory will be returned by the query
    2. there is no chance that a user can accidentally or maliciously 
       prevent the result set from being read in a expedicious manner
*/

int 
SQ_execute_query_nostore(SQ_connection_t *sql_connection, 
                         const char *query, SQ_result_set_t **result_ptr) 
{
  int err;
  SQ_result_set_t *result;

  err = mysql_query(sql_connection, query);
  if (err != 0) {
      return -1;
  }
  result = mysql_use_result(sql_connection);
  if (result == NULL) {
      return -1;
  } 
  *result_ptr = result;
  return 0;
} /* SQ_execute_query_nostore() */

/* SQ_get_column_count() */
/*++++++++++++++++++++++++++++++++++++++
  Get the column count.

  SQ_result_set_t *result The results from the query.
  
  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_num_fields">mysql_num_fields()</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
int SQ_get_column_count(SQ_result_set_t *result) {
	return mysql_num_fields(result);
} /* SQ_get_column_count() */

/* SQ_get_table_size() */
/*++++++++++++++++++++++++++++++++++++++
  Get the row count of a table

  char *table   The table to be examined
  
  More:
  +html+ <PRE>
  Authors:
        marek
  +html+ </PRE>

  ++++++++++++++++++++++++++++++++++++++*/
int SQ_get_table_size(SQ_connection_t *sql_connection,
		     char *table) {  
  int count;
  GString *sql_command;
  SQ_result_set_t *result;
  SQ_row_t *row;
  char *countstr;
  
  sql_command = g_string_new("");
  g_string_sprintf(sql_command, "SELECT COUNT(*) FROM %s", table);
  dieif(SQ_execute_query(sql_connection, sql_command->str, &result) == -1 );
  g_string_free(sql_command, TRUE);

  row = SQ_row_next(result);
  
  countstr = SQ_get_column_string(result, row, 0);
  sscanf(countstr, "%d", &count);	
  UT_free(countstr);
  
  SQ_free_result(result);
	
  return count;  
} /* SQ_get_table_size() */

/* SQ_get_affected_rows() */
/*++++++++++++++++++++++++++++++++++++++
  Get the row count of a table

  char *table   The table to be examined
  
  More:
  +html+ <PRE>
  Authors:
        marek
  +html+ </PRE>

  ++++++++++++++++++++++++++++++++++++++*/
int SQ_get_affected_rows(SQ_connection_t *sql_connection)
{
  return (int)mysql_affected_rows(sql_connection);
}/* SQ_get_affected_rows() */
		      
/* SQ_get_insert_id() */
/*++++++++++++++++++++++++++++++++++++++
  Get the ID that was most recently generated for an AUTO_INCREMENT field

 
  More:
  +html+ <PRE>
  Authors:
        andrei
  +html+ </PRE>

  ++++++++++++++++++++++++++++++++++++++*/
long SQ_get_insert_id(SQ_connection_t *sql_connection)
{
  return (long)mysql_insert_id(sql_connection);
}/* SQ_get_insert_id() */
	
/* SQ_get_column_label() */
/*++++++++++++++++++++++++++++++++++++++
  Get the column label.

  SQ_result_set_t *result The results from the query.
  
  unsigned int column The column index.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_fetch_field_direct">mysql_fetch_field_direct()</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *SQ_get_column_label(SQ_result_set_t *result, unsigned int column) {
  char *str;
/* MySQL decided to change their interface.  Doh! */
#ifdef OLDMYSQL
  MYSQL_FIELD field;

  field = mysql_fetch_field_direct(result, column);

  str = UT_strdup(field.name);
#else
  MYSQL_FIELD *field;

  field = mysql_fetch_field_direct(result, column);

  str = UT_strdup(field->name);
#endif

/*
  printf("column=%d\n", column);
  printf("field.name=%s\n", field.name);
  printf("field.table=%s\n", field.table);

  printf("field.def=%s\n", field.def);

  printf("field.type=%d\n", field.type);
  printf("field.length=%d\n", field.length);
  printf("field.max_length=%d\n", field.max_length);
  printf("field.flags=%d\n", field.flags);
  printf("field.decimals=%d\n", field.decimals);
*/

  return str;

} /* SQ_get_column_label() */

/* SQ_get_column_max_length() */
/*++++++++++++++++++++++++++++++++++++++
  Get the max length of the column.

  SQ_result_set_t *result The results from the query.
  
  unsigned int column The column index.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_fetch_field_direct">mysql_fetch_field_direct()</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
unsigned int SQ_get_column_max_length(SQ_result_set_t *result, unsigned int column) {
/* MySQL decided to change their interface.  Doh! */
#ifdef OLDMYSQL
  MYSQL_FIELD field;

  field = mysql_fetch_field_direct(result, column);

  return field.length;
#else
  MYSQL_FIELD *field;

  field = mysql_fetch_field_direct(result, column);

  return field->length;
#endif

} /* SQ_get_column_max_length() */

/* SQ_row_next() */
/*++++++++++++++++++++++++++++++++++++++
  Get the next row.

  SQ_result_set_t *result The results from the query.
  
  unsigned int column The column index.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_fetch_row">mysql_fetch_row()</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
SQ_row_t *SQ_row_next(SQ_result_set_t *result) {

  return (SQ_row_t *)mysql_fetch_row(result);

} /* SQ_row_next() */

/* SQ_get_column_string() */
/*++++++++++++++++++++++++++++++++++++++
 Get the column string.

 SQ_row_t *current_row The current row (obtained from a SQ_row_next() ).
 
 unsigned int column The column index.

 More:
 +html+ <PRE>
 Authors:
 ottrey
 +html+ </PRE><DL COMPACT>
 +html+ <DT>Online References:
 +html+ <DD><UL>
 +html+ </UL></DL>

 ++++++++++++++++++++++++++++++++++++++*/
char *SQ_get_column_string(SQ_result_set_t *result, SQ_row_t *current_row, unsigned int column) {
	char *str=NULL;
	unsigned length = mysql_fetch_lengths(result)[column];

	if (current_row != NULL && current_row[column] != NULL) {
		str = (char *)UT_malloc(length + 1);
		memcpy(str, current_row[column], length);
		str[length] = '\0';
	}

	return str;
} /* SQ_get_column_string() */

/* SQ_get_column_string_nocopy - return pointer to the column string
 without making a copy of it */
char *SQ_get_column_string_nocopy(SQ_result_set_t *result, SQ_row_t *current_row, unsigned int column) {
	if (current_row != NULL && current_row[column] != NULL) {
		return (char *)current_row[column];
	}
	return NULL;
}/* SQ_get_column_string_nocopy */

/* SQ_get_column_strings() */
/*++++++++++++++++++++++++++++++++++++++
  Get the all the strings in one column.

  SQ_result_set_t *result The results.
  
  unsigned int column The column index.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *SQ_get_column_strings(SQ_result_set_t *result, unsigned int column) {
  MYSQL_ROW row;
  GString *buf;
  char *str;

  /* allocate a buffer for building the result in */
  buf = g_string_sized_new(STR_XXL);

  /* add each row of the result as a line to our result buffer */
  while ((row = mysql_fetch_row(result)) != NULL) {
    if (row[column] != NULL) {
      g_string_append(buf, row[column]);
      g_string_append_c(buf, '\n');
    }
  }

  /* if something found, copy to a string for return */
  if (buf->len > 0) {
    str = UT_strdup(buf->str);
  } else {
    str = NULL;
  }

  /* free temporary memory */
  g_string_free(buf, TRUE);

  return str;

} /* SQ_get_column_strings() */

/* Get an integer from the column.
 * SQ_result_set_t *result        the sql resultset
 * SQ_row_t *current_row          the current row
 * unsigned int column            the column index
 * long *resultptr                pointer where the result should be stored
 * 
 * Returns <0 if error occurs, 0 otherwise
 *         -1 if NULL
 *         -2 if out of bounds
 *         -3 if not a number
 * 
 * FIXME: this function returns a long* instead of int* for legacy reasons.
 *        should be fixed, but there are just too many darn references to it.
 *        agoston, 2008-01-18
 * 
 */
int SQ_get_column_int(SQ_result_set_t *result, SQ_row_t *current_row, unsigned int column, long *resultptr) {
	long long int res;
	int ret = SQ_get_column_llint(result, current_row, column, &res); 
	
	if (ret < 0)
		return ret;

	if (res <  INT_MIN || res > INT_MAX) {
		return -2;
	}
	
	*resultptr = (long)res;
	return 0;
}

/* Get an integer from the column.
 * SQ_result_set_t *result        the sql resultset
 * SQ_row_t *current_row          the current row
 * unsigned int column            the column index
 * long *resultptr                pointer where the result should be stored
 * 
 * Returns <0 if error occurs, 0 otherwise
 *         -1 if NULL
 *         -2 if out of bounds
 *         -3 if not a number
 */
int SQ_get_column_unsigned(SQ_result_set_t *result, SQ_row_t *current_row, unsigned int column, unsigned *resultptr) {
	long long int res;
	int ret = SQ_get_column_llint(result, current_row, column, &res); 
	
	if (ret < 0)
		return ret;

	if (res < 0 || res > UINT_MAX) {
		return -2;
	}
	
	*resultptr = (unsigned)res;
	return 0;
}


/* Get a long long int from the column.
 * SQ_result_set_t *result        the sql resultset
 * SQ_row_t *current_row          the current row
 * unsigned int column            the column index
 * long long int *resultptr       pointer where the result should be stored
 * 
 * Returns <0 if error occurs, 0 otherwise
 *         -1 if NULL
 *         -2 if out of bounds
 *         -3 if not a number
 */
int SQ_get_column_llint(SQ_result_set_t *result, SQ_row_t *current_row, unsigned int column, long long int *resultptr) {
	long long int col_val;
	char *endptr;

	if (current_row[column] == NULL)
		return -1;

	col_val = strtol((char *)current_row[column], &endptr, 10);

	/* under- or over-flow */
	if (((col_val==LONG_MIN) || (col_val==LONG_MAX)) && (errno==ERANGE)) {
		return -2;

	/* unrecognized characters in string */
	} else if (*endptr != '\0') {
		return -3;

	/* good parse */
	} else {
		*resultptr = col_val;
		return 0;
	}
}


/* SQ_result_to_string() */
/*++++++++++++++++++++++++++++++++++++++
  Convert the result set to a string.

  SQ_result_set_t *result The results.
  
  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *SQ_result_to_string(SQ_result_set_t *result) {
  MYSQL_ROW row;
  unsigned int no_cols;
  unsigned int i, j;
  char str_buffer[STR_XXL];
  char str_buffer_tmp[STR_L];
  char border[STR_L];

  char *label;

  unsigned int length[STR_S];

  strcpy(str_buffer, "");

  no_cols = mysql_num_fields(result);

  /* Determine the maximum column widths */
  /* XXX Surely MySQL should keep note of this for me! */
  strcpy(border, "");
  for (i=0; i < no_cols; i++) {
    length[i] = SQ_get_column_max_length(result, i);
    /* Make sure the lenghts don't get too long */
    if (length[i] > STR_M) {
      length[i] = STR_M;
    }
    strcat(border, "*");
    for (j=0; (j <= length[i]) && (j < STR_L); j++) {
      strcat(border, "-");
    }
  }
  strcat(border, "*\n");
  /*
  for (i=0; i < no_cols; i++) {
    printf("length[%d]=%d\n", i, length[i]);
  }
  */

  strcat(str_buffer, border);

  for (i=0; i < no_cols; i++) {
    label = SQ_get_column_label(result, i);
    if (label != NULL) {
      sprintf(str_buffer_tmp, "| %-*s", length[i], label);
      strcat(str_buffer, str_buffer_tmp);
    }
  }
  strcat(str_buffer, "|\n");
  
  strcat(str_buffer, border);


  while ((row = mysql_fetch_row(result)) != NULL) {
    for (i=0; i < no_cols; i++) {
      if (row[i] != NULL) {
        sprintf(str_buffer_tmp, "| %-*s", length[i], row[i]);
      }
      else {
        sprintf(str_buffer_tmp, "| %-*s", length[i], "NuLL");
      }
      strcat(str_buffer, str_buffer_tmp);
    }
    strcat(str_buffer, "|\n");

    if (strlen(str_buffer) >= (STR_XXL - STR_XL) ) {
      strcat(str_buffer, "And some more stuff...\n");
      break;
    }
  }

  strcat(str_buffer, border);
  
  return UT_strdup(str_buffer);

} /* SQ_result_to_string() */

/* SQ_free_result() */
/*++++++++++++++++++++++++++++++++++++++
  Free the result set.

  SQ_result_set_t *result The results.
  
  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_free_result">mysql_free_result()</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
void SQ_free_result(SQ_result_set_t *result) {
  mysql_free_result(result);
} /* SQ_free_result() */


/* SQ_close_connection() */
/*++++++++++++++++++++++++++++++++++++++
  Call this function to close a connection to the server

  SQ_connection_t *sql_connection The connection to the database.
  
  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_close">mysql_close()</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
void SQ_close_connection(SQ_connection_t *sql_connection) {
	mysql_close(sql_connection);
}

/* SQ_num_rows() */
/*++++++++++++++++++++++++++++++++++++++
  Call this function to find out how many rows are in a query result

  SQ_result_set_t *result The results.
  
  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_num_rows">mysql_num_rows()</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
int SQ_num_rows(SQ_result_set_t *result) {
	if (result != NULL) return mysql_num_rows(result);
}

/* SQ_info_to_string() */
/*++++++++++++++++++++++++++++++++++++++
  Convert all available information about the sql server into a string.

  SQ_connection_t *sql_connection The connection to the database.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *SQ_info_to_string(SQ_connection_t *sql_connection) {
  GString *buf;
  char *str_tmp;
  char *result;

  buf = g_string_sized_new(STR_XXL);

  /* Makes the server dump debug information to the log. */
  g_string_sprintfa(buf, "mysql_dump_debug_info()=%d\n", 
                    mysql_dump_debug_info(sql_connection));

  /* Returns the error number from the last MySQL function. */
  g_string_sprintfa(buf, "mysql_errno()=%d\n", mysql_errno(sql_connection));

  /* Returns the error message from the last MySQL function. */
  g_string_sprintfa(buf, "mysql_error()=%s\n", mysql_error(sql_connection));

  /* Returns client version information. */
  g_string_sprintfa(buf, "mysql_get_client_info()=%s\n", 
                    mysql_get_client_info() );

  /* Returns a string describing the connection. */
  g_string_sprintfa(buf, "mysql_get_host_info()=%s\n",
                    mysql_get_host_info(sql_connection));

  /* Returns the protocol version used by the connection. */
  g_string_sprintfa(buf, "mysql_get_proto_info()=%d\n", 
                    mysql_get_proto_info(sql_connection));

  /* Returns the server version number. */
  g_string_sprintfa(buf, "mysql_get_server_info()=%s\n",
                    mysql_get_server_info(sql_connection));

  /* Information about the most recently executed query. */
  str_tmp = (char*)mysql_info(sql_connection);
  if (str_tmp != NULL) {
    g_string_sprintfa(buf, "mysql_info()=\"%s\"\n", str_tmp);
  } else {
    g_string_append(buf, "mysql_info()=NULL\n");
  }


  /* Returns a list of the current server threads. 

     NOT Used here, because it returns a RESULT struct that must be 
     iterated through.
     
     sprintf(str_buffer_tmp, "mysql_list_processes()=%x\n", mysql_list_processes(sql_connection));
     strcat(str_buffer, str_buffer_tmp);
     
  */

  /* Checks if the connection to the server is working. */
  g_string_sprintfa(buf, "mysql_ping()=%d\n", mysql_ping(sql_connection));

  /* Returns the server status as a string. */
  g_string_sprintfa(buf, "mysql_stat()=%s\n", mysql_stat(sql_connection));

  /* Returns the current thread id. */
  g_string_sprintfa(buf, "mysql_thread_id()=%ld\n",
                    mysql_thread_id(sql_connection));


  /* copy our string and return the copy */
  result = UT_strdup(buf->str);
  g_string_free(buf, TRUE);
  return result;

} /* SQ_info_to_string() */

/* SQ_error() */
/*++++++++++++++++++++++++++++++++++++++
  Get the error string for the last error.

  SQ_connection_t *sql_connection The connection to the database.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_error">mysql_error()</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *SQ_error(SQ_connection_t *sql_connection) {
	return (char*)mysql_error(sql_connection);
} /* SQ_error() */

/* SQ_errno() */
/*++++++++++++++++++++++++++++++++++++++
  Get the error number for the last error.

  SQ_connection_t *sql_connection The connection to the database.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_free_result">mysql_free_result()</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
int SQ_errno(SQ_connection_t *sql_connection) {

  return mysql_errno(sql_connection);

} /* SQ_errno() */

/************************************************************
 * get_minmax_id()                                           *
 *                                                           *
 * Returns the min or max ID of the table                    *
 *                                                           *
 * Returns:                                                  *
 *  min (max=0) or max (max=1) ID                            *
 *  -1 in case of an error                                   *
 *************************************************************/
long sq_get_minmax_id(SQ_connection_t *sql_connection, char *id_name, char *table, int max) {
	GString *query;
	SQ_result_set_t *sql_result;
	SQ_row_t *sql_row;
	char *sql_str;
	long id;
	char *minmax;
	int sql_err;

	query = g_string_sized_new(STR_M);

	if (max==1)
		minmax="max";
	else
		minmax="min";

	g_string_sprintf(query, "SELECT %s(%s) FROM %s ", minmax, id_name, table);

	sql_err = SQ_execute_query(sql_connection, query->str, &sql_result);

	if (sql_err) {
		LG_log(sq_context, LG_SEVERE, "%s[%s]\n", SQ_error(sql_connection), query->str);
		die;
	}

	if ((sql_row = SQ_row_next(sql_result)) != NULL) {
		sql_str = SQ_get_column_string(sql_result, sql_row, 0);

		/* We must process all the rows of the result,*/
		/* otherwise we'll have them as part of the next qry */
		while ( (sql_row = SQ_row_next(sql_result)) != NULL) {
			LG_log(sq_context, LG_SEVERE, "duplicate max [%s]\n", query->str);
			die;
		}
	} else
		sql_str=NULL;

	if (sql_result) {
		SQ_free_result(sql_result);
		sql_result=NULL;
	}

	if (sql_str) {
		id = atol(sql_str);
		UT_free(sql_str);
	}
	/* table is empty, max_id=min_id=0 */
	else
		id=0;

	/* free temporary space used for query */
	g_string_free(query, TRUE);

	return (id);
}


/* SQ_get_info() */
/*++++++++++++++++++++++++++++++++++++++
  Get additional information about the most 
  recently executed query.
  
  SQ_connection_t *sql_connection The connection to the database.
  int info[3] array of integers where information is stored
  
  The meaning of the numbers returned depends on the query type:
  
  info[SQL_RECORDS] - # of Records for INSERT
  info[SQL_MATCHES] - # of Matches for UPDATE
  info[SQL_DUPLICATES] - # of Duplicates
  info[SQL_WARNINGS] - # of Warnings
  
  More:
 +html+ <PRE>
 Authors:
  andrei
 +html+ </PRE><DL COMPACT>
 +html+ <DT>Online References:
 +html+ <DD><UL>
 +html+     <LI><A HREF="http://www.tcx.se/Manual/manual.html#mysql_info">mysql_info()</A>
 +html+ </UL></DL>

++++++++++++++++++++++++++++++++++++++*/  
  
int SQ_get_info(SQ_connection_t *sql_connection, int info[3])
{
int ii;
char *colon, *buf_ptr, buf[20]; 
char *infoline;

  infoline=(char*)mysql_info(sql_connection); 
  ii=0;
  colon = infoline;
  while (*colon != '\0') {
   colon++;
   buf_ptr=buf;
   if(isdigit((int)*colon)){
    while(isdigit((int)*colon)){
     *buf_ptr=*colon; buf_ptr++; colon++;
    }
    *buf_ptr='\0';
    info[ii]=atoi(buf); ii++;
   } 
  }
 return(0);
}


/* 
   open a connection with the same parameters

   by marek
*/
SQ_connection_t *
SQ_duplicate_connection(SQ_connection_t *orig)
{
  return SQ_get_connection(orig->host, orig->port, orig->db, 
			   orig->user, orig->passwd);
}

/* 
   abort the current query on the given connection

   by marek
*/
int
SQ_abort_query(SQ_connection_t *sql_connection)
{
  SQ_connection_t *contemp = SQ_duplicate_connection(sql_connection);
  int res = mysql_kill(contemp, sql_connection->thread_id);

  LG_log(sq_context, LG_DEBUG,
	    "connection %d aborted by tmp thread %d",
	    sql_connection->thread_id,
	    contemp->thread_id);

  SQ_close_connection(contemp);

  return res;
}

/* SQ_ping() */
/*++++++++++++++++++++++++++++++++++++++
  Checks whether or not the connection to the server is working. 
  If it has gone down, an automatic reconnection is attempted.

  Return values

  Zero if the server is alive. Non-zero if an error occurred.

  More:
  +html+ <PRE>
  Authors:
        andrei
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
int SQ_ping(SQ_connection_t *sql_connection)
{
	return(mysql_ping(sql_connection));
}

/* SQ_escape_string() */
/*++++++++++++++++++++++++++++++++++++++
  Returns a copy of the string passed that has been escaped so it
  may be safely used in SQL strings.

  Return values

  Escaped string (allocated memory which must be freed)

  More:
  +html+ <PRE>
  Authors:
        shane
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/

char *SQ_escape_string(SQ_connection_t *sql_connection, char *str) {
	char *new_str;
	int length;

	length = strlen(str);
	new_str = (char *)UT_malloc((length * 2) + 1);
	mysql_real_escape_string(sql_connection, new_str, str, length);
	return new_str;
}

