/***************************************
  $Revision: 1.16 $

  Functions to interface with RX module (create/update rx-nodes)

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

#include "rip.h"

/* Function to fill data for radix tree */
void get_rx_data(void *element_data, void *tr_ptr)
{
int err;
const rpsl_attr_t *attribute = (const rpsl_attr_t *)element_data;
Transaction_t *tr = (Transaction_t *)tr_ptr;
int attribute_type = rpsl_get_attr_id(rpsl_attr_get_name(attribute));

const gchar *attribute_value;

 switch(attribute_type){
   case A_IN:
   case A_RT:
   case A_R6:
   case A_I6:    
   case A_DN:
    /* it is already clean as we work with the flattened object */
    attribute_value = rpsl_attr_get_value(attribute);
    if(ERR(err = RP_asc2pack( tr->packptr, attribute_type, attribute_value))) {
      /* we can also forgive bit inconsistencies */
      /* DN can be forgiven, but radix should not be updated */
     dieif( attribute_type != A_DN  );  /* DN can be forgiven, but radix should not be updated */
    } else tr->action |= TA_UPD_RX; /* Update radix in all other cases */
   break; 
   default: break; 
 }
}
