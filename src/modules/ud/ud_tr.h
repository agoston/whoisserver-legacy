/***************************************
  $Revision: 1.7 $

  Semi-internal header file for UD module

  Status: NOT REVUED, NOT TESTED

 Author(s):       Andrei Robachevsky

  ******************/ /******************
  Modification History:
        andrei (17/01/2000) Created.
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
#ifndef _UD_TR_H
#define _UD_TR_H


#include "ud_int.h"
#include "lg.h"

/*

SQL Tables used to keep records needed for crash recovery

CREATE TABLE transaction_rec (
0  transaction_id int(11) DEFAULT '0' NOT NULL auto_increment,
1  object_id int(10) unsigned DEFAULT '0' NOT NULL,
2  sequence_id int(10) unsigned DEFAULT '1' NOT NULL,
3  serial_id int(10) unsigned DEFAULT '1' NOT NULL,
4  object_type tinyint(3) unsigned DEFAULT '0' NOT NULL,
5  save varchar(256) DEFAULT '' NOT NULL,
6  error_script blob DEFAULT '' NOT NULL,
7  mode tinyint(4) unsigned DEFAULT '0' NOT NULL,
8  succeeded tinyint(4) unsigned DEFAULT '0' NOT NULL,
9  action tinyint(4) unsigned DEFAULT '0' NOT NULL,
10 status tinyint(10) unsigned DEFAULT '0' NOT NULL,
11 clean tinyint(3) DEFAULT '0' NOT NULL,
  PRIMARY KEY (transaction_id)
);

CREATE TABLE dummy_rec (
  transaction_id int(11) DEFAULT '0' NOT NULL,
  object_id int(10) unsigned DEFAULT '0' NOT NULL,
  PRIMARY KEY (transaction_id, object_id)
);
*/

#define DUMMY_OBJECT_ID  1

#define tr_get_transaction_id(result, row)  tr_get_long(result, row, 0);
#define tr_get_object_id(result, row)       tr_get_long(result, row, 1)
#define tr_get_sequence_id(result, row)     tr_get_long(result, row, 2)
#define tr_get_serial_id(result, row)       tr_get_long(result, row, 3)
#define tr_get_class_type(result, row)      (C_Type_t)tr_get_int(result, row, 4)
#define tr_get_save(result, row)            tr_get_str(result, row, 5)
#define tr_get_escript(result, row)         tr_get_str(result, row, 6)
#define tr_get_mode(result, row)            tr_get_int(result, row, 7)
#define tr_get_success(result, row)         tr_get_int(result, row, 8)
#define tr_get_action(result, row)          tr_get_int(result, row, 9)
#define tr_get_status(result, row)          tr_get_int(result, row, 10)
#define tr_get_clean(result, row)           tr_get_int(result, row, 11)


#define TR_update_status(tr)  TR_update_record(tr, TF_STATUS)
#define TR_update_save(tr)    TR_update_record(tr, TF_STATUS)
#define TR_update_dummy(tr)   TR_update_record(tr, TF_DUMMY)
#define TR_update_escript(tr) TR_update_record(tr, TF_ESCRIPT)
#define TR_update_id(tr)      TR_update_record(tr, TF_ID)
#define TR_mark_clean(tr)     TR_update_record(tr, TF_CLEAN)






/* Transaction Status & CheckPoints
   76543210
      21NRC
*/

#define TCP_UNCLEAN         0x0000
#define TCP_CLEAN           0x0100
#define TCP_ROLLBACK        0x0200
#define TCP_COMMIT_NH       0x0400
#define TCP_ROLLBACK_NH     TCP_COMMIT_NH
#define TCP_COMMIT_I        0x0800
#define TCP_ROLLBACKED      TCP_COMMIT_I
#define TCP_COMMIT_II       0x1000
#define TCP_DELETE          0x2000
#define TCP_CREATE_S        0x4000

#define TS_CLEAN(a)               ((a)&TCP_CLEAN)
#define TS_ROLLBACK(a)            ((a)&TCP_ROLLBACK)
#define TS_ROLLBACKED(a)          ((a)&TCP_ROLLBACKED)
#define TS_COMMITTED_NH(a)        ((a)&TCP_COMMIT_NH)
#define TS_ROLLBACKED_NH(a)       ((a)&TCP_ROLLBACK_NH)
#define TS_COMMITTED_I(a)         ((a)&TCP_COMMIT_I)
#define TS_COMMITTED_II(a)        ((a)&TCP_COMMIT_II)
#define TS_DELETED(a)             ((a)&TCP_DELETE)
#define TS_CREATED_S(a)          ((a)&TCP_CREATE_S)

#define TR_ACTION(a)              ((a)&0x00FF)
#define TR_STATUS(a)              ((a)>>8)

/* some useful macros for checkpointing (CP)*/

#define CP_ROLLBACK(a)         (a)=(TR_ACTION(a) + TCP_ROLLBACK)
#define CP_COMMIT(a)           (a)= TR_ACTION(a)
#define CP_ROLLBACK_PASSED(a)  ((a)|=TCP_ROLLBACKED)
#define CP_DELETE_PASSED(a)    ((a)|=TCP_DELETE) 
#define CP_COMMIT_NH_PASSED(a) ((a)|=TCP_COMMIT_NH)
#define CP_ROLLBACK_NH_PASSED(a) ((a)|=TCP_ROLLBACK_NH)
#define CP_COMMIT_I_PASSED(a)  ((a)|=TCP_COMMIT_I)
#define CP_COMMIT_II_PASSED(a) ((a)|=TCP_COMMIT_II)
#define CP_CREATE_S_PASSED(a)  ((a)|=TCP_CREATE_S)
#define CP_CLEAN(a)            ((a)|=TCP_CLEAN)


/* Transaction record fields */
enum {
 TF_STATUS=1,
 TF_CLEAN,
 TF_DUMMY,
 TF_SAVE,
 TF_ESCRIPT,
 TF_ID
};

/* for crash recovery we need to handle only the transaction that failed */
#define TR_LAST  (-1)

long TR_create_record(Transaction_t *tr);
long TR_update_record(Transaction_t *tr, int field);
Transaction_t *TR_get_record(SQ_connection_t *sql_connection, long transaction_id, LG_context_t *src_ctx);
void TR_delete_record(Transaction_t *tr);
int TR_recover(SQ_connection_t *sql_connection, ca_dbSource_t *source_hdl, LG_context_t *src_ctx);
int TR_check(SQ_connection_t *sql_connection, long transaction_id, int sockfd, LG_context_t *src_ctx);
SQ_result_set_t *tr_get_sql_record(SQ_connection_t *sql_connection, long transaction_id);
long tr_get_long(SQ_result_set_t *result, SQ_row_t *row, int col);
int tr_get_int(SQ_result_set_t *result, SQ_row_t *row, int col);
char *tr_get_str(SQ_result_set_t *result, SQ_row_t *row, int col);
int tr_get_dummies(Transaction_t *tr);


#endif /* _UD_TR_H */



