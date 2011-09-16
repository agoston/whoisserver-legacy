/***************************************
 $Revision: 1.1 $

 Radix payload (rp) - user level functions for storing data in radix trees

 rp_convert = conversion helpers for RX_asc_node and UD module.

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

/* return the family of tree to be used for the given attribute.
 die if not defined.
 */
rx_fam_t RP_attr2fam(rp_attr_t type, ip_space_t space) {
    rx_fam_t res = DF_attrcode_radix_family(type, space);

    dieif(res == -1);

    return res;
}

/* 
 returns 1 if the given space may appear for a given attribute
 */
int RP_attr2spc(rp_attr_t type, ip_space_t space) {
    char *loadv4 = DF_attrcode_radix_load_v4(type);
    char *loadv6 = DF_attrcode_radix_load_v6(type);
    char *loadqry = (space == IP_V4) ? loadv4 : loadv6;

    return (loadqry != NULL); /* 1 if defined, 0 otherwise */
}

/* const char *astr                     string prefix/range/IP/inaddr
   rp_attr_t attr, rp_uni_t *uni        destination pointer
*/
int RP_asc2uni(const char *astr, rp_attr_t attr, rp_uni_t *uni) {
    int conv;
    switch (attr) {
    case A_IN:
        conv = IP_rang_e2b(&(uni->u.in), astr);
        uni->space = uni->u.in.begin.space;
        break;
    case A_RT:
    case A_I6:
    case A_R6:
        conv = IP_pref_e2b(&(uni->u.rt), astr);
        uni->space = uni->u.rt.ip.space;
        break;
    case A_DN: {
        ip_revd_t revd;
        if ((conv = IP_revd_t2b(&revd, astr)) == IP_OK) {
            uni->space = revd.space;
            if (revd.space == IP_V4) {
                uni->u.in = revd.rang;
            } else if (revd.space == IP_V6) {
                uni->u.rt = revd.pref;
            } else die;
        }
        } // case scope
        break;
    default:
        conv = IP_INVARG;
    }
    return conv;
}

/* Function to fill data for radix tree */
/* returns error if string is not valid, also for reverse domains */
int RP_asc2pack(rp_upd_pack_t *pack, rp_attr_t type, const char *string) {
    int err;

    pack->type = type;

//    LG_log(rp_context, LG_DEBUG, "RP_asc2pack: converted attr %s: %s to pack at %08x", DF_get_attribute_code(type), string, pack);

    err = RP_asc2uni(string, type, &(pack->uni));

    if (type == A_DN && err == IP_OK) {
        /* Check if it is an in-addr.arpa domain, set domain ptr only then */
        pack->d.domain = (char*) string;
    }

    return err;
}

/* construct -K contents
 * Assumes char *ptr is preallocated with size STR_L */
void rp_make_short(rp_upd_pack_t *pack, char *ptr, unsigned *len) {
    char prefstr[IP_PREFSTR_MAX];
    char rangstr[IP_RANGSTR_MAX];

    switch (pack->type) {
    case A_R6:
        dieif( IP_pref_b2a( &(pack->uni.u.rt), prefstr, IP_PREFSTR_MAX) != IP_OK )
        *len = snprintf(ptr, STR_L, "route6:   \t%s\norigin:   \t%s\n", prefstr, pack->d.origin);
        break;

    case A_RT:
        dieif( IP_pref_b2a( &(pack->uni.u.rt), prefstr, IP_PREFSTR_MAX) != IP_OK )
        *len = snprintf(ptr, STR_L, "route:    \t%s\norigin:   \t%s\n", prefstr, pack->d.origin);
        break;

    case A_I6:
        dieif( IP_pref_b2a( &(pack->uni.u.rt), prefstr, IP_PREFSTR_MAX) != IP_OK )
        *len = snprintf(ptr, STR_L, "inet6num: \t%s\n", prefstr);
        break;

    case A_IN:
        dieif( IP_rang_b2a( &(pack->uni.u.in), rangstr, IP_RANGSTR_MAX) != IP_OK )
        *len = snprintf(ptr, STR_L, "inetnum:  \t%s\n", rangstr);
        break;

    case A_DN:
        *len = snprintf(ptr, STR_L, "domain:   \t%s\n", pack->d.domain );
        break;

    default:
        break;
    }
}

/***************** set the values in rx_*_data thingies ***************/
int RP_pack_set_orig(rp_attr_t attr, rp_upd_pack_t *pack, const char *origin) {
    pack->d.origin = (char*) origin;
    return (IP_OK);
    /* ignore attr */
}

/* those are just interfacing to 
 * functions to convert to IP binary format and retain raw values 
 */
int RP_pack_set_type(rp_attr_t attr, rp_upd_pack_t *pack, ip_space_t space) {
    pack->type = attr;
    pack->uni.fam = RP_attr2fam(attr, space);
    return (IP_OK);
}

int RP_pack_set_pref4(rp_attr_t attr, const char *avalue, rp_upd_pack_t *pack, unsigned *prefix, unsigned *prefix_length) {
    int ret;
    if (NOERR(ret = IP_pref_a2v4(avalue, &(pack->uni.u.rt), prefix, prefix_length))) {
        pack->uni.space = IP_V4;
        RP_pack_set_type(attr, pack, IP_V4);
    }
    return (ret);
}

int RP_pack_set_revd(rp_attr_t attr, const char *avalue, rp_upd_pack_t *pack) {
    ip_revd_t revd;
    int retval = IP_OK;

    if ((retval = IP_revd_t2b(&revd, avalue)) == IP_OK) {
        pack->uni.space = revd.space;
        if (revd.space == IP_V4) {
            pack->uni.u.in = revd.rang;
        } else if (revd.space == IP_V6) {
            pack->uni.u.rt = revd.pref;
        } else die;
    } else die;

    pack->d.domain = (char*) avalue;
    RP_pack_set_type(attr, pack, pack->uni.space);
    return (IP_OK);
}

int RP_pack_set_pref6(rp_attr_t attr, const char *avalue, rp_upd_pack_t *pack, ip_v6word_t *high, ip_v6word_t *low, unsigned *prefix_length) {
    int ret;
    if (NOERR(ret = IP_pref_a2v6(avalue, &(pack->uni.u.rt), high, low, prefix_length))) {
        pack->uni.space = IP_V6;
        RP_pack_set_type(attr, pack, IP_V6);
    }
    return (ret);
}

int RP_pack_set_rang(rp_attr_t attr, const char *avalue, rp_upd_pack_t *pack, unsigned *begin_in, unsigned *end_in) {
    int ret;
    if (NOERR(ret = IP_rang_a2v4(avalue, (ip_range_t *) &(pack->uni.u.in), begin_in, end_in))) {
        pack->uni.space = IP_V4;
        RP_pack_set_type(attr, pack, IP_V4);
    }
    return (ret);
}
