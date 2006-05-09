/***************************************
  $Revision: 1.2 $

  IP handling (ip). iproutines.h  - header file for conversions routines.
                                    defines data structures for IP module.

  Status: NOT REVUED, TESTED

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


#ifndef _IP_H
#define _IP_H

/* NOTE: some #include calls are further down in the file */

#include <sys/types.h>
#include <glib.h>

#include <inet6def.h>

/* errors */
#define IP_OK      0
#define IP_INVRAN  1
#define IP_INVARG  2
#define IP_INVIP4  3
#define IP_NO6YET  4
#define IP_NOSLAS  5
#define IP_ADTOLO  6
#define IP_PRTOLO  7
#define IP_INVPRF  8
#define IP_NOREVD  9
#define IP_TOSHRT 10
#define IP_NOCAP  11
#define IP_INVPRN 12
#define IP_NOLEN  13
#define IP_NORANG 14
#define IP_WROLEN 15
#define IP_WRORANG 16

/* warning */
#define IP_INVBIT -1


/*+ the key type (for ascii keys - tells what it was before it was
    converted into prefixes in smart_conv() +*/

typedef enum {
  IPK_UNDEF = 0,
  IPK_RANGE,
  IPK_PREFIX,
  IPK_IP
} ip_keytype_t;

/*+ the space type +*/
#define MIN_IPSPACE_ID 1
#define MAX_IPSPACE_ID 2
typedef enum {
  IP_V4 = MIN_IPSPACE_ID,
  IP_V6
} ip_space_t;

/* it's faster if these two are the multiples of 32 */
#define IPV6_ADDRESS_ACL_BITS 64
#define IPV6_ADDRESS_ACCESS_BITS 64

typedef unsigned int   ip_limb_t; /* the limb must be at least 32 bit wide */
typedef uint64_t       ip_v6word_t;
/* should use 64bit for ipv6:
   u_int64_t (bsd,linux)
   uint64_t (solaris)
*/
#define IPLIMBNUM (16/sizeof(ip_limb_t))

/*+ address structure +*/
typedef struct {
  ip_limb_t  words[IPLIMBNUM];    /*+ 32/128 bit ip addr. SUBJECT TO CHANGE +*/

  ip_space_t space;       /*+ MUST NOT BE char ! prefixes are compared with
			    memcmp, so there may be absolutely no unitialised
			    bytes  +*/
} ip_addr_internal_t;

/*+ prefix structure +*/
typedef struct {
  unsigned                bits;		/*+ length in bits. +*/
  ip_addr_internal_t      ip;	        /*+ the IP of the prefix +*/
} ip_prefix_internal_t;

/*+ range structure +*/
typedef struct {
  ip_addr_internal_t         begin;        /*+ IP where the range begins. +*/
  ip_addr_internal_t         end;          /*+ IP where it ends +*/
} ip_range_internal_t;

/*+ prefix_range structure ( 1.2.3.4/24^23-22 +*/
typedef struct {
  ip_prefix_internal_t    prefix;
  unsigned                n;      /*+ the N number +*/
  unsigned                m;      /*+ the M number +*/
} ip_prefix_range_internal_t;

#if 0/* #ifndef IP_IMPL  -- set this to see accesses to structure members */
/* hide the internals */
typedef struct {char a[sizeof(ip_addr_internal_t)];}    ip_addr_t;
typedef struct {char a[sizeof(ip_range_internal_t)];}   ip_range_t;
typedef struct {char a[sizeof(ip_prefix_internal_t)];}  ip_prefix_t;
typedef struct {char a[sizeof(ip_prefix_range_internal_t)];}  ip_prefix_range_t;
#else
typedef ip_addr_internal_t   ip_addr_t;
typedef ip_range_internal_t  ip_range_t;
typedef ip_prefix_internal_t ip_prefix_t;
typedef ip_prefix_range_internal_t ip_prefix_range_t;
#endif


/*+
  stores size/span of an allocation
  SUBJECT TO CHANGE: will be bigger for IPv6
  +*/
typedef unsigned int  ip_rangesize_t;

/*+  the length of a string that should be able to hold a prefix / range
    when used with b2a functions.
+*/
#define IP_ADDRSTR_MAX 48
#define IP_PREFSTR_MAX 64
#define IP_RANGSTR_MAX 128

/*+
  IP expansion mode - for use with t2b functions, they control
  whether the input is supposed to be fully expanded or contain shortcuts
  (eg. enabling saying 0/0 instead 0.0.0.0/0)
  +*/
typedef enum {
  IP_PLAIN = 1,
  IP_EXPN
} ip_exp_t;

/* #include <erroutines.h> */

#ifdef __cplusplus
extern "C" {
#endif



/* prototypes */
/* text to binary */
int IP_addr_t2b(ip_addr_t *ipptr, const char *addr, ip_exp_t expf);
int IP_pref_t2b(ip_prefix_t *prefptr, const char *prefstr, ip_exp_t expf);
int IP_rang_t2b(ip_range_t *rangptr, const char *rangstr, ip_exp_t expf);
int IP_revd_t2b(ip_prefix_t *prefptr, const char *prefstr, ip_exp_t expf);
int IP_pref_rang_t2b(ip_prefix_range_t *prefrangptr, const char *prefrangstr, ip_exp_t expf);
/* convenience (or call it backward compatibility) macros */

/*+ the e2b macros assume fully expanded text  +*/
#define IP_addr_e2b(a,b) IP_addr_t2b(a,b,IP_PLAIN)
#define IP_pref_e2b(a,b) IP_pref_t2b(a,b,IP_PLAIN)
#define IP_rang_e2b(a,b) IP_rang_t2b(a,b,IP_PLAIN)
#define IP_revd_e2b(a,b) IP_revd_t2b(a,b,IP_PLAIN)

/*+ the a2b macros will fully expand an address.
  The details depend on the individual functions. +*/
#define IP_addr_a2b(a,b) IP_addr_t2b(a,b,IP_EXPN)
#define IP_pref_a2b(a,b) IP_pref_t2b(a,b,IP_EXPN)
#define IP_rang_a2b(a,b) IP_rang_t2b(a,b,IP_EXPN)
#define IP_revd_a2b(a,b) IP_revd_t2b(a,b,IP_EXPN)

/* text fragments to binary */
int IP_addr_f2b_v4(ip_addr_t *addrptr, const char *adrstr);
int IP_rang_f2b_v4(ip_range_t *rangptr, const char *beginstr,  const char *endstr);
int IP_pref_f2b_v4(ip_prefix_t *prefptr, const char *prefixstr,
			const char *lengthstr);
int IP_addr_f2b_v6(ip_addr_t *addrptr, const char *msbstr, const char *lsbstr );
int IP_pref_f2b_v6(ip_prefix_t *prefptr, const char *msbstr, const char *lsbstr,
                        const char *lengthstr);
int
IP_addr_f2b_v6_32(ip_addr_t *addrptr, const char *word1str, const char *word2str,
								const char *word3str, const char *word4str);
int
IP_pref_f2b_v6_32(ip_prefix_t *prefptr, const char *word1str, const char *word2str,
								const char *word3str, const char *word4str, const char *lengthstr);

int IP_addr_b2a(ip_addr_t *binaddr, char *ascaddr, unsigned strmax );
int IP_pref_b2a(ip_prefix_t *prefptr, char *ascaddr, unsigned strmax);
int IP_rang_b2a(ip_range_t *rangptr, char *ascaddr, unsigned strmax);
int IP_rang_classful(ip_range_t *rangptr, ip_addr_t *addrptr);
int IP_pref_2_rang( ip_range_t *rangptr, ip_prefix_t *prefptr );
int IP_rang_2_pref( ip_range_t *rangptr, ip_prefix_t *prefptr );

/* utility functions: testers/converters */
int  IP_addr_bit_get(ip_addr_t *binaddr, unsigned bitnum);
int IP_addr_bit_set(ip_addr_t *binaddr, unsigned bitnum, unsigned bitval);
int  IP_addr_cmp(ip_addr_t *ptra, ip_addr_t *ptrb, unsigned len);
unsigned  IP_sizebits(ip_space_t spc_id);
int IP_pref_bit_fix( ip_prefix_t *prefix );
int IP_addr_in_pref(ip_addr_t *ptra, ip_prefix_t *prefix);
int IP_addr_in_rang(ip_addr_t *ptra, ip_range_t *rangptr);
int IP_smart_conv(char *key, int justcheck, int encomp,
		       GList **preflist, ip_exp_t expf, ip_keytype_t *keytype);
int IP_smart_range(char *key, ip_range_t *rangptr, ip_exp_t expf,
			ip_keytype_t *keytype);
int IP_rang_overlap(ip_range_t *range1, ip_range_t *range2);
gboolean IP_addr_isnull(ip_addr_t *addrptr);

ip_rangesize_t IP_rang_span( ip_range_t *rangptr );
int IP_addr_s2b(ip_addr_t *addrptr, void *addr_in, int addr_len);

/* accessor functions */
unsigned IP_addr_b2_space(ip_addr_t *addrptr);
unsigned IP_pref_b2_space(ip_prefix_t *prefix);
unsigned IP_rang_b2_space(ip_range_t *myrang);

unsigned IP_addr_b2v4_addr(ip_addr_t *addrptr);
ip_limb_t IP_addr_b2v6_addr(ip_addr_t *addrptr, int limb);

unsigned IP_pref_b2_space(ip_prefix_t *prefix);
unsigned IP_pref_b2_len(ip_prefix_t *prefix);
#define IP_pref_b2v4_len(prefix) IP_pref_b2_len(prefix)
#define IP_pref_b2v6_len(prefix) IP_pref_b2_len(prefix)

ip_v6word_t IP_addr_b2v6_hi(ip_addr_t *addrptr);
ip_v6word_t IP_addr_b2v6_lo(ip_addr_t *addrptr);
unsigned IP_pref_b2v4_addr(ip_prefix_t *prefix);
void IP_addr_b2v4(ip_addr_t *addrptr, unsigned *address);
void IP_pref_b2v4(ip_prefix_t *prefptr,
		   unsigned int *prefix,
		   unsigned int *prefix_length);
#define IP_revd_b2v4(a,b,c) IP_pref_b2v4(a,b,c)
void IP_pref_b2v6(ip_prefix_t *prefptr,
                  ip_v6word_t *high,
                  ip_v6word_t *low,
                  unsigned int *prefix_length);
void IP_addr_b2v6_32(ip_addr_t *addrptr,
		  ip_limb_t *word1,
		  ip_limb_t *word2,
		  ip_limb_t *word3,
		  ip_limb_t *word4);
void IP_pref_b2v6_32(ip_prefix_t *prefptr,
		  ip_limb_t *word1,
		  ip_limb_t *word2,
		  ip_limb_t *word3,
		  ip_limb_t *word4,
		  unsigned int *prefix_length);
#define IP_revd_b2v6(a,b,c,d) IP_pref_b2v6(a,b,c,d)
void IP_rang_b2v4(ip_range_t *myrang,
		  unsigned *begin,
		  unsigned *end);

/******** constructing from raw values **********/
int IP_addr_v4_mk(ip_addr_t *addrptr, unsigned addrval);
int IP_addr_v6_mk(ip_addr_t *addrptr,
                       ip_v6word_t high, ip_v6word_t low);
int IP_addr_v6_mk_32(ip_addr_t *addrptr, ip_limb_t word1, ip_limb_t word2,
									ip_limb_t word3, ip_limb_t word4);
int IP_pref_v4_mk(ip_prefix_t *prefix,
		       unsigned prefval, unsigned preflen);
int IP_rang_v4_mk(ip_range_t *rangptr,
		       unsigned addrbegin, unsigned addrend);
/* a2v4 functions to convert the ascii to binary, and
  then set the raw values at the pointers provided. */
int IP_pref_a2v4(const char *avalue, ip_prefix_t *pref,
		      unsigned *prefix, unsigned *prefix_length);
int IP_pref_a2v6(const char *avalue, ip_prefix_t *pref,
                      ip_v6word_t *high, ip_v6word_t  *low,
                      unsigned *prefix_length);
int IP_pref_a2v6_32(const char *avalue, ip_prefix_t *pref,
	     ip_limb_t *word1, ip_limb_t  *word2,
	     ip_limb_t *word3, ip_limb_t  *word4,
	     unsigned *prefix_length);
int IP_revd_a2v4(const char *avalue, ip_prefix_t *pref,
		      unsigned int *prefix, unsigned int *prefix_length);
int IP_addr_a2v4(const char *avalue,ip_addr_t *ipaddr, unsigned int *address);
int IP_rang_a2v4(const char *rangstr, ip_range_t *myrang,
		      unsigned int *begin_in, unsigned int *end_in);

/* decompose/find encompasssing prefix */
void IP_rang_encomp(ip_range_t *rangptr);
unsigned IP_rang_decomp(ip_range_t *rangptr, GList **preflist);
int IP_pref_in_prefrang(ip_prefix_t *prefix, ip_prefix_range_t *prefixrange);

#ifdef __cplusplus
}
#endif

/*
  this is to define a constant struct for comparisons.
*/
#ifdef IP_IMPL
const ip_addr_t IP_ADDR_UNSPEC={{0,0,0,0},0}; /* unlikely to be real :-)
					       as there is no space 0
					       bonus: it's a natural state after
					       initializing to 0 */




#else
extern ip_addr_t IP_ADDR_UNSPEC;
#endif

#endif /* _IP_H */
