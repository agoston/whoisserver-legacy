/***************************************
 $Revision: 1.19 $

 Radix tree (rx).  rx_print.c - functions to print a forest/tree/node
 (mainly for debugging purposes)

 Status: NOT REVUED, TESTED, INCOMPLETE

 Design and implementation by: Marek Bukowy

 ******************//******************
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
RX_text_srch_mode(rx_srch_mt mode) {
    return rx_srch_mode_text[mode];
}

/* walk_tree hook function that prints each node to stderr. To be used with DEBUG_RADIX */
int rx_walk_hook_printnode(rx_node_t *node, int level, int nodecounter, void *con) {
    char buf[256];
    int i;

    /* indent*/
    for (i = 0; i < level; i++)
        fprintf(stderr, "    ");

    rx_nod_print(node, buf, 256);
    fprintf(stderr, "[%s], nodecounter=%d, level=%d\n", buf, nodecounter, level);

    return RX_OK;
}

/***************************************************************************/

/* prints tree to stderr, for use with DEBUG_RADIX */
int rx_tree_print_stderr(rx_tree_t *tree) {
    int cnt;
    int err;

    if (tree->top_ptr) {
        cnt = rx_walk_tree(tree->top_ptr, rx_walk_hook_printnode, RX_WALK_CNTGLU, 255, 0, 0, (void*)-1, &err);
        fprintf(stderr, "Traversed %d nodes\n", cnt);
    } else {
        fprintf(stderr, "The tree is empty!\n");
    }

    return err;
}

/***************************************************************************/

void rx_nod_print(rx_node_t *node, char *buf, unsigned maxchar) {
    char pref[IP_PREFSTR_MAX];

    if (IP_pref_b2a(&(node->prefix), pref, IP_PREFSTR_MAX) != IP_OK) {
        die;
    }

    snprintf(buf, maxchar, "%s%s", (node->glue) ? "++glue++" : "", pref);
}


/***************************************************************************/
/* stack==array of node_copies*/
void rx_stk_print(rx_nodcpy_t stack[], int stackdepth) {
    int i;
    rx_node_t *node;
    char buf[1024];

    LG_log(rx_context, LG_DEBUG, "stack dump: %d elements", stackdepth);

    for (i = 0; i < stackdepth; i++) {
        node = &stack[i].cpy;

        rx_nod_print(node, buf, 1024);

        LG_log(rx_context, LG_DEBUG, "position %d: %s", i, buf);
    }
}
