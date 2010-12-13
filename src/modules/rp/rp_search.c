/***************************************
 $Revision: 1.1 $

 Radix payload (rp) - user level functions for storing data in radix trees

 rp_search = search the loaded radix trees using an ascii key

 Motto: "And all that for inetnums..."

 Status: NOT REVIEWED, TESTED

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

static
void rp_exclude_datlink(GList **datlist, GList *element) {
    /* remove element from list(becomes a self-consistent list) */
    *datlist = g_list_remove_link(*datlist, element);

    /* free it and the payload */
    wr_clear_list(&element);
}

/**************************************************************************/
/*+++++++++++
 helper:
 this routine goes through the list of prefixes and performs a bin_search
 on each of them; attaches the results to datlist.
 +++++++++++*/
static
int rp_preflist_search(rx_srch_mt search_mode, int par_a, int par_b, rx_tree_t *mytree, GList **preflist, GList **datlist)

{
    char prefstr[IP_PREFSTR_MAX];
    GList *qitem;
    ip_prefix_t *querypref;
    int err;

    for (qitem = g_list_first(*preflist); qitem != NULL; qitem = g_list_next(qitem)) {

        querypref = qitem->data;

        if (IP_pref_b2a(querypref, prefstr, IP_PREFSTR_MAX) != IP_OK) {
            die;
        }
        LG_log(rp_context, LG_DEBUG, "rx_preflist_search: mode %d (%s) (par %d) for %s", search_mode, RX_text_srch_mode(search_mode), par_a, prefstr);

        if (mytree->num_nodes > 0) {
            err = RX_bin_search(search_mode, par_a, par_b, mytree, querypref, datlist, RX_ANS_ALL);
            if (err != RX_OK) {
                return err;
            }
        }
    }

    return RX_OK;
}

/*++++
 this is a helper: goes through a datlist and returns the smallest
 size of a range

 works for IPv4 only
 +++*/
static ip_rangesize_t rp_find_smallest_span(GList *datlist) {
    ip_rangesize_t min_span, span;
    GList *ditem;

    min_span = 0xffffffff; /* IPv4 only!!!!*/

    /* go through the list and find the shortest range.    */
    for (ditem = g_list_first(datlist); ditem != NULL; ditem = g_list_next(ditem)) {
        rx_datref_t *refptr = (rx_datref_t *) (ditem->data);

        span = IP_rang_span(&refptr->leafptr->iprange);

        if (span < min_span) {
            min_span = span;
        }
    }
    LG_log(rp_context, LG_DEBUG, "rp_find_smallest_span: minimal span is %d", min_span);

    return min_span;
}

/* helper for the inetnum/exless search - for this one a hash of pairs
 (leafptr,occurences) must be maintained.

 This routine increments the counter for a leafptr, creating a new
 pair if this leafptr was not referenced before.

 */
static
int rp_leaf_occ_inc(GHashTable *hash, rx_dataleaf_t *leafptr) {
    /* one little trick: store the number of occurences
     as cast (void *) */
    long val;

    val = (long) g_hash_table_lookup(hash, leafptr);
    /* 0 if it's not known yet. anyway: put it in the hash (value==key) */

    g_hash_table_insert(hash, leafptr, (void *) ++val);

    return (int)val;
}

/* exclude exact match - not to be merged with preselction :-( */
static void rp_exclude_exact_match(GList **datlist, ip_range_t *testrang) {
    GList *ditem, *newitem;

    ditem = g_list_first(*datlist);

    while (ditem != NULL) {
        rx_datref_t *refptr = (rx_datref_t *) (ditem->data);

        newitem = g_list_next(ditem);

        if (memcmp(&refptr->leafptr->iprange, testrang, sizeof(ip_range_t)) == 0) {
            rp_exclude_datlink(datlist, ditem);
            LG_log(rp_context, LG_DEBUG, "process_datlist: discarded an exact match");
        }
        ditem = newitem;
    } /* while */
}

static int rp_find_longest_prefix(GList **datlist) {
    GList *ditem;
    int max_pref = 0;

    for (ditem = g_list_first(*datlist); ditem != NULL; ditem = g_list_next(ditem)) {
        rx_datref_t *refptr = (rx_datref_t *) (ditem->data);

        if (refptr->leafptr->preflen > max_pref) {
            max_pref = refptr->leafptr->preflen;
        }
    }

    return max_pref;
}

/*+ rp_asc_process_datlist() - helper for RP_asc_search()

 fetches the copies of objects from the radix tree into datlist

 ASSUMES LOCKED TREE

 the behaviour for a default inetnum (range) query is:
 do an exact match;
 if it fails, do an exless match on the encompassing prefix
 for routes(prefixes):
 do an exless match

 So if it's the default search mode on an inetnum tree,
 and the key is a range,
 then an exact search is performed on one of the composing prefixes.

 Then the resulting data leaves are checked for exact matching with
 the range queried for.
 Any dataleaves that do not match are discarded, and if none are left,
 the procedure falls back to searching for the encompassing prefix.
 (calculated in the smart_conv routine).
 Add the dataleaf copies to the list of answers,
 taking span into account
 +*/
static
int rp_asc_process_datlist(rx_srch_mt search_mode, int par_a, rx_fam_t fam_id, int prefnumber, GList **datlist, ip_range_t *testrang, int *hits) {
    ip_rangesize_t min_span = 0, span;
    int use_span = 0;
    int max_pref = -1;
    GList *ditem, *newitem;
    GHashTable *lohash = g_hash_table_new(NULL, NULL);

    /* in MORE and LESS(1) search exact match must not be displayed */
    if (search_mode == RX_SRCH_MORE || (search_mode == RX_SRCH_LESS && par_a == 1)) {
        rp_exclude_exact_match(datlist, testrang);
    }

    /* Preselection moved to processing, only span calculation done here *
     *

     EXLESS and LESS(1) search: the smallest span must be found,
     but if the less spec node is not the same for all composing prefixes,
     it means it's not really this one.

     we check that by the number of references to this node is less than
     the number of composing prefixes

     We do the same for the less specific search - a node must be less
     specific to all prefixes.

     if the number of references is  not enough, then return no hits,
     another try will be made, this time with one, encompassing prefix.
     */

    if ((search_mode == RX_SRCH_EXLESS) || (search_mode == RX_SRCH_LESS && par_a == 1)) {
        /* span works only for IP_V4. We use it only for inetnums,
         although RT/v4 would work too */
        if (testrang->begin.space == IP_V4 && fam_id == RX_FAM_IN) {
            min_span = rp_find_smallest_span(*datlist);
            use_span = 1;
        } else {
            /* in IPv6 and RT trees in general,  we can obtain the same
             result by selecting the longest prefix */
            max_pref = rp_find_longest_prefix(datlist);
        }
    }

    /* Process the dataleaf copies and add to the list of answers. */
    ditem = g_list_first(*datlist);
    while (ditem != NULL) {
        rx_datref_t *refptr = (rx_datref_t *) (ditem->data);
        int exclude = 0;

        if (search_mode == RX_SRCH_EXLESS || search_mode == RX_SRCH_LESS) {

            /* min_span defined <=> EXLESS or LESS(1) search of INETNUMS:
             the smallest span must be returned */
            if (!exclude && use_span && (span = IP_rang_span(&refptr->leafptr->iprange)) != min_span) {
                LG_log(rp_context, LG_DEBUG, "process_datlist: (EX)LESS: discarded object with span %d", span);
                exclude = 1;
            }
            /* max_pref defined <=> EXLESS search of INETNUMS or LESS(1) of RT:
             */
            if (!exclude && max_pref >= 0 && refptr->leafptr->preflen < max_pref) {
                LG_log(rp_context, LG_DEBUG, "process_datlist: (EX)LESS: discarded object with preflen %d", refptr->leafptr->preflen);
                exclude = 1;
            }

            /* number of occurences */
            /* XXX this will go when the old algorithm goes */
            if (!exclude && prefnumber > 1) { /* do not check if all will be approved */

                if (rp_leaf_occ_inc(lohash, refptr->leafptr) < prefnumber) {
                    LG_log(rp_context, LG_DEBUG, "process_datlist: (EX)LESS: leafptr %x not enough", refptr->leafptr);
                    exclude = 1;
                } else {
                    LG_log(rp_context, LG_DEBUG, "process_datlist: (EX)LESS: leafptr %x GOOD enough", refptr->leafptr);
                }
            }
        } else if (search_mode == RX_SRCH_EXACT) {
            /* EXACT search - discard if the range does not match */
            if (memcmp(&refptr->leafptr->iprange, testrang, sizeof(ip_range_t)) != 0) {

                LG_log(rp_context, LG_DEBUG, "process_datlist: EXACT; discarded a mismatch");
                exclude = 1;
            } /*  EXACT match */
        } else if (search_mode == RX_SRCH_MORE) {
            /* MORE: exclude if not fully contained in the search term */
            if (!(IP_addr_in_rang(&refptr->leafptr->iprange.begin, testrang) && IP_addr_in_rang(&refptr->leafptr->iprange.end, testrang))) {
                LG_log(rp_context, LG_DEBUG, "process_datlist: MORE; discarded a not-fully contained one");
                exclude = 1;
            }
        }

        /* get next item now, before the current gets deleted */
        newitem = g_list_next(ditem);
        if (exclude) {
            /* get rid of it */
            rp_exclude_datlink(datlist, ditem);
        } else {
            /* OK, so we ACCEPT these results*/
            /* uniqueness ensured in copy_results */
            (*hits)++;
        }
        ditem = newitem;
    } /* while ditem */

    /* wr_clear_list(&lolist); */
    g_hash_table_destroy(lohash);
    return RX_OK;
}

/**************************************************************************/

/*+ appends the element pointed to by datref to finallist +*/
static
int rp_asc_append_datref(rx_datref_t *refptr, GList **finallist) {
    rx_datcpy_t *datcpy;
    void *dataptr;

    /* OK, so we ACCEPT this result. Copy it.*/

    datcpy = (rx_datcpy_t *) UT_calloc(1, sizeof(rx_datcpy_t));

    datcpy->leafcpy = *(refptr->leafptr);

    /* copy the immediate data too. Set the ptr.*/

    dataptr = UT_calloc(1, refptr->leafptr->data_len);
    memcpy(dataptr, refptr->leafptr->data_ptr, refptr->leafptr->data_len);

    datcpy->leafcpy.data_ptr = dataptr;

    *finallist = g_list_prepend(*finallist, datcpy);

    /* XXX this wouldn't work in access_control */
    LG_log(rp_context, LG_DEBUG, "rp_asc_append 'ed: %s", dataptr);

    return RX_OK;
}

/*+ goes through datlist (list of references "datref") and add copies of 
 leaves referenced to the finallist

 maintains its own uniqhash which holds pointers to copied dataleaves.

 modifies: finallist

 returns: error from wr_malloc

 +*/
static
int rp_srch_copyresults(GList * datlist, GList ** finallist, int maxcount) {
    int err;
    GList *ditem;
    GHashTable *uniqhash = g_hash_table_new(NULL, NULL); /* defaults */
    int count = 0;

    LG_log(rp_context, LG_DEBUG, "srch_copyresults");

    /*  copy dataleaves pointed to by entries from the datlist
     only once (check uniqueness in the hash table) */
    for (ditem = g_list_first(datlist); ditem != NULL; ditem = g_list_next(ditem)) {
        rx_datref_t *refptr = (rx_datref_t *) (ditem->data);
        rx_dataleaf_t *ansptr = refptr->leafptr;

        /* search for every ansptr (dataleaf pointer) in uniqhash */
        if (g_hash_table_lookup(uniqhash, ansptr) == NULL) {

            /* it's not known yet. OK: put it in the hash (value==key) */
            g_hash_table_insert(uniqhash, ansptr, ansptr);

            /* and copy the dataleaf */
            if (!NOERR(err = rp_asc_append_datref(refptr, finallist))) {
                return err;
            }
        }

        /* check the limit on number of objects if defined ( >0)  */
        count++;
        if (maxcount > 0 && count > maxcount) {
            break;
        }

    } /*  foreach (datlist) */

    g_hash_table_destroy(uniqhash); /* elements are still linked to through datlist */

    return RP_OK;
}

static
void rp_begend_preselection(GList **datlist, rx_fam_t fam_id, ip_range_t *testrang) {
    GList *ditem, *newitem;

    ditem = g_list_first(*datlist);

    while (ditem != NULL) {
        rx_datref_t *refptr = (rx_datref_t *) (ditem->data);
        newitem = g_list_next(ditem);

        /* the test is indentical for route & inetnum trees */
        if (IP_addr_in_rang(&testrang->end, &refptr->leafptr->iprange) == 0) {

            LG_log(rp_context, LG_DEBUG, "process_datlist: discarded an uncovering leafptr %x", refptr->leafptr);
            rp_exclude_datlink(datlist, ditem);
        }
        ditem = newitem;
    } /* while */
}

/*+++++++++++++++
 search.

 2 approaches:

 1. (most modes): look up all less specifics of beginning and end of range,
 compare/select/etc.

 2. More spec mode: break up the query range into prefixes, [erform a search
 for each of them. Add all results together.

 translates a query into a binary prefix (or prefixes, if range).
 for registry+space (or if they are zero, for all
 registries/spaces)
 finds tree
 calls RX_bin_search (returning node copies).
 will not put duplicate entries (composed inetnums).
 returns some sort of error code :-)

 Cuts the number of answers from RX_bin_search down to max_count,
 but since some of the answers may have been "normalized" in the
 underlying functions (multiple occurences removed),
 the result is _at_most_ max_count.

 appends to a given list of data blocks (not nodes!)

 The EXLESS search on inetnum tree should return the shortest range
 that was found, by means of comparing span (size) of the range.
 If there are more of size equal to the smallest one, they are also
 returned.

 returns RX_OK or a code from an underlying function
 ++++++++++++*/
int RP_asc_search(rx_srch_mt search_mode, int par_a, int par_b, char *key, /*+ search term: (string) prefix/range/IP + */
rp_regid_t reg_id, rp_attr_t attr, /*+ extra tree id (within the same reg/spc/fam + */
GList ** finallist, /*+ answers go here, please + */
int max_count /*+ max # of answers. RX_ALLANS == unlimited + */
) {
    GList *preflist = NULL;
    GList *datlist = NULL;
    int err;
    ip_range_t testrang;
    int locked = 0;
    ip_space_t spc_id;
    rx_fam_t fam_id = RP_attr2fam(attr);
    rx_tree_t *mytree;
    int hits = 0;
    ip_prefix_t beginpref;

#ifdef DEBUG_QUERY
    fprintf(stderr, "RP_asc_search:  query %s : mode %d (%s) (par %d) for %s\n", DF_get_attribute_name(attr), search_mode, RX_text_srch_mode(search_mode), par_a, key);
#endif

    /* parse the key into a prefix list */
    if (!NOERR(err = IP_smart_conv(key, 0, 0, &preflist, IP_EXPN, NULL))) {
        /* operational trouble (UT_*) or invalid key (IP_INVARG) */
        return err;
    }

    /* set the test values */
    IP_smart_range(key, &testrang, IP_EXPN, NULL);

    /* find the tree */
    /* I took out the surrounding "if" because it is always taken when
     we get to this point, and it causes compiler warnings otherwise - shane */
    /*if( NOERR(err) ) { */
    spc_id = IP_pref_b2_space(g_list_first(preflist)->data);
    if (!NOERR(err = RP_tree_get(&mytree, reg_id, spc_id, attr))) {
        wr_clear_list(&preflist);
        return err;
    }
    /*} */
    /* the point of no return: now we lock the tree. From here, even if errors
     occur, we still go through all procedure to unlock the tree at the end */

    /* lock the tree */
    TH_acquire_read_lockw(&(mytree->rwlock));
    locked = 1;

    /* Collection: this procedure is used for some search_modes only */
    if (search_mode == RX_SRCH_EXLESS || search_mode == RX_SRCH_LESS || search_mode == RX_SRCH_EXACT) {

        /* 1. compose a /32(/128) prefix for beginning of range */
        beginpref.ip = testrang.begin;
        beginpref.bits = IP_sizebits(spc_id);

        /* 2. dataleaves collection: look up the beginning prefix in LESS(255) mode */
        if (NOERR(err)) {
            err = RX_bin_search(RX_SRCH_LESS, 255, 0, mytree, &beginpref, &datlist, RX_ANS_ALL);
        }

        /* 3. preselection: exclude those that do not include end of range
         */
        if (NOERR(err)) {
            rp_begend_preselection(&datlist, fam_id, &testrang);
        }

    } /* if exless|less|exact */
    else {
        /* MORE */

        /* standard collection using the traditional method:
         repeat the search for all prefixes and join results */

        if (NOERR(err)) {
            err = rp_preflist_search(search_mode, par_a, par_b, mytree, &preflist, &datlist);
        }
    } /* collection */

    LG_log(rp_context, LG_DEBUG, "RP_NEW_asc_search: collected %d references ", g_list_length(datlist));

    /* 5. processing - using the same processing function */
    if (NOERR(err)) {
        err = rp_asc_process_datlist(search_mode, par_a, fam_id, 1, /* one occurence is enough */
        &datlist, &testrang, &hits);
    }

    /* 6. copy results */
    if (NOERR(err)) {
        err = rp_srch_copyresults(datlist, finallist, max_count); /* and uniq */
    }

    if (locked) {
        /* 100. unlock the tree */
        TH_release_read_lockw(&(mytree->rwlock));
    }

    /* clean up */
    wr_clear_list(&preflist);
    wr_clear_list(&datlist);

    /* NOTE if error occured, finallist may be partly filled in. */
    return err;
}
