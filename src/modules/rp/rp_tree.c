/***************************************
  $Revision: 1.1 $

  Radix payload (rp) - user level functions for storing data in radix trees

  rp_load = user level tree maintenance (knows about registries and attributes)

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

#define RP_IMPL
#include "rip.h"

LG_context_t *rp_context;

void
RP_init (LG_context_t *ctx)
{
  rp_context = ctx;
}

/***************************************************************************/
/*++++++++++++++
  finds a tree matching the specified criteria(registry+space+family+tid).

  MT-note: locks/unlocks forest (still to be done)

  Returns: RX_OK or RX_NOTREE if no such tree can be found.
+++++++++++*/

int 
RP_tree_get ( rx_tree_t **treeptr, /*+ answer goes here, please +*/
              rp_regid_t reg_id,   /*+ id of the registry +*/
              ip_space_t spc_id,   /*+ type of space (ipv4/ipv6) +*/
              rp_attr_t  attr    /*+ extra tree id (within the same reg/spc/fam +*/
              )
     
{
  GList *elem;
  rp_tentry_t  *trdef;
  /* lock the forest */
  TH_acquire_read_lockw(&rx_forest_rwlock);
  elem = g_list_first(rx_forest);
  
  while( elem != NULL ) {
    trdef = elem->data;
    
    
    if(    trdef->reg_id == reg_id   
        && trdef->attr == attr 
        && trdef->tree->space == spc_id ) {
      /* copy the value to user's data */
      *treeptr = trdef->tree;
      LG_log(rp_context, LG_DEBUG,
                "tree found at %08x -> %08x",trdef, trdef->tree);
      return RP_OK;
    }
    elem = g_list_next(elem);
  }
  
  *treeptr = NULL; /* set when NOT FOUND*/
  TH_release_read_lockw(&rx_forest_rwlock);
/*  pthread_mutex_unlock(&rx_forest_mx);*/
  return RP_NOTREE; 
}



/*++++++++++++++++++++++++++++++++
       put into LL of trees; handle alloc err ??? 

       since other threads are supposed to be reading already, 
       must create the tree locked and observe the forest mutex.
       ++++++++++++++++++++*/
int 
RP_tree_add (
             rp_regid_t reg_id,    /*+ id of the registry +*/
             rp_attr_t  attr,      /*+ extra tree id (within the same registry/space/family +*/
             char      *prefixstr, /*+ prefix the tree will cover (string) +*/
             rx_mem_mt   mem_mode, /* memory only, memory+sql, sql only +*/
             rx_subtree_mt subtrees        /*+ one of NONE, AUTO, HAND +*/
             )
{
  int     err;
  rp_tentry_t *treedef;
  rx_tree_t   *mytree;
  rx_tree_t   *existree;
  rx_fam_t   fam_id = RP_attr2fam( attr );

  if( (err = RX_tree_cre(prefixstr, fam_id, mem_mode, subtrees, &mytree)) == RX_OK) {

    /* OK, see if there is a tree for this space already */
    if( RP_tree_get(&existree, reg_id, mytree->space, attr) == RP_OK ) {
      /* In this case we need to delete and re-initialize it  */
      /* lock the tree for writing and leave it in the locked state for loading */
      TH_acquire_write_lockw(&existree->rwlock);

      if( (err = RP_tree_del(existree)) == RP_OK){
              /* Initialize the tree */
              existree->num_nodes=0;
      }
      /* free the newly created tree */
      UT_free(mytree);
      return err;      
    }

    treedef = (rp_tentry_t *)UT_malloc(sizeof(rp_tentry_t));
    
    treedef -> reg_id = reg_id;
    treedef -> attr   = attr;
    treedef -> tree   = mytree;

    /* add the tree to the forest in locked state */
    TH_acquire_write_lockw( &(mytree->rwlock) );
    
    /* Lock the forest */
    TH_acquire_write_lockw(&rx_forest_rwlock);
    rx_forest = g_list_append (rx_forest, treedef);
    TH_release_write_lockw(&rx_forest_rwlock);
  }
  
  return err;
}

/*++++++++++++++
  finds a tree and deletes its contents
  the tree itself is not deleted !

  Returns: RX_OK or RX_NOTREE if no such tree can be found.
+++++++++++*/

int RP_tree_del(rx_tree_t * tree)
{
	int cnt;

	if (tree->top_ptr != NULL) {
		int num_nodes = tree->num_nodes;
		cnt = rx_delete_tree(tree, tree->top_ptr, 16777256, 0, 0, NULL);
		LG_log(rp_context, LG_INFO, "tree at %08x: %d nodes deleted", tree, cnt);
		if (cnt != num_nodes) {
			LG_log(rp_context, LG_ERROR, "tree at %p: %d nodes deleted, should be %d", tree, cnt, num_nodes);
			return RP_NOYETI;
		}
	}
	return RP_OK;
}


int 
rp_init_attr_tree( rp_regid_t reg_id, rp_attr_t attr)
{
  int err;
  
  err = RP_OK;

  /* Some (DN) attributes are related to two trees */
  if( RP_attr2spc(attr, IP_V4) ) {
    err=RP_tree_add(reg_id, attr, "0.0.0.0/0",
                    RX_MEM_RAMONLY, RX_SUB_NONE);
  } 
  
  if( RP_attr2spc(attr, IP_V6) ) {
   err=RP_tree_add(reg_id, attr, "0::/0",
                   RX_MEM_RAMONLY, RX_SUB_NONE);
  }
  
  return err;
}
/***************************************************************************/

int 
RP_init_trees( rp_regid_t reg_id )
{
  int err;
  
  if(    NOERR(err=rp_init_attr_tree(reg_id, A_IN))
      && NOERR(err=rp_init_attr_tree(reg_id, A_RT))
      && NOERR(err=rp_init_attr_tree(reg_id, A_I6))
      && NOERR(err=rp_init_attr_tree(reg_id, A_R6))
      && NOERR(err=rp_init_attr_tree(reg_id, A_DN)) ) {
    return RP_OK;
  }
  
  return err;
}




