/***************************************
  $Revision: 1.1 $

  Near real-time mirror server module (pm).  NRTM  protocol.

  Status: NOT REVUED, NOT TESTED

  +html+ <DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL>
  +html+ </DL>
  +html+ <PRE>
  Author:
      andrei
  +html+ </PRE>
 
  ******************/ /******************
  Copyright (c) 2000                              RIPE NCC
 
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

#include <stdio.h>

/* defined in protocol_serials.c */
extern LG_context_t *pm_context;

void pm_get_source_info(GString *gbuff, ip_addr_t *client_address, char *source, ca_dbSource_t *source_hdl);

/************************************************************
* PM_get_minmax_serial()                                    *
*                                                           *
* Returns the min or max serial number.                     *
*                                                           *
* Returns:                                                  *
*  min (max=0) or max (max=1) serial number                 *
*  -1 in case of an error                                   *
*                                                           *
* Note:                                                     *
*  min serial= MIN(serial_id)+1                             *
*  MIN(serial_id) represents legacy RIPE.CURRENSERIAL       *
*  of the snapshot                                          *
*                                                           *
*************************************************************/
long PM_get_minmax_serial(SQ_connection_t *sql_connection, int max)
{
char query[STR_M];
SQ_result_set_t *sql_result;
SQ_row_t *sql_row;
char *sql_str;
long current_serial;
char *minmax;
int sql_err;

if(max==1)minmax="max"; else minmax="min";

    /* get the lock to ensure that queries are not stopped */
    /* which means access to the database is allowed */
    PW_record_query_start();

 sprintf(query, "SELECT %s(serial_id) FROM serials ", minmax);

//fprintf(stderr, "D:<get_field_str>:query: %s\n", query);
 sql_err = SQ_execute_query(sql_connection, query, &sql_result);
 
 if(sql_err) {
    LG_log(pm_context, LG_SEVERE, "%s[%s]", SQ_error(sql_connection), query);
    die;
 }
        
	 
 if ((sql_row = SQ_row_next(sql_result)) != NULL) {
	sql_str = SQ_get_column_string(sql_result, sql_row, 0);

     /* We must process all the rows of the result,*/
     /* otherwise we'll have them as part of the next qry */
	while ( (sql_row = SQ_row_next(sql_result)) != NULL) {
          LG_log(pm_context, LG_ERROR, "duplicate PK [%s]", query);
	  if(sql_str) UT_free(sql_str); sql_str=NULL;
	}
 }
 else sql_str=NULL;
 
 if(sql_result){ SQ_free_result(sql_result); sql_result=NULL; }
 
 if(sql_str) {
  current_serial = atol(sql_str);
  if(max!=1)current_serial++;  
  UT_free(sql_str);
 }
 else current_serial=-1;
 
    /* release the lock */
    PW_record_query_end();

 
 return(current_serial);
 
}

/************************************************************
* int atlast(long serial_number)
* -1 - sql error
*
***********************************************************/

static int atlast(SQ_connection_t *sql_connection, long serial_number)
{
char *sql_str;
char str_id[STR_S];
int atlast=-1;

  sprintf(str_id, "%ld", serial_number);
  sql_str= get_field_str(sql_connection, "atlast", "serials", "serial_id", str_id, NULL);
  if(sql_str) {
       	  atlast = atoi(sql_str);
       	  UT_free(sql_str);
  }
  
  return(atlast);

}


/************************************************************
* int getop(long serial_number)
* -1 - sql error
*
* **********************************************************/

static int getop(SQ_connection_t *sql_connection, long serial_number)
{
char *sql_str;
char str_id[STR_S];
int op=-1;

  sprintf(str_id, "%ld", serial_number);
  sql_str= get_field_str(sql_connection, "operation", "serials", "serial_id", str_id, NULL);
  if(sql_str) {
       	  op = atoi(sql_str);
       	  UT_free(sql_str);
  }
  
  return(op);

}


/************************************************************
* char *PM_get_serial_object()                                 *
*                                                           *
* Returns text block corresponding to the requested serial  *
*                                                           *
* Returns:                                                  *
*  operation (ADD/DEL) and text object                      *
*  NULL in case of an error                                 *
*                                                           *
* Note:                                                     *
*  returned string should be freed by the caller            *
*                                                           *
*************************************************************/
char *PM_get_serial_object(SQ_connection_t *sql_connection, long serial_number, int *operation)
{
char *table;
SQ_result_set_t * sql_result;
SQ_row_t *sql_row;
char *sql_str;
char query[STR_M];
int sql_err;
int location;

    /* get the lock to ensure that queries are not stopped */
    /* which means access to the database is allowed */
    PW_record_query_start();

  switch(location=atlast(sql_connection, serial_number)){
  
   case 0: table="history";
           break;
   case 1: table="last";   
           break;
   case 2: table="failed_transaction";
           break;	   
   default: return(NULL);   
      
  }

  if(location == 2) 
     sprintf(query, "SELECT object FROM %s "
                    "WHERE serial_id=%ld ",
		    table, serial_number);
  else	  
     sprintf(query, "SELECT %s.object FROM %s, serials "
                    "WHERE serials.serial_id=%ld "
                    "AND serials.object_id=%s.object_id "
                    "AND serials.sequence_id=%s.sequence_id ", table, table, serial_number, table, table);
                 

 sql_err = SQ_execute_query(sql_connection, query, &sql_result);
 
 if(sql_err) {
    LG_log(pm_context, LG_SEVERE, "%s[%s]", SQ_error(sql_connection), query);
    die;
 }
        
	 
 if ((sql_row = SQ_row_next(sql_result)) != NULL) {
	sql_str = SQ_get_column_string(sql_result, sql_row, 0);

     /* We must process all the rows of the result,*/
     /* otherwise we'll have them as part of the next qry */
	while ( (sql_row = SQ_row_next(sql_result)) != NULL) {
          LG_log(pm_context, LG_ERROR, "duplicate PK [%s]", query);
	  if(sql_str) UT_free(sql_str); sql_str=NULL;
	}
 }
 else sql_str=NULL;
 
 if(sql_result){ SQ_free_result(sql_result); sql_result=NULL; }
 
 *operation=getop(sql_connection, serial_number);
 
    /* release the lock */
    PW_record_query_end();
 
 return(sql_str);
 
}

/************************************************************
* void pm_get_source_info()                                 *
*                                                           *
* Fills supplied buffer with information about the source   *
*                                                           *
* Returns text block corresponding to the requested source  *
* Format:                                                   *
* <source>:<can_mirror>:min_serial-max_serial               *
* source - name of the source (e.g. RIPE, RADB, etc.)       *
* can_mirror                                                * 
*    'Y' if the client is allowed to mirror the source      *
*    'N' if not                                             *
*    'N' if there is no serials (then the range starts at 0)*    
*                                                           * 
*                                                           *
*************************************************************/
void pm_get_source_info(GString *gbuff, ip_addr_t *client_address, char *source, ca_dbSource_t *source_hdl)
{

char *db_host = ca_get_srcdbmachine(source_hdl);
int   db_port = ca_get_srcdbport(source_hdl);
char *db_name = ca_get_srcdbname(source_hdl);
char *db_user = ca_get_srcdbuser(source_hdl);
char *db_passwd = ca_get_srcdbpassword(source_hdl);
int version = ca_get_srcnrtmprotocolvers(source_hdl);
SQ_connection_t *db_connection;
long min_serial, max_serial;
char can_mirror;
  
      /* Connect to the database */
       db_connection=SQ_get_connection(db_host, db_port, db_name, db_user, db_passwd);
       min_serial=PM_get_oldest_serial(db_connection);
       max_serial=PM_get_current_serial(db_connection) - SAFE_BACKLOG;
       
       /* If it cannot be morrored at all - N, but range starts with 0 */
       /* If the client is allowed to mirror - Y         */
       /* Otherwise - N                                  */
       if(min_serial>max_serial) {
	       can_mirror='N';
	       min_serial=0;
       }
       else {
	       if(AA_can_mirror(client_address, source )) can_mirror='Y';
               else can_mirror='N';
       }	
       g_string_sprintfa(gbuff, "%s:%d:%c:%lu-%lu\n", source, version, can_mirror, min_serial, max_serial);
       
       UT_free(db_host);
       UT_free(db_name);
       UT_free(db_user);
       UT_free(db_passwd);
       SQ_close_connection(db_connection);
}

/************************************************************
* GString *PM_get_nrtm_sources()                            *
*                                                           *
* Fills supplied buffer with information about the sources  *
*                                                           *
*                                                           * 
* Note:                                                     *
*  returned GString should be freed by the caller           *
*                                                           *
*************************************************************/
GString *PM_get_nrtm_sources(ip_addr_t *client_address, char *source)
{
GString *gbuff=g_string_sized_new(STR_L);
int nsource;
ca_dbSource_t *source_hdl;

   if(source){
      source_hdl = ca_get_SourceHandleByName(source);
      if (source_hdl)pm_get_source_info(gbuff, client_address, source, source_hdl);
   } else 
      for(nsource=0; (source_hdl = ca_get_SourceHandleByPosition(nsource))!=NULL ; nsource++){
       source=ca_get_srcname(source_hdl);
       pm_get_source_info(gbuff, client_address, source, source_hdl);
       UT_free(source);
   }
   /* one extra line, another one will be put bt PW or PM */
   g_string_sprintfa(gbuff, "\n");
   return(gbuff);
}
