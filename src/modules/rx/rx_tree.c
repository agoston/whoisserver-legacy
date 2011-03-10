/***************************************
  $Revision: 1.1 $

  Radix tree (rx).  rx_tree.c - functions to operate on trees
  (creation/deletion/finding).

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

#define RX_IMPL
#include "rip.h"

/* logging */
LG_context_t *rx_context;


void
RX_init (LG_context_t *ctx)
{
  rx_context = ctx;
}


/*+++++++++  
  go down the tree calling func on every node.
  (func takes the node pointer and the current level)

  the function is called recursively with level increased
  it stops recursing when no child nodes are found or maxlevel is reached.
  
  therefore the initial call must set level to 0.
  
  the nodecounter increments at every node, and is the return value
  of the function. So start with 0 to get the number of nodes traversed.
  
  ERROR HANDLING IS DIFFERENT HERE!
  Unlike other functions it is not the return value:
  The error code from the func function IF DEFINED (== not NULL ) goes 
  to the variable pointed to by the last parameter.

  XXX: nodecounter is ALWAYS passed as 0, so should probably be removed from
       calling parameter list, shane 2001-02-01

  Arguments:
         rx_walk_mt walk_mode          controls if glue nodes are counted
                                       and if levels or prefix lenghts are checked
++++++++++++*/
int rx_walk_tree(rx_node_t *node, int(*func)(rx_node_t *node, int level, int nodecounter, void *userptr), rx_walk_mt walk_mode, int maxlevel, int level, int nodecounter, void *userptr, int *err) {
    int i, link;

    if (node == NULL)
        die; /* program error. we expect a valid, checked, node.*/

    /* check the limits and maybe quit here: prefix length for RX_WALK_PRFLEN, */
    /* level otherwise */

    if (walk_mode & RX_WALK_PRFLEN) {
        if (node->prefix.bits > maxlevel) {
            return nodecounter;
        }
    } else if (level > maxlevel) {
        return nodecounter;
    }

#ifdef DEBUG_RADIX
    if (userptr != (void*)-1) {     /* to allow avoiding printing this debug from rx_tree_print*() */
        char DEBUGbuf[256];
        rx_nod_print(node, DEBUGbuf, 256);
        fprintf(stderr, "rx_walk_tree: walk_mode=%d, nodecounter=%d, level=%d, maxlevel=%d, node=[%s]\n", walk_mode, nodecounter, level, maxlevel, DEBUGbuf);
    }
#endif

    /* process the node appropriately: */
    /* if (not node glue) or (process glue nodes) */

    if (node->glue == 0 || (walk_mode & RX_WALK_SKPGLU) == 0) {

        /* increase our depth counter */
        level++;

        /* increase the count of visited nodes */
        nodecounter++;

        /* call supplied function, if any */
        if (func != NULL) {
            *err = func(node, level, nodecounter, userptr);

            /* abort the walk on error*/
            if (*err != RX_OK) {
                LG_log(rx_context, LG_DEBUG, "walk_tree: func returned error %d, aborting", *err);
                return nodecounter;
            }
        }
    }

    /* process left and right children */
    for (i = 0; i <= 1; i++) {

        /* reverse the sense of the walk*/
        link = (walk_mode & RX_WALK_REVERS) ? !i : i;

        if (node->child_ptr[link] != NULL) {
            nodecounter += rx_walk_tree(node->child_ptr[link], func, walk_mode, maxlevel, level, 0, userptr, err);
            /* abort the walk on error*/
            if (func != NULL && *err != RX_OK) {
                break;
            }
        }
    }

    /* return count of nodes visited */
    return nodecounter;
}

/*+++++++++  
  go down the tree and delete all nodes of the tree.

  the function is called recursively with level increased
  it stops recursing when no child nodes are found or maxlevel is reached.
  
  therefore the initial call must set level to 0.
  
  the nodecounter increments at every node, and is the return value
  of the function. So start with 0 to get the number of nodes traversed.
  
  ERROR HANDLING IS DIFFERENT HERE!
  Unlike other functions it is not the return value:
  The error code from the func function IF DEFINED (== not NULL ) goes 
  to the variable pointed to by the last parameter.

  XXX: nodecounter is ALWAYS passed as 0, so should probably be removed from
       calling parameter list, shane 2001-02-01
++++++++++++*/
void rx_delete_dataleaves(void *element_data, void *result_ptr)
{
	rx_dataleaf_t *leafptr = element_data;
	rx_tree_t *tree = result_ptr;

	if (tree->family == RX_FAM_IP) {
		/* do not look at the leaf if  RX_FAM_IP */
		/* Just free the payload, there must be one and just one. */
		UT_free(leafptr);
	} else {					/* other families */
		/* if not >composed< then delete dataleaf */
		if (leafptr->composed == 0) {
			if (leafptr->data_ptr)
				UT_free(leafptr->data_ptr);
			UT_free(leafptr);

			LG_log(rx_context, LG_DEBUG, "dataleaf refcount = 0, removed");

		}
		/* else decrement the reference number ( == number of prefixes 
		   composing the range minus 1 == the >composed< flag */
		else {
			leafptr->composed--;

			LG_log(rx_context, LG_DEBUG, "dataleaf refcount -- to %d ", leafptr->composed);
		}
	}							/* if family != RX_FAM_IP */
}

void rx_delete_treenode(rx_tree_t * tree, rx_node_t * curnode)
{
	if (curnode->leaves_ptr) {
		LG_log(rx_context, LG_DEBUG, "deleting dataleaves of node at %08x", curnode);
		g_list_foreach(curnode->leaves_ptr, rx_delete_dataleaves, tree);
		/* delete the GList */
		g_list_free(curnode->leaves_ptr);
	}
	LG_log(rx_context, LG_DEBUG, "node at %08x removed", curnode);
	UT_free(curnode);
}

/* The fuction itself */ 
int rx_delete_tree(rx_tree_t * tree, rx_node_t * node, int maxlevel, int level, int nodecounter, void *userptr)
{
	int i, link;

	if (node == NULL)
		die;					/* program error. we expect a valid, checked, node. */

	/* check if the level is reached */
	if (level > maxlevel) {
		return nodecounter;
	}

	/* process the node appropriately: */
	/* increase our depth counter */
	level++;

	/* increase the count of visited nodes */
	nodecounter++;

	/* process left and right children */
	for (i = 0; i <= 1; i++) {
		link = i;

		if (node->child_ptr[link] != NULL) {
			nodecounter += rx_delete_tree(tree, node->child_ptr[link], maxlevel, level, 0, userptr);
			/* delete the processed child node */
			rx_delete_treenode(tree, node->child_ptr[link]);
		}
	}

	/* if this is the top level - delete the top node and the tree */
	if (node == tree->top_ptr) {
		rx_delete_treenode(tree, node);
		tree->top_ptr = NULL;
		tree->num_nodes = 0;
		tree->top_key = SQ_NOKEY;
		tree->parent_tree = NULL;
		strcpy(tree->data_table.val, "");
		strcpy(tree->radix_table.val, "");
		strcpy(tree->leaves_table.val, "");
	}

	/* return count of nodes deleted */
	return nodecounter;
}

/***************************************************************************/
/*++++++
  creates a (top) tree for the space, fills out sql table of trees
  generates a tablename for a tree (if NONE)
  updates LL of trees

++++++++*/
int 
RX_tree_cre (
	      char      *prefixstr, /*+ prefix the tree will cover (string) +*/
	      rx_fam_t   fam_id,
	      rx_mem_mt   mem_mode, /* memory only, memory+sql, sql only +*/
	      rx_subtree_mt subtrees,	/*+ one of NONE, AUTO, HAND +*/
	      rx_tree_t **treestore /* store the tree pointer here */
	     )

{
  rx_tree_t    *newtree;
  ip_prefix_t  newpref;
  ip_space_t   spc_id;

  if( !NOERR(IP_pref_e2b(&newpref, prefixstr))) {
    die;
  }

  spc_id = IP_pref_b2_space( &newpref );
  
  newtree = (rx_tree_t *)UT_malloc(sizeof(rx_tree_t));

  LG_log(rx_context, LG_DEBUG, "creating a tree at %08x", newtree);

  /* copy tree settings */ 
  newtree -> space  = spc_id;
  newtree -> family = fam_id;

  newtree -> subtrees = subtrees;
  newtree -> mem_mode = mem_mode;

  /* set other tree values */

  /* parent set to NULL because it's not a subtree */
  newtree -> parent_tree = NULL;
  /* PR_zeroprefix(& newtree -> prefix);*/
  newtree -> maxbits = IP_sizebits(spc_id);

  strcpy(newtree->data_table.val,"");
  strcpy(newtree->radix_table.val,"");
  strcpy(newtree->leaves_table.val,"");

  newtree->num_nodes = 0;

  newtree->top_ptr = NULL;
  newtree->top_key = SQ_NOKEY;
  
  newtree->prefix = newpref;

  TH_init_read_write_lockw( &(newtree->rwlock));

  *treestore = newtree;
  
  return RX_OK;
}


/* ************************************
   special walk function for use in consistency checks - it checks the parent
   pointer too.
************************************/
int rx_check_walk_tree( rx_node_t *node, 
			rx_node_t *parent_node, 
			int nodecounter,
			rx_treecheck_t *checkstruct )
{
int i;

 /* checks*/
 if( node == NULL ) {    
   checkstruct->code |= 1;
 }
 if( node->parent_ptr != parent_node ) {
   checkstruct->code |= 2;
 }
 if( node->glue && node->leaves_ptr ) {
   checkstruct->code |= 4;
 }
 if( node->glue && (node->child_ptr[0] == NULL || node->child_ptr[1] == NULL ) ) {
   checkstruct->code |= 8;
 }
 
 
 if( node->leaves_ptr && checkstruct->datatoo ) {
   switch( checkstruct->tree->family ) {
   case  RX_FAM_IP:
     /* the simplest (?) case: only one leaf attached to any node 
	(except for glues) */
     if( g_list_length(node->leaves_ptr) != 1 ) {
       checkstruct->code |= 16;
     }
     break;
   case RX_FAM_RT:
     /* many dataleaves attached to nodes. */
     break;
   case RX_FAM_IN:
     /* many dataleaves attached to nodes. 
	Some leaves pointed to from many nodes => from as many as the number
	of composing prefixes 
     */
     break;
   default: 
     /* ignore */
     break;
   }
 }
 
  
 if( checkstruct->code != 0 ) {
   checkstruct->node = node;
 
   return nodecounter;          /* abort the walk on error*/
 }


  nodecounter++;
  
  for(i=0; i<=1; i++) {
    if( node->child_ptr[i] != NULL ) {
      nodecounter += rx_check_walk_tree( node->child_ptr[i], 
					 node,
					 0, checkstruct );
      /* abort the walk on error*/
      if ( checkstruct->code != 0 ) {
	die; break;
      }
    }
  }
  return nodecounter;
}

/* **************************************************************************
tree consistency check.

if datatoo = 0, then only parent/child links are checked.

if datatoo = 1, then a check on the contents of the nodes is done too.

**************************************************************************/

int
RX_treecheck( rx_tree_t *tree, int datatoo, rx_treecheck_t *errorfound)
{
  int err = RX_OK;
  int nodnum;
  
  errorfound->tree = tree;
  errorfound->datatoo = datatoo;

  /* errorfound.node will be set by hook if it finds an error*/
  errorfound->code = 0;
  
  nodnum = rx_check_walk_tree( tree->top_ptr, 
			       NULL,
			       0,
			       errorfound );
  
  if( nodnum != tree->num_nodes ) { 
    errorfound->code |= 1024;
  }
  if( tree->num_nodes == 0 && tree->top_ptr != NULL ) { 
    errorfound->code |= 2048;
  }
  if( tree->num_nodes != 0 && tree->top_ptr == NULL ) { 
    errorfound->code |= 4096;
  }
  
  if( errorfound->code != 0) {
    err = RX_DATNOF;
  }
  return err;
}
