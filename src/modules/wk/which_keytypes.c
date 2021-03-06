/***************************************
  $Revision: 1.6 $

  which_keytypes:  Determine which keys to look for.

  This is based on the existing Perl code.

  Authors: ottrey, marek

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

#define  WK_IMPL
#include "rip.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <glib.h>
#include <pthread.h>
#include <regex.h>

/* structure for simple keys, with a single regular expression to match */
/* NOTE: the WK_NAME, WK_DOMAIN, and WK_HOSTNAME are not handled here   */
struct {
    int key_type;		/* identifier for key, e.g. WK_RTRSET */
    char *pattern;		/* string for regular expression */
    regex_t regex;		/* compiled regular expression */
} wk_regex_list[] = {
    { WK_NIC_HDL,       WK_REXP_NICHANDLE },
    { WK_EMAIL,         WK_REXP_EMAIL },
    { WK_MNTNER,        WK_REXP_MAINTAINER },
    { WK_KEY_CERT,      WK_REXP_KEYCERT },
    { WK_IPRANGE,       WK_REXP_IPRANGE },
    { WK_IPADDRESS,     WK_REXP_IPADDRESS },
    { WK_IPPREFIX,      WK_REXP_IPPREFIX },
    { WK_IP6PREFIX,     WK_REXP_VALIDIP6PREFIX },
    { WK_NETNAME,       WK_REXP_NETNAME },
    { WK_NET6NAME,      WK_REXP_NETNAME },
    { WK_AUTNUM,        WK_REXP_ASNUM },
    { WK_ASSETNAME,     WK_REXP_ASSETNAME },
    { WK_ROUTESETNAME,  WK_REXP_ROUTESETNAME },
    { WK_LIMERICK,      WK_REXP_LIMERICK },
    { WK_POEM,          WK_REXP_POEM },
    { WK_POETIC_FORM,   WK_REXP_POETIC_FORM },
    { WK_ASRANGE,       WK_REXP_ASRANGE },
    { WK_PEERINGSET,    WK_REXP_PEERINGSET },
    { WK_FILTERSET,     WK_REXP_FILTERSET },
    { WK_RTRSET,        WK_REXP_RTRSET },
    { WK_IRT,           WK_REXP_IRT },
    { WK_FINGERPR,      WK_REXP_FINGERPR },
    { WK_ORG_ID,        WK_REXP_ORG_ID },
    { WK_DS_RDATA,      WK_REXP_DS_RDATA },
    { WK_AUTH,          WK_REXP_AUTH },
    { WK_REVDOMAIN,     WK_REXP_REVDOMAINNAME }
};
#define WK_REGEX_LIST_LEN  (sizeof(wk_regex_list)/sizeof(wk_regex_list[0]))

/* index array to map WK_* enum values to index in wk_regex_list */
int wk_regex_index[WK_END];

/* regular expression used by isdomname() */
static regex_t domainname;
static regex_t domainalpha;

/* initialize regular expressions */
void wk_regex_init() {
    int i;
    int errcode;

    /* initialize our table */
    for (i = 0; i < WK_REGEX_LIST_LEN; i++) {
        errcode = regcomp(&wk_regex_list[i].regex, wk_regex_list[i].pattern, REG_EXTENDED | REG_NOSUB);
        dieif(errcode != 0);
        wk_regex_index[wk_regex_list[i].key_type] = i;
    }

    /* add some special cases used by our other functions */
    errcode = regcomp(&domainname, WK_REXP_DOMAINNAME, REG_EXTENDED | REG_NOSUB);
    dieif(errcode != 0);
    errcode = regcomp(&domainalpha, WK_REXP_DOMAINALPHA, REG_EXTENDED | REG_NOSUB);
    dieif(errcode != 0);
}


/* check for domain name */
static unsigned int wk_is_domain(char *key) {
    /* if it matches the general domain name search, and contains an */
    /* alphabetic character, consider it a possible domain name */
    if (regexec(&domainname, key, 0, NULL, 0) == 0) {
        if (regexec(&domainalpha, key, 0, NULL, 0) == 0) {
            return 1;
        }
    }
    return 0;
}

/* Convert the which keytypes bitmap into a string */
char *WK_to_string(mask_t wk) {
    return MA_to_string(wk, Keytypes);
}

/* check if key looks like an aut-num primary key (or, matches aut-num regexp) */
int WK_is_aut_num(char *key) {
    if (regexec(&wk_regex_list[wk_regex_index[WK_AUTNUM]].regex, (const char *) key, 0, NULL, 0) == 0) {
        return 1;
    }
    return 0;
}

/*
  Create a new which keytypes bitmap.

  This checks the string to see which keys it looks like.  This helps
  us decide what SQL tables (or radix trees) we need to query for a
  match.

  char *key The key to be examined.
*/
mask_t WK_new(char *key) {
    mask_t wk;
    int i;

    /* empty bitmask */
    wk = MA_new(MA_END);

    /* search regular expressions in the list */
    for (i = 0; i < WK_REGEX_LIST_LEN; i++) {
        if (regexec(&wk_regex_list[i].regex, key, 0, NULL, 0) == 0) {
            MA_set(&wk, wk_regex_list[i].key_type, 1);
        }
    }

    /* check our more complicated key patterns */
    MA_set(&wk, WK_NAME, !MA_isset(wk, WK_IPADDRESS) && !MA_isset(wk, WK_IPPREFIX) && !MA_isset(wk, WK_IP6PREFIX));
    MA_set(&wk, WK_DOMAIN, wk_is_domain(key));
    MA_set(&wk, WK_HOSTNAME, MA_isset(wk, WK_DOMAIN) || MA_isset(wk, WK_IPADDRESS));

    /* return resulting bitmask */
    return wk;
}
