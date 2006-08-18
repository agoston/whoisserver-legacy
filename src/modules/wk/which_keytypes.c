/***************************************
  $Revision: 1.4 $

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

/******************************************* 
If any IP reg exps change - check whois cgi which has a copy 
*******************************************/

#define DOMAINNAME "^[ ]*[a-zA-Z0-9/-]*(\\.[a-zA-Z0-9-]+)*\.?[ ]*$"
/* add a constraint: there must be at least one character in the domain name
   because the TLD must not be composed of digits only */
#define DOMAINALPHA  "[a-zA-Z]"

#define VALIDIP6PREFIX "^[0-9A-F:]*:[0-9A-F:/]*$"     /* at least one colon */
/* "^[0-9A-F]{1,4}(:[0-9A-F]{1,4}){7}$"*/

/* AS numbers, prepared for 32-bit AS numbers */
#define ASNUM "^AS[1-9][0-9]{0,9}$"

/* AS numbers, prepared for 32-bit AS numbers */
#define ASRANGE "^AS[1-9][0-9]{0,9}[ ]*([-][ ]*AS[1-9][0-9]{0,9}){0,1}$"   /* [ ]*(-[ ]*AS[0-9]+)?   */

#define NETNAME "^[A-Z][A-Z0-9_-]*$"

#define MAINTAINER "^[A-Z][A-Z0-9_-]*$"

#define LIMERICK "^LIM-[A-Z0-9_-]+$"

#define POEM "^POEM-[A-Z0-9][A-Z0-9_-]*$"

#define POETIC_FORM "^FORM-[A-Z0-9][A-Z0-9_-]*$"

#define KEYCERT "^(PGPKEY-[0-9A-F]{8})|(X509-[0-9]+)$"

/* made less restrictive to make consistent with other sets ... shane */
/* made to match what we're actually looking for - shane */
/*#define ROUTESETNAME "^RS-[A-Z0-9_:-]*$"*/
#define ROUTESETNAME "(^|:)RS-[A-Z0-9_-]*[A-Z0-9](:|$)"

/* made less restrictive to make consistent with other sets ... shane */
/* made to match what we're actually looking for - shane */
/*#define ASSETNAME "^AS-[A-Z0-9_:-]*$"*/
#define ASSETNAME "(^|:)AS-[A-Z0-9_-]*[A-Z0-9](:|$)" 

#define AUTONICPREFIXREGULAR "^AUTO-"

#define IPRANGE "^[0-9]{1,3}(\\.[0-9]{1,3}){0,3}[ ]*-[ ]*[0-9]{1,3}(\\.[0-9]{1,3}){0,3}$"

#define IPADDRESS "^[0-9.]+$"

#define IPPREFIX "^[0-9.]+/[0-9]+$"

/*#define PEERINGSET "^PRNG-"*/
#define PEERINGSET "(^|:)PRNG-[A-Z0-9_-]*[A-Z0-9](:|$)" 

/*#define FILTERSET  "^FLTR-"*/
#define FILTERSET "(^|:)FLTR-[A-Z0-9_-]*[A-Z0-9](:|$)" 

/*#define RTRSET     "^RTRS-"*/
#define RTRSET "(^|:)RTRS-[A-Z0-9_-]*[A-Z0-9](:|$)" 

#define IRT "^IRT-[A-Z0-9_-]+[A-Z0-9]$"

#define ORG_ID "^ORG-([A-Z]{2,4}([1-9][0-9]{0,5})?(-[A-Z]([A-Z0-9_-]{0,7}[A-Z0-9])))$"

#define NICHANDLE "^[A-Z0-9-]+$"

/* We do not want CRYPT-PW or MD5-PWs to be inverse searchable, so only PGPKEY and X509 defined */
#define AUTH "^(PGPKEY|X509)-"

/*
  XXX This seems to be the same as the Perl code.  But I don't see where a " " is allowed for.
  I.e. Perl -> ^[a-zA-Z][\w\-\.\'\|\`]*$
  Does \w include [ ;:,?/}{()+*#] ?
#define NAME_B "^[a-zA-Z][a-zA-Z_0-9.'|`-]*$"
*/
#define NAME_B "^[a-zA-Z][a-zA-Z_0-9.'|`;:,?/}{()+*#&-]*$"

#define EMAIL "@[a-zA-Z0-9-]+(\\.[a-zA-Z0-9-]+)*$"

/* fingerpr: ([A-Z0-9]{4})( [A-Z0-9]{4}){9}*/
#define FINGERPR "^(([A-F0-9]{4} ){9}[A-F0-9]{4})|(([A-F0-9]{2} ){15}[A-F0-9]{2})|(([A-F0-9]{2}:){15}[A-F0-9]{2})$"

/* ds-rdata: */
#define DS_RDATA "^([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-4])( ([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))( ([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])|RSAMD5|DH|DSA|ECC|RSASHA1|INDIRECT|PRIVATEDNS|PRIVATEOID)([ 0-9a-fA-F]{1,128})$"

/* structure for simple keys, with a single regular expression to match */
/* NOTE: the WK_NAME, WK_DOMAIN, and WK_HOSTNAME are not handled here   */
struct {
    int key_type;		/* identifier for key, e.g. WK_RTRSET */
    char *pattern;		/* string for regular expression */
    regex_t regex;		/* regular expression */
} wk_regex_list[] = {
    { WK_NIC_HDL,       NICHANDLE },
    { WK_EMAIL,         EMAIL },
    { WK_MNTNER,        MAINTAINER },
    { WK_KEY_CERT,      KEYCERT },
    { WK_IPRANGE,       IPRANGE },
    { WK_IPADDRESS,     IPADDRESS },
    { WK_IPPREFIX,      IPPREFIX },
    { WK_IP6PREFIX,     VALIDIP6PREFIX },
    { WK_NETNAME,       NETNAME },
    { WK_NET6NAME,      NETNAME },
    { WK_AUTNUM,        ASNUM },
    { WK_ASSETNAME,     ASSETNAME },
    { WK_ROUTESETNAME,  ROUTESETNAME },
    { WK_LIMERICK,      LIMERICK },
    { WK_POEM,          POEM },
    { WK_POETIC_FORM,   POETIC_FORM },
    { WK_ASRANGE,       ASRANGE },
    { WK_PEERINGSET,    PEERINGSET },
    { WK_FILTERSET,     FILTERSET },
    { WK_RTRSET,        RTRSET },
    { WK_IRT,           IRT },
    { WK_FINGERPR,      FINGERPR },
    { WK_ORG_ID,        ORG_ID },
    { WK_DS_RDATA,      DS_RDATA },
    { WK_AUTH,          AUTH }
};
#define WK_REGEX_LIST_LEN  (sizeof(wk_regex_list)/sizeof(wk_regex_list[0]))

/* regular expressions used by wk_is_name() */
static regex_t ipaddress;
static regex_t ipprefix;
static regex_t validip6prefix;

/* regular expression used by isdomname() */
static regex_t domainname;
static regex_t domainalpha;

/* initialize regular expressions */
static void 
wk_regex_init ()
{
    int i;
    int errcode;

    /* initialize our table */
    for (i=0; i<WK_REGEX_LIST_LEN; i++) {
        errcode = regcomp(&wk_regex_list[i].regex, 
	                  wk_regex_list[i].pattern, 
		          REG_EXTENDED|REG_NOSUB);
        dieif(errcode != 0);
    }

    /* add some special cases used by our other functions */
    errcode = regcomp(&ipaddress, IPADDRESS, REG_EXTENDED|REG_NOSUB);
    dieif(errcode != 0);
    errcode = regcomp(&ipprefix, IPPREFIX, REG_EXTENDED|REG_NOSUB);
    dieif(errcode != 0);
    errcode = regcomp(&validip6prefix, VALIDIP6PREFIX, REG_EXTENDED|REG_NOSUB);
    dieif(errcode != 0);
    errcode = regcomp(&domainname, DOMAINNAME, REG_EXTENDED|REG_NOSUB);
    dieif(errcode != 0);
    errcode = regcomp(&domainalpha, DOMAINALPHA, REG_EXTENDED|REG_NOSUB);
    dieif(errcode != 0);
}


/* see if the key looks like it could be a name */
static unsigned int 
wk_is_name (char *key) 
{
    /* if it's an address, it cannot be a name */
    if (regexec(&ipaddress, key, 0, NULL, 0) == 0) { 
        return 0;
    }
    if (regexec(&ipprefix, key, 0, NULL, 0) == 0) { 
        return 0;
    }
    if (regexec(&validip6prefix, key, 0, NULL, 0) == 0) { 
        return 0;
    }

    /* Everything apart from addresses matches to name */
    return 1;
} /* wk_is_name() */

/* check for domain name */
static unsigned int 
wk_is_domain (char *key) 
{
    /* if it matches the general domain name search, and contains an */
    /* alphabetic character, consider it a possible domain name */
    if (regexec(&domainname, key, 0, NULL, 0) == 0) {
        if (regexec(&domainalpha, key, 0, NULL, 0) == 0) {
	    return 1;
	}
    }
    return 0;
} 

/* check for a host name (could be a domain, or an IP) */
static unsigned int 
wk_is_hostname (char *key) 
{
    /* Fix - should check for IPADDRESS, not IPRANGE.  - Shane */
    return (wk_is_domain(key) || (regexec(&ipaddress, key, 0, NULL, 0) == 0));
} /* wk_is_hostname() */

/* WK_to_string() */
/*++++++++++++++++++++++++++++++++++++++
  Convert the which keytypes bitmap into a string.

  mask_t wk The which keytypes mask to be converted.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *
WK_to_string (mask_t wk) 
{

  return MA_to_string(wk, Keytypes);

} /* WK_to_string() */

/* WK_new() */
/*++++++++++++++++++++++++++++++++++++++
  Create a new which keytypes bitmap.

  This checks the string to see which keys it looks like.  This helps 
  us decide what SQL tables (or radix trees) we need to query for a
  match.

  char *key The key to be examined.

  More:
  +html+ <PRE>
  Authors:
        ottrey
	shane
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
mask_t 
WK_new (char *key) 
{
  static pthread_once_t once_control = PTHREAD_ONCE_INIT;

  mask_t wk; 
  int i;

  /* initialize our regular expressions on the first call */
  pthread_once(&once_control, wk_regex_init);

  /* empty bitmask */
  wk = MA_new(MA_END);

  /* search regular expressions in the list */
  for (i=0; i<WK_REGEX_LIST_LEN; i++) {
      if (regexec(&wk_regex_list[i].regex, key, 0, NULL, 0) == 0) { 
          MA_set(&wk, wk_regex_list[i].key_type, 1);
      }
  }

  /* check our more complicated key patterns */
  MA_set(&wk, WK_NAME,         wk_is_name(key));
  MA_set(&wk, WK_DOMAIN,       wk_is_domain(key));
  MA_set(&wk, WK_HOSTNAME,     wk_is_hostname(key));
  
  /* return resulting bitmask */
  return wk;

} /* WK_new() */
