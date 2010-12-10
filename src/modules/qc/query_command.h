#ifndef READ_QUERY_COMMAND
#define READ_QUERY_COMMAND

/***************************************
  $Revision: 1.3 $

  Query command module (qc)

  Status: NOT REVUED, TESTED

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

#include <glib.h>
#include "bitmask.h"
#include "sk.h"


typedef enum {
  QC_EMPTY,
  QC_PARERR,
  QC_SYNERR,
  QC_NOKEY,
  QC_TEMPLATE,
  QC_HELP,
  QC_REAL,
  QC_FILTERED,

  QC_TYPE_MAX
} qc_qtype_t;
/* now this must be sync'ed with that: */
#ifdef QC_IMPL
char *qrytype_str[] = {
  "EMPTY",
  "PARAMETER ERROR",
  "SYNTAX ERROR",
  "NO KEY",
  "TEMPLATE",
  "HELP",
  "REAL",
  "FILTERED"
};
#endif


/* types of -q queries */
#define QC_Q_SOURCES 0
#define QC_Q_VERSION 1
#define QC_Q_TYPES   2


typedef struct Query_environ_t {
    sk_conn_st condat;
    unsigned int k;
    GList *sources_list;
    char *version;
    ip_addr_t eIP; /* effective IP */
    char eIPs[IP_ADDRSTR_MAX]; /* effective IP string */
} Query_environ;

typedef struct Query_command_t {
  qc_qtype_t   query_type;

  /* -c: query for the most specific inetnum/inet6num with the "mnt-irt:" 
         attribute set and return the irt object */
  gboolean c_irt_search;
  /* -C: negates -c */
  gboolean C;
  /* -G: non-grouped ouput (default - grouped) */
  gboolean G_group_search;
  /* -B: original output (with e-mail, changed and notify attrs */
  gboolean B;
  /* -b: brief output (keys + abuse-mailbox from object/recursive objects) */
	gboolean b;

  int d;
  int fast;
  int g;
  mask_t inv_attrs_bitmap;
  int recursive;
  int l;
  int m;
  int q;
  int t;
  int v;
  int x;
  int filtered;
  int L;
  int M;
  int R;
  mask_t object_type_bitmap;
  mask_t keytypes_bitmap;
  char *keys;

  /* messages to return to the user about parsing problems */
  GList *parse_messages;
} Query_command;

char *QC_bitmap_to_string(mask_t bitmap);
char *QC_environ_to_string(Query_environ qe);
char *QC_query_command_to_string(Query_command *query_command);
void QC_environ_free(Query_environ *qe);
void QC_free(Query_command *qc);
Query_command *QC_create(const char *query_str, Query_environ *qe);
Query_environ *QC_environ_new(int sock);
Query_environ *QC_environ_update(Query_command *qc, Query_environ *qe);
char *QC_get_qrytype(qc_qtype_t qrytype);
void QC_init (LG_context_t *ctx);
#endif /* READ_QUERY_COMMAND */
