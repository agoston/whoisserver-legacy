#ifndef READ_PROTOCOL_WHOIS
#define READ_PROTOCOL_WHOIS

/***************************************
  $Revision: 1.1 $

  Protocol whois module (pw).  Whois protocol.

  Status: NOT REVUED, NOT TESTED

  +html+ <DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL>
  +html+ </DL>
  +html+ <PRE>
  Author:
      ottrey
      marek
  +html+ </PRE>
 
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
#include "query_command.h"
/*+ String sizes +*/
#define STR_S   63
#define STR_M   255
#define STR_L   1023
#define STR_XL  4095
#define STR_XXL 16383

void PW_init(LG_context_t *pw_ctx, LG_context_t *query_ctx);
void PW_interact(int sock);
void PW_startqueries();
void PW_stopqueries();
void PW_record_query_start();
void PW_record_query_end();
void PW_log_denied_query();
void PW_run_query();

#endif /* READ_PROTOCOL_WHOIS */
