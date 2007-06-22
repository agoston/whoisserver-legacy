/***************************************
  $Revision: 1.5 $

  Functions to process data stream( file, network socket, etc.)

  Status: NOT REVUED, NOT TESTED

 Author(s):       Chris Ottrey, Andrei Robachevsky

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

#include "rip.h"
#include "syntax_api.h"
#include "lg.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


/* logging (defined in ud_main.c) */
extern LG_context_t *ud_context;

 
typedef enum _Line_Type_t {
 LINE_ATTRIBUTE,
 LINE_COMMENT,
 LINE_EMPTY,
 LINE_EOF,
 LINE_ADD,
 LINE_UPD,
 LINE_DEL,
 LINE_OVERRIDE_ADD,
 LINE_OVERRIDE_UPD,
 LINE_OVERRIDE_DEL,
 LINE_ACK
} Line_Type_t;

/* Maximum number of objects(serials) we can consume at a time */
#define SBUNCH 1000

static int report_transaction(Transaction_t *tr, long transaction_id, Log_t *log, ut_timer_t *psotime, char *reason);
static int process_nrtm(UD_stream_t *ud_stream, Transaction_t *tr, int operation);
static int process_updates(UD_stream_t *ud_stream, Transaction_t *tr, int operation);
static int process_transaction(UD_stream_t *ud_stream, GString *obj_buff, int operation, long transaction_id, LG_context_t *src_ctx);

static GString *escape_apostrophes(GString *text);
static nic_handle_t *ud_replace_autonic(Transaction_t *tr, rpsl_object_t *object);
static long ud_replace_autokey(Transaction_t *tr, rpsl_object_t *object, int *IS_X509);
static void reorder_attr(void *element_data, void *ptr);
static void ud_reorder_attributes(rpsl_object_t *object);


/******************************************************************
* ud_replace_autonic()                                            *
*                                                                 *
* Checks if nic handle is an AUTO one (XX*-SRS)                   *
* So it tries to assign a nic handle and if successfull           *
* substitutes the attribute value                                 * 
*                                                                 *
******************************************************************/
static nic_handle_t *ud_replace_autonic(Transaction_t *tr, rpsl_object_t *object)
{
nic_handle_t *nh_ptr=NULL;
rpsl_attr_t *nic_hdl=NULL, *nic_hdl_old;
GList *nic_hdl_list, *nic_hdl_first;
gint ofs;
char *nic_submitted, *nic_db;

 nic_hdl_list = rpsl_object_get_attr(object, "nic-hdl");
 nic_hdl_first =g_list_first(nic_hdl_list);
 nic_submitted = rpsl_attr_get_clean_value((rpsl_attr_t *)nic_hdl_first->data);

 /* check if it is an auto NIC handle and replace it with a real one */ 
 if(NH_parse(nic_submitted, &nh_ptr) == 0) {
  /* this is an AUTO nic handle */
  /* Try to allocate it */
  if(NH_check(nh_ptr, tr->sql_connection)>0){
    /* Convert nh to the database format */
    nic_db = NH_convert(nh_ptr);
    /* we need to make a copy as we free the whole list later */
    nic_hdl = rpsl_attr_copy((rpsl_attr_t *)nic_hdl_first->data);
    rpsl_attr_replace_value(nic_hdl, nic_db); 
    UT_free(nic_db);
  }
  /* replace this attribute in the object */
  ofs = rpsl_attr_get_ofs(nic_hdl);
  /* remove attribute from object and free it */
  nic_hdl_old = rpsl_object_remove_attr(object, ofs, NULL);
  if(nic_hdl_old) rpsl_attr_delete(nic_hdl_old); 
  else die; 
  rpsl_object_add_attr(object, nic_hdl, ofs, NULL);
 }/* if this was an AUTO nic handle */

 
 UT_free(nic_submitted);

 rpsl_attr_delete_list(nic_hdl_list);
 return(nh_ptr);
}

/******************************************************************
* ud_replace_autokey()                                            *
*                                                                 *
* Checks if an (key-cert) object name is an AUTO one (X509-*)     *
* So it tries to assign an integer and if successfull             *
* substitutes the attribute value                                 * 
* if it is not AUTO-nic, check if id is consistent with the DB    *
* and strip leading zeroes                                        *
* returns: > 0 if index found and replaced;                       *
*            0 wrong or not allowed                               *
*          < 0 if error                                           * 
* assigns the value of the pointer to an integer:                 *
* pointing to 0 if it is not a X509 object                        *
*  and to 1 if it is X509 object                                  *
*                                                                 *
******************************************************************/

static long ud_replace_autokey(Transaction_t *tr, rpsl_object_t *object, int *IS_X509)
{
long keycert_id=0;
const rpsl_attr_t *key_cert;
char *key_cert_submitted, *key_cert_from_db, *key_cert_formatted;
const char *key;
long keycert_id_submitted;

 key_cert = rpsl_object_get_attr_by_ofs(object, 0);
 key_cert_submitted = rpsl_attr_get_clean_value(key_cert);

 if (strstr(key_cert_submitted,"PGPKEY-"))
 {
   UT_free(key_cert_submitted);
   *IS_X509 = 0; 
   return(keycert_id);
 }
 else
 {
   *IS_X509 = 1; 
 }

 /* get latest keycert_id */
 keycert_id = nh_get_keycert_id(tr->sql_connection);
 
 if (keycert_id >= 0)
 {

   /* check if it is an auto NIC handle and replace it with a real one */
   if(strncmp(key_cert_submitted,"X509-*",strlen(key_cert_submitted)) == 0) 
	 {
     /* this is an AUTO key-cert name */
     key_cert_from_db = UT_malloc(MAX_NIC_HDL);
     /* increment keycert_id */
     keycert_id = keycert_id + 1;
     sprintf(key_cert_from_db, "X509-%ld", keycert_id);
     rpsl_attr_replace_value((rpsl_attr_t *)key_cert, key_cert_from_db);
     UT_free(key_cert_from_db);
   }/* if this was an AUTO keycert name */

   else 
   {  /* this is "X509-NNN" name */
      key = key_cert_submitted + 5; // "X509-"
      keycert_id_submitted = atol(key);
      if (keycert_id_submitted == 0)
      {  /* reject X509-0 */
        keycert_id = 0;    
      }
      else 
      {
        keycert_id = keycert_id_submitted;
        /* strip leading zeroes */
        key_cert_formatted = UT_malloc(strlen(key_cert_submitted)+1);
        sprintf(key_cert_formatted, "X509-%ld", keycert_id_submitted);
        rpsl_attr_replace_value((rpsl_attr_t *)key_cert, key_cert_formatted);
        UT_free(key_cert_formatted);
      }
   }
 }

 UT_free(key_cert_submitted);

 return(keycert_id);

}

/******************************************************************
* ud_replace_autonic_org()                                        *
*                                                                 *
* Checks if org object NIC handle is an AUTO one (ORG-XX*-SRS)    *
* If so it tries to assign a NIC handle and if successfull        *
* substitutes the attribute value                                 * 
*                                                                 *
******************************************************************/
static nic_handle_t *ud_replace_autonic_org(Transaction_t *tr, rpsl_object_t *object)
{
nic_handle_t *nh_ptr=NULL;
const rpsl_attr_t *organisation_attr;
char *nic_submitted, *nic_db;

  organisation_attr = rpsl_object_get_attr_by_ofs(object, 0);
  nic_submitted = rpsl_attr_get_clean_value(organisation_attr);

  /* check if it is an auto NIC handle and replace it with a real one */ 
  if(NH_parse_org(nic_submitted, &nh_ptr) == 0) {
    /* this is an AUTO nic handle */
    /* Try to allocate it */
    if(NH_assign_org_id(nh_ptr, tr->sql_connection)>0){
      /* Convert nh to the database format */
      nic_db = NH_convert_org(nh_ptr);
      /* we need to make a copy as we free the whole list later */
      rpsl_attr_replace_value((rpsl_attr_t*)organisation_attr, nic_db);
      UT_free(nic_db);
    }

  }/* if this was an AUTO nic handle */

 
  UT_free(nic_submitted);

  return(nh_ptr);
}


static void reorder_attr(void *element_data, void *ptr)
{
rpsl_attr_t *attr = (rpsl_attr_t *)element_data;
rpsl_object_t *object = (rpsl_object_t *)ptr;
int ofs;

  /* move the attribute to the beginning 
     so that all mnt-by are at the beginning of the object */
  ofs = rpsl_attr_get_ofs(attr);
  attr = rpsl_object_remove_attr(object, ofs, NULL);
  if (attr) rpsl_object_add_attr(object, attr, 1, NULL);
  else die;
}

/******************************************************************
* reorder attributes                                              *
*  . mnt-by should go before member-of to allow correct           * 
*      membership autorization (still done in RIPupd)             *
*  . nic-hdl should go before any admin-c, tech-c to prevent      * 
*       errrors in self referencing role objects                  *
******************************************************************/
static void ud_reorder_attributes(rpsl_object_t *object)
{
GList *mnt_by_list, *nic_hdl_list;
 /* get the list of mnt-by attributes */
 mnt_by_list = rpsl_object_get_attr(object, "mnt-by");
 /* reorder so that all mnt-by are at the beginning of the object */
 if(mnt_by_list != NULL) {
  g_list_foreach(mnt_by_list, reorder_attr, object);

  /* free GList only, not members as they are stored in the object */
  rpsl_attr_delete_list(mnt_by_list);
 } 

 /* move nic-hdl to the beginning */ 
 nic_hdl_list = rpsl_object_get_attr(object, "nic-hdl");
 if(nic_hdl_list != NULL) {
  g_list_foreach(nic_hdl_list, reorder_attr, object);

  /* free GList only, not members as they are stored in the object */
  rpsl_attr_delete_list(nic_hdl_list);
 } 
  
  
} 

/******************************************************************
* split the role, person or org-name attribute value into         *
* multiple names                                                  *
*                                                                 *
* destroys the original object and builds a new one (only for     *
* C_RO or C_PN                                                    *
******************************************************************/

static rpsl_object_t *ud_split_names(rpsl_object_t *object)
{
const rpsl_attr_t *attr;
gchar **names;
const GList *old_attrs;
GList *org_name_attrs;
const GList *p;
GString *new_obj;
rpsl_object_t *ret_val;
rpsl_error_t error;
C_Type_t class_type;
int i;

  class_type = rpsl_get_class_id(rpsl_object_get_class(object));
  if ((class_type != C_PN) && (class_type != C_RO) && (class_type !=C_OA)) 
  {
    return object;
  }
  
  /* get the list of person, role or org-name attribute */
  if((class_type == C_PN) || (class_type == C_RO))
  {
    attr = rpsl_object_get_attr_by_ofs(object, 0);
  }else
  {
    /* Else, this is an organisatin object C_OA */
    org_name_attrs = rpsl_object_get_attr(object, "org-name");
    attr = g_list_nth_data(org_name_attrs, 0);

  }
  

  /* split the names into words */
  names = ut_g_strsplit_v1((gchar*)rpsl_attr_get_value(attr), " ", 0);
  new_obj =  g_string_new("");


  if((class_type == C_PN) || (class_type == C_RO))
  {
    /* replace the value of the first attribute */
    for (i=0; names[i] != NULL; i++) {
        g_string_sprintfa(new_obj, "%s:%s\n", rpsl_attr_get_name(attr), names[i]);
    }

    g_strfreev(names);

    /* copy all other attributes */
    old_attrs = rpsl_object_get_all_attr(object);
    for (p=g_list_next(old_attrs); p != NULL; p = g_list_next(p)) {
        g_string_sprintfa(new_obj, "%s:%s\n", rpsl_attr_get_name(p->data), rpsl_attr_get_value(p->data));
    }
  }else{/* else, this is an organisation object */

    /* remove org-name attribute */
    rpsl_object_remove_attr_name(object, "org-name", &error);

    /* We will add the org-name attributes at the end of the object */ 
    old_attrs = rpsl_object_get_all_attr(object);
    for (p=old_attrs; p != NULL; p = g_list_next(p)) {
        g_string_sprintfa(new_obj, "%s:%s\n", rpsl_attr_get_name(p->data), rpsl_attr_get_value(p->data));
    }

    for (i=0; names[i] != NULL; i++) {
        g_string_sprintfa(new_obj, "%s:%s\n", rpsl_attr_get_name(attr), names[i]);
    }

    g_strfreev(names);
    
  }

  ret_val = rpsl_object_init(new_obj->str);
  g_string_free(new_obj, TRUE);
  rpsl_object_delete(object);
  return ret_val;
  
  
}


/******************************************************************
* GString *escape_apostrophes()                                   *
* Escapes apostrophes in the text so they do not confuse printf   *
* functions and don't corrupt SQL queries                         *
*                                                                 *
* *****************************************************************/
static GString *escape_apostrophes(GString *text) {
  int i;
  for (i=0; i < text->len; i++) {
    if ((text->str[i] == '\'') || (text->str[i] == '\\')) {
      text = g_string_insert_c(text, i, '\\');
      i++;
    }
  }
 return(text); 
} /* escape_apostrophes() */


/******************************************************************
* Line_Type_t line_type(e)                                        *
* Determines the line type analysing the first letters            *
*                                                                 *
* ****************************************************************/
static Line_Type_t line_type(const char *line, long *transaction_id) {

  if (strncmp(line, "# EOF", 4) == 0) return(LINE_EOF);
  if (strncmp(line, "#", 1) == 0)     return(LINE_COMMENT);
  if (strcmp(line, "\n") == 0)        return(LINE_EMPTY);
 
  if (strncmp(line, "ACK", 3) == 0) {
    *transaction_id = atol(line+3);	  
    return(LINE_ACK);
  }
  if (strncmp(line, "ADD_OVERRIDE", 12) == 0) {
    *transaction_id = atol(line+12);	  
    return(LINE_OVERRIDE_ADD);
  }
  if (strncmp(line, "UPD_OVERRIDE", 12) == 0) {
    *transaction_id = atol(line+12);	  
    return(LINE_OVERRIDE_UPD);
  }
  if (strncmp(line, "DEL_OVERRIDE", 12) == 0) {
    *transaction_id = atol(line+12);	  
    return(LINE_OVERRIDE_DEL);
  }
 
  if (strncmp(line, "ADD", 3) == 0) {
    *transaction_id = atol(line+3);
    return(LINE_ADD);
  }
  if (strncmp(line, "UPD", 3) == 0) {
    *transaction_id = atol(line+3);
    return(LINE_UPD);
  }
  if (strncmp(line, "DEL", 3) == 0) {
    *transaction_id = atol(line+3); 
    return(LINE_DEL);
  }
 
/* Otherwise this is an attribute */  
    return(LINE_ATTRIBUTE);

} /* line_type() */


/******************************************************************
* report_transaction()                                            *
*                                                                 * 
* Prints error report to the log                                  *
*                                                                 *
* reason - additional message that will be included               *
*                                                                 *
* *****************************************************************/
static int report_transaction(Transaction_t *tr, long transaction_id,  Log_t *log, ut_timer_t *psotime, char *reason)
{
int result=0;
ut_timer_t fotime;
float timediff;
const char *class_name = DF_class_type2name(tr->class_type);
char *primary_key = tr->K->str;


 /* calculate statistics */
  UT_timeget(&fotime);
  timediff = UT_timediff(psotime, &fotime);
 
 if(tr->succeeded==0) {
  result=tr->error;
  log->num_failed++;
  LG_log(tr->src_ctx, LG_INFO, "[%ld] %.2fs FAILED [%s:%s][%s]", transaction_id, timediff, class_name, primary_key, reason);
  if(result & ERROR_U_OBJ) LG_log(tr->src_ctx, LG_DEBUG, "[%ld] object: referential integrity error", transaction_id);
  if(result & ERROR_U_AUT) LG_log(tr->src_ctx, LG_DEBUG, "[%ld] object: authentication error", transaction_id);
  if(result & ERROR_U_BADOP) LG_log(tr->src_ctx, LG_DEBUG, "[%ld] object: unsupported operation", transaction_id);
  if(result & ERROR_U_COP) LG_log(tr->src_ctx, LG_DEBUG, "[%ld] object: conflicting operation", transaction_id);
  if(result & ERROR_U_NSUP) LG_log(tr->src_ctx, LG_DEBUG, "[%ld] object: this type is not supported", transaction_id);
  LG_log(tr->src_ctx, LG_DEBUG, "[%ld] object: details [%s]", transaction_id, (tr->error_script)->str);
  result=1; /* # of failures */
 }
 else {
  result=0;
  log->num_ok++;
  LG_log(tr->src_ctx, LG_INFO, "[%ld] %.2fs OK     [%s:%s][%s]", transaction_id, timediff, class_name, primary_key, reason);
  LG_log(tr->src_ctx, LG_DEBUG, "[%ld] object: details [%s]", transaction_id, (tr->error_script)->str);
 }
                                                                                                                                               
 return(result);
}/* report_transaction() */



/************************************************************
* process_nrtm()                                            *
*                                                           *
* Process object in NRTM client mode                        *
*                                                           *
* nrtm - pointer to _nrtm structure                         *
* log - pointer to Log_t structure                          *
* object_name - name of the object                          * 
* operation - operation code (OP_ADD/OP_DEL)                *
*                                                           *
* Returns:                                                  *
* 1  - okay                                                 *
* <0 - error                                                *
*                                                           *
************************************************************/

static int process_nrtm(UD_stream_t *ud_stream, Transaction_t *tr, int operation)
{
int result=0;
int dummy=0;
struct _nrtm *nrtm = ud_stream->nrtm;
long serial_id;
Log_t *log_ptr= &(ud_stream->log);
ut_timer_t sotime;
int ta_upd_nhr;

    /* Start timer for statistics */
    UT_timeget(&sotime);

  /* We allow NRTM updates for some inconsistent objects                  */
  /* One of the examples is reference by name which looks like nic-handle */
  /* For this purpose we allow dummy creation when updating an object     */
  /* We also check for dummy allowance when deleting an object            */
  /* this is done to allow deletion of person objects referenced by name  */

  tr->mode|=B_DUMMY;
  if(IS_NO_NHR(tr->mode))ta_upd_nhr=0; else ta_upd_nhr = TA_UPD_NHR;
  
  switch (operation) {
  
  case OP_ADD:
    if(nrtm->tr){ /* DEL ADD => saved*/
      if(tr->object_id==0) { 
	/* object does not exist in the DB */      
	/* delete the previous(saved) object*/
        object_process(nrtm->tr); 
        /* create DEL serial */
	UD_lock_serial(nrtm->tr);
	serial_id = UD_create_serial(nrtm->tr);
	CP_CREATE_S_PASSED(nrtm->tr->action); TR_update_status(nrtm->tr); 
	UD_commit_serial(nrtm->tr);
	UD_unlock_serial(nrtm->tr);
        /* Mark TR as clean */
	TR_mark_clean(nrtm->tr);
        /* log the transaction */
	result=report_transaction(nrtm->tr, serial_id, log_ptr, &sotime, "M:DEL");
        
        transaction_free(nrtm->tr); nrtm->tr=NULL;
        
	/* Create an object and update NHR */
        tr->action=(TA_CREATE | ta_upd_nhr);
	/* restart the timer for statistics */
	UT_timeget(&sotime);
        object_process(tr); /* create a new one*/
	/* create ADD serial */
        UD_lock_serial(tr);
	serial_id = UD_create_serial(tr); 
	CP_CREATE_S_PASSED(tr->action); TR_update_status(tr);
	UD_commit_serial(tr);
	UD_unlock_serial(tr);
	/* Mark TR as clean */
	TR_mark_clean(tr);
        /* log the transaction */
	result=report_transaction(tr, serial_id, log_ptr, &sotime, "M:ADD");
      }
      else { 
      /* object already exists in the DB - update or dummy replacement*/
        /*compare the two, may be we may collapse operations*/
        if(tr->object_id==nrtm->tr->object_id) {
          /* DEL-ADD ->> UPDATE */ 
          transaction_free(nrtm->tr); nrtm->tr=NULL;
          tr->action=TA_UPD_CLLPS;
          object_process(tr);
	  /* create DEL+ADD serial records */
	  UD_lock_serial(tr);
	  tr->action=TA_DELETE; serial_id = UD_create_serial(tr);
	  result=report_transaction(tr, serial_id, log_ptr, &sotime, "M:DEL(UPD)");

	  /* restart the timer for statistics */
          UT_timeget(&sotime);
	  tr->sequence_id++;
          tr->action=TA_CREATE; serial_id = UD_create_serial(tr);
	  CP_CREATE_S_PASSED(tr->action); TR_update_status(tr);
	  UD_commit_serial(tr);
	  UD_unlock_serial(tr);
	  /* Mark TR as clean */
	  TR_mark_clean(tr);
	  result=report_transaction(tr, serial_id, log_ptr, &sotime, "M:ADD(UPD)");
        }
        else { /* this should be a dummy object in the database(that we are going to replace with the real one */
        /* or an interleaved operation*/
          object_process(nrtm->tr); /* delete the previous(saved) object*/
          /* create a DEL serial record */
	  UD_lock_serial(nrtm->tr);
	  serial_id = UD_create_serial(nrtm->tr); 
	  CP_CREATE_S_PASSED(nrtm->tr->action); TR_update_status(nrtm->tr);
	  UD_commit_serial(nrtm->tr);
	  UD_unlock_serial(nrtm->tr);
	  /* Mark TR as clean */
	  TR_mark_clean(nrtm->tr);
          /* log the transaction */
	  result=report_transaction(nrtm->tr, serial_id, log_ptr, &sotime, "M:DEL");


          transaction_free(nrtm->tr); nrtm->tr=NULL;

	  /* restart the timer for statistics */
          UT_timeget(&sotime);
	  
          tr->action=TA_UPDATE;
          /* check if we are replacing a dummy object */
	  dummy=isdummy(tr);
          if(dummy==1) tr->action = TA_UPD_DUMMY;
          object_process(tr); /* create a new one*/
	  /* For serials this is CREATE operation. Increase sequence to have it correct in serals */
          if(dummy==1) { tr->action=TA_CREATE; tr->sequence_id++; } 
	  /* create ADD serial record */
          UD_lock_serial(tr);
	  serial_id = UD_create_serial(tr); 
	  CP_CREATE_S_PASSED(tr->action); TR_update_status(tr);
          UD_commit_serial(tr);
	  UD_unlock_serial(tr);
	  /* Mark TR as clean */
	  TR_mark_clean(tr);
          /* log the transaction */
          result=report_transaction(tr, serial_id, log_ptr, &sotime, "M:ADD");

        }
      }
    }
    else { /* ADD ADD =>brand new object*/
      if(tr->object_id==0) {
/*      fprintf(stderr,"CREATE new\n");*/
        /* Create an object and update NHR */
        tr->action=(TA_CREATE | ta_upd_nhr);
        object_process(tr);
        /* create ADD serial */
	UD_lock_serial(tr);
	serial_id = UD_create_serial(tr); 
	CP_CREATE_S_PASSED(tr->action); TR_update_status(tr);
        UD_commit_serial(tr);
	UD_unlock_serial(tr);

	/* Mark TR as clean */
	TR_mark_clean(tr);
        /* log the transaction */
        result=report_transaction(tr, serial_id, log_ptr, &sotime, "M:ADD");
      }
      else { /* object already exists in the database */
	/* this may happen because of dummies*/
	/* or with some implementations of mirroring protocol that have atomic update */
	/* instead of add + del */
        tr->action=TA_UPDATE;
        dummy=isdummy(tr);
        if(dummy==1) tr->action = TA_UPD_DUMMY;
        object_process(tr);
        /* For serials this is CREATE operation. Increase sequence to have it correct in serals */
        if(dummy==1) { tr->action=TA_CREATE; tr->sequence_id++; }
	/* create ADD serial record */
	UD_lock_serial(tr);
	serial_id = UD_create_serial(tr); 
	CP_CREATE_S_PASSED(tr->action); TR_update_status(tr);
        UD_commit_serial(tr);
	UD_unlock_serial(tr);
	/* Mark TR as clean */
	TR_mark_clean(tr);
        /* log the transaction */
        result=report_transaction(tr, serial_id, log_ptr, &sotime, "M:ADD");
	
      } 
    }
    break;
    
  case OP_DEL:
    if(nrtm->tr){ /*DEL DEL =>saved */
      /* check this is not a deletion of the same object twice */
      /* this should not happen but we cannot trust foreign sources */
      /* in such case process saved transaction but fail the current one */
      if(nrtm->tr->object_id == tr->object_id) tr->object_id=0;
	    
      object_process(nrtm->tr); /* delete the previous(saved) object*/
      /* create DEL serial record */
      UD_lock_serial(nrtm->tr);
      serial_id = UD_create_serial(nrtm->tr);  
      CP_CREATE_S_PASSED(nrtm->tr->action); TR_update_status(nrtm->tr);
      UD_commit_serial(nrtm->tr);
      UD_unlock_serial(nrtm->tr);
      /* Mark TR as clean */
      TR_mark_clean(nrtm->tr);
      /* log the transaction */
      result=report_transaction(nrtm->tr, serial_id, log_ptr, &sotime, "M:DEL");
      
      transaction_free(nrtm->tr); nrtm->tr=NULL;
    }
    /* save the real object (not a dummy one ) */
    if(tr->object_id>0 && !isdummy(tr)){ 
      /* save the object*/
      tr->action=(TA_DELETE | ta_upd_nhr);
      nrtm->tr=tr;
      return(0);
    }
    else { /* this is an error - Trying to DEL non-existing object*/
      tr->succeeded=0; tr->error|=ERROR_U_COP;
      tr->action=(TA_DELETE | ta_upd_nhr);
      /* create and initialize TR record for crash recovery */
      TR_create_record(tr);
      /* create DEL serial record anyway */
      UD_lock_serial(tr);
      serial_id = UD_create_serial(tr); 
      CP_CREATE_S_PASSED(tr->action); TR_update_status(tr);
      UD_commit_serial(tr);
      UD_unlock_serial(tr);
      /* Mark TR as clean */
      TR_mark_clean(tr);
      /* log the transaction */
      result=report_transaction(tr, serial_id, log_ptr, &sotime, "M:DEL: non-existing object");
      
    }
    break;
  
  default:
    tr->succeeded=0; tr->error |=ERROR_U_BADOP;
    break;  
  }

 /* Free resources */  
  transaction_free(tr);
  
  return(result);
} /* process_nrtm() */



/************************************************************
* process_updates()                                         *
*                                                           *
* Process object in update mode                             *
*                                                           *
* ud_stream - pointer to UD_stream structure                *
* object_name - name of the object                          *
* operation - operation code (OP_ADD/OP_DEL)                *
*                                                           *
* Note:                                                     *
* Frees tr and tr->obj on exit                              *
*                                                           *
* Returns:                                                  *
* 0  - okay                                                 *
* <0- number of failed objects                              *
*                                                           * 
************************************************************/

static int process_updates(UD_stream_t * ud_stream, Transaction_t * tr, int operation)
{
	int result = 0;
	Log_t *log_ptr = &(ud_stream->log);
	int dummy = 0;
	ut_timer_t sotime;
	int ta_upd_nhr;
	char *reason;

	/* Start timer for statistics */
	UT_timeget(&sotime);
	if (IS_NO_NHR(tr->mode))
		ta_upd_nhr = 0;
	else
		ta_upd_nhr = TA_UPD_NHR;

	switch (operation) {
		/* Compare operations and report an error if they do not match */
	case OP_ADD:
		if (tr->object_id != 0) {	/* trying to create, but object exists */
			tr->succeeded = 0;
			tr->error |= ERROR_U_COP;
			reason = "U:ADD:object already exists";
			g_string_sprintfa(tr->error_script, "E[%d]:NEW requested but object already exists\n", ERROR_U_COP);
			UD_ack(tr);			/* Send a NACK */
		} else {
			/* Action: create the object and update NHR */
			tr->action = (TA_CREATE | ta_upd_nhr);
			reason = "U:ADD";
			object_process(tr);
		}
		break;
		
	case OP_UPD:
		if (tr->object_id == 0) {	/* trying to update non-existing object */
			tr->succeeded = 0;
			tr->error |= ERROR_U_COP;
			reason = "U:UPD:non-existing object";
			g_string_sprintfa(tr->error_script, "E[%d]:UPD requested but no existing object found\n", ERROR_U_COP);
			UD_ack(tr);			/* Send a NACK */
		} else {
			tr->action = TA_UPDATE;
			reason = "U:UPD";
			dummy = isdummy(tr);
			if (dummy == 1)
				tr->action = TA_UPD_DUMMY;
			object_process(tr);
		}
		break;

	case OP_DEL:
		if (tr->object_id == 0) {	/* trying t delete non-existing object */
			tr->succeeded = 0;
			tr->error |= ERROR_U_COP;
			reason = "U:DEL:non-existing object";
			g_string_sprintfa(tr->error_script, "E[%d]:DEL requested but no existing object found\n", ERROR_U_COP);
			UD_ack(tr);
		} else {
			tr->action = (TA_DELETE | ta_upd_nhr);
			reason = "U:DEL";
			object_process(tr);
		}
		break;

	default:
		/* bad operation for this mode if not standalone */
		if (IS_STANDALONE(tr->mode)) {
			if (tr->object_id == 0) {
				tr->action = (TA_CREATE | ta_upd_nhr);
				reason = "U:ADD";
			} else {
				tr->action = TA_UPDATE;
				reason = "U:UPD";
			}
			object_process(tr);
		} else {
			tr->succeeded = 0;
			tr->error |= ERROR_U_BADOP;
			g_string_sprintfa(tr->error_script, "E[%d]:Unknown operation requested\n", ERROR_U_BADOP);
			reason = "U:bad operation";
			UD_ack(tr);			/* Send a NACK */
		}
		break;
	}
	/* If not in standalone mode create serial and copy error transcript */
	if (!IS_STANDALONE(tr->mode)) {
		if (tr->succeeded) {
			/* we don't want to generate DEL serial for dummy replacement */
			if (dummy == 1) {
				tr->action = TA_CREATE;
				tr->sequence_id++;
			}
			UD_lock_serial(tr);
			UD_create_serial(tr);
			CP_CREATE_S_PASSED(tr->action);
			TR_update_status(tr);
			UD_commit_serial(tr);
			UD_unlock_serial(tr);
			/* Mark the TR as clean */
			TR_mark_clean(tr);
		}
	}

	/* Make a report. U stands for update stream. No reason */
	result = report_transaction(tr, tr->transaction_id, log_ptr, &sotime, reason);

	/* Free resources */
/*    rpsl_object_delete(tr->object); */
	transaction_free(tr);

	return (result);

}								/* process_updates() */


/************************************************************
*                                                           *
* int process_transaction()                                 *
*                                                           *
* Processes the transaction                                 *
*                                                           *
* ud_stream - pointer to UD_stream_t structure              *
*                                                           *
* Returns:                                                  *
* 0 - no error                                              *
* <0- number of failed objects                              *
*                                                           *
************************************************************/

/* It frees the obj */

static int process_transaction(UD_stream_t *ud_stream, 
                         GString *g_obj_buff,
			 int operation,
			long transaction_id,
                        LG_context_t *src_ctx)
{
Transaction_t *tr = NULL;
int result;
nic_handle_t *nh_ptr=NULL;
long keycert_id = 0;
int is_x509 = 0;
rpsl_object_t *submitted_object=NULL, *sql_object=NULL;
const GList *rpsl_err_list;
gchar *object_txt;
ut_timer_t sotime;
long serial_id;


 /* check if the requested transaction has already been processed */
 /* this may happen in case of crash. If so, just send an ack and return */
 if(TR_check(ud_stream->db_connection, transaction_id, (ud_stream->condat).sock, src_ctx)) return(1);

 /* escape apostrophes, otherwise sql will be confused */
 g_obj_buff=escape_apostrophes(g_obj_buff);

 /* check if it is an object and init it */
 if ((submitted_object=rpsl_object_init(g_obj_buff->str))==NULL) return(-1);

/* start new transaction now */
 tr = transaction_new(ud_stream->db_connection, rpsl_get_class_id(rpsl_object_get_class(submitted_object)), src_ctx);
 if (tr == NULL) die;
 
 tr->mode = ud_stream->ud_mode;
 tr->load_pass = ud_stream->load_pass;
 tr->source_hdl = ud_stream->source_hdl;
 tr->socket = (ud_stream->condat).sock;
 tr->transaction_id = transaction_id;
 tr->object = submitted_object;
 tr->keycert_id = 0;
 tr->x509_commit = 0;

 UT_timeget(&sotime);
 

 /* check for syntax errors (with whois_rip syntax set) */
 if((rpsl_err_list = rpsl_object_errors(submitted_object)) != NULL) {
 int num_err = 0;
 gboolean is_garbage = FALSE;
 gboolean is_only_comments = FALSE;


 /* check the severity of the errors */
 /* we need to catch: */
 /* template errors - missing primary keys _only_ */
 /* attribute syntax errors - just to abort without processing */
  do {
     const rpsl_attr_t *attr;
     const rpsl_error_t *err = rpsl_err_list->data;
     gboolean is_important_error = FALSE;

     switch (err->code) {
      /* totally bogus object, or missing primary keys */
      case RPSL_ERR_ONLYCOMMENTS:
           is_only_comments = TRUE;
      case RPSL_ERR_MISSINGKEY:
      case RPSL_ERR_BADCLASS:
      case RPSL_ERR_UNKNOWNCLASS:
           is_garbage = TRUE;
           is_important_error = TRUE;
           break;
      /* inappropriate, duplicate attributes */
      case RPSL_ERR_ATTRNOTALLOWED:
           attr = rpsl_object_get_attr_by_ofs(submitted_object, err->attr_num);
           if (rpsl_attr_is_key(submitted_object, rpsl_attr_get_name(attr))) {
              is_important_error = TRUE;
           }
           break;
      /* bad and single attribuets that have multiple appearance in the object are dangerous */     
      case RPSL_ERR_BADATTR:
      case RPSL_ERR_ATTRSINGLE:
           is_important_error = TRUE;
           break;
           
     }
     /* report error */
     /* no need to have an extended report */
     /* basically for debuggung purposes   */
     /* it will never be passed to the user */
     if (is_important_error) {
        g_string_sprintfa(tr->error_script,"E[%d]:%s\n", ERROR_U_OBJ, err->descr);
        num_err++;
     }
     rpsl_err_list = g_list_next(rpsl_err_list);
  } while (rpsl_err_list != NULL);

  if (num_err > 0) {
     tr->succeeded = 0;
     tr->error |= ERROR_U_OBJ;
    /* garbage just ignore */
    if (is_garbage){
      if(!is_only_comments)
        LG_log(tr->src_ctx, LG_DEBUG, "garbage in NRTM stream [%s]", g_obj_buff->str);
    } else {
      LG_log(tr->src_ctx, LG_DEBUG, "Malformed object:[%s]\n%s\n",tr->error_script->str, g_obj_buff->str);
      /* we cannot process this object and store it in the database */
      /* but in case of NRTM this indicates a malformed stream */
      /* we try our best to be in sync with the master, so we need to store */
      /* this block (maybe object) in failed_transaction */
      if(IS_NRTM_CLNT(ud_stream->ud_mode)){
        object_txt = rpsl_object_get_text(submitted_object, RPSL_STD_COLUMN);
        tr->object_txt = object_txt;
        if(operation==OP_DEL) tr->action=TA_DELETE; else tr->action=TA_CREATE;
        UD_lock_serial(tr);
        serial_id = UD_create_serial(tr); 
        CP_CREATE_S_PASSED(tr->action); TR_update_status(tr);
        UD_commit_serial(tr);
        UD_unlock_serial(tr);
        /* Mark TR as clean */
        TR_mark_clean(tr);
        /* log the transaction */
        result=report_transaction(tr, serial_id, &ud_stream->log, &sotime, "M:---");
      }
    }
    UD_ack(tr);
    transaction_free(tr);
    return(-1);
  }
 }/* checked for syntax errors */


 /* If we maintain NHR, for person, role and organisation objects */
 /* check and replace the AUTO nic handle */
 if(!IS_NO_NHR(ud_stream->ud_mode)){
    if((tr->class_type == C_PN) || (tr->class_type == C_RO) || (tr->class_type == C_OA)) {
      if(tr->class_type == C_OA){/* it is an organisation object */
        nh_ptr = ud_replace_autonic_org(tr, submitted_object);
      }else{/* it is a person or role object */
        nh_ptr = ud_replace_autonic(tr, submitted_object);
      }
      /* NULL nh_ptr indicates a problem with a NIC handle */
      if(nh_ptr == NULL) {
        tr->succeeded = 0;
        tr->error |= ERROR_U_OBJ;
        g_string_sprintfa(tr->error_script,"E[%d]:Wrong NIC handle format\n", ERROR_U_OBJ);
        LG_log(tr->src_ctx, LG_INFO, "Malformed object:[Wrong NIC handle]\n%s\n", g_obj_buff->str);
        /* we cannot process this object and store it in the database */
        /* but in case of NRTM this indicates a malformed stream */
        /* we try our best to be in sync with the master, so we need to store */
        /* this block (maybe object) in failed_transaction */
        if(IS_NRTM_CLNT(ud_stream->ud_mode)){
          object_txt = rpsl_object_get_text(submitted_object, RPSL_STD_COLUMN);
          tr->object_txt = object_txt;
          if(operation==OP_DEL) tr->action=TA_DELETE; else tr->action=TA_CREATE;
          UD_lock_serial(tr);
          serial_id = UD_create_serial(tr); 
          CP_CREATE_S_PASSED(tr->action); TR_update_status(tr);
          UD_commit_serial(tr);
          UD_unlock_serial(tr);
          /* Mark TR as clean */
          TR_mark_clean(tr);
          /* log the transaction */
          result=report_transaction(tr, serial_id, &ud_stream->log, &sotime, "M:---");
        }
        UD_ack(tr);
        transaction_free(tr);
        return(-1);
      }  
    }
 }

 /* if key-cert object, check for AUTO name */
 if(tr->class_type == C_KC) {
   keycert_id = ud_replace_autokey(tr, submitted_object, &is_x509);
   if (is_x509 == 0) 
   {
     /* this is PGPKEY object, do nothing */
   } 
   else 
   {
     /* this is X509 object */
     tr->x509_commit = 1;
     if (keycert_id <= 0) {
       /* some problem with x509 sql table which holds the last assigned index */
       tr->succeeded = 0;
       tr->error |= ERROR_U_OBJ;
       g_string_sprintfa(tr->error_script,"E[%d]:Wrong KEY-CERT name format\n", ERROR_U_OBJ);
       LG_log(tr->src_ctx, LG_INFO, "Malformed object:[Wrong KEY-CERT name]\n%s\n", g_obj_buff->str);
       /* we cannot process this object and store it in the database */
       /* but in case of NRTM this indicates a malformed stream */
       /* we try our best to be in sync with the master, so we need to store */
       /* this block (maybe object) in failed_transaction */
       if(IS_NRTM_CLNT(ud_stream->ud_mode)){
         object_txt = rpsl_object_get_text(submitted_object, RPSL_STD_COLUMN);
         tr->object_txt = object_txt;
         if(operation==OP_DEL) tr->action=TA_DELETE; else tr->action=TA_CREATE;
         UD_lock_serial(tr);
         serial_id = UD_create_serial(tr);
         CP_CREATE_S_PASSED(tr->action); TR_update_status(tr);
         UD_commit_serial(tr);
         UD_unlock_serial(tr);
         /* Mark TR as clean */
         TR_mark_clean(tr);
         /* log the transaction */
         result=report_transaction(tr, serial_id, &ud_stream->log, &sotime, "M:---");
       }
       UD_ack(tr);
       transaction_free(tr);
       return(-1);
     }
   }
 }
 
 /* normalize the object (reorder and split attributes */
 sql_object = rpsl_object_copy_flattened(submitted_object);

 /* put nic-hdl and mnt-by at the top of the list */
 ud_reorder_attributes(sql_object);

 /* split the names */
  sql_object = ud_split_names(sql_object); 
         
 /* save the original text of submitted object */
 object_txt = rpsl_object_get_text(submitted_object, RPSL_STD_COLUMN);
	 
 tr->object = sql_object;
 tr->object_txt = object_txt; /* needs to be freed */
 tr->nh = nh_ptr;
 tr->keycert_id = keycert_id; /* is meaningful only if tr->x509_commit is true */
 
/* We perform no commit/rollback in the loader mode, so thread_id should be set to 0 */
 if(IS_STANDALONE(tr->mode)){ tr->thread_ins=0; tr->thread_upd=0; }

/* For the first load pass we only create objects */ 

 if(tr->load_pass==1) {
	 /* still we need to fill tr->K (last.pkey) field */ 
         /* tr->load_pass ==1 will trigger this behaviour in ud_each_primary_key_select */
	 g_list_foreach((GList *)rpsl_object_get_all_attr(tr->object), ud_each_primary_key_select, tr);
	 tr->object_id=0;
 }
 else tr->object_id=get_object_id(tr);

 /* check if error has been detected */
 /*XXX replace error codes with symbolic consts */
 if(tr->object_id==-2) { /* Object erorr - wrong syntax in one of the PK */
    LG_log(tr->src_ctx, LG_INFO, "Malformed object:[%s]\n%s\n",tr->error_script->str, g_obj_buff->str);   
    UD_ack(tr);
    /* this object is not deleted by transaction_delete() */
    rpsl_object_delete(submitted_object);
    transaction_free(tr);
    return(-1);
 }   
 
/* Object cannot be retrieved */
 if(tr->object_id==-1) { /* DB error*/
    tr->succeeded=0;
    tr->error |= ERROR_U_DBS;
    LG_log(ud_context, LG_SEVERE, "Object cannot be retrieved:[%s]", tr->object_txt);
    die;
 }

/* Process transaction. tr and obj are freed inside the process_* functions */
/* tr is freed there because in NRTM we need to save a transaction for later processing */
 if(IS_UPDATE(ud_stream->ud_mode))
 {
 /* We are in update mode */
    result=process_updates(ud_stream, tr, operation);
 }
 else
 {
 /* We are in NRTM mode */   
    result=process_nrtm(ud_stream, tr, operation);
 }
 
 /* free the original submitted object */
 rpsl_object_delete(submitted_object);
 return(result);

}          
          

/************************************************************
*                                                           *
* int UD_process_stream(UD_stream_t *ud_stream)             *
*                                                           *
* Processes the stream                                      *
*                                                           *
* ud_stream - pointer to UD_stream_t structure              *
*                                                           *
* Returns:                                                  *
* in update mode (!standalone)(1 object processed):         *
* 1 - no error                                              *
* <0- errors                                                *
*                                                           *
* in NRTM & standalone modes                                *
* total number of object processed                          *
*                                                           *
************************************************************/

int 
UD_process_stream (UD_stream_t *ud_stream, LG_context_t *src_ctx)
{
  char line_buff[STR_XXL];
  SQ_connection_t *sql_connection;
  struct _nrtm *nrtm;
  Log_t *log_ptr= &(ud_stream->log);
  ut_timer_t stime, ftime;
  float obj_second1, obj_second10, timediff;
  int result=0;
  int operation=0;
  int interrupt=0;
  int do_update;
  int default_ud_mode = ud_stream->ud_mode;
  Line_Type_t linetype;
  Transaction_t *tr;
  long transaction_id=0; /* transaction_id (to be supplied by DBupdate and stored in Database) */
  GString *g_obj_buff;
  
  nrtm=ud_stream->nrtm;

  g_obj_buff = g_string_sized_new(STR_XXL);

  /* Check connection to the database */
  if(SQ_ping(ud_stream->db_connection)) {
   LG_log(ud_context, LG_ERROR, "%s", SQ_error(ud_stream->db_connection));
   die;
  }
  sql_connection=ud_stream->db_connection;
  
  UT_timeget(&stime);

 /* Main loop. Reading input stream line by line */
 /* Empty line signals to start processing an object, if we have it */ 
  while (SK_cd_gets(&ud_stream->condat, line_buff, sizeof(line_buff))>0) {


    switch (linetype=line_type(line_buff, &transaction_id)) {

      case LINE_COMMENT:
      case LINE_EOF:
      break;

      case LINE_ACK:
       tr = transaction_new(ud_stream->db_connection, 0, src_ctx);
       tr->transaction_id=transaction_id;
       TR_delete_record(tr);
       transaction_free(tr);
      break;
      
      
      case LINE_ADD:
      /* restore the default operation mode */
       operation=OP_ADD;
       ud_stream->ud_mode=default_ud_mode;
      break;
      
      case LINE_OVERRIDE_ADD:
      /* for override - switch the dummy bit on */
       operation=OP_ADD;
       ud_stream->ud_mode=default_ud_mode|B_DUMMY;
      break;
      
      case LINE_UPD:
      /* restore the default operation mode */
       operation=OP_UPD;
       ud_stream->ud_mode=default_ud_mode;
      break;  

      case LINE_OVERRIDE_UPD:
      /* for override - switch the dummy bit on */
       operation=OP_UPD;
       ud_stream->ud_mode=default_ud_mode|B_DUMMY;
      break;
      
      case LINE_DEL:
      /* restore the default operation mode */
       operation=OP_DEL;
       ud_stream->ud_mode=default_ud_mode;
      break; 

      case LINE_OVERRIDE_DEL:
      /* for override - switch the dummy bit on */
       operation=OP_DEL;
       ud_stream->ud_mode=default_ud_mode|B_DUMMY;
      break;
 
      case LINE_EMPTY:
       /* start processing the object */
        /* escape apostrophes, otherwise sql will be confused */
 	 /* XXX */
/*	 print_object(obj); */
         /* check if we have collected something */
         if(g_obj_buff->len >0){
#if 0         
           /* no operation suggest a garbage in the stream - just ignore it */
           if(IS_NRTM_CLNT(ud_stream->ud_mode) && (operation==OP_NOOP)) { 
               ER_inf_va(FAC_UD, ASP_UD_UPDLOG, "garbage in NRTM stream");
           } else 
#endif           
           {    
             /* start new transaction now */
             result=process_transaction(ud_stream, g_obj_buff, operation, transaction_id, src_ctx);
             /* process_transaction() frees tr and obj structures, */
             /* so make sure we'll not reference these objects in the future */
             operation=OP_NOOP;
             transaction_id=0;
           }  
           ud_stream->ud_mode=default_ud_mode;
           g_obj_buff = g_string_truncate(g_obj_buff,0);
         } 
         /* this is a good place for quick interrupt */
         do_update=CO_get_do_update();
         if (do_update) interrupt=0; else interrupt=1;

      break;

      default:
       /* this may be an object - fill the buffer */
        g_string_sprintfa(g_obj_buff, "%s", line_buff);
        break;
    } /* switch */
    
    /* Finish processing if interrupt has been set */
    if (interrupt) break;
  } /* Main loop of data stream processing : while */
 

 /* Some postprocessing */
  if(IS_NRTM_CLNT(ud_stream->ud_mode)){
  /* We are in NRTM mode */
  /* Clean up */
  /* In NRTM mode there may be a saved object that is unprocessed */   
   if(nrtm->tr){ /*saved backlog?*/
/*XXX Do nothing with this backlog; next time we connect we will process it */   
#if 0   
    /* restart the timer for statistics */
    UT_timeget(&sotime);
    object_process(nrtm->tr); /* delete the previous(saved) object*/
/*    result=report_transaction(nrtm->tr, &(ud_stream->log), nrtm->object_name, 
                              "NRTM:DEL:While deleting previous(saved) object"); */
    /* create DEL serial record no matter what the result is */
    UD_lock_serial(nrtm->tr);
    serial_id = UD_create_serial(nrtm->tr); 
    CP_CREATE_S_PASSED(nrtm->tr->action); TR_update_status(nrtm->tr);
    UD_commit_serial(nrtm->tr);
    UD_unlock_serial(nrtm->tr);
    /* Mark TR as clean */
    TR_mark_clean(nrtm->tr);
    /* log the transaction */
    result=report_transaction(nrtm->tr, serial_id, log_ptr, &sotime, "M:DEL");
#endif
    transaction_free(nrtm->tr); nrtm->tr=NULL;
   } 
  }

 /* That's all. Free GString */
  g_string_free(g_obj_buff, TRUE);

                                                                                                       
 /* Calculate some statistics */
  UT_timeget(&ftime);
  timediff = UT_timediff(&stime, &ftime);
  obj_second1 = (float)(log_ptr->num_ok)/timediff;
  obj_second10 = (float)(log_ptr->num_ok+log_ptr->num_failed)/timediff;
  
  /* Print the report */
  if(IS_STANDALONE(ud_stream->ud_mode) || (!IS_UPDATE(ud_stream->ud_mode))) {

   LG_log(src_ctx, LG_INFO, "%s ******** report **********", UD_TAG);
   LG_log(src_ctx, LG_INFO, "%s %d objects OK (%7.4f obj/s)", UD_TAG, log_ptr->num_ok, obj_second1);
   LG_log(src_ctx, LG_INFO, "%s %d objects failed", UD_TAG, log_ptr->num_failed);
   LG_log(src_ctx, LG_INFO, "%s average processing time %7.4f obj/s (%6.2f obj/min)", UD_TAG, 
                          obj_second10, obj_second10*60);
   result=log_ptr->num_ok+log_ptr->num_failed;
  }
  return(result);

} /* UD_process_stream */

