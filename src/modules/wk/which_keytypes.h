#ifndef READ_WHICH_KEYTYPES
#define READ_WHICH_KEYTYPES

/***************************************
  $Revision: 1.5 $

  Which Keytypes module (wk)

  Status: NOT REVUED, NOT TESTED

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
#include "bitmask.h"

/*******************************************
---> If any of the regexes below change - check whois.cgi (and other tools), which could have a copy
---> Keys are converted to uppercase before regexp matching!
*******************************************/

/* covers domain name as defined in RFC1034 - forward AND reverse */
#define WK_REXP_DOMAINNAME "^[a-zA-Z0-9/-]*(\\.[a-zA-Z0-9-]+)*\\.?$"
/* add a constraint: there must be at least one character in the domain name
   because the TLD must not be composed of digits only */
#define WK_REXP_DOMAINALPHA  "[a-zA-Z]"

/* Covers reverse domain names that are convertable to an IP range that we can stuff in a radix tree.
 * In other words, it HAS TO be parsable by IP_revd_t2b()
 *
 * Note: we skipped classless in-addr.arpa now, as it seems to be impossible to determine their size, making it impossible to stuff in a radix tree */
#define WK_REXP_REVDOMAINNAME "^[0-9A-F]*(\\.[0-9A-F]+)*\\.(IN-ADDR|IP6)\\.ARPA\\.?$"


#define WK_REXP_VALIDIP6PREFIX "^[0-9A-F:]*:[0-9A-F:/]*$"     /* at least one colon */
/* "^[0-9A-F]{1,4}(:[0-9A-F]{1,4}){7}$"*/

/* AS numbers, prepared for 32-bit AS numbers */
#define WK_REXP_ASNUM "^AS([0-9]|[1-9][0-9]{1,8}|[1-3][0-9]{9}|4[0-1][0-9]{8}|42[0-8][0-9]{7}|429[0-3][0-9]{6}|4294[0-8][0-9]{5}|42949[0-5][0-9]{4}|429496[0-6][0-9]{3}|4294967[0-1][0-9]{2}|42949672[0-8][0-9]|429496729[0-5])$"

/* AS numbers, prepared for 32-bit AS numbers */
#define WK_REXP_ASRANGE "^AS([0-9]|[1-9][0-9]{1,8}|[1-3][0-9]{9}|4[0-1][0-9]{8}|42[0-8][0-9]{7}|429[0-3][0-9]{6}|4294[0-8][0-9]{5}|42949[0-5][0-9]{4}|429496[0-6][0-9]{3}|4294967[0-1][0-9]{2}|42949672[0-8][0-9]|429496729[0-5])[ ]*([-][ ]*AS([0-9]|[1-9][0-9]{1,8}|[1-3][0-9]{9}|4[0-1][0-9]{8}|42[0-8][0-9]{7}|429[0-3][0-9]{6}|4294[0-8][0-9]{5}|42949[0-5][0-9]{4}|429496[0-6][0-9]{3}|4294967[0-1][0-9]{2}|42949672[0-8][0-9]|429496729[0-5])){0,1}$"

#define WK_REXP_NETNAME "^[A-Z][A-Z0-9_-]*$"

#define WK_REXP_MAINTAINER "^[A-Z][A-Z0-9_-]*$"

#define WK_REXP_LIMERICK "^LIM-[A-Z0-9_-]+$"

#define WK_REXP_POEM "^POEM-[A-Z0-9][A-Z0-9_-]*$"

#define WK_REXP_POETIC_FORM "^FORM-[A-Z0-9][A-Z0-9_-]*$"

#define WK_REXP_KEYCERT "^(PGPKEY-[0-9A-F]{8})|(X509-[0-9]+)$"

/* made less restrictive to make consistent with other sets ... shane */
/* made to match what we're actually looking for - shane */
/*#define WK_REXP_ROUTESETNAME "^RS-[A-Z0-9_:-]*$"*/
#define WK_REXP_ROUTESETNAME "(^|:)RS-[A-Z0-9_-]*[A-Z0-9](:|$)"

/* made less restrictive to make consistent with other sets ... shane */
/* made to match what we're actually looking for - shane */
/*#define WK_REXP_ASSETNAME "^AS-[A-Z0-9_:-]*$"*/
#define WK_REXP_ASSETNAME "(^|:)AS-[A-Z0-9_-]*[A-Z0-9](:|$)"

/* #define WK_REXP_AUTONICPREFIXREGULAR "^AUTO-" */

#define WK_REXP_IPRANGE "^[0-9]{1,3}(\\.[0-9]{1,3}){0,3}[ ]*-[ ]*[0-9]{1,3}(\\.[0-9]{1,3}){0,3}$"

#define WK_REXP_IPADDRESS "^[0-9.]+$"

#define WK_REXP_IPPREFIX "^[0-9.]+/[0-9]+$"

/*#define WK_REXP_PEERINGSET "^PRNG-"*/
#define WK_REXP_PEERINGSET "(^|:)PRNG-[A-Z0-9_-]*[A-Z0-9](:|$)"

/*#define WK_REXP_FILTERSET  "^FLTR-"*/
#define WK_REXP_FILTERSET "(^|:)FLTR-[A-Z0-9_-]*[A-Z0-9](:|$)"

/*#define WK_REXP_RTRSET     "^RTRS-"*/
#define WK_REXP_RTRSET "(^|:)RTRS-[A-Z0-9_-]*[A-Z0-9](:|$)"

#define WK_REXP_IRT "^IRT-[A-Z0-9_-]+[A-Z0-9]$"

#define WK_REXP_ORG_ID "^ORG-([A-Z]{2,4}([1-9][0-9]{0,5})?(-[A-Z]([A-Z0-9_-]{0,7}[A-Z0-9])))$"

#define WK_REXP_NICHANDLE "^[A-Z0-9-]+$"

/* We do not want CRYPT-PW or MD5-PWs to be inverse searchable, so only PGPKEY and X509 defined */
#define WK_REXP_AUTH "^(PGPKEY|X509)-"

/*
  XXX This seems to be the same as the Perl code.  But I don't see where a " " is allowed for.
  I.e. Perl -> ^[a-zA-Z][\w\-\.\'\|\`]*$
  Does \w include [ ;:,?/}{()+*#] ?
#define WK_REXP_NAME_B "^[a-zA-Z][a-zA-Z_0-9.'|`-]*$"
*/
/* #define WK_REXP_NAME_B "^[a-zA-Z][a-zA-Z_0-9.'|`;:,?/}{()+*#&-]*$" */

#define WK_REXP_EMAIL "@[a-zA-Z0-9-]+(\\.[a-zA-Z0-9-]+)*$"

/* fingerpr: ([A-Z0-9]{4})( [A-Z0-9]{4}){9}*/
#define WK_REXP_FINGERPR "^(([A-F0-9]{4} ){9}[A-F0-9]{4})|(([A-F0-9]{2} ){15}[A-F0-9]{2})|(([A-F0-9]{2}:){15}[A-F0-9]{2})$"

/* ds-rdata: */
#define WK_REXP_DS_RDATA "^([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-4])( ([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))( ([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])|RSAMD5|DH|DSA|ECC|RSASHA1|INDIRECT|PRIVATEDNS|PRIVATEOID)([ 0-9a-fA-F]{1,128})$"

/*+ Enumeration of which keytypes: +*/
typedef enum WK_Type_t {
  WK_NAME=0,         /*+ A name                              +*/
  WK_NIC_HDL,        /*+ NIC_HDL                             +*/
  WK_EMAIL,          /*+ RFC822 e-mail address               +*/
  WK_MNTNER,         /*+ Maintainer name                     +*/
  WK_KEY_CERT,       /*+ PGPKEY | X509 certificate           +*/
  WK_IPADDRESS,      /*+ IP address                          +*/
  WK_IPRANGE,        /*+ IP range                            +*/
  WK_IPPREFIX,       /*+ IP prefix                           +*/
  WK_IP6PREFIX,      /*+ IPv6 prefix                         +*/
  WK_NETNAME,        /*+ Network name                        +*/
  WK_NET6NAME,       /*+ IPv6 Network name                   +*/
  WK_AUTNUM,         /*+ AS number                           +*/
  WK_ASSETNAME,      /*+ AS set name                         +*/
  WK_ROUTESETNAME,   /*+ Route set name                      +*/
  WK_DOMAIN,         /*+ Domain name                         +*/
  WK_REVDOMAIN,      /*+ Reverse Domain name                 +*/
  WK_HOSTNAME,       /*+ Host name                           +*/
  WK_LIMERICK,       /*+ Limerick name                       +*/
  WK_ASRANGE,        /*+ AS range (possibly only beginning)  +*/
  WK_FILTERSET,      /*+ filter-set name                     +*/
  WK_PEERINGSET,     /*+ peering-set name                    +*/
  WK_RTRSET,         /*+ router-set name                     +*/
  WK_IRT,            /*+ irt name                            +*/
  WK_FINGERPR,       /*+ fingerprint                         +*/
  WK_ORG_ID,         /*+ organisation object ID              +*/
  WK_AUTH,           /*+ auth attribute                      +*/
  WK_POEM,           /*+ poem name                           +*/
  WK_POETIC_FORM,    /*+ poet type name                      +*/
  WK_DS_RDATA,       /*+ ds-rdata contents                   +*/
  WK_END
} WK_Type;

/* for easy maintenance, the tables that depend on the enums are defined here.
   They must be kept in sync with each other. Also, the WK_new function must
   be kept in sync!!!

   Guarded with WK_IMPL so that they are compiled only once, inside wk.c
 */
#ifdef WK_IMPL
char * const Keytypes[] = {
  "name",
  "nic_hdl",
  "email",
  "mntner",
  "key_cert",
  "ipaddress",
  "iprange",
  "ipprefix",
  "ip6prefix",
  "netname",
  "net6name",
  "autnum",
  "assetname",
  "routesetname",
  "domain",
  "revdomain",
  "hostname",
  "limerick",
  "asrange",
  "filter_set",
  "peering_set",
  "rtr_set",
  "irt",
  "fingerpr",
  "organisation",
  "auth",
  "poem",
  "poetic_form",
  "ds_rdata",
  NULL
}; /* Keytypes[] */

#endif

void wk_regex_init();
char *WK_to_string(mask_t wk);
mask_t WK_new(char *key);
int WK_is_aut_num(char *key);

#endif /* READ_WHICH_KEYTYPES */
