/***************************************
  $Revision: 1.1 $

  Access control (AC).  ac_persistence.c - functions to make the access
  control tree persistent.

  Status: NOT REVIEWED, NOT TESTED, COMPLETE

  Implementation by: Tiago Antao

  ******************/ /******************
  Copyright (c) 2002                              RIPE NCC

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

/*
  ac_persistence - Does access control persistence

  External entry points are:

  AC_persistence_daemon - Persistence thread
  AC_persistence_save   - Saves the current access control tree
*/

#include "rip.h"
#include <math.h>

/*
  Global variables.
*/

int ac_auto_save; /* This should be encapsulated. */

/* defined in access_persistence.c */
extern LG_context_t *access_ctx;


/*
  Local typedefs.
*/


/* acc_ip
   It is needed because we need to know the IP of a acc on a GList context.
*/
typedef struct {
  acc_st acc;
  ip_prefix_internal_t ip; /* internal?????*/
} acc_ip;


/*
  Local variables.
*/

static rw_lock_t save_lock; /* lock for save operation*/
static char on_save = 0;    /* lack of trylock in TH requires this */

/*
  From access_control.c
*/
void AC_decay_leaf_l(acc_st *leaf);
char AC_prunable(acc_st *leaf);
extern ut_timer_t oldest_timestamp[MAX_IPSPACE_ID+1];

/*
  AC_acc_copy_l:

  Copies the contents of a acc_st variable.

  maybe it should go to access_control.c
*/
void AC_acc_copy_l(acc_st *dest, acc_st *src) {
  memcpy(dest, src, sizeof(acc_st)); /* This is OK for the current struct data */
}

/*
  AC_persistence_get_leaves_hook_l:

  Hook for the return of a Glist of leaves. Constructs an acc_ip.
*/
/* static */
int AC_persistence_get_leaves_hook_l(rx_node_t* node, int level,
    int node_counter, void *con) {
  GList** list;
  acc_ip* ip_acc;
  acc_st* acc;



  list = (GList**) con;
  acc = node->leaves_ptr->data;
  if (acc->changed != AC_ACC_NOT_CHANGED) {
    ip_acc = UT_malloc(sizeof(acc_ip));
    AC_acc_copy_l(&(ip_acc->acc), acc);

    acc->changed = AC_ACC_NOT_CHANGED;

    ip_acc->ip = node->prefix;

    *list = g_list_prepend(*list, ip_acc);
  }
  return RX_OK;
} /* AC_persistence_get_leaves_hook_l */

/*
  ac_persistence_get_leaves:

  Returns a GList of leaves (clone, ie don't forget to deallocate elements)

  Write locks act_runtime.
*/
GList* ac_persistence_get_leaves() {
  GList *list;
  int ret_err, i;

  list = NULL;

	for (i = MIN_IPSPACE_ID; i <= MAX_IPSPACE_ID; i++) {
	  TH_acquire_write_lock(&(act_runtime[i]->rwlock));
	  rx_walk_tree(act_runtime[i]->top_ptr, AC_persistence_get_leaves_hook_l,
	    RX_WALK_SKPGLU, 255, 0, 0, &list, &ret_err);
	  /* dieif(ret_err != RX_OK); -- can be safely ignored as result is always OK*/
	  TH_release_write_lock(&(act_runtime[i]->rwlock));
	}

  return list;
} /* ac_persistence_get_leaves */


/*
  AC_delete_timestamp_l
*/
void AC_delete_timestamp_l(SQ_connection_t* conn) {
  char delete_query[120];

  sprintf(delete_query,
          "DELETE FROM access "
          "      WHERE timestamp<%11.0f AND addr_passes=0 AND denials=0 ",
          ceil(UT_time_getvalue(&oldest_timestamp[IP_V4])) - 10);
  SQ_execute_query(conn, delete_query, NULL);

	/* ... and the same for v6 table */
  sprintf(delete_query,
          "DELETE FROM access6 "
          "      WHERE timestamp<%11.0f AND addr_passes=0 AND denials=0 ",
          ceil(UT_time_getvalue(&oldest_timestamp[IP_V6])) - 10);
  SQ_execute_query(conn, delete_query, NULL);
}


int AC_persistence_get_resultset(SQ_connection_t* sql_conn, int space, SQ_result_set_t **rs) {
  switch (space) {
  	case IP_V4:
  		return SQ_execute_query(sql_conn, "SELECT connections, "
	                                         "addr_passes, denials, queries, "
	                                         "referrals, public_objects, "
	                                         "private_objects, public_bonus, "
	                                         "private_bonus, timestamp, prefix, "
	                                         "prefix_length "
                                 				"FROM   access", rs);

    case IP_V6:
  		return SQ_execute_query(sql_conn, "SELECT connections, "
	                                         "addr_passes, denials, queries, "
	                                         "referrals, public_objects, "
	                                         "private_objects, public_bonus, "
	                                         "private_bonus, timestamp, prefix1, "
	                                         "prefix2, prefix3, prefix4, prefix_length  "
                                 				"FROM   access6", rs);

		default:
			die;
  }
}
/*
  AC_persistence_load:

  Loads the SQL access control data into the act_runtime tree.
  Also, clears old leaves from SQL.

  This is different from discussed design!!! Explain:

  This operation blocks the server. It is all in-memory so it should not take too
  long.

  Pseudo-code:

  connect to admin DB
  SELECT * FROM access
  write lock act_runtime
  for each row
    creates acc_st and ip_prefix
    calculates new decay
    if node still active then
      adds acc_st/ip_prefix to act_runtime
      if timestamp<oldest_timestamp
        oldest_timestamp = timestamp;
  release lock
  DELETE FROM access WHERE timestamp<oldest_timestamp
  close DB

*/
void AC_persistence_load(void) {
  SQ_connection_t* sql_conn;
  SQ_result_set_t* rs;
  SQ_row_t *row;
  int ret_err = RX_OK, i;
  unsigned long timestamp;


  sql_conn = AC_dbopen_admin();

	for (i = MIN_IPSPACE_ID; i <= MAX_IPSPACE_ID; i++) {
	  if (AC_persistence_get_resultset(sql_conn, i, &rs) == 0) {
	    TH_acquire_write_lock(&(act_runtime[i]->rwlock));

	    while ((row = SQ_row_next(rs)) != NULL && ret_err == RX_OK) {
	      acc_st *acc;
	      ip_prefix_t ip;

	      /* Repeat with me: With pre-historic languages you have to do basic
	                         memory management. 8-) */
	      acc = UT_malloc(sizeof(acc_st));

	      switch (i) {
	      	case IP_V4:
	      		IP_pref_f2b_v4(&ip,
	          		           SQ_get_column_string_nocopy(rs, row, 10),
	              		       SQ_get_column_string_nocopy(rs, row, 11));
		       	break;

		      case IP_V6:
	      		IP_pref_f2b_v6_32(&ip,
	          		           SQ_get_column_string_nocopy(rs, row, 10),
	          		           SQ_get_column_string_nocopy(rs, row, 11),
	          		           SQ_get_column_string_nocopy(rs, row, 12),
	          		           SQ_get_column_string_nocopy(rs, row, 13),
	              		       SQ_get_column_string_nocopy(rs, row, 14));
		       	break;
	      }

	      /*
	        SQ_get_column_int errors are not detected.
	        In theory it should not be a problem

	        FIXME: on x86+gcc, sizeof(long)==sizeof(int), however, this is not
	        			 necessarily true on other platforms, which leads to 0s in all
	        			 fields on lsb->msb architectures
	        			 Should fix SQ_* module declaration to int*
	      */
	      SQ_get_column_int(rs, row,  0, (long *)&acc->connections);
	      SQ_get_column_int(rs, row,  1, (long *)&acc->addrpasses);
	      SQ_get_column_int(rs, row,  2, (long *)&acc->denials);
	      SQ_get_column_int(rs, row,  3, (long *)&acc->queries);
	      SQ_get_column_int(rs, row,  4, (long *)&acc->referrals);
	      SQ_get_column_int(rs, row,  5, (long *)&acc->public_objects);
	      SQ_get_column_int(rs, row,  6, (long *)&acc->private_objects);
	      acc->public_bonus = atof(SQ_get_column_string_nocopy(rs, row, 7));
	      acc->private_bonus = atof(SQ_get_column_string_nocopy(rs, row, 8));
	      SQ_get_column_int(rs, row, 9, (long*)&timestamp);

	      UT_time_set(&acc->timestamp, timestamp, 0);

	      /* clear simultaneous connections counter */

	      acc->sim_connections = 0;

	      /* Mark it as not changed */
	      acc->changed = AC_ACC_NOT_CHANGED;

	      AC_decay_leaf_l(acc);
	      /* lets be memory efficient and not create if it is decayed */
	      if (AC_prunable(acc)) {
	        UT_free(acc);
	      } else {
	        if (UT_timediff(&acc->timestamp, &oldest_timestamp[i]) > 0 &&
	            acc->denials == 0 && acc->addrpasses == 0) {
	          oldest_timestamp[i] = acc->timestamp;
	        }
	        // what about if it already exists?
	        ret_err = rx_bin_node(RX_OPER_CRE, &ip, act_runtime[i], (rx_dataleaf_t*) acc);
	      }
	    }
	    // if ret_err...

	    SQ_free_result(rs);
	    TH_release_write_lock(&(act_runtime[i]->rwlock));
	  }
	  else {
	    LG_log(access_ctx, LG_ERROR, "Couldn't load access table: %s", SQ_error(sql_conn));
	  }
	}

  AC_delete_timestamp_l(sql_conn);
  SQ_close_connection(sql_conn);

} /* AC_persistence_load */

/* insert/update the acc element into DB
 * handle both v4 and v6 addresses */
int AC_persistence_store_record(SQ_connection_t *sql_conn, ip_prefix_t *ip, acc_st *acc) {
	char *fieldnames, *tablename, actvalues[64], sql[512];
	switch (ip->ip.space) {
		case IP_V4: {
			fieldnames = "prefix";
			tablename = "access";
			snprintf(actvalues, 256, "%u", IP_addr_b2v4_addr(&ip->ip));
			break;
		}

		case IP_V6: {
			fieldnames = "prefix1, prefix2, prefix3, prefix4";
			tablename = "access6";
			snprintf(actvalues, 256, "%u, %u, %u, %u", IP_addr_b2v6_addr(&ip->ip, 0), IP_addr_b2v6_addr(&ip->ip, 1),
																								 IP_addr_b2v6_addr(&ip->ip, 2), IP_addr_b2v6_addr(&ip->ip, 3));
			break;
		}

		default:
			die;
	}

	/* use REPLACE, so we don't need to run an UPDATE command if duplicate key found */
	/* (this is a mysql-specific sql command) */
  sprintf(sql, "REPLACE INTO %s(%s, prefix_length, connections, addr_passes, denials, queries, referrals, public_objects, private_objects, public_bonus, private_bonus, timestamp)"
			" VALUES(%s, %d, %d, %d, %d, %d, %d, %d, %d, %f, %f, %10.0f)",
    tablename, fieldnames, actvalues,
    IP_pref_b2_len(ip),
    acc->connections, acc->addrpasses,
    acc->denials, acc->queries,
    acc->referrals, acc->public_objects,
    acc->private_objects, acc->public_bonus,
    acc->private_bonus,
    ceil(UT_time_getvalue(&acc->timestamp)));

  return SQ_execute_query(sql_conn, sql, NULL);
}

/*
  AC_persistence_walk_l:

  Writes each leaf (or not, depending on timestamp & changed).

  Pseudo-code:

  for each element
    if acc.changed = not changed
      continue
    if acc.changed == new
      insert
      if insert ok
        return
    update (also on special acc.changed cases)
    if fail log

*/
static void AC_persistence_walk_l(GList *list) {
  acc_ip *element;
  SQ_connection_t *sql_conn;
  GList *curr_list;

  sql_conn  = AC_dbopen_admin();
  curr_list = list;

  while (curr_list) {
    element   = curr_list->data;
    curr_list = g_list_next(curr_list);
    if (element->acc.changed == AC_ACC_NOT_CHANGED
    		|| IP_addr_isnull(&element->ip.ip) == 0) continue;

    if (element->acc.changed == AC_ACC_NEW) {
    	if (AC_persistence_store_record(sql_conn, &element->ip, &element->acc) != 0 ) {
	      LG_log(access_ctx, LG_ERROR, "Problems with database updates on "
	                                   "admin database");
	      SQ_close_connection(sql_conn);
	      return;
    	}
    }
  }
  AC_delete_timestamp_l(sql_conn);
  SQ_close_connection(sql_conn);
}


/*
  AC_persistence_save_l:

  Saves the act_runtime tree into the SQL access control data table:
    This is the function that realy does it (not the public
    AC_persistence_save).

  Pseudo-code:

  gets a list of relevant leaves
  writes the leaves to SQL
  destroy the list of leaves

*/
static void AC_persistence_save_l(void) {
  GList *leaves;
  int length;
  int cont;

  leaves = ac_persistence_get_leaves();

  if (leaves) {
    AC_persistence_walk_l(leaves);

    /* free GList (&contents!!) */
    length = g_list_length(leaves);
    for(cont=0; cont<length; cont++) {
      UT_free(g_list_nth_data(leaves, cont));
    }
    g_list_free(leaves);
  }
} /* AC_persistence_save_l */


/*
  AC_persistence_save:

  Saves the act_runtime tree into the SQL access control data table:
   Checks if another thread is saving, if so doesn't save.
   Saving is done by a call to ac_persistence_save which does the real job.

   Returns AC_OK if done or AC_SAVING in the case of another save process
   is running.

   Pseudo-code:

   if !(acquire saving rights)
     return AC_SAVING
   save
   release saving rights
   return AC_OK
*/
int AC_persistence_save(void) {


  /* Let's see if a save is already running... */
  TH_acquire_write_lock(&save_lock);
  /* Save already running*/
  if (on_save) {
    TH_release_write_lock(&save_lock);
    return AC_SAVING;
  }
  /* else ...
     We have a go!
  */
  on_save = 1;
  TH_release_write_lock(&save_lock);

  AC_persistence_save_l();

  /* Signal end of save operation */
  TH_acquire_write_lock(&save_lock);
  on_save = 0;
  TH_release_write_lock(&save_lock);

  return AC_OK;

} /* AC_persistence save */


/*
  AC_persistence_init:

  Initializes the persistence system:

   1. Initializes the dump control mutex.
   2. Initializes the oldest_timestamp.
   3. If required by user loads SQL accounting data.
   4. Sets auto save.
*/
int AC_persistence_init(void) {
	int i;

  TH_init_read_write_lock(&save_lock);

	for (i = MIN_IPSPACE_ID; i <= MAX_IPSPACE_ID; i++) {
	  UT_timeget(&oldest_timestamp[i]);
	}

  if (ca_get_ac_load) {
    AC_persistence_load();
  }

  ac_auto_save = ca_get_ac_auto_save;
  return AC_OK;
} /* AC_persistence_init */


/*
  AC_persistence_daemon:

  Implements the top-level thread code:

  Pseudo-code:

  while (CO_get_do_server)
    Sleep.
    If auto save
      Dumps the current version of the act_runtime tree to the persistence
        store.
*/
int AC_persistence_daemon(void) {

  TA_add(0, "persistence");

  while(CO_get_do_server()) {
    SV_sleep(ca_get_ac_save_interval);
    if (ac_auto_save) {
      AC_persistence_save();
    }
  }

  TA_delete();

  return AC_OK;
} /* AC_persistence_daemon */
