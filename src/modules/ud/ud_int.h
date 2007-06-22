/***************************************
  $Revision: 1.2 $

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
#ifndef _UD_INT_H
#define _UD_INT_H

#include <stdio.h>
#include <strings.h>
#include <glib.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "defs.h"
#include "mysql_driver.h"
#include "iproutines.h"
#include "rp.h"
#include "nh.h"
#include "sk.h"
#include "memwrap.h"

#include "ca_configFns.h"
#include "ca_dictionary.h"
#include "ca_macros.h"
#include "ca_srcAttribs.h"

#include "globaldefs.h"
#include "bitmask.h"
#include "which_keytypes.h"

#include "lg.h"


/*+ String sizes +*/
#define STR_S   63
#define STR_M   255
#define STR_L   1023
#define STR_XL  4095
#define STR_XXL 16383
#define STR_XXXL 65535


#define MAX_NIC_HDL  30



#define MAX_DUMMIES 256

typedef struct _Transaction {
  int thread_ins;      /* Thread number to mark(backup) creates */
  int thread_upd;      /* Thread number to mark(backup) updates */
  SQ_connection_t *sql_connection;
  rpsl_object_t *object;    /* ptr to the object */
  C_Type_t class_type;
  gchar *object_txt; /* textual representation (as submitted) */
  long object_id;      /* Object id which the transaction is carried out for */
  long sequence_id;    /* for last and history tables */
  long serial_id;      /* serial ID, kept for crash recovery */
  long transaction_id; /* ID of DBuopdate transaction */
  int ndummy; /* number of attempts to create dummy */
  long dummy_id[MAX_DUMMIES]; /* to store object_id's of dummies */
  int load_pass; /* No. of the pass for initial loading , 0 otherwise */
  /* XXX later we should merge 'succeeded' & 'error' into single 'succeeded' bitmask */
  int action; /* indicates wether we are updating/deleting creating a new object */
  int mode; /* indicates mode of operation (standalone, dummy, update, nrtm) */
  int succeeded;
  int error;
  GString *error_script;
  GString *query;
  GString *K; /* object primary key */
  char *save; /* for saving attribute (U2 & pn:) values */ 
  rp_upd_pack_t *packptr; /* To store radix tree data */
  nic_handle_t *nh; /* To store NIC handle data */
  long keycert_id; /* To store KEY index */
  int x509_commit; /* To indicate if any keycert_id has to be commited: true if yes, false if no */
  ca_dbSource_t *source_hdl;
  int socket; /* socket to report to DBupdate */

  LG_context_t *src_ctx;
} Transaction_t;

/*++++++++++++++++++++++++++++++++++++++++++++*/


struct _nrtm {
 char *server; /* the hostname of the server */
 int port;
 int version;
 long current_serial;
 Transaction_t *tr;
 char object_name[STR_XXL];
 int op;
};
       
typedef struct _Log_t {
 int num_ok;
 int num_failed;
/* FILE *logfile; */
} Log_t;

typedef struct UD_stream_t_ {
 FILE * stream;
 sk_conn_st condat;
 int num_skip;
 int load_pass;
 int ud_mode;
 struct _nrtm *nrtm;
 SQ_connection_t *db_connection;
 ca_dbSource_t *source_hdl; /* source we are mirroring */
 Log_t log; 
 char *error_script;
} UD_stream_t;
         



/*++++++++++++++++++ Function Prototypes +++++++++++++++++++++*/

int get_NRTM_fd(struct _nrtm *nrtm, int upto_last, char *source);

int UD_process_stream(UD_stream_t *ud_stream, LG_context_t *src_ctx);

int object_process(Transaction_t *tr);

int UD_commit(Transaction_t *tr);
int UD_commit_I(Transaction_t *tr);
int UD_commit_II(Transaction_t *tr);


int UD_rollback(Transaction_t *tr);

int UD_delete(Transaction_t *tr);

int UD_update_rx(Transaction_t *tr, rx_oper_mt mode);

int UD_ack(Transaction_t* tr);
int UD_check_ref(Transaction_t *tr);

int isdummy(Transaction_t *tr);
long get_object_id(Transaction_t *tr);
long get_sequence_id(Transaction_t *tr);
char *get_field_str(SQ_connection_t *sql_connection, const char *field,
                    const char *ref_tbl_name, const char *ref_name,
                    const char * attr_value,  const char *condition);
char *get_qresult_str(SQ_connection_t *sql_connection, char *query);                 

int ud_build_lock_query(Transaction_t *tr, char **common_tables);

void transaction_free(Transaction_t *tr);

Transaction_t *transaction_new(SQ_connection_t *sql_connection, C_Type_t class_type, LG_context_t *src_ctx);

void get_rx_data(void *element_data, void *tr_ptr);

void ud_each_primary_key_select(void *element_data, void *result_ptr);

char *convert_rf(const char *avalue, int *type, int *port);
int convert_as_range(const char *as_range, long *begin, long *end);
time_t convert_time(const char *asc_time);

int UD_lock_serial(Transaction_t *tr);
int UD_unlock_serial(Transaction_t *tr);
long UD_create_serial(Transaction_t *tr);
int UD_comrol_serial(Transaction_t *tr, int commit);

#define UD_commit_serial(tr) UD_comrol_serial(tr, 1)
#define UD_rollback_serial(tr) UD_comrol_serial(tr, 0)
#endif /* _UD_INT_H */




