/***************************************

  Functions for handling serials  

  Status: NOT REVUED, NOT TESTED

 Author(s):       Andrei Robachevsky

  ******************/ /******************
  Modification History:
        andrei (08/02/2000) Created.
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

/************************************************************
* int UD_lock/unlock_serial()                               *
*                                                           *
* Performs lockind/unlocking of the relevant tables         *
*                                                           *
* Returns:                                                  *
* 0 - success                                               *
* Non-zero if error occured (XXX dies now)                  *
*                                                           *
************************************************************/
/* FIXME: this is surplus locking, should be removed. With innodb,
 * there is no reason to lock those tables. Not even with myisam:
 * the auto_increment pkey would take care of this.
 * On the other hand, it kills any sort of parallelism, which we aim for.
 * agoston, 2008-02-06 */
int UD_lock_serial(Transaction_t *tr)
{
int sql_err;

/* lock all tables we are going to update and commit */	
/* this also includes transaction_rec table, as we update the status */
   sql_err=SQ_execute_query(tr->sql_connection, "LOCK TABLES serials WRITE, failed_transaction WRITE, transaction_rec WRITE ", NULL);
   if (sql_err) { 
	LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), "LOCK TABLES serials WRITE, failed_transaction WRITE, transaction_rec WRITE ");
        die;
    }
 return(sql_err);
}

int UD_unlock_serial(Transaction_t *tr)
{
int sql_err;
	
   sql_err=SQ_execute_query(tr->sql_connection, "UNLOCK TABLES ", NULL);
   if (sql_err) { 
	LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), "UNLOCK TABLES");
        die;
    }
 return(sql_err);
}
/************************************************************
 * UD_create_serial()                                        *
 *                                                           *
 * Creates a serial record for given transaction             *
 *                                                           *
 * Important fields of transaction are:                      *
 * tr->action        TR_CREATE/TR_UPDATE/TR_DELETE           *
 * tr->object_id     should be filled in                     *
 * tr->sequence_id   should be set to object updated         *
 *                                                           *
 * So given object with id=k and seq=n                       *
 * Create:  ADD(k,n)                                         *
 * Update:  ~S(k,n), ADD(k,n+1)                              *
 * Delete:  ~S(k,n), DEL(k,n)                                *
 *                                                           *
 * Returns:                                                  *
 *  current serial number.                                   *
 *  -1 in case of an error                                   *
 *                                                           *
 *************************************************************/

long UD_create_serial(Transaction_t *tr)
{
    int sql_err;
    int operation;
    long timestamp;
    long sequence_id;

    /* Calculate the object_id - should be max+1 */
    /* XXX we cannot use autoincrement with MyISAM tables */
    /* XXX because they keep the max inserted id even if  */
    /* XXX it was deleted later, thus causing gaps we don't want */
    tr->serial_id = SQ_get_max_id(tr->sql_connection, "serial_id", "serials") + 1;

    /* if we have a transaction_id provided in NRTM mode, try using that instead of coming up with one */
    if (IS_NRTM_CLNT(tr->mode) && (tr->transaction_id > 0))
    {
        if (tr->serial_id >= tr->transaction_id)
        {
            /* the serial_id we received is lower than the one already in the database - serious problem!
             * we shouldn't continue this operation */
            LG_log(ud_context, LG_FATAL, "UD_create_serial(): got transaction_id %ld, but already got %ld in the DB\n",
                   tr->transaction_id, tr->serial_id);
            die;
        }

        /* overrule normal serial_id */
        tr->serial_id = tr->transaction_id;
    }

    /* if the transaction failed store it in transaction table */
    if (tr->succeeded == 0)
    {
        if (ACT_DELETE(tr->action))operation = OP_DEL;
        else operation = OP_ADD;

        g_string_sprintf(tr->query, "INSERT serials SET "
                         "thread_id=%d, serial_id=%ld, object_id=%ld, sequence_id=0, "
                         "atlast=2, "
                         "operation=%d ", tr->thread_ins, tr->serial_id, tr->object_id, operation);

        sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **) NULL);

        if (sql_err)
        {
            LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
            die;
        }
        else
        {
            timestamp = time(NULL);
            g_string_sprintf(tr->query, "INSERT failed_transaction SET "
                             "thread_id=%d, serial_id=%ld, timestamp=%ld, "
                             "object='%s' ", tr->thread_ins, tr->serial_id, timestamp, tr->object_txt);
            /* make a record in transaction table */
            sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **) NULL);
            if (sql_err)
            {
                LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
                die;
            }
        }
        return (tr->serial_id);
    }


    /* if the transaction has succeeded */
    sequence_id = tr->sequence_id;
    /* If this is an update or delete */
    if (!ACT_CREATE(tr->action))
    {
        /* Increase the sequence_id so we insert correct ADD serial in case of Update */
        sequence_id = tr->sequence_id + 1;

        /* set the atlast field of the latest record for this object to 0 */
        /* because it is moved to history */
        g_string_sprintf(tr->query, "UPDATE serials SET atlast=0, thread_id=%d "
                         "WHERE object_id=%ld "
                         "AND sequence_id=%ld ", tr->thread_upd, tr->object_id, sequence_id - 1);

        sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **) NULL);
        if (sql_err)
        { // we can have empty updates, but not errors
            LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
            die;
        }
    }
    /* XXX below is a code for protocol v2, where updates are atomic */
    /* XXX this is fine (and should always be used) for NRTM, since we */
    /* XXX store failed transactions and playback stream exactly as it comes */
    /* XXX However, for update this may be configurable option */
    /* XXX In case v1 protocol both sections (DEL + ADD) should be executed */

    /* get the next serial_id */
    /* XXX we cannot use autoincrement with MyISAM tables */
    /* XXX because they keep the max inserted id even if  */
    /* XXX it was deleted later, thus causing gaps we don't want */
    tr->serial_id = SQ_get_max_id(tr->sql_connection, "serial_id", "serials") + 1;

    /* if this a DEL */


    if (ACT_DELETE(tr->action))
    {
        /* generate DEL serial */

        g_string_sprintf(tr->query, "INSERT serials SET "
                         "thread_id=%d, serial_id=%ld, object_id=%ld, "
                         "sequence_id=%ld, "
                         "atlast=0, "
                         "operation=%d ", tr->thread_ins, tr->serial_id, tr->object_id, sequence_id - 1, OP_DEL);

        sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **) NULL);
        if (sql_err)
        {
            LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
            die;
        }

    }
    else
    { /* otherwise this is an ADD */

        /* now insert creation serial */
        g_string_sprintf(tr->query, "INSERT serials SET "
                         "thread_id=%d, serial_id=%ld, object_id=%ld, "
                         "sequence_id=%ld, "
                         "atlast=1, "
                         "operation=%d ", tr->thread_ins, tr->serial_id, tr->object_id, sequence_id, OP_ADD);

        sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **) NULL);
        if (sql_err)
        {
            LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
            die;
        }

    }
    return (tr->serial_id);
}


/************************************************************
* UD_comrol_serial()                                        *     
*                                                           *
* Commits/Rollbacks a serial record for given transaction   *
* Returns:                                                  *
* 0 in success                                              *
*  -1 in case of an error                                   *
*                                                           *
*************************************************************/

char *Q_rollback_serial1="DELETE FROM serials WHERE thread_id=%ld ";
char *Q_rollback_serial2="UPDATE serials SET atlast=1, thread_id=0 WHERE thread_id=%ld ";
char *Q_rollback_transaction="DELETE FROM failed_transaction WHERE thread_id=%ld ";
char *Q_commit_serial="UPDATE serials SET thread_id=0 WHERE thread_id=%ld OR thread_id=%ld ";
char *Q_commit_transaction="UPDATE failed_transaction SET thread_id=0 WHERE thread_id=%ld ";



int UD_comrol_serial(Transaction_t *tr, int commit)
{
int sql_err;
char *Q_transaction;

   /* check if something is left in serials from the crash */
   
   /* compose the appropriate query depending on operation (commit/rollback) */
   if(commit) {
	   /* commit changes to serials table */
	   g_string_sprintf(tr->query, Q_commit_serial, tr->thread_ins, tr->thread_upd);
	   sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
           if (sql_err) {
	     LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
             die;
           }
	   Q_transaction=Q_commit_transaction;
   } else {
	   /* delete new insertions */
	   g_string_sprintf(tr->query, Q_rollback_serial1, tr->thread_ins);
	   sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
           if (sql_err) {
	     LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
             die;
           }
	   /* restore modified atlast */ 
           g_string_sprintf(tr->query, Q_rollback_serial2, tr->thread_upd);
	   sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
           if (sql_err) {
	     LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
             die;
           } 
	   Q_transaction=Q_rollback_transaction;
   }
   
    /* clean up transaction  table */
    g_string_sprintf(tr->query, Q_transaction, tr->thread_ins);
    sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
    if (sql_err) {
	LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
        die;
    }
 return(0);
} 
    

