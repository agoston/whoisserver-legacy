/***************************************
  $Revision: 1.2.6.1 $

  Access control module (ac) - access control for the query part

  Status: NOT REVIEWED, TESTED

  Design and implementation by: Marek Bukowy

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

/*
   test excercises:

   1. add a function to delete an entry from the acl table,
      it should be called from the pc module.

*/

#include "config.h"
#define AC_IMPL
#include "rip.h"

#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <math.h>

#include <unistd.h>
#include <stdlib.h>

/* logging */
LG_context_t *access_ctx;

extern char *suboptarg;
extern int getsubopt(char **optionp, char * const *tokens, char **valuep);

/* formats for printing the access control list entries */
#define ACL_FORMAT        "%10d %10d %10d %10d %10d %10d %10d"
#define ACL_HEADER  "%-25s %10s %10s %10s %10s %10s %10s %10s\n"

/* formats for printing the accounting entries */
#define ACC_FORMAT       "%4d %4d %4d %4d %7d %7d %7d %7.2f %7.1f %7d %10.0f"
#define ACC_HEADER "%-25s %4s %4s %4s %4s %7s %7s %7s %7s %7s %7s %s\n"


typedef struct {
/*  double decay_factor;*/
  unsigned newtotal;
  ut_timer_t current_time;
  GList *prunelist;
} ac_decay_data_t;

ut_timer_t oldest_timestamp[MAX_IPSPACE_ID+1];

/* null addresses of IP spaces (can be extern'd if needed) */
char *NULLIPADDR[] = {NULL, "0.0.0.0", "::"};
char *NULLIPRANG[] = {NULL, "0.0.0.0/0", "::/0"};

/*
  Initialises the access persistence

  ctx   - logging context
 */
void
AC_init (LG_context_t *ctx)
{
  access_ctx = ctx;
}

/*++++++++++++++++++++++++++++++++++++++
  ac_to_string_header:

  produce a header for the access stats printout

  returns an allocated string
  ++++++++++++++++++++++++++++++++++++++*/
static
char *ac_to_string_header(void)
{
  char *result_buf;

  result_buf = UT_malloc(256);

  sprintf(result_buf, ACC_HEADER,
	  "ip", "conn", "pass", "deny", "qry", "refs", "priv_o", "pub_o",
          "priv_b","pub_b", "simconn", "ts");

  return result_buf;
}

/*++++++++++++++++++++++++++++++++++++++
  ac_to_string:

  Show an access structure

  returns an allocated string
  ++++++++++++++++++++++++++++++++++++++*/
static
char *ac_to_string(GList * leafptr)
{
	char *result_buf;
	acc_st *a = leafptr->data;

	result_buf = UT_malloc(256);

	if (a == NULL) {
		strcpy(result_buf, "DATA MISSING!");
	} else {
		sprintf(result_buf, ACC_FORMAT,
			a->connections,
			a->addrpasses,
			a->denials,
			a->queries,
			a->referrals,
			a->private_objects,
			a->public_objects, a->private_bonus, a->public_bonus, a->sim_connections, UT_time_getvalue(&a->timestamp)
			);
	}

	return result_buf;
}								/* ac_to_string() */


/*++++++++++++++++++++++++++++++++++++++
  AC_credit_to_string:

 Show credit used (for logging of queries)

 acc_st *a     - the credit structure

 returns an allocated string
 ++++++++++++++++++++++++++++++++++++++*/
char *AC_credit_to_string(acc_st *a)
{
  char *result_buf;

  result_buf = UT_malloc(64);

  dieif( a == NULL );

  sprintf(result_buf,"%d+%d+%d%s",
	  a->private_objects,
	  a->public_objects,
	  a->referrals,
	  a->denials ? " **DENIED**" : ""
	  );

  return result_buf;
} /* AC_credit_to_string */


/*+++++++++++++++++++++++++++++++++++++++
  ac_acl_to_string_header:

  produce a header for the acl printout

  returns an allocated string
  ++++++++++++++++++++++++++++++++++++++*/
static char *
ac_acl_to_string_header(void)
{
  char *result_buf;

  result_buf = UT_malloc(256);

  sprintf(result_buf, ACL_HEADER, "ip",
	  /* the names must match those in AC_ar_acl, so just take
	   them from there */
	  AC_ar_acl[AC_AR_MAXPRIVATE],
	  AC_ar_acl[AC_AR_MAXPUBLIC],
	  AC_ar_acl[AC_AR_MAXDENIALS],
	  AC_ar_acl[AC_AR_DENY],
	  AC_ar_acl[AC_AR_TRUSTPASS],
	  AC_ar_acl[AC_AR_THRESHOLD],
	  AC_ar_acl[AC_AR_MAXCONN]
	  );


  return result_buf;
}



/*++++++++++++++++++++++++++++++++++++++
  ac_acl_to_string:

  Show an access control list structure

  returns an allocated string
  ++++++++++++++++++++++++++++++++++++++*/
static
char *ac_acl_to_string(GList *leafptr)
{
  char *result_buf;
  acl_st *a = leafptr->data;

  result_buf = UT_malloc(256);

  if( a != NULL ) {
    sprintf(result_buf, ACL_FORMAT,
            a->maxprivate,
	    a->maxpublic,
	    a->maxdenials,
            a->deny,
            a->trustpass,
			a->threshold,
			a->maxconn
            );
  }
  else
  {
    strcpy(result_buf, "DATA MISSING\n");
  }

  return result_buf;
} /* ac_acl_to_string() */


/*+++++++++++++++++++++++++++++++++++++++
  ac_find_acl_l:

  find the exact or exact/less specific match for the given prefix in the acl tree.
  DOES NOT COPY THE RESULT!

  ip_prefix_t *prefix - prefix to look for
  acl_st *store_acl  - pointer to store the acl struct

  returns error code from RX or OK

  MT-Note: assumes locked acl tree
  ++++++++++++++++++++++++++++++++++++++*/
static int ac_find_acl_l(rx_srch_mt searchmode, ip_prefix_t * prefix, acl_st **store_acl)
{
	GList *datlist = NULL;
	int ret_err;
	rx_datref_t *datref;
	ip_prefix_t *localpref = IP_truncate_pref_v6(prefix, IPV6_ADDRESS_ACL_BITS);

	/* accept only RX_SRCH_EXLESS | RX_SRCH_EXACT modes */
	dieif(searchmode != RX_SRCH_EXLESS && searchmode != RX_SRCH_EXACT);

	/* it must work */
	dieif((ret_err = RX_bin_search(searchmode, 0, 0, act_acl[localpref->ip.space], localpref, &datlist, RX_ANS_ALL)) != RX_OK);

	/* In exless mode, something must be found or the acl tree is not configured at all !
	   There always must be a catch-all record with defaults */
	dieif(searchmode == RX_SRCH_EXLESS && g_list_length(datlist) == 0);

	// check if we found something
	if (datlist) {
	    datref = (rx_datref_t *) g_list_nth_data(datlist, 0);
	    *store_acl = ((acl_st *) datref->leafptr);
	    wr_clear_list(&datlist);
	} else {
	    *store_acl = NULL;
	    ret_err = RX_DATNOF;
	}

	/* free malloced area */
	if (prefix != localpref)
		free(localpref);

	return ret_err;
}
/* ac_find_acl_l */

/*+++++++++++++++++++++++++++++++++++++++
  AC_findcreate_acl_l:

  find or create an entry for the given prefix in the acl tree.

  ip_prefix_t *prefix - prefix to look for

  acl_st **store_acl  - pointer to store the ptr to the acl struct
                        (initialised to the values of the parent entry
			if just created)

  returns error code from RX or OK

  MT-Note: assumes locked acl tree
  ++++++++++++++++++++++++++++++++++++++*/
int AC_findcreate_acl_l(ip_prefix_t * prefix, acl_st ** store_acl)
{
	GList *datlist = NULL;
	int ret_err;
	acl_st *newacl = NULL, *retacl = NULL;
	ip_prefix_t *localpref = IP_truncate_pref_v6(prefix, IPV6_ADDRESS_ACL_BITS);

	ret_err = RX_bin_search(RX_SRCH_EXACT, 0, 0, act_acl[localpref->ip.space], localpref, &datlist, RX_ANS_ALL);

	if (NOERR(ret_err)) {
		switch (g_list_length(datlist)) {
		case 0:
			/* make the new one inherit all parameters after the old one */
			ac_find_acl_l(RX_SRCH_EXLESS, localpref, &retacl);

			newacl = UT_malloc(sizeof(acl_st));
			*newacl = *retacl;

			/* link in */
			rx_bin_node(RX_OPER_CRE, localpref, act_acl[localpref->ip.space], (rx_dataleaf_t *) newacl);
			break;

		case 1:{
				/* Uh-oh, the guy is already known ! (or special, in any case) */
				rx_datref_t *datref = (rx_datref_t *) g_list_nth_data(datlist, 0);
				newacl = (acl_st *) datref->leafptr;
			}
			break;
		default:
			die;
		}
	}

	/* free malloced area */
	if (prefix != localpref)
		free(localpref);

	/* free search results */
	wr_clear_list(&datlist);

	/* store */
	*store_acl = newacl;
	return ret_err;
}
/* AC_findcreate_acl_l */


/*+++++++++++++++++++++++++++++++++++++++
  AC_findcreate_account_l:

  finds exact prefix in the accounting tree
  or creates area initialised to zeros + sets ptr to it.

  rx_tree_t *tree     - the tree

  ip_prefix_t *prefix - prefix to look for

  acc_st **store_acl  - pointer to store the ptr to the account struct

  returns error code from RX or OK

  MT-Note: assumes locked accounting tree

  FIXME: It is called with a write lock on the tree from _EVERYWHERE_. There is no need to do that;
         a simple read lock is OK, and we only need to fetch a write lock if we create a new node
         I won't fix this now, as a complete thread+locking optimization is needed anyway - Agoston, 2006
  ++++++++++++++++++++++++++++++++++++++*/
int AC_findcreate_account_l(rx_tree_t * tree, ip_prefix_t * prefix, acc_st ** acc_store)
{
	GList *datlist = NULL;
	int ret_err;
	acc_st *recacc = NULL;
	ip_prefix_t *localpref = IP_truncate_pref_v6(prefix, IPV6_ADDRESS_ACCESS_BITS);

	if ((ret_err = RX_bin_search(RX_SRCH_EXACT, 0, 0, tree, localpref, &datlist, RX_ANS_ALL)) == RX_OK) {
		switch (g_list_length(datlist)) {
		case 0:
			/* need to create a new accounting record */
			recacc = UT_malloc(sizeof(acc_st));

			/*  counters = init to zeros */
			memset(recacc, 0, sizeof(acc_st));

			recacc->changed = AC_ACC_NEW;

			/* attach. The recacc is to be treated as a dataleaf
			   (must use lower levels than RX_asc_*)
			 */
			ret_err = rx_bin_node(RX_OPER_CRE, localpref, act_runtime[localpref->ip.space], (rx_dataleaf_t *) recacc);
			if (ret_err != RX_OK) {
				LG_log(access_ctx, LG_ERROR, "rx_bin_node() returned %d", ret_err);
			}
			break;
		case 1:
			{
				rx_datref_t *datref = (rx_datref_t *) g_list_nth_data(datlist, 0);
				/* OK, there is a record already */
				recacc = (acc_st *) datref->leafptr;
			}
			break;
		default:
			die;				/* there shouldn't be more than 1 entry per IP */
		}
	} else {
		LG_log(access_ctx, LG_ERROR, "RX_bin_search() returned %d", ret_err);
	}

	/* free malloced area */
	if (prefix != localpref)
		free(localpref);

	wr_clear_list(&datlist);

	*acc_store = recacc;

	return ret_err;
}


/*++++++++++++++++++++++++++++++++++++++
  AC_fetch_acc:

  Finds the runtime accounting record for this IP,
  stores a copy of it in acc_store.
  If not found, then it is created and initialised to zeros in findcreate()

  ip_addr_t *addr  - address

  acc_st *acc_store - pointer to store the account struct

  MT-Note: locks/unlocks the accounting tree
  ++++++++++++++++++++++++++++++++++++++*/
int AC_fetch_acc(ip_addr_t * addr, acc_st * acc_store)
{
	int ret_err;
	ip_prefix_t prefix;
	acc_st *ac_ptr;

	prefix.ip = *addr;
	prefix.bits = IP_sizebits(addr->space);

	TH_acquire_write_lock(&(act_runtime[addr->space]->rwlock));

	ret_err = AC_findcreate_account_l(act_runtime[addr->space], &prefix, &ac_ptr);
	*acc_store = *ac_ptr;

	TH_release_write_lock(&(act_runtime[addr->space]->rwlock));

	return ret_err;
}								/* AC_fetch_acc() */


/*++++++++++++++++++++++++++++++++++++++
  AC_check_acl:

  search for this ip or less specific record in the access control tree

  if( bonus in combined runtime+connection accountings > max_bonus in acl)
            set denial in the acl for this ip (create if needed)
  if( combined denialcounter > max_denials in acl)
            set the permanent ban in acl; save in SQL too
  calculate credit if pointer provided
  save the access record (ip if created or found/prefix otherwise)
            at *acl_store if provided

  ip_addr_t *addr  - address

  acc_st *acc_store - pointer to store the *credit* account struct

  acl_st *acl_store - pointer to store the acl struct

  any of the args except address can be NULL

  returns error code from RX or OK

  MT-Note: locks/unlocks the accounting tree
  ++++++++++++++++++++++++++++++++++++++*/
int AC_check_acl(ip_addr_t * addr, acc_st * credit_acc, acl_st * acl_store)
{
	ip_prefix_t prefix;
	int ret_err = AC_OK;
	acl_st *acl_record;

	prefix.ip = *addr;
	prefix.bits = IP_sizebits(addr->space);

	/* lock the tree accordingly */
	TH_acquire_read_lock(&(act_acl[addr->space]->rwlock));

	/* find an applicable record */
	ac_find_acl_l(RX_SRCH_EXLESS, &prefix, &acl_record);

	/* calculate the credit if pointer given */
	if (credit_acc) {
		acc_st run_acc;
		AC_fetch_acc(addr, &run_acc);

		memset(credit_acc, 0, sizeof(acc_st));

		/* credit = -1 if unlimited, otherwise credit = limit - bonus */
		credit_acc->public_objects = (acl_record->maxpublic == -1)
			? -1				/* -1 == unlimited */
			: (acl_record->maxpublic - run_acc.public_bonus);

		credit_acc->private_objects = (acl_record->maxprivate == -1)
			? -1				/* -1 == unlimited */
			: (acl_record->maxprivate - run_acc.private_bonus);
	}

	/* copy the acl record if asked for it */
	if (acl_store) {
		*acl_store = *acl_record;
	}

	/* release lock */
	TH_release_read_lock(&(act_acl[addr->space]->rwlock));

	return ret_err;
}

/* the decay factor of
   f(t) = exp(-a*t)
   a = -ln(0.5) / t
   so for T being the half-life period and v being the sampling interval
   used as the unit of time
   a = -ln(0.5) / T;
   f(t+x) = exp(-a(t+x)) = f(t)*f(x) = f(t)*exp(-ax) =
   = f(t)*exp(ln(0.5)*v/T)
   so you multiply the previous value by exp(ln(0.5)*v/T)

    dec_dat.decay_factor =
      exp ( -0.693147180559945 * exactinterval / ca_get_ac_decay_halflife) ;
*/
inline void AC_decay_leaf_l(acc_st * leaf, ut_timer_t * current)
{
	if (UT_time_getvalue(&leaf->timestamp) > 1) {
		float time_diff;

		time_diff = UT_timediff(&leaf->timestamp, current);
		if (abs(time_diff) > 0.2) {
			double factor;
			factor = exp(-0.693147180559945 * time_diff / ca_get_ac_decay_halflife);
			leaf->private_bonus *= factor;
			leaf->public_bonus *= factor;
		}
	}
}

/*++++++++++++++++++++++++++++++++++++++
  AC_acc_addup:

  Add/subtract the values from one accounting structure to another

  acc_st *a   this one gets changed

  acc_st *b   this one provides the values to change a

  int minus   triggers subtraction if non-zero

+++++++++++++++++++++++++++++++++++++++*/
void AC_acc_addup(acc_st *a, acc_st *b, int minus)
{
  int mul = minus ? -1 : 1;
  ut_timer_t current_time;

  UT_timeget(&current_time);

  AC_decay_leaf_l(a, &current_time);

  a->timestamp = current_time;

  /* add all counters from b to those in a */
  a->connections     +=  mul * b->connections;
  a->addrpasses      +=  mul * b->addrpasses;

  a->denials         +=  mul * b->denials;
  a->queries         +=  mul * b->queries;
  a->referrals       +=  mul * b->referrals;
  a->public_objects  +=  mul * b->public_objects;
  a->private_objects +=  mul * b->private_objects;
  a->private_bonus   +=  mul * b->private_bonus;
  a->public_bonus    +=  mul * b->public_bonus;

  a->sim_connections +=  mul * b->sim_connections;

  if (a->changed == AC_ACC_NOT_CHANGED) {
    a->changed = AC_ACC_CHANGED;
  }


}/* AC_acc_addup */

/*++++++++++++++++++++++++++++++++++++++
  commit_credit_l:

  performs the commit on an accounting tree (locks them first)
  stores a copy of the accounting record at rec_store

  Assumes locked tree.

  rx_tree_t *tree      - the tree

  ip_prefix_t *prefix  - prefix (usually a /32)

  acc_st *acc_conn     - credit used

  acc_st *rec_store    - pointer to store the account struct or NULL

  returns error code from AC_findcreate_account_l or OK

  MT-Note: locks/unlocks the accounting tree
+++++++++++++++++++++++++++++++++++++++*/
static
int
AC_commit_credit_l(rx_tree_t *tree, ip_prefix_t *prefix,
		 acc_st *acc_conn, acc_st *rec_store )
{
  acc_st      *accountrec;
  int    ret_err;


  acc_conn->private_bonus = acc_conn->private_objects;
  acc_conn->public_bonus  = acc_conn->public_objects;

  ret_err = AC_findcreate_account_l(act_runtime[prefix->ip.space], prefix, &accountrec);

  if( NOERR(ret_err)) {
    AC_acc_addup(accountrec, acc_conn, ACC_PLUS);
  }

  if( rec_store ) {
    *rec_store = *accountrec;
  }

  return ret_err;
}/* AC_commit_credit */

/*++++++++++++++++++++++++++++++++++++++
  AC_dbopen_admin:

  opens the ADMIN database and returns a pointer to the connection structure
  (rationale: the opening process became a bit bloated and is done twice,
  so I put it into a separate function)
++++++++++++++++++++++++++++++++++++++*/
SQ_connection_t *AC_dbopen_admin(void)
{
	SQ_connection_t *con = NULL;
	char *dbhost = ca_get_ripadminhost;
	char *dbname = ca_get_ripadmintable;
	char *dbuser = ca_get_ripadminuser;
	char *dbpass = ca_get_ripadminpassword;
	unsigned dbport = ca_get_ripadminport;

	if ((con = SQ_get_connection(dbhost, dbport, dbname, dbuser, dbpass)) == NULL) {
		fprintf(stderr, "ERROR %d: %s\n", SQ_errno(con), SQ_error(con));
		die;
	}

	UT_free(dbhost);
	UT_free(dbname);
	UT_free(dbuser);
	UT_free(dbpass);

	return con;
}

/*++++++++++++++++++++++++++++++++++++++
  AC_acl_sql:

  updates/creates a record for the given prefix in the acl table of
  the RIPADMIN database. Adds a comment.

  ip_prefix_t *prefix  - prefix

  acl_st *newacl       - new values to store in the database

  char *newcomment     - comment to be added (must not be NULL)

  placeholder: it may return an error code from SQ - as soon as sq
  implements common error scheme

 ++++++++++++++++++++++++++++++++++++++*/
int AC_acl_sql(ip_prefix_t * prefix, acl_st * newacl, char *newcomment)
{
	SQ_connection_t *sql_connection = NULL;
	SQ_result_set_t *result;
	SQ_row_t *row;
	char *oldcomment;
	char *query;
	char querybuf[256];
	ip_prefix_t *localpref = IP_truncate_pref_v6(prefix, IPV6_ADDRESS_ACL_BITS);

	sql_connection = AC_dbopen_admin();

	/* get the old entry, extend it */
	switch (localpref->ip.space) {
	case IP_V4:
		sprintf(querybuf, "SELECT comment FROM acl WHERE "
			"prefix = %u AND prefix_length = %d", IP_addr_b2v4_addr(&localpref->ip), localpref->bits);
		break;

	case IP_V6:
		sprintf(querybuf, "SELECT comment FROM acl6 WHERE "
			"prefix1 = %u AND prefix2 = %u AND prefix3 = %u "
			"AND prefix4 = %u AND prefix_length = %d",
			IP_addr_b2v6_addr(&localpref->ip, 0), IP_addr_b2v6_addr(&localpref->ip, 1),
			IP_addr_b2v6_addr(&localpref->ip, 2), IP_addr_b2v6_addr(&localpref->ip, 3), localpref->bits);
		break;

	default:
		die;
	}

	dieif(SQ_execute_query(sql_connection, querybuf, &result) == -1);

	if (SQ_num_rows(result) == 1) {
		dieif((row = SQ_row_next(result)) == NULL);
		oldcomment = SQ_get_column_string(result, row, 0);
	} else {
		oldcomment = UT_strdup("");
	}

	SQ_free_result(result);

	/* must hold the thing below (REPLACE..blah blah blah) + text */
	query = UT_malloc(strlen(oldcomment) + strlen(newcomment) + 256);

	/* compose new entry and insert it */
	switch (localpref->ip.space) {
	case IP_V4:
		sprintf(query, "REPLACE INTO acl VALUES(%u, %d, %d, %d, %d, %d, %d, %d, %d,"
			"\"%s%s%s\")",
			IP_addr_b2v4_addr(&localpref->ip), localpref->bits,
			newacl->maxprivate, newacl->maxpublic,
			newacl->maxdenials, newacl->deny,
			newacl->trustpass, newacl->threshold, newacl->maxconn, oldcomment, strlen(oldcomment) > 0 ? "\n" : "", newcomment);
		break;

	case IP_V6:
		sprintf(query, "REPLACE INTO acl6 VALUES(%u, %u, %u, %u, %d, %d, %d, %d, %d, %d, %d, %d,"
			"\"%s%s%s\")",
			IP_addr_b2v6_addr(&localpref->ip, 0), IP_addr_b2v6_addr(&localpref->ip, 1),
			IP_addr_b2v6_addr(&localpref->ip, 2), IP_addr_b2v6_addr(&localpref->ip, 3),
			localpref->bits,
			newacl->maxprivate, newacl->maxpublic,
			newacl->maxdenials, newacl->deny,
			newacl->trustpass, newacl->threshold, newacl->maxconn, oldcomment, strlen(oldcomment) > 0 ? "\n" : "", newcomment);
		break;

	default:
		die;
	}

	SQ_execute_query(sql_connection, query, NULL);
	SQ_close_connection(sql_connection);

	UT_free(oldcomment);
	UT_free(query);

	/* free malloced area */
	if (prefix != localpref)
		free(localpref);

	return AC_OK;

}								/* AC_acl_sql */

/*++++++++++++++++++++++++++++++++++++++
  AC_ban_set:

  re/sets the permanent ban flag both in the acl tree in memory
  and the sql table. The "text" is appended to the comment
  in the sql record (the expected cases are
  - "automatic" in case the limit is exceeded and ban is set by s/w
  - "manual"    in case it is (un)set from the config iface

  ip_prefix_t *prefix   - prefix

  char *text            - usually "automatic" or "manual"

  int denyflag          - new value of the denyflag (ban)

  returns error code from AC_acl_sql or OK
  +++++++++++++++++++++++++++++++++++++++*/
int
AC_ban_set(ip_prefix_t *prefix, char *text, int denyflag)
{
  acl_st *treeacl;
  char newcomment[256];
  int ret_err;
  time_t  clock;
  char timebuf[26];
  char prefstr[IP_PREFSTR_MAX];

  time(&clock);
  ctime_r(&clock, timebuf);

  sprintf(newcomment,"%s permanent ban set to %d at %s", text,
	  denyflag, timebuf);

  if( IP_pref_b2a(prefix, prefstr, IP_PREFSTR_MAX) != IP_OK ) {
    die; /* program error - this is already converted so must be OK */
  }

  LG_log(access_ctx, LG_INFO,
	 "%s permanent ban set to %d for %s", text, denyflag, prefstr );

  TH_acquire_write_lock( &(act_acl[prefix->ip.space]->rwlock) );

  /* find a record in the tree */
  if( NOERR(ret_err = AC_findcreate_acl_l( prefix, &treeacl )) ) {
    treeacl->deny = denyflag;
    ret_err = AC_acl_sql( prefix, treeacl, newcomment );
  }
  TH_release_write_lock( &(act_acl[prefix->ip.space]->rwlock) );

  return ret_err;
}/* AC_ban_set */


/*++++++++++++++++++++++++++++++++++++++
  AC_asc_ban_set:

  This is not used, should be removed?

  sets ban on text address/range. Parses the text address/range/prefix
  and then calls AC_ban_set on that prefix.

  Precondition: if the key is a range, it must decompose into one prefix

  returns error code from IP_smart_conv, AC_ban_set or
  AC_INVARG if range composed
  +++++++++++++++++++++++++++++++++++++++*/
int AC_asc_ban_set(char *addrstr, char *text, int denyflag) {
    int ret_err;
    GList *preflist = NULL;

    if ((ret_err = IP_smart_conv(addrstr, 0, 0, &preflist, IP_PLAIN, NULL)) != IP_OK) {
        return ret_err;
    }

    /* allow only one prefix */
    /* The argument can be even a range, but must decompose into one prefix */
    if (NOERR(ret_err) && g_list_length(preflist) != 1) {
        ret_err = AC_INVARG;
    }

    if (NOERR(ret_err)) {
        ret_err = AC_ban_set((g_list_first(preflist)->data), text, denyflag);
    }

    wr_clear_list(&preflist);

    return ret_err;
}/* AC_asc_ban_set */

/*++++++++++++++++++++++++++++++++++++++
  AC_asc_all_set:

  take ascii prefix and find/create a new entry, inheriting all parameters
  and then set them according to the array of args.

+*/
int AC_asc_all_set(ip_prefix_t * prefix, char *comment, char *array[])
{
	int ret_err;
	acl_st *treeacl;
	int i;

	TH_acquire_write_lock(&(act_acl[prefix->ip.space]->rwlock));

	/* find/create a record in the tree */
	if (NOERR(ret_err = AC_findcreate_acl_l(prefix, &treeacl))) {

		/* update it from the array */
		for (i = 0; i < AC_AR_SIZE; i++) {
			if (array[i] != NULL) {	/* set only those that have been specified */
				int val, k;

				if ((k = sscanf(array[i], "%d", &val)) < 1) {
					ret_err = AC_INVARG;
					break;		/* quit the for */
				}

				/* otherwise, the value makes sense. Put it in the structure. */
				switch (i) {
				case AC_AR_MAXPRIVATE:
					treeacl->maxprivate = val;
					break;
				case AC_AR_MAXPUBLIC:
					treeacl->maxpublic = val;
					break;
				case AC_AR_MAXDENIALS:
					treeacl->maxdenials = val;
					break;
				case AC_AR_DENY:
					treeacl->deny = val;
					break;
				case AC_AR_TRUSTPASS:
					treeacl->trustpass = val;
					break;
				case AC_AR_THRESHOLD:
					treeacl->threshold = val;
					break;
				case AC_AR_MAXCONN:
					treeacl->maxconn = val;
					break;
				}				/* switch */
			}					/* if array[i] not null */
		}						/* for each array element */

		if (NOERR(ret_err)) {	/* protect against AC_INVARG */
			ret_err = AC_acl_sql(prefix, treeacl, comment);
		}
	}
	/* if find/create OK */
	TH_release_write_lock(&(act_acl[prefix->ip.space]->rwlock));

	return ret_err;
}


/*++++++++++++++++++++++++++++++++++++++
AC_asc_acl_command_set:

  parse a command and set acl options for an entry.
  command syntax:

  <prefix> option=value,option=value,option=value...

  where <option> is defined in AC_ar_acl[] array, value is an integer

  char *command  text of the command.
                 Syntax: ip[/prefixlength] column=value,column=value...
                 Column names as in acl display. Unset columns are inherited.

  char *comment  text to be added to the acl record's comment column.

  ++++++++++++++++++++++++++++++++++++++*/

int AC_asc_acl_command_set(char *command, char *comment)
{
	ip_prefix_t *prefix = NULL;
	char *eop, *eoc, *value;
	char *array[AC_AR_SIZE];
	int ret_err = AC_OK;
	GList *preflist = NULL;

	char *copy = UT_strdup(command);
	char *addrstr = copy;
	eoc = strchr(copy, '\0');	/* points to the end of it */

	memset(array, 0, sizeof(array));

	/* first comes the prefix. Find the space after it
	   and break the string there.
	 */
	if ((eop = strchr(copy, ' ')) == NULL) {
		ret_err = AC_INVARG;
	}

	if (NOERR(ret_err)) {
		*eop++ = 0;

		/* now eop points to the rest of the string (if any). Take options.
		 */
		while (eop != eoc && ret_err == AC_OK) {
			char *sp;

			/* give getsubopt chunks with no spaces */
			if ((sp = strchr(eop, ' ')) != NULL) {
				*sp = 0;
			}

			while (*eop != '\0') {
				int k = getsubopt(&eop, AC_ar_acl, &value);
				if (k < 0) {
					ret_err = AC_INVARG;
					break;
				}

				array[k] = value;
			}

			if (eop != eoc) {	/*getsubopt finished but did not consume all string */
				eop++;			/* must have been a space. advance one */
			}
		}
	}

	/* convert the prefix */
	if (NOERR(ret_err)) {
		ret_err = IP_smart_conv(addrstr, 0, 0, &preflist, IP_PLAIN, NULL);

		/* allow only one prefix */
		/* The argument can be even a range, but must decompose into one prefix */
		if (NOERR(ret_err) && g_list_length(preflist) == 1) {
			prefix = (g_list_first(preflist)->data);
		} else {
			ret_err = AC_INVARG;
		}
	}

	/* perform changes */
	if (NOERR(ret_err)) {
		ret_err = AC_asc_all_set(prefix, comment, array);
	}

	wr_clear_list(&preflist);
	UT_free(copy);

	return ret_err;
}								/* AC_asc_acl_command_set */


/*++++++++++++++++++++++++++++++++++++++
  AC_asc_set_nodeny:

  reset the deny counter in the access tree to 0 (after reenabling).

  Operates on the runtime access tree.

  char *ip      text IP (ip only, not prefix or range).
  +++++++++++++++++++++++++++++++++++++++*/
int AC_asc_set_nodeny(char *ip)
{
	ip_prefix_t *prefix = NULL;
	int ret_err;
	acc_st *ac_ptr;
	GList *preflist = NULL;
	acl_st *treeacl;

	/* convert the prefix */
	ret_err = IP_smart_conv(ip, 0, 0, &preflist, IP_PLAIN, NULL);

	/* allow only one prefix */
	/* The argument can be even a range, but must decompose into one prefix */
	if (NOERR(ret_err) && g_list_length(preflist) == 1) {
		prefix = (g_list_first(preflist)->data);
	} else {
		ret_err = AC_INVARG;
	}

	/* perform changes */
	if (NOERR(ret_err)) {
		TH_acquire_write_lock(&(act_runtime[prefix->ip.space]->rwlock));

		ret_err = AC_findcreate_account_l(act_runtime[prefix->ip.space], prefix, &ac_ptr);
		if (NOERR(ret_err)) {
			// zero out all values influencing denial of a specific ip
			ac_ptr->denials = 0;
			ac_ptr->private_bonus = 0;
			ac_ptr->public_bonus = 0;
			UT_timeget(&ac_ptr->timestamp);
			if (ac_ptr->changed != AC_ACC_NEW)
				ac_ptr->changed = AC_ACC_CHANGED;
		}

		TH_release_write_lock(&(act_runtime[prefix->ip.space]->rwlock));
	}

	if (NOERR(ret_err)) {
		TH_acquire_read_lock(&(act_acl[prefix->ip.space]->rwlock));

		ret_err = ac_find_acl_l(RX_SRCH_EXACT, prefix, &treeacl);
		if (NOERR(ret_err)) {
			treeacl->deny = 0;
			ret_err = AC_acl_sql(prefix, treeacl, "Manual unban");
		}

		TH_release_read_lock(&(act_acl[prefix->ip.space]->rwlock));
	}

	/* cleanup */
	wr_clear_list(&preflist);

	return ret_err;
}

/*++++++++++++++++++++++++++++++++++++++
 *
 * set an access entry from the console command 'set access'
 *
 *++++++++++++++++++++++++++++++++++++++*/
int AC_set_access_command(char *command)
{
	ip_prefix_t prefix;
	char *eop, *eoc, *value;
	int ret = AC_OK, i;
	char *array[AC_ACC_SIZE];
	acc_st *acc;
	ut_timer_t currtime;

	UT_timeget(&currtime);

	memset(array, 0, sizeof(array));
	eoc = strchr(command, '\0');	/* points to the end of it */

	/* first comes the prefix. Find the space after it and break the string there. */
	if ((eop = strchr(command, ' ')) == NULL) {
		ret = AC_INVARG;
	} else {

		*eop++ = 0;

		if (NOERR(ret = IP_addr_t2b(&prefix.ip, command, IP_EXPN))) {
			/* fix the prefix length */
			prefix.bits = IP_sizebits(prefix.ip.space);
			/* if it's a v6 address, truncate to acl length */
			IP_truncate_pref_v6_inplace(&prefix, IPV6_ADDRESS_ACCESS_BITS);

			/* get params into array */
			while (eop != eoc && NOERR(ret)) {
				char *sp;

				/* give getsubopt chunks with no spaces */
				if ((sp = strchr(eop, ' ')) != NULL) {
					*sp = 0;
				}

				while (*eop != '\0') {
					int k = getsubopt(&eop, AC_set_access_opts_str, &value);
					if (k < 0) {
						ret = AC_INVARG;
						break;
					}

					array[k] = value;
				}

				if (eop != eoc) {	/*getsubopt finished but did not consume all string */
					eop++;		/* must have been a space. advance one */
				}
			}

			/* fetch access record */
			TH_acquire_read_lock(&(act_runtime[prefix.ip.space]->rwlock));

			ret = AC_findcreate_account_l(act_runtime[prefix.ip.space], &prefix, &acc);
			if (NOERR(ret)) {
				/* set its status to 'changed' */
				if (acc->changed == AC_ACC_NOT_CHANGED) acc->changed = AC_ACC_CHANGED;
				/* we only change timestamp if a decayed field is modified - see below */

				/* update it from the array */
				for (i = 0; i < AC_ACC_SIZE; i++) {
					if (array[i] != NULL) {	/* set only those that have been specified */
						int val, k;

						if ((k = sscanf(array[i], "%d", &val)) < 1) {
							ret = AC_INVARG;
							break;	/* quit the for */
						}

						/* otherwise, the value makes sense. Put it in the structure. */
						switch (i) {
						case AC_ACC_CONN:
							acc->connections = val;
							break;
						case AC_ACC_PASS:
							acc->addrpasses = val;
							break;
						case AC_ACC_DENY:
							acc->denials = val;
							break;
						case AC_ACC_REFS:
							acc->referrals = val;
							break;
						case AC_ACC_PRIV_O:
							acc->private_objects = val;
							acc->timestamp = currtime;
							break;
						case AC_ACC_PUB_O:
							acc->public_objects = val;
							acc->timestamp = currtime;
							break;
						case AC_ACC_PRIV_B:
							acc->private_bonus = val;
							acc->timestamp = currtime;
							break;
						case AC_ACC_PUB_B:
							acc->public_bonus = val;
							acc->timestamp = currtime;
							break;
						case AC_ACC_SIMCONN:
							acc->sim_connections = val;
							break;
						}
					}
				}
			}

			TH_release_read_lock(&(act_runtime[prefix.ip.space]->rwlock));
		}
	}
	return ret;
}

/*++++++++++++++++++++++++++++++++++++++
  AC_commit:

  commits the credit into all accounting trees, (XXX: only one at the moment)
  checks the limits and sets automatic ban if limit exceeded.

  ip_addr_t *addr  - user's address

  acc_st *acc_conn - credit used

  acl_st *acl_copy - pointer to store a copy of the acl

  returns error code from AC_commit_credit or AC_ban_set or OK.

  outline:
        lock runtime + minute accounting trees
	-----------------------  XXX runtime only for the moment
           find or create entries,
           increase accounting values by the values from passed acc
           check values against acl, see if permanent ban applies

           reset the connection acc
        unlock accounting trees

        if permanent ban - set it! :
            lock acl
            find/create IP in memory
            set ban
            find/create IP in SQL
            copy old values (if any), set ban, append comment
            unlock acl

 +++++++++++++++++++++++++++++++++++++++*/
int AC_commit(ip_addr_t *addr, acc_st *acc_conn, acl_st *acl_copy) {
  acc_st   account;
  int ret_err;
  ip_prefix_t prefix;

  prefix.ip = *addr;
  prefix.bits = IP_sizebits(addr->space);

  TH_acquire_write_lock( &(act_runtime[addr->space]->rwlock) );
  ret_err = AC_commit_credit_l(act_runtime[addr->space], &prefix, acc_conn, &account);
  TH_release_write_lock( &(act_runtime[addr->space]->rwlock) );

  memset(acc_conn,0, sizeof(acc_st));

  /* set permanent ban if deserved  and if not set yet */
  if( account.denials > acl_copy->maxdenials
      && acl_copy->deny == 0
      && NOERR(ret_err) ) {

    ret_err = AC_ban_set(&prefix, "Automatic", 1);
  }

  return ret_err;
} /* AC_commit */



/*++++++++++++++++++++++++++++++++++++++


unsigned AC_prune     deletes the entries listed in the prunelist
                    (this cannot be done from within the rx_walk_tree,
		    because the walk would be confused).
		    Returns number of nodes deleted.

GList *prunelist  list of pointers to nodes that should be deleted.
                  the prefixes actually are allocated in the node
		  structures, so they must not be dereferenced after
		  they are freed here.

  ++++++++++++++++++++++++++++++++++++++*/
unsigned AC_prune(GList *prunelist, int space)
{
  GList *pitem;
  char prstr[IP_PREFSTR_MAX];
  unsigned count = 0;
  acc_st accu; /* to accumulate the accounting of deleted nodes */
  ip_prefix_t globalpref;

  memset( &accu, 0, sizeof(accu));

  for( pitem = g_list_first(prunelist);
       pitem != NULL;
       pitem = g_list_next(pitem)) {

    rx_node_t   *nodeptr = (rx_node_t *) pitem->data;
    ip_prefix_t *prefptr = &(nodeptr->prefix);
    acc_st      *nodeacc = nodeptr->leaves_ptr->data;

    AC_acc_addup(&accu, nodeacc, ACC_PLUS); /* transfer the account */
    dieif( IP_pref_b2a( prefptr, prstr, IP_PREFSTR_MAX ) != IP_OK );
    LG_log(access_ctx, LG_DEBUG, "AC_prune: entry %s", prstr );
    /* delete the node. Assume there's one and only one dataleaf */
    rx_bin_node( RX_OPER_DEL, prefptr, act_runtime[space], (void *)nodeacc );
    count ++;
  }
  /* store the accumulated account at 0/0 */
  dieif( !NOERR (IP_pref_a2b( &globalpref, NULLIPRANG[space] )));
  AC_commit_credit_l(act_runtime[space], &globalpref, &accu, NULL);

  return count;
}


char AC_prunable(acc_st *leaf) {
  if(    leaf->private_bonus < 0.5
      && leaf->public_bonus  < 0.5
      && leaf->denials == 0
      && leaf->addrpasses == 0 ) {
    return 1;
  }
  return 0;
}

/*++++++++++++++++++++++++++++++++++++++
  AC_decay_hook:

  action performed on a single account node during decay (diminishing the
  bonus). Conforms to rx_walk_tree interface, therefore some of the
  arguments do not apply and are not used.

  rx_node_t *node  - pointer to the node of the radix tree

  int level        - not used

  int nodecounter  - not used

  void *con        - in real life: (double *) - points to the decay factor.

  returns always OK
+++++++++++++++++++++++++++++++++++++++*/
int AC_decay_hook(rx_node_t * node, int level, int nodecounter, void *con)
{
	acc_st *a = node->leaves_ptr->data;
	acc_st clone;
	ac_decay_data_t *dec_dat_p = (ac_decay_data_t *) con;
	clone = *a;

	AC_decay_leaf_l(&clone, &dec_dat_p->current_time);

	/* XXX pending: if bonus is close to zero and the node did not hit
	   its limit, and it's not an address-passing node
	   then add it to the list of nodes for deletion */

	if (AC_prunable(&clone)) {
		dec_dat_p->prunelist = g_list_prepend(dec_dat_p->prunelist, node);
	}

	/* process accounting - add all queries to the total counter */
	dec_dat_p->newtotal += a->queries;

	/* change oldest timestamp */
	if (UT_timediff(&clone.timestamp, &oldest_timestamp[node->prefix.ip.space]) > 0 && clone.addrpasses == 0 && clone.denials == 0) {
		oldest_timestamp[node->prefix.ip.space] = clone.timestamp;
	}

	return RX_OK;
}								/* AC_decay_hook() */

/*++++++++++++++++++++++++++++++++++++++
  AC_decay:

  Started as a thread from server.c.

  Every AC_DECAY_TIME goes through the accounting tree(s) and decays the
  bonus values.

  returns always OK

  MT-Note  This should be run as a detached thread.
  +++++++++++++++++++++++++++++++++++++++*/
int AC_decay(void)
{
	int ret_err = AC_OK;
	ac_decay_data_t dec_dat;
	ut_timer_t begintime, endtime;
	unsigned pruned = 0;
	float elapsed, rate, exactinterval;
	unsigned oldtotal = 0;
	unsigned increase;
	unsigned count = 0;
	int i;

	TA_add(0, "decay");

	UT_timeget(&endtime);

	/* XXX uses CO_get_do_server() to see when to exit the program.
	   this will change */
	while (CO_get_do_server()) {

		UT_timeget(&begintime);
		exactinterval = UT_timediff(&endtime, &begintime);	/* old endtime */

		/* those values can be changed in runtime - so recalculate
		   the decay factor before each pass */
		dieif(ca_get_ac_decay_halflife == 0);

		dec_dat.prunelist = NULL;
		dec_dat.newtotal = 0;
		dec_dat.current_time = begintime;
		for (i = MIN_IPSPACE_ID; i <= MAX_IPSPACE_ID; i++) {
			TH_acquire_write_lock(&(act_runtime[i]->rwlock));

			UT_timeget(&oldest_timestamp[i]);
			if (act_runtime[i]->top_ptr) {
				count += rx_walk_tree(act_runtime[i]->top_ptr, AC_decay_hook, RX_WALK_SKPGLU, 255, 0, 0, &dec_dat, &ret_err);
			}

			/* it should also be as smart as to delete nodes that have reached
			   zero, otherwise the whole of memory will be filled.
			   Next release :-)
			 */

			pruned += AC_prune(dec_dat.prunelist, i);
			g_list_free(dec_dat.prunelist);
			dec_dat.prunelist = NULL;

			TH_release_write_lock(&(act_runtime[i]->rwlock));
		}

		UT_timeget(&endtime);

		elapsed = UT_timediff(&begintime, &endtime);

		LG_log(access_ctx, LG_DEBUG,
			"AC_decay: Pruned %d of %d nodes. Took %5.3fs. Runs every %ds.", pruned, count, elapsed, ca_get_ac_decay_interval);

		/* number/rate of queries within the last <interval> */
		{
			char actbuf[32];

			increase = dec_dat.newtotal - oldtotal;
			rate = increase / exactinterval;

			sprintf(actbuf, "%.2f q/s in %.1fs", rate, exactinterval);
			TA_setactivity(actbuf);

			oldtotal = dec_dat.newtotal;
		}

		SV_sleep(ca_get_ac_decay_interval);
	}							/* while */

	TA_delete();

	return ret_err;
}								/* AC_decay() */



int AC_access_control_get_resultset(SQ_connection_t* sql_conn, int space, SQ_result_set_t **rs) {
  switch (space) {
  	case IP_V4:
  		return SQ_execute_query(sql_conn, "SELECT maxbonus, maxpublic, "
	                                         "maxdenials, deny, trustpass, "
	                                         "threshold, maxconn, "
	                                         "prefix, prefix_length "
                                 				"FROM   acl", rs);

    case IP_V6:
  		return SQ_execute_query(sql_conn, "SELECT maxbonus, maxpublic, "
	                                         "maxdenials, deny, trustpass, "
	                                         "threshold, maxconn, "
	                                         "prefix1, prefix2, prefix3, prefix4, "
	                                         "prefix_length "
                                 				"FROM   acl6", rs);

		default:
			die;
  }
  return -1;    /* should never reach this point, but gcc whines */
}

/*++++++++++++++++++++++++++++++++++++++
  AC_acc_load:

  loads the acl access tree from the acl table of the RIPADMIN database.
  (takes port/host/user/password from the config module).

  bails out if encounters problems with the database (logs to stderr).

  returns error code from RX_bin_node or wr_malloc.
  ++++++++++++++++++++++++++++++++++++++*/
int AC_acc_load(void)
{
	SQ_connection_t *con = NULL;
	SQ_result_set_t *rs;
	SQ_row_t *row;
	int ret_err = RX_OK, ret;
	int i, myint;

	con = AC_dbopen_admin();

	for (i = MIN_IPSPACE_ID; i <= MAX_IPSPACE_ID; i++) {
		if (AC_access_control_get_resultset(con, i, &rs) == -1) {
			fprintf(stderr, "ERROR %d: %s\n", SQ_errno(con), SQ_error(con));
			die;
		}

		TH_acquire_write_lock(&(act_acl[i]->rwlock));

		while ((row = SQ_row_next(rs)) != NULL && ret_err == RX_OK) {
			ip_prefix_t mypref;
			acl_st *newacl;

			newacl = UT_malloc(sizeof(acl_st));

			/* handle the prefix */
			switch (i) {
			case IP_V4:
				ret = IP_pref_f2b_v4(&mypref, SQ_get_column_string_nocopy(rs, row, 7), SQ_get_column_string_nocopy(rs, row, 8));
				dieif(ret != IP_OK);
				break;

			case IP_V6:
				ret = IP_pref_f2b_v6_32(&mypref,
					SQ_get_column_string_nocopy(rs, row, 7),
					SQ_get_column_string_nocopy(rs, row, 8),
					SQ_get_column_string_nocopy(rs, row, 9),
					SQ_get_column_string_nocopy(rs, row, 10), SQ_get_column_string_nocopy(rs, row, 11));
				dieif(ret != IP_OK);
				dieif(mypref.ip.space != IP_V6);	/* v4 or v4-mapped address in acl6 table!!! */
				break;
			}

			/* acl contents */
			if (sscanf(SQ_get_column_string_nocopy(rs, row, 0), "%u", &(newacl->maxprivate)) < 1)
				die;
			if (sscanf(SQ_get_column_string_nocopy(rs, row, 1), "%u", &(newacl->maxpublic)) < 1)
				die;
			if (sscanf(SQ_get_column_string_nocopy(rs, row, 2), "%hd", &(newacl->maxdenials)) < 1)
				die;

			/* these are chars therefore cannot read directly */
			if (sscanf(SQ_get_column_string_nocopy(rs, row, 3), "%u", &myint) < 1) {
				die;
			} else {
				newacl->deny = myint;
			}

			if (sscanf(SQ_get_column_string_nocopy(rs, row, 4), "%u", &myint) < 1) {
				die;
			} else {
				newacl->trustpass = myint;
			}

			if (sscanf(SQ_get_column_string_nocopy(rs, row, 5), "%d", &(newacl->threshold)) < 1)
				die;
			if (sscanf(SQ_get_column_string_nocopy(rs, row, 6), "%d", &(newacl->maxconn)) < 1)
				die;

			/* now add to the tree */
			IP_truncate_pref_v6_inplace(&mypref, IPV6_ADDRESS_ACL_BITS);
			ret_err = rx_bin_node(RX_OPER_CRE, &mypref, act_acl[i], (rx_dataleaf_t *) newacl);
		}

		TH_release_write_lock(&(act_acl[i]->rwlock));
	    SQ_free_result(rs);
	}

	/* Close connection */
	SQ_close_connection(con);

	return ret_err;
}								/* AC_acc_load */



/*++++++++++++++++++++++++++++++++++++++
  AC_build:

  creates empty trees for accounting/acl.

  returns error code from RX_tree_cre or OK.
  (XXX): just now only bails out when encounters problems.
  ++++++++++++++++++++++++++++++++++++++*/
int AC_build(void)
{
  /* create trees */
  if (RX_tree_cre(NULLIPRANG[IP_V4], RX_FAM_IP, RX_MEM_RAMONLY,
			RX_SUB_NONE, &act_runtime[IP_V4]) != RX_OK
	 || RX_tree_cre(NULLIPRANG[IP_V6], RX_FAM_IP, RX_MEM_RAMONLY,
			RX_SUB_NONE, &act_runtime[IP_V6]) != RX_OK
	 || RX_tree_cre(NULLIPRANG[IP_V6], RX_FAM_IP, RX_MEM_RAMONLY,
			RX_SUB_NONE, &act_acl[IP_V6]) != RX_OK
	 || RX_tree_cre(NULLIPRANG[IP_V4], RX_FAM_IP, RX_MEM_RAMONLY,
			RX_SUB_NONE, &act_acl[IP_V4]) != RX_OK
         )
    die; /*can be changed to an error and handled ... some day */

  return RX_OK;
}

/*++++++++++++++++++++++++++++++++++++++
  ac_rxwalkhook_print:

  action performed on a single account node
  when listing the contents of the access tree: format and print the
  data from this node.

  Conforms to rx_walk_tree interface, therefore some of the
  arguments do not apply and are not used.

  rx_node_t *node  - pointer to the node of the radix tree

  int level        - not used

  int nodecounter  - not used

  void *con        - pointer to the target string (prints to it)

  returns always OK
+++++++++++++++++++++++++++++++++++++++*/
static
int ac_rxwalkhook_print(rx_node_t *node,
                             int level, int nodecounter,
                             void *outvoid)
{
  char adstr[IP_PREFSTR_MAX];
  char *dat;
  GString *output = outvoid;

  dieif( IP_pref_b2a(&(node->prefix), adstr, IP_ADDRSTR_MAX) != IP_OK );
  /* program error. */

  dat = ac_to_string( node->leaves_ptr );
  g_string_sprintfa(output, "%-25s %s\n", adstr, dat );
  UT_free(dat);

  return RX_OK;
} /* ac_rxwalkhook_print */


/*++++++++++++++++++++++++++++++++++++++
  This function displays the access table to the given connection.

  unsigned AC_print_access    Returns the number of nodes traversed

  GString *output             target string
  ++++++++++++++++++++++++++++++++++++++*/
unsigned AC_print_access(GString *output)
{
  int cnt = 0;
  int err;
	int i;

	for (i = MIN_IPSPACE_ID; i <= MAX_IPSPACE_ID; i++) {
	  if( act_runtime[i]->top_ptr != NULL ) {
	    char *header = ac_to_string_header();

	    /* print header */
	    g_string_append(output, header);
	    UT_free(header);

	    cnt = rx_walk_tree(act_runtime[i]->top_ptr, ac_rxwalkhook_print, RX_WALK_SKPGLU, 255, 0, 0, output, &err);
	  }
	}

  return cnt;
} /* show_access() */



/*++++++++++++++++++++++++++++++++++++++
  ac_rxwalkhook_print_acl:

  action performed on a single account node
  when listing the contents of the acl tree: format and print the
  data from this node.

  Conforms to rx_walk_tree interface, therefore some of the
  arguments do not apply and are not used.

  rx_node_t *node  - pointer to the node of the radix tree

  int level        - not used

  int nodecounter  - not used

  void *con        - pointer to the target string (prints to it)

  returns always OK
  +++++++++++++++++++++++++++++++++++++++*/
static
int ac_rxwalkhook_print_acl(rx_node_t *node,
                             int level, int nodecounter,
                             void *outvoid)
{
  char prefstr[IP_PREFSTR_MAX];
  char *dat;
  GString *output = outvoid;

  dieif( IP_pref_b2a(&(node->prefix), prefstr, IP_PREFSTR_MAX) != IP_OK );

  dat = ac_acl_to_string( node->leaves_ptr );
  g_string_sprintfa(output, "%-25s %s\n", prefstr, dat );
  UT_free(dat);

  return RX_OK;
}/* ac_rxwalkhook_print_acl */


/*++++++++++++++++++++++++++++++++++++++
  This function writes the acl (access control) table to the given
  Gstring (auto-expandable)

  unsigned AC_print_acl     Returns the number of nodes traversed

  GString *output           target string
  ++++++++++++++++++++++++++++++++++++++*/
unsigned AC_print_acl(GString *output)
{
  /* Administrator wishes to show access control list. */
  int cnt = 0;
  int err;
  int i;

	for (i = MIN_IPSPACE_ID; i <= MAX_IPSPACE_ID; i++) {
	  if (act_acl[i]->top_ptr != NULL) {
	    char *header = ac_acl_to_string_header();

	    /* print header */
	    g_string_append(output, header);
	    UT_free(header);

	    cnt = rx_walk_tree(act_acl[i]->top_ptr, ac_rxwalkhook_print_acl, RX_WALK_SKPGLU, 255, 0, 0, output, &err);
	  }
	}

  return cnt;
}


/*++++++++++++++++++++++++++++++++++++++
  AC_count_object:

  accounts an objects in the credit accordingly to its type,
  or sets denial if the limit is defined and the credit is exceeded.

  acc_st    *acc_credit     pointer to the credit structure (gets modified)

  acl_st    *acl            acl, contains the limits for private/public objects

  int private               indicates if the object type is private
  ++++++++++++++++++++++++++++++++++++++*/
void AC_count_object(acc_st *acc_credit, acl_st *acl, int private) {
    if (!acc_credit || !acl) return;

    if (private) {
        if (acc_credit->private_objects <= 0 && acl->maxprivate != -1) {
            /* must be negative - will be subtracted */
            acc_credit->denials = -1;
        } else {
            acc_credit->private_objects--;
        }
    } else {
        if (acc_credit->public_objects <= 0 && acl->maxpublic != -1) {
            acc_credit->denials = -1;
        } else {
            acc_credit->public_objects--;
        }
    }
}


/*++++++++++++++++++++++++++++++++++++++

  checks the denied flag in credit (-1 or 1 means denied)

  int
  AC_credit_isdenied     returns 1 if denied, 0 otherwise

  acc_st    *acc_credit    pointer to the credit structure
  ++++++++++++++++++++++++++++++++++++++*/
int AC_credit_isdenied(acc_st *acc_credit) {
    return (acc_credit && acc_credit->denials != 0);
}


/*++++++++++++++++++++++++++++++++++++++

  returns the higher number of the two acl limits: maxprivate & maxpublic
  corrected w.r.t the current credit left,
  or unlimited if any of them is 'unlimited'.

  int AC_get_higher_limit       returns the higher limit

  acc_st    *acc_credit         current credit left

  acl_st    *acl                acl for that user
++++++++++++++++++++++++++++++++++++++*/
int AC_get_higher_limit(acc_st *acc_credit, acl_st *acl) {
    if (!acl || !acc_credit || acl->maxprivate == -1 || acl->maxpublic == -1) {
        return -1;
    } else {
        int a = acc_credit->private_objects;
        int b = acc_credit->public_objects;

        return (a > b ? a : b);
    }
}

/* AC_commit_denials */
/*+++++++++++++++++++++++++++++++++++++
a wrapper to AC_commit, to increase ONLY the denial count.

	ip_addr_t *addr		IP address of the host

    acl_st *acl_copy	pointer to acl structure for this IP

+++++++++++++++++++++++++++++++++++++*/
void
AC_commit_denials(ip_addr_t *addr, acl_st *acl_copy)
{

  acc_st tmp_acc;
  memset (&tmp_acc, 0, sizeof(acc_st));
  tmp_acc.denials = 1;
  AC_commit(addr, &tmp_acc, acl_copy);

}
