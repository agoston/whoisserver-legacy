/***************************************
  $Revision: 1.19 $

  Radix tree (rx).  rx_print.c - functions to print a forest/tree/node
                                 (mainly for debugging purposes)

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
#define RX_IMPL_PRINT
#include "rip.h"

/* logging */
extern LG_context_t *rx_context;

const char *
RX_text_srch_mode(rx_srch_mt mode) 
{
  return rx_srch_mode_text[mode];
}

int
rx_walk_hook_printnode(rx_node_t *node, int level, int nodecounter, void *con)
{
  sk_conn_st *condat = (sk_conn_st *) con;
  char line[200]="", buf[1024];

  int i;

  /* indent*/
  for(i=0;i<level;i++) strcat(line,"  ");

  sprintf( line+strlen(line) ,"** level %d ** ", level);
  /* @ %p; parent %p, child[0]=%p, child[1]=%p\n", */
  /*  node, node->parent_ptr, node->child_ptr[0], node->child_ptr[1] );*/

  rx_nod_print(node, buf, 1024);
  
  SK_cd_puts(condat, line);
  SK_cd_puts(condat, buf);
  SK_cd_puts(condat, "\n");
  return RX_OK;
}

/***************************************************************************/

int
rx_tree_print( sk_conn_st *condat, rx_tree_t *tree ) 
{
int cnt;
int err;
char line[200]="";

 if( tree->top_ptr != NULL ) {
   cnt = rx_walk_tree(tree->top_ptr, rx_walk_hook_printnode, 
		      RX_WALK_CNTGLU,  /* print also glue nodes*/
		      255, 0, 0, condat, &err);
   sprintf(line,"Traversed %d nodes\n", cnt);
   SK_cd_puts(condat,line);
 }
 else {
   SK_cd_puts(condat,"The tree is empty!\n");
 }

 return err;
}


/***************************************************************************/

#if 0

This function is never used, and may potentially cause a buffer overflow.
If you need it, check that %s and add it back in.   Shane

void
rx_space_printone(void *voptr, void *condat)
{
  rx_tree_t *ptr = voptr;
  char aout[1024];
  char prstr[IP_PREFSTR_MAX];
  
  *aout=0;

  sprintf(aout+strlen(aout), "%50s:%d\n", "space",    ptr->space );
  sprintf(aout+strlen(aout), "%50s:%d\n", "family",   ptr->family );
  sprintf(aout+strlen(aout), "%50s:%d\n", "subtrees", ptr->subtrees);
  sprintf(aout+strlen(aout), "%50s:%d\n", "mem_mode", ptr->mem_mode);
  sprintf(aout+strlen(aout), "%50s:%d\n", "num_nodes",ptr->num_nodes);
  sprintf(aout+strlen(aout), "%50s:%08x\n", "top_ptr", (int) ptr->top_ptr);
  sprintf(aout+strlen(aout), "%50s:%d\n", "maxbits",  ptr->maxbits);

  if( IP_pref_b2a(  &(ptr->prefix), prstr, IP_PREFSTR_MAX) != IP_OK )
    die; /* program error.*/

  sprintf(aout+strlen(aout), "%50s:%s\n", "prefix", prstr);
  SK_cd_puts( (sk_conn_st *)condat,aout);
}

#endif /* 0 */

/***************************************************************************/

void
rx_nod_print( rx_node_t *node, char *buf, unsigned maxchar )
{
  char pref[IP_PREFSTR_MAX];
  
  if( IP_pref_b2a(  &(node->prefix), pref, IP_PREFSTR_MAX) != IP_OK ) {
    die;
  }
  
  snprintf(buf, maxchar, "%s%s", 
	   ( node->glue ) ? "++glue++" : "", pref);
}
/***************************************************************************/

void 
rx_stk_print( rx_nodcpy_t   stack[],         /* stack==array of node_copies*/
	      int           stackdepth )
{
  int i;
  rx_node_t *node;
  char buf[1024];

  LG_log(rx_context, LG_DEBUG,
	    "stack dump: %d elements", stackdepth);

  for(i = 0; i < stackdepth; i++) {
    node = & stack[i].cpy;

    rx_nod_print(node, buf, 1024);

    LG_log(rx_context, LG_DEBUG, "position %d: %s", i, buf);
  }
}
