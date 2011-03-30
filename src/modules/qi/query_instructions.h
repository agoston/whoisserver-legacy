#ifndef READ_QUERY_INSTRUCTIONS
#define READ_QUERY_INSTRUCTIONS

/***************************************
  $Revision: 1.5 $

  Query instruction module (qi)
  config module.

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
#include "rxroutines.h"
#include "iproutines.h"
#include "mysql_driver.h"
#include "query_command.h"
#include "defs.h"
#include "which_keytypes.h"

#include "access_control.h"


#include "ca_configFns.h"
#include "ca_dictionary.h"
#include "ca_macros.h"
#include "ca_srcAttribs.h"
#include "lg.h"

/* errors */
#define QI_OK       0
#define QI_CANTDB   1
#define QI_SQLERR   2

typedef struct {
  sql_key_t object_id;    /* the key to a full-text record in SQL */
  char *    parent_list;  /* formatted list of parent ranges; */
                          /* please free after use */
} id_parent_t;

#define MAX_INSTRUCTIONS 100

typedef struct Ref_queries_t {
    char *query;
    char *search_table;
} Ref_queries;



typedef struct Query_instruction_t {
  R_Type_t search_type;
  int  queryindex;      /* index into the Query table showing origin of this entry */
  char *query_str;
  char *rx_keys;
  unsigned int rx_srch_mode;    /* radix tree search mode */
  int rx_par_a;                 /* radix tree search depth */
  ip_space_t space;
  rx_fam_t family;
} Query_instruction;

typedef struct Query_instructions_t {
  Query_instruction *instruction[MAX_INSTRUCTIONS];
  const Query_command *qc;       /* pointer to the Query_command structure of this query */
  const ca_dbSource_t *source;   /* current source */
  Query_environ *qe;        /* current query environment (not const, as qe.condat can change value on any socket operation, e.g. connection close by client) */
} Query_instructions;


int QI_execute(Query_instructions *qis);
void QI_free(Query_instructions *qis);
Query_instructions *QI_new(Query_command *qc, Query_environ *qe);
char *QI_queries_to_string(Query_instructions *qis);
char *QI_fast_output(const char *str);
void QI_init (LG_context_t *qi_ctx, LG_context_t *sql_ctx);

#endif /* READ_QUERY_INSTRUCTIONS */
