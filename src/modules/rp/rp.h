/***************************************
  $Revision: 1.1 $

  Radix tree (rx).  rx_global.h - header file with definition of 
                                  global vars for radix

  Status: NOT REVUED, TESTED, INCOMPLETE

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

#ifndef RP_H
#define RP_H

#include <stddef.h>  /* definition of NULL */
#include <glib.h>
#include <pthread.h>
/*#include <DF_attribute_enum.def> / * attribute codes */

#include <defs.h>

#include <iproutines.h>
/*#include <erroutines.h>*/
#include <rxroutines.h>
#include <mysql_driver.h>


#include "ca_defs.h"
#include <thread.h>
#include "lg.h"

/* error codes */
#define RP_OK      0
#define RP_NOTREE  1
#define RP_NOYETI  2
#define RP_BADKEY  3

/* generic data holder for radix trees */
typedef struct {
    rx_fam_t fam;       /* family: prefix, range, address */
    ip_space_t space;   /* v4 or v6 */
    union {
        ip_prefix_t rt;
        ip_range_t in;
    } u;                /* data */
} rp_uni_t;


typedef ca_dbSource_t *rp_regid_t;
typedef A_Type_t rp_attr_t;


typedef struct {
    rp_attr_t type;     /* attribute type */
    rp_uni_t uni;       /* data */
    sql_key_t key;      /* object_id */
    union {
        char *origin;
        char *domain;
    } d;                /* ascii blob representation of data */
} rp_upd_pack_t;

typedef struct {
    rp_attr_t attr;         /* attribute */
    ip_space_t space;       /* space (v4/v6) */
    rp_regid_t reg_id;      /* a.k.a. source */
    int result;             /* result of thread execution */
    pthread_t thread_id;    /* pthread id */
} RP_thread_info;

#ifdef RP_IMPL
#define EXTDEF(a,b) a = b;
/* forest read write lock */
rw_lock_t rx_forest_rwlock = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, 0, 0 };
#else
#define EXTDEF(a,b) extern a;
extern rw_lock_t rx_forest_rwlock; 
#endif

EXTDEF (GList *rx_forest, NULL)		/* linked list of trees */
     
typedef struct {
  rp_regid_t      reg_id;	/*+ registry id +*/
  rp_attr_t       attr;         /*+ extra tree attribute (within the same reg/spc/fam), allows 
				  using the same tree type for different object trees +*/ 

  rx_tree_t       *tree;        /*+ tree +*/
} rp_tentry_t;

int RP_asc_load(char *filename, int maxobj, 
		     int operation, rp_regid_t reg_id);
int 
RP_tree_get ( rx_tree_t **treeptr, /*+ answer goes here, please +*/
	      rp_regid_t reg_id,   /*+ id of the registry +*/
	      ip_space_t spc_id,   /*+ type of space (ipv4/ipv6) +*/
	      rp_attr_t  attr    /*+ extra tree id (within the same reg/spc/fam +*/
	      );
int 
RP_tree_add (
	     rp_regid_t reg_id,    /*+ id of the registry +*/
	     rp_attr_t  attr,      /*+ extra tree id (within the same registry/space/family +*/
	     char      *prefixstr, /*+ prefix the tree will cover (string) +*/
	     rx_mem_mt   mem_mode, /* memory only, memory+sql, sql only +*/
	     rx_subtree_mt subtrees	/*+ one of NONE, AUTO, HAND +*/
	     );

int 
RP_tree_del (rx_tree_t *treeptr); /* pointer to a tree structrute */

int 
RP_init_trees( rp_regid_t reg_id );


int
RP_asc_search ( 
               rx_srch_mt search_mode, 
               int par_a,
               int par_b,
               char *key,          /*+ search term: (string) prefix/range/IP +*/
               rp_regid_t   reg_id,
	       rp_attr_t  attr,    /*+ extra tree id (within the same reg/spc/fam +*/
               GList **anslist,    /*+ answers go here, please +*/
               int    max_count    /*+ max # of answers. RX_ALLANS == unlimited +*/
               );
int
RP_uni_node ( rx_oper_mt mode,       /*+ MODE={cre|mod|del} +*/
	      rp_uni_t   *uni,       /*+ unified argument structure +*/
	      rp_attr_t  attr,       /*+ additional tree attribute +*/
	      rp_regid_t reg_id,     /*+ id of the registry +*/
	      void *data,            /*+ pointer to the payload +*/
	      unsigned datalen,      /*+ length of the payload +*/
	      sql_key_t key          /*+ the key to the SQL full-text +*/
	      );
int
RP_asc_node( rx_oper_mt mode,       /*+ MODE={cre|mod|del} +*/
	       char *rangstr,         /*+ string prefix/range/IP/inaddr +*/
	       rp_attr_t  attr,       /*+ additional tree attribute +*/
	       rp_regid_t reg_id,     /*+ id of the registry +*/
	       void *data,            /*+ pointer to the payload +*/
	       unsigned  datalen,     /*+ length of the payload +*/
	       sql_key_t key          /*+ the key to the SQL full-text +*/
              );

rx_fam_t RP_attr2fam( rp_attr_t type );
int RP_attr2spc(rp_attr_t type, ip_space_t space);
int
RP_asc2uni(const char *astr,       /*+ string prefix/range/IP/inaddr +*/
	   rp_attr_t  attr,
	   rp_uni_t   *uni);        /* destination pointer */
int
RP_asc2pack(rp_upd_pack_t *pack, rp_attr_t type, const char *string);


int RP_pack_set_orig( rp_attr_t  attr,rp_upd_pack_t *pack, const char *origin);
int RP_pack_set_pref4(rp_attr_t  attr,const char *avalue, rp_upd_pack_t *pack,
		       unsigned *prefix, unsigned *prefix_length);
int RP_pack_set_revd( rp_attr_t  attr,const char *avalue, rp_upd_pack_t *pack);
int RP_pack_set_pref6(rp_attr_t  attr,const char *avalue, rp_upd_pack_t *pack,
		       ip_v6word_t *high, ip_v6word_t *low, unsigned *prefix_length);
int RP_pack_set_rang( rp_attr_t  attr,const char *avalue, rp_upd_pack_t *pack,
		      unsigned *begin_in, unsigned *end_in);

int
RP_pack_update(rx_oper_mt   mode, 
	       rp_upd_pack_t *pack,
	       rp_regid_t    reg_id);     /*+ id of the registry +*/


int
RP_pack_node(rx_oper_mt   mode, 
	     rp_upd_pack_t *pack, 
	     rp_regid_t    reg_id);
int
RP_pack_node_l(rx_oper_mt    mode, 
	       rp_upd_pack_t *pack, 
	       rx_tree_t     *mytree);

void rp_make_short(rp_upd_pack_t *pack, char **ptr, unsigned *len);

int RP_sql_load_reg(rp_regid_t reg_id);
int RP_asc_load(char *filename, int maxobj, int operation, 
	    rp_regid_t reg_id);
int RP_attr2spc(rp_attr_t type, ip_space_t space);

void RP_init (LG_context_t *ctx);

int RP_sql_load_start();
int RP_sql_load_wait_until_finished();

#undef EXTDEF
#endif /* RP_H */
