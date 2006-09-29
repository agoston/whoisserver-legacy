/***************************************
  $Revision: 1.1 $

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
#ifndef _UD_H
#define _UD_H

/* Error codes definition */
#define ERROR_U_MEM     0x0001 /* memory allocation error */
#define ERROR_U_DBS     0x0002 /* database (sql) error */
#define ERROR_U_OBJ     0x0004 /* referential integrity failure */
#define ERROR_U_AUT     0x0008 /* membership authorization failure */

#define ERROR_U_BADOP   0x0010 /* unknown operation */
#define ERROR_U_COP     0x0020 /* conflicting operation (e.g. deletig non-existent object) */
#define ERROR_U_NSUP    0x0040 /* object type not supported */
#define ERROR_U_BUG     0x0080 /*  software bug - report to developer */

/* definition of mode bits */
#define B_DUMMY                 0x01
#define B_UPDATE                0x02
#define B_NRTM_CLNT             0x04
#define B_STANDALONE            0x08

/* no NHR store - when mirroring foreign db */
#define B_NO_NHR                0x10
/* invitation includes -k */
#define B_PERSIST_MIRR          0x20

 
/* macros for update mode operation */
#define IS_DUMMY_ALLOWED(a)     ((a)&(B_DUMMY))
#define IS_UPDATE(a)            ((a)&(B_UPDATE))
#define IS_NRTM_CLNT(a)         ((a)&(B_NRTM_CLNT))
#define IS_STANDALONE(a)        ((a)&(B_STANDALONE))

#define IS_PERSIST_MIRR(a)      ((a)&(B_PERSIST_MIRR))
#define IS_NO_NHR(a)            ((a)&(B_NO_NHR))

/* XXX SQL transaction types */

#define TR_UPDATE   1000
#define TR_INSERT   2000
#define TR_CREATE   TR_INSERT
#define TR_DELETE   3000



/* Transaction action (tr.action ) */
#define TA_CREATE	0x01
#define TA_UPDATE	0x02
#define TA_DELETE       0x04
#define TA_UPD_NHR      0x08
#define TA_UPD_RX       0x10
#define TA_DUMMY        0x20
#define TA_CLLPS        0x40
#define TA_UPD_DUMMY    (TA_UPDATE | TA_DUMMY)
#define TA_UPD_CLLPS    (TA_UPDATE | TA_CLLPS)



/* macros for transaction operation bitmask (tr.action) */
#define ACT_CREATE(a)           ((a)&TA_CREATE)
#define ACT_UPDATE(a)           ((a)&TA_UPDATE)
#define ACT_DELETE(a)           ((a)&TA_DELETE)
#define ACT_UPD_NHR(a)          ((a)&TA_UPD_NHR) /* update Nic Handle Repository  */
#define ACT_UPD_RX(a)           ((a)&TA_UPD_RX) /* update radix tree */

#define ACT_UPD_DUMMY(a)        ((a)&TA_DUMMY) /* update of a dummy object */
#define ACT_UPD_CLLPS(a)        ((a)&TA_CLLPS) /* collapsed (DEL+ADD) update */

 

#define OP_ADD          1
#define OP_DEL          2
#define OP_UPD          3
#define OP_NOOP         4

/* class type for dummy objects */
#define DUMMY_TYPE	100

/* Definitions for "refer" (domain referral mechanism) */
#define S_RIPE "RIPE"
#define S_INTERNIC "InterNIC"
#define S_SIMPLE "SIMPLE"
#define S_CLIENTADDERSS "CLIENTADDRESS"

/* Definitions for "interface:" attribute */
#define S_IPV4 "ipv4"
#define S_IPV4_UNICAST "ipv4.unicast"
#define S_IPV4_MULTICAST "ipv4.multicast"
#define S_IPV6 "ipv6"
#define S_IPV6_UNICAST "ipv6.unicast"
#define S_IPV6_MULTICAST "ipv6.multicast"

typedef enum {
 RF_RIPE=1,
 RF_INTERNIC,
 RF_SIMPLE,
 RF_CLIENTADDRESS
} refer_t;

#define RF_DEF_PORT	43


/* THESE WERE IN er_UD_errors.h */
#define UD_TAG " "

#include "lg.h"

void UD_do_nrtm(void *args);
void UD_do_updates(void *args);
void UD_init(LG_context_t *ctx);


#endif /* _UD_H */

