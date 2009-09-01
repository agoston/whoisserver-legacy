/***************************************
  $Revision: 1.14 $

  Functions to keep records for crash recovery

  Status: NOT REVUED, NOT TESTED

  Author(s):       Andrei Robachevsky

  ******************/ /******************
  Modification History:
        andrei (11/08/2000) Created.
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
#include "lg.h"

/* logging (defined in ud_main.c) */
extern LG_context_t *ud_context;


/*************************************************************

SQL Tables used to keep records needed for crash recovery

CREATE TABLE transaction_rec (
0  transaction_id int(11) DEFAULT '0' NOT NULL auto_increment,
1  object_id int(10) unsigned DEFAULT '0' NOT NULL,
2  sequence_id int(10) unsigned DEFAULT '1' NOT NULL,
3  object_type tinyint(3) unsigned DEFAULT '0' NOT NULL,
4  save varchar(256) DEFAULT '' NOT NULL,
5  error_script blob DEFAULT '' NOT NULL,
6  mode tinyint(4) unsigned DEFAULT '0' NOT NULL,
7  succeeded tinyint(4) unsigned DEFAULT '0' NOT NULL,
8  action tinyint(4) unsigned DEFAULT '0' NOT NULL,
9  status tinyint(10) unsigned DEFAULT '0' NOT NULL,
10  clean tinyint(3) DEFAULT '0' NOT NULL,
  PRIMARY KEY (transaction_id)
);



CREATE TABLE dummy_rec (
  transaction_id int(11) DEFAULT '0' NOT NULL,
  object_id int(10) unsigned DEFAULT '0' NOT NULL,
  PRIMARY KEY (transaction_id, object_id)
);

*************************************************************/

/************************************************************
* int TR_create_record()                                    *
*                                                           *
* Create TR record                                          *
*                                                           *
* First tries to delete record with the same transaction_id *
* ( transaction_id == tr->transaction_id )                  *
* Then creates a new record in transaction_rec table        *
*                                                           *
* Returns: transaction_id                                   *
*                                                           *
************************************************************/
 
long TR_create_record(Transaction_t *tr)
{
SQ_result_set_t *sql_result;
GString *query;
int sql_err;

 if(IS_STANDALONE(tr->mode)) return(0); /* for loader just return */

 query = g_string_sized_new(STR_L);
 /* delete record if exists*/
 
 TR_delete_record(tr);
 
 
 /* compose record */

 tr->action = TR_ACTION(tr->action) + TCP_ROLLBACK;
 
 g_string_sprintf(query, "INSERT transaction_rec "
                         "SET transaction_id=%ld, "
			 "object_id=%ld, "
			 "sequence_id=%ld, "
			 "object_type=%d, "
			 "mode=%d, "
			 "action=%d, "
			 "status=%d ",
			 tr->transaction_id, tr->object_id, tr->sequence_id, tr->class_type, tr->mode, TR_ACTION(tr->action), TR_STATUS(TCP_ROLLBACK));
 sql_err=SQ_execute_query(tr->sql_connection, query->str, &sql_result);
 
 
 /* in case of an error copy error code and return */ 
 if(sql_err) {
   LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), query->str);
   die;
 }
 g_string_free(query, TRUE);
 return(tr->transaction_id); 
}		


/************************************************************
* int TR_update_record()                                    *
*                                                           *
* UPdates TR record (transaction_rec or dummy_rec tables)   *
*                                                           *
* Updates the following fields:                             *
* TF_DUMMY - dummy_rec, adding ID's as dummies are created  *
* TF_SAVE  - writes down tr->save                           *
* TF_STATUS - updates status (checkpointing)                *
* TF_ESCRIPT - saves error script tr->error_script          *
*                                                           *
* Returns: transaction_id                                   *
*                                                           *
************************************************************/
 
long TR_update_record(Transaction_t *tr, int field)
{
SQ_result_set_t *sql_result;
GString *query;
int sql_err;
 
 if(IS_STANDALONE(tr->mode)) return(0); /* for loader just return */ 
 
 query = g_string_sized_new(STR_L);
 
 switch(field){
   case TF_DUMMY:
          g_string_sprintf(query, "INSERT dummy_rec "
                                  "SET transaction_id=%ld, "
				  "object_id=%ld ",
			           tr->transaction_id, tr->dummy_id[tr->ndummy-1]);
	  break;

   case TF_STATUS:
          g_string_sprintf(query, "UPDATE transaction_rec "
                                  "SET status=%d "
				  "WHERE transaction_id=%ld ",
			           TR_STATUS(tr->action), tr->transaction_id);
	  break;

   case TF_SAVE:
          g_string_sprintf(query, "UPDATE transaction_rec "
                                  "SET save='%s' "
				  "WHERE transaction_id=%ld ",
			           tr->save, tr->transaction_id);
	  break;

   case TF_ESCRIPT:
          g_string_sprintf(query, "UPDATE transaction_rec "
                                  "SET error_script='%s' "
				  "WHERE transaction_id=%ld ",
			           (tr->error_script)->str, tr->transaction_id);
	  break;
  
   case TF_ID:
          g_string_sprintf(query, "UPDATE transaction_rec "
                                  "SET object_id=%ld, sequence_id=%ld, serial_id=%ld, succeeded=%d "
				  "WHERE transaction_id=%ld ",
			           tr->object_id, tr->sequence_id, tr->serial_id, tr->succeeded, tr->transaction_id);
	  break;
  
   case TF_CLEAN:
          g_string_sprintf(query, "UPDATE transaction_rec "
                                  "SET clean=1 "
				  "WHERE transaction_id=%ld ",
			           tr->transaction_id);
	  break;
	  	  
  default: die; break;
 }

 sql_err=SQ_execute_query(tr->sql_connection, query->str, &sql_result);
 
 
 /* in case of an error copy error code and return */ 
 if(sql_err) {
   LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), query->str);
   die;
 }
 g_string_free(query, TRUE);
 return(tr->transaction_id); 
}

/* Query the database for transaction record */
/* if there is no record with the specified ID - this is a new transaction */
/************************************************************/ 
SQ_result_set_t *tr_get_sql_record(SQ_connection_t *sql_connection, long transaction_id)
{
SQ_result_set_t *sql_result;
GString *query;
int sql_err;
 
 query = g_string_sized_new(STR_L);
 
 /* compose query */
 if (transaction_id == TR_LAST)
  g_string_sprintf(query, "SELECT * FROM transaction_rec WHERE clean=%d", TCP_UNCLEAN);
 else 	 
  g_string_sprintf(query, "SELECT * FROM transaction_rec WHERE transaction_id=%ld", transaction_id);
 
 /* execute query */
 sql_err=SQ_execute_query(sql_connection, query->str, &sql_result);
 
 
/* in case of an error copy error code and return */ 
 if(sql_err) {
   LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(sql_connection), query->str);
   die;
 }
 g_string_free(query, TRUE);
 return(sql_result);
}


/************************************************************/
long tr_get_long(SQ_result_set_t *result, SQ_row_t *row, int col)
{
 long val;
 if( sscanf(SQ_get_column_string_nocopy(result, row, col), "%ld", &val) < 1 ) { die; }
 return(val);
}
/************************************************************/
int tr_get_int(SQ_result_set_t *result, SQ_row_t *row, int col)
{
 int val;
 if( sscanf(SQ_get_column_string_nocopy(result, row, col), "%d", &val) < 1 ) { die; }
 return(val);
}
/************************************************************/
char *tr_get_str(SQ_result_set_t *result, SQ_row_t *row, int col)
{
 return(SQ_get_column_string_nocopy(result, row, col));
}
/************************************************************/
int tr_get_dummies(Transaction_t *tr)
{
SQ_result_set_t *sql_result;
GString *query;
int sql_err;
SQ_row_t *sql_row;

 query = g_string_sized_new(STR_L);
 
 /* compose query */
 g_string_sprintf(query, "SELECT * FROM dummy_rec WHERE transaction_id=%ld", tr->transaction_id);
 sql_err=SQ_execute_query(tr->sql_connection, query->str, &sql_result);
 
 
 /* in case of an error copy error code and return */ 
 if(sql_err) {
   LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), query->str);
   die;
 }
 g_string_free(query, TRUE);
 
 tr->ndummy=0;
 while ( (sql_row = SQ_row_next(sql_result)) != NULL) {
   if( sscanf(SQ_get_column_string_nocopy(sql_result, sql_row, DUMMY_OBJECT_ID), "%ld", &(tr->dummy_id[tr->ndummy])) < 1 ) { die; }
   tr->ndummy++;
 }

 SQ_free_result(sql_result);
 return(tr->ndummy);	 	
}

/************************************************************
* Transaction_t * TR_get_record()                           *
*                                                           *
* Get the record left from the failed transaction           * 
* and fill the tr structure                                 *
*                                                           *
* The following fields from transaction are essential:      *
*                                                           *
* class_type                                                *
* action                                                    *
* object_id                                                 *
* sequesnce_id                                              *
* save                                                      *
* ndummy                                                    *
* dummy_id[]                                                *
* error_script                                              *
* transaction_id                                            *
*                                                           *
* The following fields are filled in by transaction_new()   *
* thread_upd                                                *
* thread_ins                                                *
* standalone                                                *
                                                            *
* Return codes:                                             *
*                                                           *
* NULL - everything is clean, no cleanup is needed             *
* 1 - the database was recovered successfully               *
*                                                           *
************************************************************/
Transaction_t *TR_get_record(SQ_connection_t *sql_connection, long transaction_id, LG_context_t *src_ctx)
{
  Transaction_t *tr;
  /* get the record from SQL table */
  SQ_result_set_t *result;
  SQ_row_t *row;
  C_Type_t class_type;
  int res;


  result = tr_get_sql_record(sql_connection, transaction_id);
  if (result == NULL) return (NULL); /* no further actions */
	  
  /* fill in the Transaction structure */
  if ((row = SQ_row_next(result))== NULL) {
    tr = NULL;
  }
  else {
    /* Check if there is more than one row */
    res = 0;
    while(SQ_row_next(result))res = -1;
    if(res == -1) die;
  

    class_type = tr_get_class_type(result, row);
    if ((tr = transaction_new(sql_connection, class_type, src_ctx)) == NULL) die;
    tr->transaction_id = tr_get_transaction_id(result, row);
    tr->object_id = tr_get_object_id(result, row);

    /* Fill in all dummies that were created */
    tr_get_dummies(tr);

    tr->sequence_id = tr_get_sequence_id(result, row); 
    tr->serial_id = tr_get_serial_id(result, row);
    tr->save = g_strdup(tr_get_save(result, row)); 
    g_string_sprintf(tr->error_script, tr_get_escript(result, row)); 


    /* mode of operation */
    tr->mode = tr_get_mode(result, row);
    /* indication of success */
    tr->succeeded = tr_get_success(result, row);
    /* action is low byte */
    tr->action = tr_get_action(result, row);
    /* status is high byte */
    tr->action |= (tr_get_status(result, row) <<8);
    tr->action |= (tr_get_clean(result, row) << 8); /* bit0 bears this flag */
  }

  SQ_free_result(result);
  return(tr);
}

/************************************************************
* int TR_delete_record()                                    *
*                                                           *
* Deletes all associated sql records                        *
*                                                           *
*                                                           *
************************************************************/
void TR_delete_record(Transaction_t *tr)
{
GString *query;
int sql_err;

  if(IS_STANDALONE(tr->mode)) return; /* for loader just return */
  
  /* Delete a record from SQL DB */
  query = g_string_sized_new(STR_L);
 
  /* compose query */
  g_string_sprintf(query, "DELETE FROM dummy_rec WHERE transaction_id=%ld", tr->transaction_id);
  sql_err=SQ_execute_query(tr->sql_connection, query->str, NULL);
  /* in case of an error copy error code and return */ 
  if(sql_err) {
   LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), query->str);
   die;
  }
  g_string_sprintf(query, "DELETE FROM transaction_rec WHERE transaction_id=%ld", tr->transaction_id);
  sql_err=SQ_execute_query(tr->sql_connection, query->str, NULL);
  /* in case of an error copy error code and return */ 
  if(sql_err) {
   LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), query->str);
   die;
  }

  g_string_free(query, TRUE);
 
}

/************************************************************
 * int TR_recover()                                          *
 *                                                           *
 * Cleans up the database after RIP daemon failure           *
 *                                                           *
 * Return codes:                                             *
 *                                                           *
 * 0 - everything is clean, no cleanup is needed             *
 * 1 - the database was recovered successfully               *
 *                                                           *
 ************************************************************/
int TR_recover(SQ_connection_t *sql_connection, ca_dbSource_t *source_hdl, LG_context_t *src_ctx)
{
    int res;
    Transaction_t * tr;
    char *act_m;

    /* XXX SQ_db_name() ? */
    fprintf(stderr, "Checking the Database [%s]...", sql_connection->db);

    /* Get the transaction record */
    /* XXX for NRTM we may specify transaction_id = 0 ? */
    if ((tr = TR_get_record(sql_connection, TR_LAST, src_ctx)) == NULL)
    {
        /* everything is clean */
        res = 0;
        fprintf(stderr, "[OK]\n");
        LG_log(ud_context, LG_INFO, "STATUS:[%s]=OK", sql_connection->db);
    }
    else
    {/* Not everything was perfect :( */
        tr->source_hdl = source_hdl; /* this is not filled in by TR_get_record */
        if (ACT_CREATE(tr->action))act_m = "CREATE";
        else if (ACT_UPDATE(tr->action))act_m = "UPDATE";
        else act_m = "DELETE";
        LG_log(ud_context, LG_INFO, "STATUS:[%s]=FAILED [object_id=%ld, sequence_id=%ld, serial_id=%ld, transaction_id=%ld, action=%s]",
               sql_connection->db, tr->object_id, tr->sequence_id, tr->serial_id, tr->transaction_id, act_m);
        fprintf(stderr, "[FAILED]\n"
                "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n"
                "+ LAST TRANSACTION IS INCOMPLETE. ENTERING CRASH RECOVERY MODE +\n"
                "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        /* Failure occured before the ack was sent */
        /* Roll back the transaction */
        /* Delete transaction record (TR) as if it never happened */
        /************************* R O L L B A C K ***************************/
        if (TS_ROLLBACK(tr->action))
        {
            fprintf(stderr, "  STATUS: Rollback\n");
            LG_log(ud_context, LG_INFO, "STATUS:[%s]=ROLLBACK", sql_connection->db);

            /* don't rollback the transaction if we were to delete the object, but could not */
            if (!TS_ROLLBACKED(tr->action))
            {
                fprintf(stderr, "  STATUS: Rollback incomplete, completing...");
                UD_rollback(tr);
                CP_ROLLBACK_PASSED(tr->action);
                TR_update_status(tr);
                fprintf(stderr, "[OK]\n");
                LG_log(ud_context, LG_INFO, "STATUS:[%s] Rollback incomplete, completing - OK", sql_connection->db);
            }
            else
            {
                fprintf(stderr, "  STATUS: Rollback complete [PASSED]\n");
                LG_log(ud_context, LG_INFO, "STATUS:[%s] Rollback complete - PASSED", sql_connection->db);
            }


            if (!TS_ROLLBACKED_NH(tr->action))
            {
                fprintf(stderr, "  STATUS: NH rollback incomplete, completing...");
                NH_rollback(tr->sql_connection);
                CP_ROLLBACK_NH_PASSED(tr->action);
                TR_update_status(tr);
                fprintf(stderr, "[OK]\n");
                LG_log(ud_context, LG_INFO, "STATUS:[%s] NH rollback incomplete, completing - OK", sql_connection->db);
            }
            else
            {
                fprintf(stderr, "  STATUS: NH rollback complete [PASSED]\n");
                LG_log(ud_context, LG_INFO, "STATUS:[%s] NH rollback complete - PASSED", sql_connection->db);
            }
            /* In update mode delete TR record. Next time (if any) DBupdate tries to submit, we'll start from scratch */
            /* In NRTM mode we create a serial record even in case of failure (tr->succeeded ==0)*/
            /* So in NRTM we need to clean up serials/transaction as well */
            if (IS_UPDATE(tr->mode))
            {
                fprintf(stderr, "  STATUS: Serial does not need to be restored, deleting TR...");
                TR_delete_record(tr);
                fprintf(stderr, "[OK]\n");
                LG_log(ud_context, LG_INFO, "STATUS:[%s] Serial does not need to be restored, deleting TR - OK", sql_connection->db);
            }
            else
            {
                fprintf(stderr, "  STATUS: Cleaning serial, deleting TR...");
                if (!TS_CREATED_S(tr->action))
                    UD_rollback_serial(tr);
                else
                    UD_commit_serial(tr);
                TR_delete_record(tr);
                fprintf(stderr, "[OK]\n");
                LG_log(ud_context, LG_INFO, "STATUS:[%s] Cleaning serial, deleting TR - OK", sql_connection->db);
            }

            res = 1;
        }
            /************************* C O M M I T ******************************/
        else
        { /* commit */
            /* The ack was sent */
            /* Complete the commit */
            fprintf(stderr, "  STATUS: Commit\n");
            LG_log(ud_context, LG_INFO, "STATUS:[%s]=COMMIT", sql_connection->db);
            /* We keep the transaction record in case DBupdate failed */
            /* and requests the same transaction after recovery ? */
            /* Such approach will allow us to avoid 3-way handshaking with DBupdate */
            /* So we never blocked or timed out during that phase */

            /* XXX But first I implemented another approach (to keep DB tiny/tidy): */
            /* 1. Process the transaction */
            /* 2. In case of failure - rollback - NACK */
            /* 3. Before commit - ACK (UD_ack()) */
            /* 4. If UD_ack returns an error preserve a tr_record */
            /* 5. Commit */
            /* 6. If still alive and UD_ack passed - delete the record - all is clean */
            /*    Otherwise preserve a tr_record */

            if (ACT_DELETE(tr->action))
            {
                /* check if we passed deletion process */
                if (!TS_DELETED(tr->action))
                {
                    fprintf(stderr, "  STATUS: Delete incomplete, completing...");
                    UD_delete(tr);
                    CP_DELETE_PASSED(tr->action);
                    TR_update_status(tr);
                    fprintf(stderr, "[OK]\n");
                    LG_log(ud_context, LG_INFO, "STATUS:[%s] Delete incomplete, completing - OK", sql_connection->db);
                }
                else
                {
                    fprintf(stderr, "  STATUS: Delete complete [PASSED]\n");
                    LG_log(ud_context, LG_INFO, "STATUS:[%s] Delete complete - PASSED", sql_connection->db);
                }
            }
            else
            { /* update or create */
                /* Check if we passed the deletion pass of commit */
                if (!TS_COMMITTED_I(tr->action))
                {
                    fprintf(stderr, "  STATUS: Commit phase I incomplete, completing...");
                    UD_commit_I(tr);
                    CP_COMMIT_I_PASSED(tr->action);
                    TR_update_status(tr);
                    fprintf(stderr, "[OK]\n");
                    LG_log(ud_context, LG_INFO, "STATUS:[%s] Commit phase I incomplete, completing - OK", sql_connection->db);
                }
                else
                {
                    fprintf(stderr, "  STATUS: Commit phase I complete [PASSED]\n");
                    LG_log(ud_context, LG_INFO, "STATUS:[%s] Commit phase I complete - PASSED", sql_connection->db);
                }
                /* Check if we passed the second pass of commit */
                if (!TS_COMMITTED_II(tr->action))
                {
                    fprintf(stderr, "  STATUS: Commit phase II incomplete, completing...");
                    UD_commit_II(tr);
                    CP_COMMIT_II_PASSED(tr->action);
                    TR_update_status(tr);
                    fprintf(stderr, "[OK]\n");
                    LG_log(ud_context, LG_INFO, "STATUS:[%s] Commit phase II incomplete, completing - OK", sql_connection->db);
                }
                else
                {
                    fprintf(stderr, "  STATUS: Commit phase II complete [PASSED]\n");
                    LG_log(ud_context, LG_INFO, "STATUS:[%s] Commit phase II complete - PASSED", sql_connection->db);
                }
            } /* end of delete, create, update specific operations */

            /* Check if we passed the NH repository commit */
            if (!TS_COMMITTED_NH(tr->action))
            {
                fprintf(stderr, "  STATUS: NH commit incomplete, completing...");
                NH_commit(tr->sql_connection);
                if (tr->x509_commit)
                { /* commit the keycert_id for X509 object */
                    NH_commit_keycert_id(tr->sql_connection);
                }
                CP_COMMIT_NH_PASSED(tr->action);
                TR_update_status(tr);
                fprintf(stderr, "[OK]\n");
                LG_log(ud_context, LG_INFO, "STATUS:[%s] NH commit incomplete, completing - OK", sql_connection->db);
            }
            else
            {
                fprintf(stderr, "  STATUS: NH commit complete [PASSED]\n");
                LG_log(ud_context, LG_INFO, "STATUS:[%s] NH commit complete - PASSED", sql_connection->db);
            }

            /* create serial file */
            if (!TS_CREATED_S(tr->action))
            {
                fprintf(stderr, "  STATUS: Serial rollback and restore...");
                UD_rollback_serial(tr);
                if (ACT_UPD_CLLPS(tr->action))
                { /* this is a collapsed update (DEL + ADD) */
                    tr->action = TA_DELETE;
                    UD_create_serial(tr);
                    tr->sequence_id++;
                    tr->action = TA_CREATE;
                    UD_create_serial(tr);
                }
                else if (ACT_UPD_DUMMY(tr->action))
                { /* this was a dummy update - we need only CREATE serial */
                    tr->action = TA_CREATE;
                    tr->sequence_id++; /* because in fact this is an update (sequence_id=2) */
                    UD_create_serial(tr);
                }
                else UD_create_serial(tr);
                CP_CREATE_S_PASSED(tr->action);
                TR_update_status(tr);
            }
            fprintf(stderr, "[OK]\n");
            LG_log(ud_context, LG_INFO, "STATUS:[%s] Serial rollback and restore - OK", sql_connection->db);
            UD_commit_serial(tr);

            fprintf(stderr, "  STATUS: Marking TR as clean...");
            TR_mark_clean(tr);

            fprintf(stderr, "[OK]\n");
            LG_log(ud_context, LG_INFO, "STATUS:[%s] Marking TR as clean - OK", sql_connection->db);
            res = 2;
        }
    }
    transaction_free(tr);
    fprintf(stderr, "  STATUS: The Database is clean \n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");
    LG_log(ud_context, LG_INFO, "STATUS:[%s]=CLEAN", sql_connection->db);

    return (res);
}

/************************************************************
* int TR_check()                                            *
*                                                           *
* Checks if the requested transaction has already been      *
* processed. This could happen when DBupdate crashes while  *
* RIPupdate successfully completes the transaction.         *
*                                                           *
* If this is the case, RIPupdate will return an ack to      *
* DBupdate as if the transaction was processed again        *
*                                                           *
* Return codes:                                             *
* 0 - everything is clean - this is a new transaction       *
* 1 - the stored transaction was re-played                  *
*                                                           *
************************************************************/
int 
TR_check (SQ_connection_t *sql_connection, long transaction_id, int sockfd, LG_context_t *src_ctx)
{
Transaction_t * tr;


 /* transaction_id == 0 means that only one record is maintained */
 /* therefore it is not possible to replay the transaction */
 /* and transaction_id does not uniquely identify the transaction */
 /* suitable for NRTM and for backwards compatibility */
 if(transaction_id <=0) return(0);
 /* Get the transaction record */
 /* XXX for NRTM we may specify transaction_id = 0 ? */
 if ((tr = TR_get_record(sql_connection, transaction_id, src_ctx)) == NULL) return(0); /* everything is clean */
 
 /* Check if the record is clean (it should be ) */
 /* that means that either the transaction finished normally */
 /* or crash recovery procedure cleaned up the database (and record as well ) */
 if (TS_CLEAN(tr->action)) {
   /* send an acknowledgement */
   /* XXX Wait for ack */
   /* XXX if ack is timed out just return, else delete the tr_record */
   /* if(UD_ack(tr)==0) TR_delete_record(tr); */

   /* Send an acknowledgement, append note that transaction was rerun */
   tr->socket=sockfd;
   g_string_sprintfa(tr->error_script,"I[%ld]: requested transaction was processed before\n", transaction_id);
   UD_ack(tr);
   LG_log(src_ctx, LG_INFO, "[%ld] requested transaction was processed before\n", transaction_id);   
   transaction_free(tr);
 }
 else {
         LG_log(ud_context, LG_SEVERE, "TR is not clean\n");
	 die; /* the record should be clean */ 
 }
 return(1);
}


