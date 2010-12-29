/***************************************
 $Revision: 1.1 $

 Radix tree (rx).  rx_payload.c - functions to operate on user data
 (creation/deletion).

 Status: NOT REVUED, UNTESTED, INCOMPLETE

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

#include "rip.h"

/* logging */
extern LG_context_t *rp_context;

/*
 assumes locked tree

 mode       MODE={cre|mod|del}
 data       pointer to the payload
 datalen    length of the payload
 key        the key to the SQL full-text
 */
int RP_uni_node_l(rx_oper_mt mode, rp_uni_t *uni, rx_tree_t *mytree, void *data, unsigned datalen, sql_key_t key) {
    rx_dataleaf_t *leafptr = calloc(sizeof(rx_dataleaf_t), 1);
    int err;

    leafptr->data_key = key; /* also for deletion - it is used for finding the right dataleaf in the tree */

    if (mode == RX_OPER_CRE) {
        leafptr->data_ptr = UT_malloc(datalen);
        memcpy(leafptr->data_ptr, data, datalen);
        leafptr->data_len = datalen;
    }

    switch (mytree->family) {
    case RX_FAM_IN:
        err = RX_in_node(mode, &(uni->u.in), mytree, leafptr);
        break;
    case RX_FAM_RT:
        err = RX_rt_node(mode, &(uni->u.rt), mytree, leafptr);
        break;
    default:
        die;
    }

    return err;
}

/*+ ***************************************************************

 MAIN FRONTEND FUNCTION


 finds tree, locks it.

 builds a dataleaf and puts into the node(s),
 calling rx_bin_node for every prefix.

 MT-note: locks/unlocks the tree.
 
 +++++++++++++++++*/
int RP_uni_node(rx_oper_mt mode, /*+ MODE={cre|mod|del} +*/
rp_uni_t *uni, /*+ unified argument structure +*/
rp_attr_t attr, /*+ additional tree attribute +*/
rp_regid_t reg_id, /*+ id of the registry +*/
void *data, /*+ pointer to the payload +*/
unsigned datalen, /*+ length of the payload +*/
sql_key_t key /*+ the key to the SQL full-text +*/
) {

    /*
     For creation of a new node:

     READ-LOCK THE FOREST 

     get the root tree for this space (rx_get_tree)
     got it ? good. No ? error!!!

     Check if any of the prefixes spans more than one subtree...
     Check if they all exist already..

     abort if any is missing

     UNLOCK THE FOREST

     **now start writing the data:**

     put *data* records in memory and sql table

     for all matchind [sub]trees (in order of the list)
     WRITE-LOCK the in-memory [sub]tree
     WRITE-LOCK the sql-table for it

     for(all prefixes in memory that match this tree)
     create a node in the tree pointing to the data
     rof
     UNLOCK the tree
     rof


     */

    rx_tree_t *mytree;
    ip_space_t space = uni->space;
    int err;

    dieif( RP_tree_get ( &mytree, reg_id, space, attr ) != RP_OK );

    /* LOCK */
    TH_acquire_write_lockw(&(mytree->rwlock));

    err = RP_uni_node_l(mode, uni, mytree, data, datalen, key);

    /* UNLOCK */
    TH_release_write_lockw(&(mytree->rwlock));

    return err;
}

/*+ *********************************************************************
 Ascii frontend - translates key according to attr and calls rx_uni_node.
 FOR TESTING PURPOSES ONLY
 ********************************************************************* +*/
int RP_asc_node(rx_oper_mt mode, /*+ MODE={cre|mod|del} +*/
char *rangstr, /*+ string prefix/range/IP/inaddr +*/
rp_attr_t attr, /*+ additional tree attribute +*/
rp_regid_t reg_id, /*+ id of the registry +*/
void *data, /*+ pointer to the payload +*/
unsigned datalen, /*+ length of the payload +*/
sql_key_t key /*+ the key to the SQL full-text +*/
) {
    rp_uni_t uni;

    if (!NOERR(RP_asc2uni(rangstr, attr, &uni))) {
        LG_log(rp_context, LG_DEBUG, "can't understand the key, discarding the OBJECT.");
        return RP_BADKEY;
    } else {
        LG_log(rp_context, LG_DEBUG, "rx_asc_node: inserting object %s for attr %d reg %d \twith %d bytes at %x", rangstr, attr, reg_id, datalen, data);
        return RP_uni_node(mode, &uni, attr, reg_id, data, datalen, key);
    }
}

int RP_pack_node(rx_oper_mt mode, rp_upd_pack_t *pack, rp_regid_t reg_id) {
    char rx_text[STR_L];
    unsigned len = 0;/* doesn't matter for deletion */
    rp_uni_t *uniptr;

    if (mode != RX_OPER_DEL) {
        rp_make_short(pack, rx_text, &len);
    }

    uniptr = &(pack->uni);
    return RP_uni_node(mode, uniptr, pack->type, reg_id, rx_text, len+1, pack->key);
}

int RP_pack_node_l(rx_oper_mt mode, rp_upd_pack_t *pack, rx_tree_t *mytree) {
    char rx_text[STR_L];
    unsigned len = 0;/* doesn't matter for deletion */
    rp_uni_t *uniptr;

    if (mode != RX_OPER_DEL) {
        rp_make_short(pack, rx_text, &len);
    }

    uniptr = &(pack->uni);
    return RP_uni_node_l(mode, uniptr, mytree, rx_text, len+1, pack->key);
}
