#ifndef READ_WHICH_KEYTYPES
#define READ_WHICH_KEYTYPES

/***************************************
  $Revision: 1.18 $

  Which Keytypes module (wk)

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
#include "bitmask.h"

/*+ Enumeration of which keytypes: +*/
typedef enum WK_Type_t {
  WK_NAME=0,         /*+ A name                              +*/
  WK_NIC_HDL,        /*+ NIC_HDL                             +*/
  WK_EMAIL,          /*+ RFC822 e-mail address               +*/
  WK_MNTNER,         /*+ Maintainer name                     +*/
  WK_KEY_CERT,       /*+ PGPKEY | X509 certificate           +*/
  WK_IPADDRESS,      /*+ IP address                          +*/
  WK_IPRANGE,        /*+ IP range                            +*/
  WK_IPPREFIX,       /*+ IP prefix                           +*/
  WK_IP6PREFIX,      /*+ IPv6 prefix                         +*/
  WK_NETNAME,        /*+ Network name                        +*/
  WK_NET6NAME,       /*+ IPv6 Network name                   +*/
  WK_AUTNUM,         /*+ AS number                           +*/
  WK_ASSETNAME,      /*+ AS set name                         +*/
  WK_ROUTESETNAME,   /*+ Route set name                      +*/
  WK_DOMAIN,         /*+ Domain name                         +*/
  WK_HOSTNAME,       /*+ Host name                           +*/
  WK_LIMERICK,       /*+ Limerick name                       +*/
  WK_ASRANGE,        /*+ AS range (possibly only beginning)  +*/
  WK_FILTERSET,      /*+ filter-set name                     +*/
  WK_PEERINGSET,     /*+ peering-set name                    +*/
  WK_RTRSET,         /*+ router-set name                     +*/
  WK_IRT,            /*+ irt name                            +*/
  WK_FINGERPR,       /*+ fingerprint                         +*/
  WK_ORG_ID,         /*+ organisation object ID              +*/
  WK_AUTH,           /*+ auth attribute                      +*/
  WK_END
} WK_Type;

/* for easy maintenance, the tables that depend on the enums are defined here.
   They must be kept in sync with each other. Also, the WK_new function must
   be kept in sync!!!

   Guarded with WK_IMPL so that they are compiled only once, inside wk.c
 */ 
#ifdef WK_IMPL
char * const Keytypes[] = {
  "name",
  "nic_hdl",
  "email",
  "mntner",
  "key_cert",
  "ipaddress",
  "iprange",
  "ipprefix",
  "ip6prefix",
  "netname",
  "net6name",
  "autnum",
  "assetname",
  "routesetname",
  "domain",
  "hostname",
  "limerick",
  "asrange",
  "filter_set",
  "peering_set",
  "rtr_set",
  "irt",
  "organisation",
  "auth",
  NULL
}; /* Keytypes[] */

#endif 

char *WK_to_string(mask_t wk);
mask_t WK_new(char *key);

#endif /* READ_WHICH_KEYTYPES */
