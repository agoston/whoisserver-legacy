/***************************************
  $Revision: 1.40 $

  Radix tree (rx).  rx_search.c - functions to search nodes of the tree

  Status: NOT REVUED, TESTED, COMPLETE

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

/* logging */
extern LG_context_t *rx_context;

/***************************************************************************/

/*++++++++++++++
Descends the given tree following the last prefix bit to get [past]
the node with the given prefix.
It fills up a stack of COPIES of nodes, including glue nodes.

Then it also sets the number of elements on the stack: 
set maxdepth to the position where a next one would be written
( = last + 1, or number of nodes pushed)

   The dmodes:
           
   RX_STK_QUERY_NOGLUE = (search exact/less spec) stop when 
                         * the current prefix length >= newprefix length 
                         * the current prefix does not match anymore
                         * do not add glue nodes
                        
   RX_STK_QUERY_ALLNOD = as above, except that the glue and data nodes are
                         treated equally (i.e. glue nodes are not skipped)

   RX_STK_CREAT        = descend until the next non-glue node past the one found
                         in exact mode (for creation)

++++++++++++++*/

int
rx_build_stack(rx_nodcpy_t    stack[], 
               int            *maxdepth, 
               rx_tree_t      *tree, 
               ip_prefix_t    *newpref,
               rx_stk_mt      dmode
               )
{
  register rx_node_t *curnode;
  register int link, quit_now=0;
  char bbf[IP_PREFSTR_MAX];

  IP_pref_b2a( newpref , bbf, IP_PREFSTR_MAX);
  LG_log(rx_context, LG_DEBUG,
            "rx_build_stack: searching for %s in mode %d", bbf, dmode);

  *maxdepth = 0;
  
  if ( tree -> num_nodes == 0) { 
    /* The tree was empty. */
    return RX_OK;
  }
  
  curnode = tree->top_ptr;
  /* this works for RAM, for SQL one would have to call a 'getsqlnode' here*/
  
  /* OK, there is at least one node. Descend the tree */
  /* as long as the correct bit length is not exceeded*/
  /* or a glue is being found (take the last non-glue node then) */
  /* or you run out of nodes in the direction of descending*/
  
  do {
    /* check at the current node, where the one we look for would fit*/
    /* (the second argument of IP_addr_bit_get starts with 0,*/
    /* so this effectively looks at the bit next to the last significant bit*/
    /* of the current node*/
    
    link = IP_addr_bit_get( & newpref->ip, curnode->prefix.bits );    
    
    /* check conditions for leaving the loop    */
    if(curnode->child_ptr[link] == NULL) {
      /* end of branch. quit after adding the current node to the stack*/
      /* (or before - subject to bit test in QUERY mode)*/
      quit_now = 1;
    }
    else {
    /* check the node. 
       BIG DIFFERENCE between the modes:
       in CREAT we don't mind the stack to go too deep, 
       in QUERY it can lead to false answers
       (e.g. a /24 is found for a /23 query). 

       So this must be "peeled off the stack" later in the search routine,
       if both types of stack are to work properly with query searches.
    */


      if( curnode->prefix.bits > newpref->bits ) {
        /* deep enough.*/
        quit_now = 2;
      }

      if(dmode == RX_STK_CREAT && curnode->glue) {
        /* mode: creation. */
        /* Cancel quitting if glue -- in CREAT mode the stack building */
        /* should stop at the next real (non-glue) node.*/
        /* ("next" meaning following link #0)*/
        quit_now = 0;
      }
    }
    
    /* now that the conditions for leaving the loop after the node is
       added on the stack, see if we shouldn't leave the loop BEFOREHAND */
    
    /* In query mode, we should quit as soon as we see a mismatch */

    if(dmode != RX_STK_CREAT
       && 0 != IP_addr_cmp(&curnode->prefix.ip, &newpref->ip, 
                           curnode->prefix.bits) ) {
        /*QUIT NOW! (but add this node)*/
      quit_now = 4;
    }

    /* push the current node on the stack. RAM only.*/
    /* */
    /* (unless quit_now is 64 which means do NOT copy the current node.*/
    /**/
    /* In CREAT and QUERY_ALLNOD modes, push everything. */
    /* In QUERY_NOGLUE mode, only non-glues.*/
      
    if( /* quit_now < 64 &&           disabled as 64 is not in use right now */
       (dmode != RX_STK_QUERY_NOGLUE || curnode->glue == 0 )) {
	memcpy( & stack[*maxdepth].cpy, curnode, sizeof(rx_node_t));
	stack[*maxdepth].srcptr = curnode;
	stack[*maxdepth].srckey = SQ_NOKEY;
	stack[*maxdepth].tree = tree;
	(*maxdepth)++;
    }
    
    /* make debug info.*/
   
    IP_pref_b2a( & curnode->prefix , bbf, IP_PREFSTR_MAX );
    LG_log(rx_context, LG_DEBUG,
                "rx_build_stack: %s%d at %s%s (stk len: %d)",
                quit_now ? "stop/" : "link ",  
                quit_now ? quit_now : link,
                bbf, ( curnode->glue ) ? " ++glue++" : "",
                *maxdepth  );
    
    curnode = curnode -> child_ptr[link];

  } while( !quit_now ); 

  return RX_OK;
}

/***************************************************************************/
/*+++++++++
   helper for the nod_search routine: 

   allocate a new node copy struct, copy the struct and add to nodlist
++++++++++*/

static
int
rx_nod_append( GList **nodlist, rx_nodcpy_t *element) 
{
  rx_nodcpy_t *newcpy;
  
  newcpy = (rx_nodcpy_t *)UT_malloc(sizeof(rx_nodcpy_t));
  memcpy(newcpy, element, sizeof(rx_nodcpy_t));        
  (*nodlist) = g_list_prepend( *nodlist, newcpy );

  return RX_OK;
}




/***************************************************************************/

/*+++++++++++
  helper for MORE specific lookup in rx_nod_search 

  adds a node to the list of answers.
+++++++++++*/

static
int
rx_walk_hook_addnode(rx_node_t *node, int level, int nodecounter, 
                     void *userptr)
{   
  rx_nodcpy_t nodcpy;
  hook_addnode_userdat_t *userdat = userptr;
  

  /* do not append glue nodes*/
  if( node->glue == 1 ) return RX_OK;
  
  /* in RAM mode, do not copy the node.*/
  /*  memcpy( &nodcpy.cpy, node, sizeof(rx_node_t));*/

  /* XXX reset to 0 to avoid warnings from workshop: but it 
     slows things down! */
  memset( &nodcpy.cpy, 0, sizeof(rx_node_t));

  nodcpy.srcptr = node;
  nodcpy.srckey = SQ_NOKEY;
  nodcpy.tree = userdat->tree;
  
  return rx_nod_append( userdat->nodlist, &nodcpy);
}


/***************************************************************************/

/*+++++++++++
  helper for DBLS lookup in rx_nod_search 

  adds a node to the list of answers.
+++++++++++*/

static
int
rx_walk_hook_adddoubles(rx_node_t *node, int level, int nodecounter, 
                        void *userptr)
{
  rx_nodcpy_t nodcpy;
  hook_addnode_userdat_t  *userdat = userptr;
  int leaves = g_list_length(node->leaves_ptr);
  char buf[1024];
  
  /* do not append glue nodes*/
  if( node->glue == 1 ) return RX_OK;

 
  /* add only nodes with more than 1 dataleaf*/
  if( leaves < 2 ) return RX_OK;

  rx_nod_print(node, buf, 1024);
  LG_log(rx_context, LG_DEBUG,
              "rx_walk_hook_adddoubles: %30s, %d leaves", buf, leaves);

  /*  memcpy( &nodcpy.cpy, node, sizeof(rx_node_t));*/
  nodcpy.srcptr = node;
  nodcpy.srckey = SQ_NOKEY;
  nodcpy.tree = userdat->tree;
  
  return rx_nod_append( userdat->nodlist, &nodcpy);
}


/***************************************************************************/
int
rx_nod_search (
               rx_srch_mt  search_mode,
               int         par_a,
               int         par_b,
               /* see rx_asc_search() for explanation */
               rx_tree_t  *tree,           /* tree ptr*/
               ip_prefix_t  *prefix,          /* binary prefix*/

               rx_nodcpy_t stack[],         /* stack==array of node_copies*/
               int         stackcount,      /* number of element on the stack,*/
                                            /* can come from a creat stack!*/

               GList       **nodlist,       /* answers go here*/
               int         max_count        /* max # of answers*/
               )                        
     /*
        searches the stack for a given prefix, finds *nodes* in the stack 
        and appends *copies of the nodes* to the nodlist;

        finds
        0 or 1 nodes for exact search
        0 or 1 nodes for exless (0 if no less specific node found)
        any number (incl. 0) for {more|less}^n-m specific 
     
       returns errcode.

       
     */
{
  char buf[1024];
  int sps = stackcount-1;       /* stack position.*/
  int depthcounter=0;
  int err=RX_OK;
  int i;
  hook_addnode_userdat_t datstr;
  int (*hook_function)();  /* pointer to the walk_hook function*/
                                /* (see MORE spec lookup)*/

  /* structure for carrying data to walk_tree hook functions, used only
     in MORE, DBLS and RANG search modes 
  */
  datstr.nodlist = nodlist;
  datstr.tree    = tree;
  datstr.prefix  = prefix;
    
  
  IP_pref_b2a( prefix , buf, IP_PREFSTR_MAX);
  LG_log(rx_context, LG_DEBUG,
              "rx_nod_search: searching for %s in mode %d (%s)", 
	      buf, search_mode, RX_text_srch_mode(search_mode) );

  /* in non-CREAT modes, glue nodes are skipped anyway. 
     (they should normally not be there if the stack was created in
     the STK_QUERY mode, but it's possible to use a CREAT stack too).

     It's also possible that the stack is too deep.
     So, truncate the stack to the last non-glue node 
     of the length <= search term.
     otherwise a /24 would be returned for a /23 query.
     
     For LESS SPECIFIC searches one has to peel off entries
     whose prefixes do not contain the search term, 
  */

  if( search_mode != RX_SRCH_CREAT ) {
    
    while( sps >= 0 ) {
      char *reason = NULL;

      if( stack[sps].cpy.prefix.bits > prefix->bits ) {            /* too deep*/ 
	reason = "2deep";
      } 
      else if( 0 != IP_addr_cmp(& stack[sps].cpy.prefix.ip, &prefix->ip, 
				stack[sps].cpy.prefix.bits) ) {   /* mismatch */
	reason = "mismatch";
      }
      else if ( search_mode != RX_SRCH_MORE && search_mode != RX_SRCH_DBLS 
		&& search_mode != RX_SRCH_RANG && stack[sps].cpy.glue == 1 ) {                    /* is glue*/
	reason = "glue";
      }	
#if 0 
      /* mhm. it can't be limited here, must be done in RP */
      else if ( search_mode == RX_SRCH_LESS && par_a == 1
		&& stack[sps].cpy.prefix.bits == prefix->bits ) {  /* too deep*/
	reason = "2deep4less";
      }
#endif
	
      else {
	
	break;  /* stop peeling off */
      }
      
      rx_nod_print( & stack[sps].cpy , buf, IP_PREFSTR_MAX);
      LG_log(rx_context, LG_DEBUG,
		  "rx_nod_search: peeling off %d: %s (%s)", sps, buf, reason);
      sps--;
    }
  }
  
  /* nothing left on the stack. Sorry.*/
  /* we allow that for more spec search -- this means*/
  /* that the search term is a shorter prefix than the one*/
  /* in the top node. Possibly it's 0/0 which is valid for more spec search.*/

  if( search_mode != RX_SRCH_MORE && search_mode != RX_SRCH_DBLS 
      && sps < 0 ) {       
    return RX_OK;
  }
      
  switch(search_mode) {
  case RX_SRCH_EXACT:
  case RX_SRCH_CREAT:
    /* go up the tree (stack) and exit when the proper prefix is found.*/
    /* For RX_SRCH_EXACT skip glue nodes, for RX_SRCH_CREAT take all.*/
    /* They may contain a valid prefix, so watch out.*/

    while(sps >= 0) {

      rx_nod_print(& stack[sps].cpy, buf, 1024);
      LG_log(rx_context, LG_DEBUG,
                  "rx_nod_search: position %d: %s", sps, buf);
      
      if ( search_mode == RX_SRCH_EXACT 
           && stack[sps].cpy.glue ) {
        die;
      }
      
      if ( memcmp( & stack[sps].cpy.prefix, 
                   prefix, 
                   sizeof(ip_prefix_t)) == 0 ) {
        /* FOUND!!*/
        /* add to the nodlist.*/

        if( (err=rx_nod_append( nodlist, & stack[sps])) != RX_OK ) {
          return err;
        }

        LG_log(rx_context, LG_DEBUG, "rx_nod_search: found!");
        break;
      }
      sps--;
    }
    break;

  case RX_SRCH_EXLESS:
    /* just fetch the last element off the stack (if any). */
    /* Must be non-glue for EXLESS.*/

    if( sps >= 0 ) {
      rx_nod_append( nodlist, & stack[sps]); 
    }

    /* else : nothing found.*/
    /* For EXLESS: check if the stack contains only non-glue nodes.*/
    /* If it contains a glue, it means it was created in the CREAT mode,*/
    /* which renders the above algorithm absolutely useless. Then crash,*/
    /* this is a programmer's error.*/

    while( sps >= 0 ) {
      if( stack[sps].cpy.glue ) {
        die;
      }
      sps--;
    }

    break;

  case RX_SRCH_LESS:
    while( sps >= 0 && depthcounter < par_a ) {
      if( stack[sps].cpy.glue == 0 ) {
        rx_nod_append( nodlist, & stack[sps]); 
        depthcounter++;
      }
      sps--;
    }
    break;

  case RX_SRCH_MORE:
  case RX_SRCH_DBLS:   /* special (debug?) mode : find nodes with multiple*/
                       /* data leaves. Much like more specific, except that*/
                       /* most nodes will be skipped.*/
                       /* The difference is in calling another hook function*/
    hook_function = ( search_mode == RX_SRCH_MORE )  
      ? rx_walk_hook_addnode
      : rx_walk_hook_adddoubles;
    
    /* the result of a more spec search should NOT contain the object exactly*/
    /* matching the query, even if it exists in the database. So two walks are */
    /* performed, one for each child (if it exists). */
    /* MEMORY IMPLEMENTATION ONLY FOR THE MOMENT*/

    /* start from the top node if the searched prefix is between the 
       top node and the first node on the stack (i.e. the first node is
       contained within the search term) */
    
    /* COVERS THE CASE 0.0.0.0/0 */
    /* or any other prefix that the tree might be set to represent,*/
    /* but there is no actual object for it (not even glue)*/

    if( sps < 0 ) {
      if( tree->num_nodes > 0         /* there is any node in the tree */  
	  && 0 == IP_addr_cmp( & prefix->ip, 
			       & stack[0].cpy.prefix.ip,
			       prefix->bits) ) {     /* addr match */
	rx_walk_tree( tree->top_ptr, hook_function,
		      /* RX_WALK_REVERS | */ RX_WALK_SKPGLU, /* skip glue nodes while counting*/
		      par_a, /* display this many levels */
		      0, 0, &datstr, &err);
	if( err != RX_OK ) {
	  return err;
	}
      }
    } /* if nothing on stack */
    else { 

      /* walk from this node if it matches the query prefix and is
       long enough (if it is shorter, then it will harvest too many
       results
      */
      if(  prefix->bits <= stack[sps].srcptr->prefix.bits
	   && 0 == IP_addr_cmp( & stack[sps].srcptr->prefix.ip, 
				& prefix->ip, 
				prefix->bits) ) {
	rx_walk_tree( stack[sps].srcptr,  hook_function,
		      /* RX_WALK_REVERS | */ RX_WALK_SKPGLU, /* skip glue nodes while counting*/
		      par_a, /* display up to this max length*/
		      0, 0, &datstr, &err);
	if( err != RX_OK ) {
	  return err;
	}
      }      
      else {
	/* or walk the child nodes otherwise (still check the address) */
	
	for( i = 1; i >= 0; i--) {
	  if( stack[sps].cpy.child_ptr[i] != NULL ) {
	    IP_pref_b2a(& stack[sps].cpy.child_ptr[i]->prefix, buf, 1023);
	    
	    if( 0 == IP_addr_cmp( & stack[sps].cpy.child_ptr[i]->prefix.ip, 
				  & prefix->ip, 
				  prefix->bits) ) {
	      
              LG_log(rx_context, LG_DEBUG,
			"rx_nod_search: digging child %d: %s", i, buf);
	      
	      rx_walk_tree( stack[sps].cpy.child_ptr[i],  hook_function, 
			    /* RX_WALK_REVERS | */ RX_WALK_SKPGLU, /* skip glue nodes while counting*/
			    par_a, /* display this many levels */
			    0, 0, &datstr, &err);
	      if( err != RX_OK ) {
		return err;
	      }
          }
	    else {
              LG_log(rx_context, LG_DEBUG,
			"rx_nod_search: prefix mismatch with child %d: %s", 
			i, buf);
	    }
	  }
	}
      } /* if node does not match, dig child nodes */
      
    }
    break;

  case RX_SRCH_RANG:
    /* OK, start from the node at the end of the stack (exless match including
       glue nodes) then

       
       if its prefix length is 
         OK -> found! descend from here as long as the prefixes are in range
         shorter -> apparently there is even no such glue node. come back down
                    one step
                   
    */
    
    i = sps;               /* go up the tree (down the stack) */
                           /* until too far (one node too much, after >= )*/
    while( i >= 0 && stack[i].cpy.prefix.bits >= prefix->bits ) {
      i--;
    }
    
    /* look where you are:*/
    
    if( i < 0 )          /* it was the top object, but its prefix was too long*/
      i=0;               /* take the top object as the base*/
    else
      i++;               /* went one too much, now come back one step*/
    
    
    rx_walk_tree( stack[i].srcptr,  rx_walk_hook_addnode, 
                  RX_WALK_PRFLEN, /* skip glue nodes while counting*/
                  par_a, /* display up to this max length*/
                  0, 0, &datstr, &err);
    if( err != RX_OK ) {
      return err;
    }
    
    break;    

    /* return RX_NOYETI;*/
    /*not implemented*/
    /*    die; */
  default:
    die; /* are you nuts??*/
  }

  return err;

}



/*****************************************************************************/

/*+++++++++++++
  builds a stack for this prefix, finds *nodes* in the stack 
  and appends *copies of the data leaves* to the LL of answers;
  
  sorts by SQL object keys and uniq's the data
  
  finds:
  0 or 1 nodes for exact search
  0 or 1 nodes for exless (0 if no less specific node found)
  any number (incl. 0) for {more|less}-n specific 
  
  then copies the nodes/dataleaves to the answer structs and appends them
  to the given LL. So, effectively, the number of answers can be
  anything from 0 to infinity, because objects may be duplicate 
  even at the same node.
  
  returns errcode.
  
  algorithm:
  
  builds stack[MAXBIT (==128)];
  
  if( more/less-depth && par_a == 0)
  
  run rx_nod_search, then 
  
  if(more spec) rx_nod_walk(maxdepth=n, append_to_LL() );
  if(less spec) do { append(LL, stack[i]) } while(i-- && n--);
  otherwise just set LL
  
  
  The routine provides _at_least_ max_count answers. 
  It will *try* to stop after max_count as soon as possible 
  - but it's the higher level routine that should do the final cut.
+++++++++++++++*/

int
RX_bin_search (
               rx_srch_mt  search_mode,
               int         par_a,
               int         par_b,
               rx_tree_t  *tree,           /* tree ptr*/
               ip_prefix_t *prefix,         /* binary prefix*/
               GList       **datleaves,    /* data leaves go here*/
               int         max_count 
               )
   
{
  rx_nodcpy_t  stack[128];
  unsigned k;
  int stkcnt, resnum = 0, maxleaves;
  GList  *nodlist = NULL, *nitem;
  rx_node_t *curnode;
  rx_nodcpy_t *curcpy;
  rx_datref_t *datref;
  rx_stk_mt     dmode;

  /* more specific node search may start from a glue node, */
  /* for all others the stack should not contain glues.*/

  dmode = ( search_mode == RX_SRCH_MORE 
            || search_mode == RX_SRCH_DBLS
            || search_mode == RX_SRCH_RANG ) 
    ? RX_STK_QUERY_ALLNOD
    : RX_STK_QUERY_NOGLUE;
  
  rx_build_stack(stack, &stkcnt, tree, prefix, dmode);

  rx_nod_search( search_mode, par_a, par_b, tree, prefix, 
                 stack, stkcnt, &nodlist, 1000);
  
  LG_log(rx_context, LG_DEBUG, "RX_bin_search: processing nodes");

  for( nitem = g_list_first(nodlist);
       nitem != NULL;
       nitem = g_list_next(nitem)) {    
    
    resnum++;
    curcpy = nitem->data;
    
    /*
      if memory mode includes RAM:
      * do not expect copies of nodes in the list received from bin_search.
      * iterate through data leaves with g_list_nth_data.
      */
    
    curnode = curcpy->srcptr;
    
    /*    rx_nod_print( curnode, buf, 1024 );*/
    
    maxleaves = g_list_length(curnode->leaves_ptr);
    /*    fprintf(stderr,"###node %d, %d dataleaves attached:", i, maxleaves);*/

    /* iterate through dataleafs attached to this node*/
    for(k=0; k<maxleaves; k++) {
      rx_dataleaf_t *leafptr = g_list_nth_data(curnode->leaves_ptr, k);

      /* 
	 check the conditions to add the leaf:

	 XXX never add composed inetnum for exact prefix search
	 (but do for exact range search...) - must be solved in upper layer.

      */

 
      /* add*/
      
      datref = (rx_datref_t *)UT_calloc(1, sizeof(rx_datref_t));
      datref->leafptr = leafptr;
      /* srckey and excluded fields are initialised to 0 by calloc */
      
      *datleaves = g_list_prepend(*datleaves, datref);
    }
  }

  wr_clear_list( &nodlist );

  LG_log(rx_context, LG_DEBUG, 
            "RX_bin_search: found %d nodes", resnum);
    
  
  /* the LL of answers (*datleaves) contains pointers to answer structs, 
     that SHOULD BE NORMALIZED HERE (==with no redundant entries)
  */

return RX_OK;
}

