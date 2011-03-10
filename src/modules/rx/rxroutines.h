/***************************************
  $Revision: 1.31 $

  Radix tree (rx). rxroutines.h - header file for radix tree handling module.

  Status: NOT REVUED, TESTED

  Design and implementation by: marek

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

#ifndef _RX_H
#define _RX_H

#include <glib.h>
#ifndef G_THREADS_ENABLED
#error "GLib must be compiled with thread support enabled to be MT-Safe"
#endif

#include <pthread.h>
#include <stubs.h>

#include <memwrap.h>
#include <stdio.h>
#include <iproutines.h>
/*#include <erroutines.h>*/

#include "sk.h" /*condat*/

#include "thread.h" /*rwlock*/
#include "lg.h"

/* errors */
#define RX_OK      0
#define RX_DATNOF  1

typedef enum {
  RX_FAM_RT = 1,    /* prefix (as in route object) */
  RX_FAM_IN = 2,    /* range (as in inetnum object) */
  RX_FAM_IP = 4     /* address (as in access counters) */
} rx_fam_t;

      
/*+ the node operation modes +*/
typedef enum {
  RX_OPER_CRE = 1,
  RX_OPER_DEL
} rx_oper_mt;


/*+ stack building modes +*/
typedef enum {
  RX_STK_CREAT = 1, /*+ - creation = all glue nodes, stop at first non-glue +*/
  RX_STK_QUERY_ALLNOD,   /*+ - query = all glue nodes, stop when deep enough */
  RX_STK_QUERY_NOGLUE    /*+ - query = no glue nodes, stop when deep enough */
} rx_stk_mt;

/*+ the search modes +*/
typedef enum {
  RX_SRCH_CREAT = 1,      /*+ special search - does not skip glue nodes +*/
  RX_SRCH_EXLESS,         /*+ the default search +*/
  RX_SRCH_EXACT,
  RX_SRCH_LESS,
  RX_SRCH_MORE,           /*+ more specific search +*/
  RX_SRCH_DBLS,           /*+ special more spec: return only nodes with more than one data leaves +*/
  RX_SRCH_RANG            /*+ more specific range search, RPSL style : ^n-m     ---=== UNUSED as of 2011-03-10 - agoston +*/
} rx_srch_mt;


#ifdef RX_IMPL_PRINT
char *rx_srch_mode_text[]={
  "", /* begins with 1 */
  "CREAT",
  "EXLESS",       
  "EXACT",	
  "LESS",		
  "MORE",         
  "DBLS",         
  "RANG"      
};
#endif

/* constant to mean 'unlimited number of answers from a search' */
#define RX_ANS_ALL (-1)

#define RX_ALL_DEPTHS 255


/*+ radix tree's memory modes -- not yet implemented +*/
typedef enum {
  RX_MEM_RAMONLY,
  RX_MEM_RAMSQL,
  RX_MEM_SQLONLY
} rx_mem_mt;



/*+ subtree modes -- not yet implemented +*/
typedef enum {
  RX_SUB_NONE,
  RX_SUB_AUTO,
  RX_SUB_HAND
} rx_subtree_mt;

/* modes for tree traversal (walk_tree) */
typedef enum {
  RX_WALK_CNTGLU=1,  /*+ default: count also glue nodes and make the level
			   checking aware of them +*/

  RX_WALK_SKPGLU=2,  /*+ only real nodes counted & watched in level checks +*/

  RX_WALK_PRFLEN=4,  /*+ make level check a check for prefix length;
		       still only non-glue nodes are counted +*/
  RX_WALK_REVERS=8   /*+ reverse the order of traversing the tree 
		       (first link 1 then 0) +*/
} rx_walk_mt;


/*+  A struct for data hooked via a double linked list at a radix node. 
Must uniquely define the object for lookups in the SQL tables and/or memory. 
Must also contain enough info to let the delete_node choose (and remove) 
the proper object from the (linked) list +*/

typedef struct { 
  ip_range_t      iprange;      /*+ filled for all trees. Used in rp_search
				   for determining exact matches (all trees)
				   and to see if an address is in range
				   (only IPv4 inetnum trees) +*/
  unsigned char   preflen;      /* to avoid arithmetics on IPv6 ranges,
				   we have to store prefix length to
				   determine the shortest object in search.
				   This is filled in for all route trees
				*/

  char            composed;	/*+ non-zero for composed inetnums 
				  equal to: the number of prefixes composing
				  the range - minus 1 +*/

  void           *data_ptr;	/*+ to in-memory immediate data +*/
  unsigned        data_len;     /*+ and its length +*/

  sql_key_t       data_key;	/*+ key to the SQL full-text data record +*/
  sql_key_t       leaf_key;     /*+ pointer to the SQL data leaf record  +*/
} rx_dataleaf_t;

/*+ 
The struct for radix nodes.

Must contain prefix, parent, left/right child links in memory and sql, 
link to the sql version of the node. 
And of course data: pointer to a double linked list of rx_data_t's. 
+*/

typedef struct _rx_node_str {
  ip_prefix_t     prefix;	/*+ who am i. +*/

  char            glue;         
                                /*+ now this is an indicator for a node that it
				   is not holding a real prefix, 
				   but is only a glue node +*/
  
  GList          *leaves_ptr;	/*+ a double-linked list of rx_data_t structs 
                                   the data leaves can be multiple at each node
				   (due to a user error the inetnum ranges can
				   overlap, due to multihoming or error routes 
				   can be duplicated ). 
				   So we link a dynamic thing here +*/

  /*  sql_key_t       leaves_key;   */
                                /* "linked list" not needed in sql - 
				   the data leaves can be found in node_leaf 
				   table as those that have 
				   node_key in node_id  */

  struct _rx_node_str
                 *parent_ptr,	/*+ radix links in memory +*/
                 *child_ptr[2];	/*+ NULL means empty +*/

  sql_key_t       parent_key,	/*+ radix links in SQL +*/
                  child_key[2];	/*+ zero means empty +*/

  sql_key_t       node_key;	/*+ key of the corresponding SQL radix node +*/

} rx_node_t;


/*+
 rx_tree_t - defines a radix tree.

 includes a pointer(key) to the top node, 
 
 names of the corresponding SQL tables 
    (they can be generated automatically, 
    but this is the place to store the result)
    Data_table  is for data_key.
    Radix_table is for parent_id, right_id, left_id, node_id.
    Leaves_table is for leaves_key (double linked list in SQL).
 +*/

typedef struct _rx_tree_str {  
  ip_space_t      space;	/*+ one of IPv4, IPv6 +*/
  rx_fam_t        family;       /*+ one of RT, IN +*/

  rx_subtree_mt   subtrees;	/*+ one of NONE, AUTO, HAND +*/
  rx_mem_mt       mem_mode;     /*+ where the tree will sit - SQL or RAM +*/
  struct rx_tree_str 
                 *parent_tree;  /*+ pointer to the parent tree +*/

  ip_prefix_t     prefix;	/*+ of the IP space this tree covers +*/

  int             maxbits;	/*+ max depth of this tree 
				   (depends on the space, so it is redundant)+*/
  sql_tblnam_t    data_table;
  sql_tblnam_t    radix_table; 
  sql_tblnam_t    leaves_table;

  int             num_nodes;	/*+ number of nodes in tree - for assertions +*/

  rx_node_t      *top_ptr;	/*+ pointer to the top node +*/
  long            top_key;	/*+ the same in SQL +*/

  rw_lock_t      rwlock;	/*+ per-tree reader/writer lock +*/

} rx_tree_t;


/*+ this is a definition of a node copy used for:

 * stack elements returned from rx_stack_build,

 * answer elements from an rx_nod_search.
   
   It *must* hold pointers to the original locations of it (in terms of 
   memory and SQL) so that one could actually modify the node...
   In SQL tree mode it holds also a copy of a node.
   This seems to be unnecessary for in-memory radix trees but is a must
   for sql ones.

   WARNING!!!!! The fact that pointers to tree and memory / SQL nodes are
   here is a subject to race condition. The location of the tree in the 
   forest list and the node in memory must not change.

+*/

typedef struct {
  rx_tree_t   *tree;  /*+ contains registry_id, space_id, sql table names +*/
  rx_node_t *srcptr;
  sql_key_t srckey;
  rx_node_t cpy;      /*+ filled in if the tree is kept in SQL only mode +*/
} rx_nodcpy_t;


/*+ 
  This represents one data leaf (by reference). It's used for returning data 
from rx_bin_search() to rx_asc_search().
+*/
typedef struct {
  sql_key_t      srckey;
  rx_dataleaf_t *leafptr;
} rx_datref_t;


/*+ this is a structure used for returning the data from the search.
   It contains a copy of the dataleaf and a pointer to the source +*/
typedef struct {
  sql_key_t      srckey;
  rx_dataleaf_t  leafcpy;
} rx_datcpy_t;

typedef struct {
  rx_node_t *node;
  int        code;
  int     datatoo;
  rx_tree_t *tree; 
} rx_treecheck_t;

/* 
 * -----------------------------------------------------------------------
 *  
 *       now, THIS is a scrap heap for things that MAY BE useful 
 */

/* a definite pointer to an sql object: table name + key
However, it might be nice to include the table TYPE so that one knows 
what data it holds :-) 
 */
typedef struct {
  sql_tblnam_t  name;
  sql_key_t     key;
} rx_sqlobj_t;


typedef struct {
  GList        **nodlist;
  rx_tree_t     *tree; 
  ip_prefix_t   *prefix;
} hook_addnode_userdat_t;

/********************* P R O T O T Y P E S **********************/


void rx_free_list_element(void *cpy, void *trash);



int
RX_treecheck( rx_tree_t *tree, int datatoo, rx_treecheck_t *errorfound);

int 
RX_tree_cre (
	      char      *prefixstr, /*+ prefix the tree will cover (string) +*/
	      rx_fam_t   fam_id,
	      rx_mem_mt   mem_mode, /* memory only, memory+sql, sql only +*/
	      rx_subtree_mt subtrees,	/*+ one of NONE, AUTO, HAND +*/
	      rx_tree_t **treestore /* store the tree pointer here */
	     );

int
RX_bin_search (
	       rx_srch_mt  search_mode,
	       int     	   par_a,
	       int         par_b,
	       rx_tree_t  *tree,           /* tree ptr */
	       ip_prefix_t *prefix,        /* binary prefix */
	       GList       **datleaves,    /* data leaves go here */
	       int         max_count 
	       );
int
rx_bin_node (
	     rx_oper_mt   mode,       /*+ MODE={cre|mod|del} +*/
	     ip_prefix_t *newpref,    /*+ prefix of the node +*/
	     rx_tree_t	*tree,        /*+ pointer to the tree structure +*/
	     rx_dataleaf_t *dataleaf  /*+ dataleaf to attach at the node +*/
	     );
int
RX_rt_node (
	     rx_oper_mt   mode,       /*+ MODE={cre|mod|del} +*/
	     ip_prefix_t *newpref,    /*+ prefix of the node +*/
	     rx_tree_t	*tree,        /*+ pointer to the tree structure +*/
	     rx_dataleaf_t *dataleaf  /*+ dataleaf to attach at the node +*/
	     );
int
RX_in_node(   rx_oper_mt mode,       /*+ MODE={cre|mod|del} +*/
	      ip_range_t *rang,      /*+ range of IP addresses +*/
	      rx_tree_t *tree,       /*+ pointer to the tree structure +*/
	      rx_dataleaf_t *leafptr /*+ dataleaf to attach at the node +*/
	      );
int
rx_build_stack(rx_nodcpy_t    stack[], 
	       int            *maxdepth, 
	       rx_tree_t      *tree, 
	       ip_prefix_t    *newpref,
	       rx_stk_mt      dmode
	       );

int
rx_nod_search (
	       rx_srch_mt  search_mode,
	       int     	   par_a,
	       int         par_b,
	       /* see rx_asc_search() for explanation */
	       rx_tree_t  *tree,           /* tree ptr */
	       ip_prefix_t  *prefix,        /* binary prefix */
	       
	       rx_nodcpy_t stack[],         /* stack==array of node_copies */
	       int         stackcount,      /* number of element on the stack*/
	                                    /* can be set in creat stack */

	       GList	   **nodlist,	    /* answers go here */
	       int         max_count        /* max # of answers */
	       );
int
rx_walk_tree(rx_node_t *node, 
	     int (*func)(rx_node_t *node, int level, int nodecounter,
			      void *userptr), 
	     rx_walk_mt walk_mode, 
	     int maxlevel, 
	     int level, 
	     int nodecounter,
	     void *userptr,
	     int *err);


int rx_tree_print( sk_conn_st *condat,rx_tree_t     *tree );
void rx_space_list(sk_conn_st *condat);
void rx_nod_print( rx_node_t *node, char *buf, unsigned maxchar );
void rx_stk_print( rx_nodcpy_t   stack[], int stackdepth );
const char *RX_text_srch_mode(rx_srch_mt mode);


/* Function needed to delete radix tree */
void 
rx_delete_treenode(rx_tree_t *tree, rx_node_t *curnode);

int
rx_delete_tree(rx_tree_t *tree, rx_node_t *node,
             int maxlevel, 
	     int level, 
	     int nodecounter,
	     void *userptr);
void
RX_init (LG_context_t *ctx);

void
RX_init (LG_context_t *ctx);

#undef EXTDEF
#endif /* _RX_H */
