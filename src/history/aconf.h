/***************************************
  $Revision: 1.1 $

  Archive.  archive.c - whois DB archiving.

  Status: NOT REVIEWED, NOT TESTED, NOT COMPLETE

  Implementation by: Tiago Antao

  ******************/ /******************
  Copyright (c) 2002               RIPE NCC
 
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

#ifndef _ACONF_H
#define _ACONF_H
#include <rip.h>
#include "miniconf.h"

extern miniconf_conf conf[10];

extern char* host1;
extern char* db1;
extern char* user1;
extern char* pass1;
extern char* host2;
extern char* db2;
extern char* user2;
extern char* pass2;

extern SQ_connection_t* RIPE_conn;
extern SQ_connection_t* archive_conn;

void read_configuration();
void get_db_connections();

#endif
