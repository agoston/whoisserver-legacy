/***************************************
  $Revision: 1.17 $

  Whois query (wh) - connects to a whois server and returns result

  Status: NOT REVIEWED, TESTED
  
  Design and implementation by: Marek Bukowy
  
  Note:  still not final. Probably SK calls should be moved to the
         calling routine
  
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

#include "rip.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>


/*+ as above - but with connection structures, timeouts, maxlines, etc.
  +*/
int
WH_cd_sock(sk_conn_st *out_cd, char *hostname, unsigned port, 
	char *query, unsigned maxlines, unsigned timeout, int *sk_err)
{
  int s; 
  int lines = 0;
  int err;
  
  
  *sk_err = SK_connect(&s, hostname, port, timeout);
  if (*sk_err != SK_OK)
  {
    err = WH_SK_ERR;
  }
  else 
  {
#define MAX_LINE_SIZE 1024

    char reply[MAX_LINE_SIZE];
    sk_conn_st peer_cd;

    err = WH_OK;

    SK_cd_make( &peer_cd, s, timeout );
    
    SK_cd_puts( &peer_cd, query);
    SK_cd_puts( &peer_cd, "\r\n");
  
    do {
      SK_cd_gets(&peer_cd, reply, MAX_LINE_SIZE);
      SK_cd_puts(out_cd, reply);
    }
    while( ++lines <= maxlines && peer_cd.rtc == 0 );
    
    
    switch(peer_cd.rtc) {
    case 0:
    case SK_DISCONNECT:      
      /* OK */
      break;
    case SK_TIMEOUT:
      *sk_err = SK_TIMEOUT;
      err = WH_SK_ERR;
      break;
    default: 
      die; /* it should not happen - change here when allowing this */
    }

    if( lines >= maxlines ) {
      err = WH_MAXLINES;
    }
    

    SK_cd_close(&peer_cd);
    SK_cd_free (&peer_cd);
  } /* if OK */

  return err;
}

