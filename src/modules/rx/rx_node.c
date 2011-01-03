/***************************************
  $Revision: 1.38 $

  Radix tree (rx).  rx_node.c - functions to operate on nodes of the tree
  (creation/deletion).

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

#include "rip.h"
#include <glib.h>
#include <assert.h>

/* logging */
extern LG_context_t *rx_context;


/*
  rx_creat_node = create a new data node 
  (empty{glue} nodes get created automatically).

  Takes a pointer to the (already allocated) data leaf to be included 
  in the list of data nodes (presumably empty as the node is only now being
  created).
  
  Requires a stack of nodes created in CREAT mode (with glue nodes, 
  until deep enough and the last node being non-glue).
  
  MT notes: requires the tree to be locked.

  ip_prefix_t   *newpref        prefix of the node to be added
  rx_tree_t     *tree           tree the new node goes to
  rx_dataleaf_t *dataleaf       dataleaf to attach at this node
  rx_nodcpy_t   stack[]         stack==array of node_copies
  int           stackdepth      length of the stack
  
  Returns: RX_OK or error code.
*/
static int rx_creat_node(ip_prefix_t *newpref, rx_tree_t *tree, rx_dataleaf_t *dataleaf, rx_nodcpy_t stack[], int stackdepth) {
    rx_node_t *newnode, *curnode, *memnode, *gluenode;
    unsigned chk_bit, dif_bit, link, curpos;

    /* assume no such node yet. Will die if there is one.*/

    /* calloc, because parent/child keys and child ptrs are not always set.*/

    newnode = (rx_node_t *) UT_calloc(1, sizeof(rx_node_t));

    /* increment the number of nodes in the tree*/
    tree->num_nodes++;

    newnode->prefix = *newpref;

    /* attach the leaf to a (presumably empty?! hence NULL) list...*/
    newnode->leaves_ptr = g_list_prepend(NULL, dataleaf);

    /* OK, so take a look at the tree*/
    if (tree->num_nodes == 1) {
        /* The tree was empty. Create a new top node.*/
        tree->top_ptr = newnode;
        //        LG_log(rx_context, LG_DEBUG, "Created as the top node");
        return RX_OK;
    }

    /* OK, there is at least one node in the tree. Take a look at the stack.*/

    /*    we've got a real node there (not a glue), but we may be too deep.*/
    /*   (it's not a glue, because glues have always two children.*/
    /*    we had to go that deep because from a glue alone one doesn't know */
    /*    what it glues)*/
    /* GO UP.*/
    /* take the first differing bit from comparing */
    /* the new and the found nodes' prefixes. */
    /* (not deeper than the shorter of the two)*/

    curpos = stackdepth - 1;
    curnode = &stack[curpos].cpy;

    chk_bit = smaller(curnode->prefix.bits, newpref->bits );

    for (dif_bit = 0; dif_bit < chk_bit; dif_bit++) {
        /* break the loop when the first different bit is found*/

        if (IP_addr_bit_get(&curnode->prefix.ip, dif_bit) != IP_addr_bit_get(&newpref->ip, dif_bit)) {
            break;
        }
    }

    //    LG_log(rx_context, LG_DEBUG, "cur = %d, new = %d, chk_bit = %d, dif_bit = %d", curnode->prefix.bits, newpref->bits, chk_bit, dif_bit);

    if (dif_bit == IP_sizebits(newpref->ip.space))
        die; /* it mustn't happen!!!*/

    /* go up to that level (watch the head of the tree!)*/

    while (curpos > 0 && stack[curpos - 1].cpy.prefix.bits >= dif_bit) {
        curpos--;
        //        LG_log(rx_context, LG_DEBUG, "up to level %d", curpos);
    }

    /*
     if the bit lenghts of the node, new prefix and the diffbit are equal
     {
     YOU'VE GOT THE NODE where the new one will be attached.
     Either it has data (and will be moved accordingly),
     or is a glue (and will be turned into a regular node).
     }
     */

    curnode = &stack[curpos].cpy;

    /* RAM: set a pointer to the real node in memory*/
    memnode = stack[curpos].srcptr;

    if (dif_bit == newpref->bits && dif_bit == curnode->prefix.bits) {

        /* such node already exists, nothing to change in the tree!!!*/
        /* this should be checked before calling this function, so..*/

        die;
    }
    /*
     else  ** the branch ends here; we must create a new node... **
     {
     OK, how is the new node's prefix length w.r.t the dif_bit ?
     longer  -> make it a child of the node found
     shorter -> make it the parent of the node found and take its place
     equal   -> make a glue node the parent of both
     }

     WHEN ATTACHING THE NODE, VALUES FROM THE STACK ARE USED,
     TO PREVENT EXCESSIVE LOOKUPS AGAIN.

     */

    /* **** attach it.*/
    //        rx_nod_print(curnode, buf, 1024);
    //        LG_log(rx_context, LG_DEBUG, "Looking at node %s", buf);

    if (curnode -> prefix.bits == dif_bit) {

        /* attach here as a child of the node found      */
        link = IP_addr_bit_get(&newpref->ip, dif_bit);

        LG_log(rx_context, LG_DEBUG, "attaching as child %d", link);

        if (memnode -> child_ptr[link] != NULL) {
            die;
        }

        memnode -> child_ptr[link] = newnode;
        newnode -> parent_ptr = memnode;
    } else if (newpref->bits == dif_bit) {
        /* make it the parent of the node found and take its place,*/
        /* moving it down.*/

        /* set the link from the NEW node to the OLD one (different than before)*/

        link = IP_addr_bit_get(&curnode->prefix.ip, dif_bit);

        LG_log(rx_context, LG_DEBUG, "shifting down as child %d", link);

        /* PARENT<->NEW LINKS*/
        /* see if the node was the top_node*/
        if (curnode -> parent_ptr == NULL) {
            /*  update tree struct */
            tree -> top_ptr = newnode;
        } else {
            /* no - fix the child link at the parent.*/
            /* at the link where it was attached*/
            int link = (curnode->parent_ptr->child_ptr[1] == memnode);
            memnode -> parent_ptr -> child_ptr[link] = newnode;
        }
        memnode -> parent_ptr = newnode;

        /* NEW<->CHILD LINKS*/
        newnode -> parent_ptr = curnode->parent_ptr;
        newnode -> child_ptr[link] = memnode;
    } else {
        /* create a glue and shift the curnode below the glue,*/
        /* then attach the new node at the glue*/

        /* calloc, because parent/child keys are not set.*/

        gluenode = UT_calloc(1, sizeof(rx_node_t));
        tree -> num_nodes++;

        LG_log(rx_context, LG_DEBUG, "created glue node at %p", gluenode);

        gluenode -> prefix.bits = dif_bit;

        /* fill in the address. The glue node should get the prefix*/
        /* shorter by one than the shorter of the two prefixes that are glued*/
        /* (difbit)*/
        /**/

        gluenode -> prefix.ip = newpref->ip;
        gluenode -> prefix.bits = dif_bit;

        /* the ip in this prefix is probably incorrect. Fix it.*/
        IP_pref_bit_fix(&gluenode -> prefix);

        gluenode -> leaves_ptr = NULL;
        gluenode -> glue = 1;

        /* 1. Fix the link to and from the parent to the gluenode.*/

        gluenode -> parent_ptr = curnode->parent_ptr;
        if (gluenode->parent_ptr == NULL) {
            tree -> top_ptr = gluenode;
        } else {
            /* fix the child link in the parent. */
            /* if it was at 1, then let fix the link 1, 0 otherwise*/

            link = (curnode->parent_ptr->child_ptr[1] == memnode);

            memnode->parent_ptr->child_ptr[link] = gluenode;
        }

        /* 2. Fix the links between gluenode and the OLD node*/

        link = IP_addr_bit_get(&newpref->ip, dif_bit);

        gluenode -> child_ptr[!link] = memnode;
        memnode->parent_ptr = gluenode;

        /* 3. Fix the links between gluenode and the NEW node*/

        gluenode -> child_ptr[link] = newnode;
        newnode -> parent_ptr = gluenode;
    }
    return RX_OK;
}


/*
 an auxiliary function to delete data from a node 
 (and delete the node or turn it into a glue afterwards)

 takes 

 tree              tree
 curnode           pointer to the node 
 dataleaf          pointer to a dataleaf with ObjectID (dataleaf->data_key) 
                   set; which is used to choose the right dataleaf
		   when browsing data leaves. It is never assumed to be 
		   allocated via malloc, can be a local variable as well.

    If the composed flag of the dataleaf in the tree 
    (being the reference count at the same time) 
    is non zero, decrements the count.
    Deletes the dataleaf when it reaches zero.

 succeeds always or dies when dataleaf with such data cannot be found
 in the node
*/
void rx_delete_node(rx_tree_t *tree, rx_node_t *curnode, rx_dataleaf_t *dataleaf) {
    rx_dataleaf_t *leaffound = NULL;
    GList *qitem;
    int leavesum = 0;

    /*+ RX_FAM_IP implies there's no dataleaf!!!
     The structure in place of a dataleaf is payload
     +*/

    /* go through leaves, comparing the objectID (data_key) */
    for (qitem = g_list_first(curnode->leaves_ptr); qitem != NULL; qitem = g_list_next(qitem)) {
        rx_dataleaf_t *leafptr = qitem->data;

        if (tree->family == RX_FAM_IP /* do not look at the pointers */
        || leafptr->data_key == dataleaf->data_key) { /* if RX_FAM_IP */
            leaffound = leafptr;
            /* no break - we're counting leaves..*/
        }
        leavesum++;
    }

//    LG_log(rx_context, LG_DEBUG, "%d dataleaves at the node", leavesum);

    /* return error if none of the dataleaves matched */
    if (leaffound == NULL)
        die;

    /* NO error? good. Remove the leaf from the list */
    curnode->leaves_ptr = g_list_remove(curnode->leaves_ptr, leaffound);

    if (tree->family == RX_FAM_IP) {
        /* again: do not look at the leaf if  RX_FAM_IP */
        /* Just free the payload, there must be one and just one. */
        UT_free(leaffound);
    } else { /* other families */
        /* if not >composed< then delete dataleaf */
        if (leaffound->composed == 0) {
            if (leaffound->data_ptr != NULL /* allow dataleafs without attached */
            && leaffound->data_len > 0) { /* data */
                UT_free(leaffound->data_ptr);
            }
            UT_free(leaffound);

//            LG_log(rx_context, LG_DEBUG, "dataleaf refcount = 0, removed");

        }
        /* else decrement the reference number ( == number of prefixes
         composing the range minus 1 == the >composed< flag */
        else {
            leaffound->composed--;

//            LG_log(rx_context, LG_DEBUG, "dataleaf refcount -- to %d ", leaffound->composed);
        }
    } /* if family != RX_FAM_IP */

    /* if that was the last leave at this node, then delete node. */
    if (leavesum == 1) {
        rx_node_t *parent = curnode->parent_ptr;

//        LG_log(rx_context, LG_DEBUG, "last dataleaf, removing node");

        assert(curnode->leaves_ptr == NULL);
        /* To do this, check the number of children: */

        /*  0 - just delete this node and the link to it */
        if (curnode->child_ptr[0] == NULL && curnode->child_ptr[1] == NULL) {

//            LG_log(rx_context, LG_DEBUG, "no children, just removing");

            if (parent != NULL) { /* watch the head! */
                int plink = (parent->child_ptr[1] == curnode);
                parent->child_ptr[plink] = NULL;
            } else {
                assert(tree->top_ptr == curnode);
                tree->top_ptr = NULL;
            }
            tree->num_nodes--;
            UT_free(curnode);

            /* now, if we deleted curnode, let's see if the parent node is a glue.
             If it is, then hook the remaining child up the grandparent,
             and delete the parent */
            if (parent != NULL && parent->glue) {
                int slink = (parent->child_ptr[1] != NULL);
                rx_node_t *schild = parent->child_ptr[slink];
                rx_node_t *gparent = parent->parent_ptr;

                assert( schild != NULL && parent->child_ptr[ ! slink] == NULL);

                /* upd parent */
                if (gparent != NULL) { /* watch the head! */
                    int plink = (gparent->child_ptr[1] == parent);
                    gparent->child_ptr[plink] = parent->child_ptr[slink];
                } else {
                    assert(tree->top_ptr == parent);
                    tree->top_ptr = parent->child_ptr[slink];
                }

                /* update the child's parent link too */
                parent->child_ptr[slink]->parent_ptr = gparent;

                /* del */
                tree->num_nodes--;
                UT_free(parent);

            } /* if parent glue */
        }
        /*  2 - turn into a glue  */
        else if (curnode->child_ptr[0] != NULL && curnode->child_ptr[1] != NULL) {

//            LG_log(rx_context, LG_DEBUG, "two children, turning into a glue");

            curnode->glue = 1;

        }
        /*  1 - copy the child's link to parent. then delete */
        else {
            int clink = (curnode->child_ptr[1] != NULL);

//            LG_log(rx_context, LG_DEBUG, "one child at %d, shifting it up", clink);

            /* upd parent */
            if (parent != NULL) { /* watch the head! */
                int plink = (parent->child_ptr[1] == curnode);
                parent->child_ptr[plink] = curnode->child_ptr[clink];
            } else {
                /* no parent; the child becomes the top node now */
                tree->top_ptr = curnode->child_ptr[clink];
            }

            /* update the child's parent link too */
            curnode->child_ptr[clink]->parent_ptr = parent;

            /* del */
            tree->num_nodes--;
            UT_free(curnode);
        }

    } /* leavesum == 1 <=> that was the last data leaf */
} /* rx_delete_node */


/*
  General function to operate on dataleaves attached to a single node
  (create / modify / delete).
  
  searches tree, finds and creates/deletes a node,
  copies modified nodes to disk using rx_sql_node_set (not yet implemented).
  Updates memory rollback info.
  
  creation: 
    Add a dataleaf at the node defined by prefix. 
    Create a new node if it doesn't exist yet.
 
  MT notes: requires the tree to be locked.
  
  rx_oper_mt mode           MODE={cre|mod|del}
  ip_prefix_t *newpref      prefix of the node
  rx_tree_t *tree           pointer to the tree structure
  rx_dataleaf_t *leafptr    dataleaf to attach at the node

  Returns: RX_OK or error code.

  Errors from:
  rx_bin_search,
  memory alloc routines.
  
  - no such node (if not in create mode)
  
  - too many nodes found (strange).
*/
int rx_bin_node(rx_oper_mt mode, ip_prefix_t *newpref, rx_tree_t *tree, rx_dataleaf_t *dataleaf) {
    GList *nodlist = NULL;
    int nodesfound, stackdepth;
    int glue;
    rx_nodcpy_t *curcpy;
    rx_node_t *curnode;
    rx_nodcpy_t stack[128];
    int err;
    char bbf[IP_PREFSTR_MAX];

    IP_pref_b2a(newpref, bbf, IP_PREFSTR_MAX);
//    LG_log(rx_context, LG_DEBUG, "rx_bin_node: %s in spc %d /fam %d operation %d", bbf, tree->space, tree->family, mode);

    /* first check: are we using the correct tree ???*/
    if (tree->space != newpref->ip.space) {
        /* trying to insert a prefix of space %d into a tree of space %d\n",
         tree->space,
         newpref->ip.space);
         */
        die;
    }

    assert( dataleaf );
    assert( newpref->bits <= IP_sizebits(tree->space) );

    /* fix the prefix, to make sure all insignificant bits are 0*/
    IP_pref_bit_fix(newpref);

    if ((err = rx_build_stack(stack, &stackdepth, tree, newpref, RX_STK_CREAT)) != RX_OK) {
        return err; /*die*/
    }

    /*   rx_stk_print(stack, stackdepth);*/

    /* perform a search on the stack. The result is a list, and it must*/
    /* be properly deleted after use!!*/

    if ((err = rx_nod_search(RX_SRCH_CREAT, 0, 0, tree, newpref, stack, stackdepth, &nodlist, RX_ANS_ALL)) != RX_OK) {
        return err; /* die;*/
    }

    /* count number of nodes in the answer */
    nodesfound = g_list_length(nodlist);

    switch (nodesfound) {
    case 0:
        /* no such node (yet). See what we're up to.
         if( mode==cre )  create, else - program error, die */

        /*  C R E A T I O N */
//        LG_log(rx_context, LG_DEBUG, "rx_bin_node: Creating a new node %s in spc %d /fam %d ", bbf, tree->space, tree->family);
        if (mode != RX_OPER_CRE) {
            die;
        }

        rx_creat_node(newpref, tree, dataleaf, stack, stackdepth);
        break;

    case 1: /* found */
        /* set the curnode pointer   */
        curcpy = g_list_nth_data(nodlist, 0);
        curnode = curcpy->srcptr;

        switch (mode) {
        case RX_OPER_CRE:
            /*  attach the data at the node that was found;*/

            /* was it glue ?*/
            glue = curnode->glue;

            curnode->leaves_ptr = g_list_prepend(curnode->leaves_ptr, dataleaf);
            /* now it's not a glue anymore */
            curnode->glue = 0;

//            LG_log(rx_context, LG_DEBUG, "Appended data to a %s node", glue ? "glue" : "data");

            break;

        case RX_OPER_DEL:
//            LG_log(rx_context, LG_DEBUG, "rx_bin_node: Deleting node %s in spc %d /fam %d ", bbf, tree->space, tree->family);
            rx_delete_node(tree, curnode, dataleaf);
            break;
        }
        break;

    default:
        /* too many nodes found! from an exact/exact-less-1 search. */
        die;
    }

    wr_clear_list(&nodlist);

    return RX_OK;
}


/*
  A wrapper around RX_bin_node.

  It's there only to control the freeing of dataleaf copies passed
  for comparison during deletion.

  rx_oper_mt mode           MODE={cre|mod|del}
  ip_prefix_t *newpref      prefix of the node
  rx_tree_t *tree           pointer to the tree structure
  rx_dataleaf_t *leafptr    dataleaf to attach at the node
*/
int RX_rt_node(rx_oper_mt mode, ip_prefix_t *newpref, rx_tree_t *tree, rx_dataleaf_t *leafptr) {
    int reterr;

    IP_pref_2_rang(&leafptr->iprange, newpref);
    leafptr->preflen = IP_pref_b2_len(newpref);

    /* store the object's range, used in rp.search */
    reterr = rx_bin_node(mode, newpref, tree, leafptr);

    return reterr;
}
     

/*
  performs the actual update for inetnums (possibly composed of many prefixes).
  Decomposes the ranges into prefixes and then falls back to rx_bin_node
  to perform changes at the nodes.
  
  Requires/returns - practically the same as rx_bin_node.

  rx_oper_mt mode           MODE={cre|mod|del}
  ip_range_t *rang          range of IP addresses
  rx_tree_t *tree           pointer to the tree structure
  rx_dataleaf_t *leafptr    dataleaf to attach at the node
*/
int RX_in_node(rx_oper_mt mode, ip_range_t *rang, rx_tree_t *tree, rx_dataleaf_t *leafptr) {
    unsigned i, prefcount;
    GList *preflist = NULL;
    char buf[IP_RANGSTR_MAX];

//    IP_rang_b2a(rang, buf, IP_RANGSTR_MAX);
//    LG_log(rx_context, LG_DEBUG, "rx_inum_node: adding %s", buf);

    /* decompose, put links to the data leaf into every prefix*/
    /* that makes up this range.*/
    IP_rang_decomp(rang, &preflist);

    /* see if there is more than 1 prefix, set the composed flag*/
    prefcount = g_list_length(preflist);
    leafptr->composed = (prefcount - 1);

    leafptr->iprange = *rang;

    if (prefcount == 0) {
        /* XXX This indicates that some inetnum ranges are not correct (e.g. start>end) */
        /* XXX Should not happen and may break normal operation of the rx lookups */
        /* XXX maybe we need to die here, but this is too harsh at this point */
        IP_rang_b2a(rang, buf, IP_RANGSTR_MAX);
        LG_log(rx_context, LG_ERROR, "prefcount=0 range: [%s][%u-%u]\n", buf, (rang->begin).words[0], (rang->end).words[0]);
    }

    for (i = 0; i < prefcount; i++) {
        ip_prefix_t *mypref = g_list_nth_data(preflist, i);
        rx_bin_node(mode, mypref, tree, leafptr);
    }

    /* free the storage from decomposition*/
    wr_clear_list(&preflist);

    return RX_OK;
}
