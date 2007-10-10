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
 
 ******************//******************
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
#ifndef PM_H
#define PM_H

#include <glib.h>
#include "mysql_driver.h"
#include "iproutines.h"
#include "aa.h"
#include "lg.h"

/*+ String sizes +*/
#define STR_S   63
#define STR_M   255
#define STR_L   1023
#define STR_XL  4095
#define STR_XXL 16383

/*+ Maximum size of input that can be recieved from the client. +*/
#define MAX_PM_INPUT_SIZE  256

/* server needs to yield one serial less just in case the last one causes the crash */
#define SAFE_BACKLOG 1

typedef struct _nrtm_q_t {
	char *source;
	int version;
	long first;
	long last;
} nrtm_q_t;

void PM_get_minmax_serial(SQ_connection_t *sql_connection, long *min, long *max);
char *PM_get_serial_object(SQ_connection_t *sql_connection, long serial_number, long *object_type, int *operation);
GString *PM_get_nrtm_sources(ip_addr_t *client_address, char *source);

void PM_interact(int sock);
void PM_init(LG_context_t *ctx);

#endif /* PM_H */
