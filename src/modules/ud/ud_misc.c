/***************************************
  $Revision: 1.21 $

  Miscellaneous functions to support UD

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
#include "lg.h"

/************************************************************
* void transaction_free()                                   *
*                                                           *
* Frees memory allocated for a transaction                  *
*                                                           *
************************************************************/

void transaction_free(Transaction_t *tr) {
  if(tr) {
    g_string_free(tr->error_script, TRUE);
    g_string_free(tr->K, TRUE);
    /* free nic_handle_t structure used for NHR stuff */
    if(tr->nh)free_nh(tr->nh);
    if(tr->save){
	    UT_free(tr->save);
    }
    if(tr->packptr)UT_free(tr->packptr);
    
    if(tr->query)g_string_free(tr->query, TRUE);
    if(tr->object)rpsl_object_delete(tr->object);
    UT_free(tr->object_txt);
    
    UT_free(tr);
  }  
} /* transaction_free() */

/************************************************************
* Transaction_t *transaction_new()                          *
*                                                           *
* Creates a transaction                                     *
*                                                           *
************************************************************/
Transaction_t *
transaction_new (SQ_connection_t *sql_connection, C_Type_t class_type, LG_context_t *src_ctx) 
{
  Transaction_t *tr = (Transaction_t *)UT_calloc(1, sizeof(Transaction_t));

  tr->sql_connection = sql_connection;
  tr->class_type = class_type;
  tr->thread_upd=TR_UPDATE;
  tr->thread_ins=TR_INSERT;
  tr->succeeded = 1;
  tr->error_script = g_string_sized_new(STR_XL);
  tr->K = g_string_sized_new(STR_L);
  tr->sequence_id=1; /* we start from 1*/
  tr->packptr=UT_calloc(1, sizeof(rp_upd_pack_t));
  tr->query = g_string_sized_new(STR_XL);
  tr->src_ctx = src_ctx;
  
  return tr;
} /* transaction_new() */
/************************************************************
* int UD_ack()                                            *
*
* Sends an acknowledgement to DBupdate and receives a reply *                                                           *
*                                                           *
* Return codes:                                             *
*                                                           *
* 0  - OK
* -1 - 
************************************************************/
int UD_ack(Transaction_t* tr)
{
GString *g_buff;
int res, error;
 
 /* if we are not in update/server mode - no ack is needed - just return */
 if(IS_STANDALONE(tr->mode)) return(0);
 if(!IS_UPDATE(tr->mode)) return(0);
 
 g_buff = g_string_sized_new(STR_L);

 if(tr->succeeded==0) { /* update failed */
   error = tr->error;
 }
 else error = 0;

 g_string_sprintf(g_buff, "%%ERROR %d\n%s\n", error, (tr->error_script)->str);
 res = SK_puts(tr->socket, g_buff->str, NULL);
 g_string_free(g_buff, TRUE);
 /* close the connection */
/* Let DBupdate close the connection */
/* close(tr->socket); */
 return(res);		
}

