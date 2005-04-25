#ifndef READ_QUERY_INSTRUCTIONS
#define READ_QUERY_INSTRUCTIONS

/***************************************
  $Revision: 1.2.2.2 $

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
  sql_key_t object_id;    // the key to a full-text record in SQL
  char *    parent_list;  // formatted list of parent ranges; 
                          // please free after use
} id_parent_t;

/*
--- snipped from http://www.tcx.se/Manual/manual.html ---

5.3 Functionality missing from MySQL

The following functionality is missing in the current version of MySQL. For a prioritized list indicating when new extensions may be added to MySQL, you should consult the
online MySQL TODO list. That is the latest version of the TODO list in this manual. See section F List of things we want to add to MySQL in the future (The TODO). 

5.3.1 Sub-selects

The following will not yet work in MySQL: 

SELECT * FROM table1 WHERE id IN (SELECT id FROM table2);
SELECT * FROM table1 WHERE id NOT IN (SELECT id FROM table2);

However, in many cases you can rewrite the query without a sub select: 

SELECT table1.* FROM table1,table2 WHERE table1.id=table2.id;
SELECT table1.* FROM table1 LEFT JOIN table2 ON table1.id=table2.id where table2.id IS NULL

For more complicated sub queries you can create temporary tables to hold the sub query. 

MySQL only supports INSERT ... SELECT ... and REPLACE ... SELECT ... Independent sub-selects will be probably be available in 3.24.0. You can now use the function IN() in other
contexts, however. 

--- end snip ---

Ie. Try using a LEFT JOIN to do the "NOT IN"/ "MINUS" equivalent.

*/

/*
  mysql optimizer is sometimes sub-optimal, 
  therefore we force the join order with STRAIGHT_JOIN (MB, 2000/05/02)

  
*/

/* RIPE 6 */
//#define Q_OBJECTS     "SELECT last.object_id, last.sequence_id, last.object ,last.object_type FROM  %s IDS STRAIGHT_JOIN last,object_order WHERE last.object_id=IDS.id AND last.object_type != 100 AND last.object_type = object_order.object_type ORDER BY recursive, order_code" 

#define Q_OBJECTS "SELECT last.object_id, last.sequence_id, last.object, last.object_type, last.pkey, recursive, gid FROM %s IDS, last, last glast, object_order, object_order gorder WHERE (IDS.gid=glast.object_id AND glast.object_type=gorder.object_type AND glast.object_type != 100) AND (IDS.id=last.object_id AND last.object_type=object_order.object_type AND last.object_type != 100) ORDER BY %s recursive, object_order.order_code" 

/* Query for finding person/role objects recursively (when -r  isn't specified) */
#define Q_REC         "INSERT IGNORE INTO %s SELECT pe_ro_id,1,object_id FROM %s IDS, %s WHERE object_id = IDS.id"
/* Query for finding organisation objects recursively (when -r isn't specified) */
#define Q_REC_ORG  "INSERT IGNORE INTO %s SELECT org_id,1,object_id FROM %s IDS, %s WHERE object_id = IDS.id"
/* Query for finding irt objects recursively (when -c (or -b) is specified) */
#define Q_REC_IRT  "INSERT IGNORE INTO %s SELECT irt_id,1,object_id FROM %s IDS, %s WHERE object_id = IDS.id"
 
#define Q_ALTER_TMP "ALTER TABLE %s ADD COLUMN gid INT NOT NULL DEFAULT 0"
#define Q_ALTER_TMP_GROUPED "ALTER TABLE %s ADD COLUMN gid INT NOT NULL DEFAULT 0, DROP PRIMARY KEY, ADD PRIMARY KEY (id, gid)"
#define Q_UPD_TMP "UPDATE %s SET gid=id"

#if 0
#define Q_NO_OBJECTS  "SELECT object_id, sequence_id, object FROM last WHERE object_id = 0"
#endif 

#define MAX_INSTRUCTIONS 100

#define LIST_HAS_ATTR "SELECT DISTINCT gid FROM %s IDS,%s REF WHERE IDS.id=REF.object_id"

typedef struct Query_instruction_t {
  R_Type_t search_type;
  int  queryindex;/* index into the Query table showing origin of this entry */
  char *query_str;
  char *rx_keys;
  unsigned int rx_srch_mode;
  int rx_par_a;
  ip_space_t space;
  rx_fam_t family;
} Query_instruction;

typedef struct Query_instructions_t {
  Query_instruction *instruction[MAX_INSTRUCTIONS];
  unsigned int filtered;
  unsigned int fast;
  unsigned int recursive;
  const Query_command *qc; /* pointer to the Query_command structure of this query */
} Query_instructions;


int QI_execute(ca_dbSource_t *dbhdl, Query_instructions *qis, Query_environ *qe, acc_st *acc_credit, acl_st *acl);
void QI_free(Query_instructions *qis);
Query_instructions *QI_new(const Query_command *qc, const Query_environ *qe);
char *QI_queries_to_string(Query_instructions *qis);
char *QI_fast_output(const char *str);
void QI_init (LG_context_t *qi_ctx, LG_context_t *sql_ctx);

#endif /* READ_QUERY_INSTRUCTIONS */
