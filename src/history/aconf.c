/***************************************
  $Revision: 1.1 $

  Archive.  archive.c - whois DB archiving.

  Status: NOT REVIEWED

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


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "aconf.h"

miniconf_conf conf[10];

char* host1, * port1, * db1, * user1, * pass1;
char* host2, * port2, * db2, * user2, * pass2;

SQ_connection_t* RIPE_conn;
SQ_connection_t* archive_conn;

void read_configuration() {
  miniconf_create_param(&conf[0], "host1", 60);
  miniconf_create_param(&conf[1], "port1", 60);
  miniconf_create_param(&conf[2], "db1", 60);
  miniconf_create_param(&conf[3], "user1", 60);
  miniconf_create_param(&conf[4], "pass1", 60);
  miniconf_create_param(&conf[5], "host2", 60);
  miniconf_create_param(&conf[6], "port2", 60);
  miniconf_create_param(&conf[7], "db2", 60);
  miniconf_create_param(&conf[8], "user2", 60);
  miniconf_create_param(&conf[9], "pass2", 60);
  if (miniconf_read(conf, 10, "history.conf") == 0 &&
      miniconf_read(conf, 10, "/usr/local/ncc/etc/history.conf") == 0 ) {
    printf("Error reading configuration file\n");
    exit(1);
  }
  host1 = conf[0].value;
  port1 = conf[1].value;
  db1   = conf[2].value;
  user1 = conf[3].value;
  pass1 = conf[4].value;
  host2 = conf[5].value;
  port2 = conf[6].value;
  db2   = conf[7].value;
  user2 = conf[8].value;
  pass2 = conf[9].value;
}

void get_db_connections() {
  mysql_init(NULL);

  RIPE_conn = SQ_get_connection(host1, atoi(port1), db1, user1, pass1);
  archive_conn = SQ_get_connection(host2, atoi(port2), db2, user2, pass2);
}
