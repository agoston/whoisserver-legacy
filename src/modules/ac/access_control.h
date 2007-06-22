#ifndef READ_ACCESS_CONTROL
#define READ_ACCESS_CONTROL

/***************************************
  $Revision: 1.1.6.1 $

  Access Control module (ac) - the header file.

  Status: NOT REVUED, NOT TESTED

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

#include "timediff.h"
#include "iproutines.h"
#include "rxroutines.h"
#include "mysql_driver.h"
#include "lg.h"

#ifdef AC_IMPL
#define EXTDEF
#else
#define EXTDEF extern
#endif

/* Access control structure */
typedef struct {
                        /* max bonus values before temporary denial,
			 * -1 == unlimited: */
  int      maxprivate;  /* --  private objects */
  int      maxpublic;   /* --  public objects */
  short    maxdenials;  /* before the permanent ban is set */
  char     deny;        /* THE ban itself */
  char     trustpass;   /* has power to pass ip addresses */
  int	   threshold;	/* the threshold value of the number of simultaneous connections */
  int	   maxconn;	/* maximum number of simultaneous connections */
} acl_st;


#ifdef AC_IMPL
/* order must correspond to the array below */
typedef enum {
  AC_AR_MAXPRIVATE = 0,
    AC_AR_MAXPUBLIC,
    AC_AR_MAXDENIALS,
    AC_AR_DENY,
    AC_AR_TRUSTPASS,
	AC_AR_THRESHOLD,
	AC_AR_MAXCONN,
    AC_AR_SIZE
} AC_ar_elements;

/* this array is used for setting the values from the command line
   of the admin interface (with getsubopt)
*/
char* AC_ar_acl[]  = {
  "maxprivate",
  "maxpublic",
  "maxdenials",
  "deny",
  "trustpass",
  "threshold",
  "maxconn",
  NULL };

/* order must correspond to the array below */
typedef enum {
 	AC_ACC_CONN = 0,
    AC_ACC_PASS,
    AC_ACC_DENY,
    AC_ACC_REFS,
    AC_ACC_PRIV_O,
	AC_ACC_PUB_O,
    AC_ACC_PRIV_B,
	AC_ACC_PUB_B,
	AC_ACC_SIMCONN,
    AC_ACC_SIZE
} AC_set_access_opts_enum;

char* AC_set_access_opts_str[]  = {
  "conn",
  "pass",
  "deny",
  "refs",
  "priv_o",
  "pub_o",
  "priv_b",
  "pub_b",
  "simconn",
  NULL };
#endif

typedef enum {
  AC_ACC_NOT_CHANGED = 0,
  AC_ACC_CHANGED,
  AC_ACC_NEW
} AC_acc_status;

/* errors */
#define AC_OK      0
#define AC_SAVING  1
#define AC_INVARG  2

/* Accounting == counters */
typedef struct {
  int connections;
  int addrpasses;
  int denials;
  int queries;
  int referrals;
  int public_objects;
  int private_objects;
  float public_bonus;       /* those two are .. */
  float private_bonus;      /* .. maintained only in the runtime tree */
  ut_timer_t timestamp;     /* in-memory is ut_timer_t */
  int sim_connections;
  AC_acc_status changed;
} acc_st;


#define ACC_PLUS 0
#define ACC_MINUS 1

#ifdef __cplusplus
extern "C" {
#endif


/* prototypes */
int AC_build(void);
int AC_fetch_acc( ip_addr_t *, acc_st * );
int AC_check_acl( ip_addr_t *, acc_st *, acl_st *);
void AC_acc_addup(acc_st *, acc_st *, int);
int AC_commit(ip_addr_t *, acc_st *,acl_st * );
int AC_acc_load(void);
int AC_decay(void);

int AC_persistence_init(void);
int AC_persistence_daemon(void);
int AC_persistence_save(void);

/* interface to modifications on the fly */
/* er_ret_t AC_asc_ban_set(char *addrstr, char *text, int denyflag); */
void AC_commit_denials( ip_addr_t *, acl_st * );


/* printing */
char *AC_credit_to_string(acc_st *a);
unsigned AC_print_acl(GString *output);
unsigned AC_print_access(GString *output);

int AC_credit_isdenied(acc_st    *acc_credit);
void AC_count_object( acc_st    *acc_credit, acl_st    *acl, int private );
int AC_get_higher_limit(acc_st    *acc_credit, acl_st    *acl);

int AC_asc_acl_command_set( char *command, char *comment );
int AC_asc_set_nodeny(char *ip);
int AC_set_access_command(char *command);
SQ_connection_t *AC_dbopen_admin(void);

#ifdef __cplusplus
}
#endif


/* declare global accounting trees */
EXTDEF rx_tree_t  *act_runtime[MAX_IPSPACE_ID+1];

/* Unused variables
EXTDEF rx_tree_t  *act_hour;
EXTDEF rx_tree_t  *act_minute;*/

EXTDEF int ac_auto_save;


/* declare global access control list tree */
EXTDEF rx_tree_t  *act_acl[MAX_IPSPACE_ID+1];

#undef EXTDEF

/* initialisation */
void AC_init(LG_context_t *ctx);

#endif /* READ_ACCESS_CONTROL */
