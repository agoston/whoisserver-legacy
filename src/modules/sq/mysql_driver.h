/***************************************
  $Revision: 1.28 $

  SQL module (sq) - MySQL implementation of SQL driver.

  Status: NOT REVUED, NOT TESTED

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
#ifndef READ_MYSQL_DRIVER
#define READ_MYSQL_DRIVER

#include "mysql.h"
#include "mysqld_error.h"
#include "lg.h"
#include "ca_defs.h"

/* types for mysql_info */
#define SQL_RECORDS 0
#define SQL_MATCHES 0 // for UPDATE queries (checking for duplicates)
#define SQL_DUPLICATES 1
#define SQL_WARNINGS 2

/* errors */
#define SQ_OK       0
#define SQ_CTCONN   1
#define SQ_NORES    2

#define SQ_connection_t MYSQL
#define SQ_result_set_t MYSQL_RES

#define SQ_row_t MYSQL_ROW

#ifdef __cplusplus
extern "C" {
#endif



int SQ_try_connection(SQ_connection_t **conn, const char *host,
                           unsigned int port, const char *db,
		           const char *user, const char *password);
SQ_connection_t *SQ_get_connection(const char *host, unsigned int port, const char *db, const char *user, const char *password);
int SQ_execute_query(SQ_connection_t *sql_connection,
		     const char *query, SQ_result_set_t **result_ptr);
int SQ_execute_query_nostore(SQ_connection_t *sql_connection,
                             const char *query, SQ_result_set_t **result_ptr);
int SQ_get_column_count(SQ_result_set_t *result);
char *SQ_get_column_label(SQ_result_set_t *result, unsigned int column);
unsigned int SQ_get_column_max_length(SQ_result_set_t *result, unsigned int column);
SQ_row_t *SQ_row_next(SQ_result_set_t *result);
char *SQ_get_column_string(SQ_result_set_t *result, SQ_row_t *current_row, unsigned int column);
char *SQ_get_column_string_nocopy(SQ_result_set_t *result,
				  SQ_row_t *current_row,
				  unsigned int column);
char *SQ_get_column_strings(SQ_result_set_t *result, unsigned int column);
int SQ_get_column_int(SQ_result_set_t *result, SQ_row_t *current_row, unsigned int column, long  *resultptr);
int SQ_get_column_unsigned(SQ_result_set_t *result, SQ_row_t *current_row, unsigned int column, unsigned *resultptr);
int SQ_get_column_llint(SQ_result_set_t *result, SQ_row_t *current_row, unsigned int column, long long int *resultptr);
char *SQ_result_to_string(SQ_result_set_t *result);
void SQ_free_result(SQ_result_set_t *result);
void SQ_close_connection(SQ_connection_t *sql_connection);

/* report number of rows */
int SQ_num_rows(SQ_result_set_t *result);
int SQ_get_table_size(SQ_connection_t *sql_connection, char *table);
int SQ_get_affected_rows(SQ_connection_t *sql_connection);
long SQ_get_insert_id(SQ_connection_t *sql_connection);


char *SQ_info_to_string(SQ_connection_t *sql_connection);
char *SQ_error(SQ_connection_t *sql_connection);
int SQ_errno(SQ_connection_t *sql_connection);
int SQ_get_info(SQ_connection_t *sql_connection, int info[3]);
SQ_connection_t *SQ_duplicate_connection(SQ_connection_t *orig);
int SQ_abort_query(SQ_connection_t *sql_connection);
int SQ_ping(SQ_connection_t **sql_connection);
char *SQ_escape_string(SQ_connection_t *sql_connection, char *str);

long sq_get_minmax_id(SQ_connection_t *sql_connection, char *id_name, char *table, int max);
void SQ_init (LG_context_t *ctx);

#define SQ_get_max_id(conn, id, table) sq_get_minmax_id(conn, id, table, 1)
#define SQ_get_min_id(conn, id, table) sq_get_minmax_id(conn, id, table, 0)

SQ_connection_t *SQ_get_connection_by_source_name(char *source);
SQ_connection_t *SQ_get_connection_by_source_hdl(ca_dbSource_t *source_hdl);


#ifdef __cplusplus
}
#endif


#endif /* READ_MYSQL_DRIVER */
