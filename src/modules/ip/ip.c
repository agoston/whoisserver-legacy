/***************************************
  $Revision: 1.1 $

  IP handling (ip). ip.c  - conversions between ascii and binary forms 
                            of IP addresses, prefixes and ranges.
 
			    various operations on binary forms.

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

#define IP_IMPL
#include "rip.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <sys/param.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <glib.h>

/* workaround to fix broken include files with Linux */
#ifndef ULLONG_MAX
#define ULLONG_MAX 18446744073709551615ULL
#endif


/**************************************************************************/
/*+ return the max. length of bits per space

   Yes, it *could* be a macro - but as a function it can detect 
   more programmer's errors. And will get inlined anyway.

+*/

unsigned IP_sizebits(ip_space_t spc_id) {
  switch (spc_id) {
  case IP_V4:
    return 32;
  case IP_V6:
    return 128;
  default:
    die; /* error: bad IP version specified */
    return 999999; /* just for the compiler */
  }
}

static
int
ip_rang_validate(ip_range_t *rangptr) 
{
  if( rangptr->begin.space != rangptr->end.space ) {
    /* die;  */ /* incompatible IP spaces */
    return IP_INVRAN;
  }

  /* XXX IPv6 range check missing */
  if( rangptr->begin.space == IP_V4 ) {
    if( rangptr->begin.words[0] > rangptr->end.words[0] ) {
      return IP_INVRAN;
    }
  }

  return IP_OK;
}
/**************************************************************************/
/*+
   ascii IP address to binary.  

   In IP_EXPN mode IP will be "expanded"
   (missing octets will be set to 0, MSB's will be set).
   In IP_PLAIN mode the routine will complain if it sees less octets. 
   
   why not use the standard inet_blabla routine ?
   it's because if some octets are missing, we make the address zero-padded
   (unlike the inet_blabla, which puts zeros in the middle). We also want 
   to control the expansion with a flag.

   +*/

int 
IP_addr_t2b(ip_addr_t *ipptr, const char *addr, ip_exp_t expf)
{
  if( index(addr, ':') == NULL ) {
    /* IPv4 */
    const char *dot=addr;
    unsigned len, byte, result=0;
    char cpy[4];
    int last = 0, dotsfound=0;
    int bytes=0;

    if( expf != IP_PLAIN && expf != IP_EXPN ) {
      return IP_INVARG;
    }

    do {
      const char *olddot = dot+1;
      /* dot should point to the "end of this number", not necessarily a dot */

      if ( (dot = index (addr, '.')) == NULL) {
	/* after the ip it can contain lots of junk spaces */
	while( *olddot != 0 && ! isspace(* (unsigned char *) olddot)  ) {
	  olddot++;
	}
	dot = olddot;
	last = 1;
      }
      else {
	if( ++dotsfound > 3 ) {
	  /* handle syntax ERROR - too many dots found */
	  return IP_INVIP4;
	}
      }
	
      if ((len = dot - addr) > 3) {
	/* syntax ERROR - too many digits in an octet */
	return IP_INVIP4;
      }
      strncpy( cpy, addr, len );
      cpy[len]=0;

      /* sscanf is waay too slow */
       
      if( ut_dec_2_uns(cpy, &byte) < 0 ) {
	/* handle syntax ERROR - invalid characters found */
	return IP_INVIP4;
      }
      
	
      if( byte > 255 ) {
	/* handle syntax ERROR - number between dots too high */
	return IP_INVIP4;
      }
      
      result <<= 8;
      result += byte;
      bytes++;
      
      addr = dot + 1;
    } while (!last);

    if( expf == IP_PLAIN ) {
      if( bytes!=4 ) {
	return IP_INVIP4;
      }
    } 
    else {
      while( bytes<4 ) {
	result <<= 8;
	bytes++;
      }
    }

    memset(ipptr, 0, sizeof(ip_addr_t));
    ipptr->space = IP_V4;
    ipptr->words[0] = result; 
  }
  else {
    /* IPv6 */
#define  _IPV6_LENGTH 128
    char addrcpy[_IPV6_LENGTH];
    char *ch, *start;
    int i;
    
    strncpy(addrcpy, addr, _IPV6_LENGTH-1);
    addrcpy[_IPV6_LENGTH-1] = 0;
    
    /* get rid of superfluous whitespaces */
    /* leading... */
    for( ch = start = addrcpy ; *ch != 0; ch++ ) {
      if( isspace( (int) *ch) ) { 
	start++;
      }
      else {
	break;
      }
    }

    /* and trailing */
    while( *ch != 0 ) {
      if( isspace( (int) *ch) ) {
	*ch = 0;
	break;
      }
      ch++;
    }
    
    if( inet_pton(AF_INET6, start, (ipptr->words)) == 0 ) {
      return  IP_NO6YET;
    }
    /* now change the byte order from network to host native */
    for( i=0; i<4; i++ ) {
      ipptr->words[i] = ntohl(ipptr->words[i]);
    }

    ipptr->space = IP_V6;

#undef _IPV6_LENGTH
  }
  return IP_OK;	
}

/**************************************************************************/

/*+ converts a "IP/length" string into a binary prefix 
  
 

+*/

int
IP_pref_t2b(ip_prefix_t *prefptr, const char *prefstr, ip_exp_t expf)
{
  char ip[256];
  char *trash;
  char *slash;
  unsigned len;
  int err;

  if( expf != IP_PLAIN && expf != IP_EXPN ) {
    return IP_INVARG;
  }
  
  if( (slash=index(prefstr, '/')) == NULL ) {
    /* die;  */ /* error: missing slash in prefix */
    return    IP_NOSLAS;
  }
  else {
    /* copy the IP part to another string, ERROR if 256 chars not enough */
    
    len = slash - prefstr;
    if( len > 255 ) { 
      /* die;  */ /* ERROR - ip address part of the string too long. */
      return  IP_ADTOLO;
    }
    strncpy(ip, prefstr, len);
    ip[len]=0;

    if( (err=IP_addr_t2b( &(prefptr->ip), ip, expf)) != IP_OK) {
      /* die;   */ /* set error flag: incorrect address format */
      return err;
    }

    /* stop at first non-digit */
    for(trash = slash+1; 
	isdigit(* (unsigned char*) trash);         /* cast for stupid gcc */
	trash++)
      ;
    len = trash - (slash+1) ;
    if(( len > 4 ) || ( len < 1 )) { 
      /* ERROR - prefix length part of the string too long or too short. */
      return IP_PRTOLO;
    }
    strncpy(ip, slash+1, len);
    ip[len]=0;

    if( ut_dec_2_uns(ip, &prefptr->bits) < 0 
	|| prefptr->bits > IP_sizebits(prefptr->ip.space))
      {
      /*    if( sscanf (slash+1, "%d", &(prefptr->bits)) < 1 ) {
            die; */ /* handle syntax ERROR invalid characters found */
      return IP_INVPRF;
    }
  }
  /* sanitify the prefix - maybe some irrelevant bits are set */
  /* report a warning IP_INVBIT if this was the case */
  /* never create broken binary prefixes. */

  err = IP_pref_bit_fix(prefptr);

  return err;
}

/**************************************************************************/

/*+ converts an inaddr/ip6int string into a binary prefix.

  RFC2317 support for IPv4:

  For expf==IP_EXPN (e2b macro) the unparsable part will be silently accepted 
  (with the result being the prefix of the succesfully parsed bits). 

  For expf==IP_PLAIN  the unparsable part will make the function return an error.

  For IPv6 the expf doesn't matter, the address must be parsable in whole.

+*/
int
IP_revd_t2b(ip_prefix_t *prefptr, const char *domstr, ip_exp_t expf)
{
#define CPYLEN 264
  char ip[256], temp[256];
  char prefstr[CPYLEN+1];
  char *arpa;
  unsigned len;
  int octets=0, goon=1, quads = 0;
  char  *dot;
  int err = IP_OK;
  gchar **domains;
  int i, j;
  int zeros_to_add;

  dieif( expf != IP_PLAIN && expf != IP_EXPN );

  /* The input may not be in lowercase, but must be processed as well.
     The simplest solution: make a copy and change it to lowercase. */
  
  strncpy( prefstr, domstr, CPYLEN );
  prefstr[CPYLEN] = '\0';
  g_strdown(prefstr);

  if( (arpa=strstr(prefstr, ".in-addr.arpa")) != NULL ) {
    prefptr->ip.space = IP_V4;
  }
  else if( (arpa=strstr(prefstr, ".ip6.int")) != NULL ) {
    prefptr->ip.space = IP_V6;
  }
#if 1 
/* support for ip6.arpa domains */
  else if( (arpa=strstr(prefstr, ".ip6.arpa")) != NULL ) {
    prefptr->ip.space = IP_V6;
  }
#endif
  else {
    return    IP_NOREVD; 
  }
  
  /* copy the IP part to another string, ERROR if 256 chars not enough */
  len = arpa - prefstr;
  if( len > 255 ) { 
    /* die;  */ /* ERROR - ip address part of the string too long. */
    return  IP_ADTOLO;
  }
  strncpy(temp, prefstr, len);
  temp[len]=0;
  
  /* now: get the octets/quads reversed one by one. Then conversion. */
  ip[0]=0; /* init */
  switch( prefptr->ip.space ) {
  case IP_V6: 
    /* ipv6 originally looked like: "1.8.0.6.0.1.0.0.2.ip6.int" */
    /* here it will look like: "1.8.0.6.0.1.0.0.2" */
    g_strreverse(temp);
    /* now it will look like: "2.0.0.1.0.6.0.8.1" */
    /* we split into domains, and then add each one on, putting ':' where
       necessary */
    ip[0] = '\0';
    i = 0;
    quads = 0;
    domains = ut_g_strsplit_v1(temp, ".", -1);
    while (domains[i] != NULL) {
        strcat(ip, domains[i]);
        quads++;
        i++;
        if (((i % 4) == 0) && (i < 32)) {
            strcat(ip, ":");
        }
    }
    g_strfreev(domains);
    /* our ip string will look like this: "2002:0608:1" */
    /* add zeros to pad the string, and a final "::" */
    switch (i % 4) {
        case 0: zeros_to_add = 0; break;
        case 1: zeros_to_add = 3; break;
        case 2: zeros_to_add = 2; break;
        case 3: zeros_to_add = 1; break;
    }
    if (zeros_to_add) {
        for (j=0; j<zeros_to_add; j++) {
            strcat(ip, "0");
            i++;
        }
        if (i < 32) {
            strcat(ip, ":");
        }
    }
    if (i < 32) {
        strcat(ip, ":");
    }
    /* now we have a fully-formed IPv6 address: "2002:0608:1000::" */

    /* convert using our text-to-binary function */
    err=IP_addr_t2b( &(prefptr->ip), ip, IP_EXPN);
    prefptr->bits = quads * 4;
    break;
    
  case  IP_V4:
    do {
      if( (dot = strrchr( temp, '.' )) == NULL ) {
	goon = 0;
	dot = temp;
      }
      
      strcat(ip, dot + ( goon ) ); 
      octets++;
      
      /* add a dot, unless that was the last octet */
      if( goon ) {
	strcat(ip, ".");
      }
      
      *dot = 0;
      
    } while( goon );

    /* now try to convert the ip. 
       
       Support for RFC2317:
       If expf==IP_EXPN, then on failure leave out the last octet 
       (nibble/piece) and try again. On success, quit the loop.

       In any case use the EXPN mode for the conversion.
    */
    do {
      char *lastdot;
      
      if( (err=IP_addr_t2b( &(prefptr->ip), ip, IP_EXPN)) == IP_OK) {
	break;
      }
      
      /* cut the last octet */
      if( (lastdot=strrchr(ip, '.')) == NULL ) {
	break;
      }
      *lastdot = '\0';
      octets--;
      
    } while( expf == IP_EXPN && octets>0 );

    prefptr->bits = octets * 8;
    break;
  } /* switch */

  return err;
}

/**************************************************************************/

/*+ convert a range string into a binary range struct. 
+*/
int
IP_rang_t2b(ip_range_t *rangptr, const char *rangstr, ip_exp_t expf)
{
  char *ips, *dash;
  int err;

  if( expf != IP_PLAIN && expf != IP_EXPN ) {
    return IP_INVARG;
  }

  if( (dash=index(rangstr, '-')) == NULL ) {
    /*    die;  */ /* error: missing dash in range */
    return IP_INVRAN;
  }
  else {
    unsigned partlen = dash - rangstr;

    /* copy the first IP */
    ips = (char *)UT_calloc(1, partlen+1);

    strncpy(ips, rangstr, partlen);
    
    /* convert the first IP into a binary struct */
    err=IP_addr_t2b( &(rangptr->begin), ips, expf);

    /* check later */ /* set error flag: incorrect address format */

    UT_free(ips);

    if( err != IP_OK ) {
      return err;
    }

    /* now find the other ip, skip the space */
    ips=dash+1;
/*XXX the bug: tabs are also valid whitespace */    
/*    while( *ips == ' ' ) { */
    while(isspace((int)*ips)) {
      ips++;
    }
    
    /* convert the second IP into a binary struct */
    if( (err=IP_addr_t2b( &(rangptr->end), ips, expf)) != IP_OK ) {
      /* die;  */ /* incorrect address format */
      return err;
    }
    
    
    
    return ip_rang_validate(rangptr);
  }
}

/**************************************************************************/

/*+ convert a prefix range string into a binary range struct. 
    the prefix range can be the following:
    x.x.x.x/l^n        n = m
    x.x.x.x/l^-        n = (l + 1), m = 32
    x.x.x.x/l^+        n = l, m = 32
    x.x.x.x/l^n-m      n, m
+*/ 
int IP_pref_rang_t2b(ip_prefix_range_t *prefrangptr, const char *prefrangstr, ip_exp_t expf)
{
  char prefix[256]; 
  char *cap;
  unsigned len;
  int err = IP_OK;
    
  if( expf != IP_PLAIN && expf != IP_EXPN ) {
    return IP_INVARG;
  }

  if( (cap=index(prefrangstr, '^')) == NULL ) {
    /* die;  */ /* error: missing cap in prefix range */
    return    IP_NOCAP;
  }
  else 
  {
    /* copy the IP prefix part to another string, ERROR if 256 chars not enough */

    len = cap - prefrangstr;
    if( len > 255 ) {
      /* die;  */ /* ERROR - ip prefix part of the string too long. */
      return  IP_ADTOLO;
    }
    strncpy(prefix, prefrangstr, len);
    prefix[len]=0;

    /* assign the prefix */
    if( (err=IP_pref_t2b( &(prefrangptr->prefix), prefix, expf)) != IP_OK) {
      /* die;   */ /* set error flag: incorrect address format */
      return err;
    }

    /* calculate n and m, tight checks */
    cap ++;

    switch (*cap) {
      case '-': 
        /* all exclusive more specifics */
        prefrangptr->n = prefrangptr->prefix.bits + 1;
        prefrangptr->m = 32; 
        break;
      case '+':
        /* all inclusive more specifics */
        prefrangptr->n = prefrangptr->prefix.bits;
        prefrangptr->m = 32;
        break;
      default:
        if (strstr (cap, "-") != NULL )
        {
          if (sscanf (cap, "%d-%d", &(prefrangptr->n), &(prefrangptr->m)) != 2) 
          {
            return IP_INVPRN;
          } 
        }
        else 
        {
          if ( sscanf (cap, "%d", &(prefrangptr->n)) != 1)
          {
            return IP_INVPRN;
          }
          else 
          {
            prefrangptr->m = prefrangptr->n;
          }
        }
        break;
    }
  }
  if ((prefrangptr->prefix.bits > 32) || (prefrangptr->prefix.bits < 0))
  {
    return IP_WROLEN;
  }
  if ((prefrangptr->n > 32) || (prefrangptr->n < 0) ||
      (prefrangptr->m > 32) || (prefrangptr->m < 0) )
  {
    return IP_WRORANG;
  }
  if (prefrangptr->n > prefrangptr->m)
  {
    return IP_NORANG;
  }
  if ((prefrangptr->prefix.bits > prefrangptr->n) ||
      (prefrangptr->prefix.bits > prefrangptr->m)
     ) 
  {
    return IP_NOLEN;
  }
  return IP_OK;
}

/**************************************************************************/
/* accessor functions */

/******** address **********/

unsigned IP_addr_b2_space(ip_addr_t *addrptr) 
{
  return addrptr->space;
}

unsigned IP_addr_b2v4_addr(ip_addr_t *addrptr) 
{
  dieif( addrptr->space != IP_V4 );
  return addrptr->words[0];
}
/* ipv4 */

ip_v6word_t IP_addr_b2v6_hi(ip_addr_t *addrptr) 
{
  dieif( addrptr->space != IP_V6 );
  return (  (((ip_v6word_t) addrptr->words[0]) << 32)
	  + (((ip_v6word_t) addrptr->words[1]) ));
}

ip_v6word_t IP_addr_b2v6_lo(ip_addr_t *addrptr) 
{
  dieif( addrptr->space != IP_V6 );
  return (   (((ip_v6word_t) addrptr->words[2]) << 32)
	   + (((ip_v6word_t) addrptr->words[3]) ));
}

/******** prefix **********/

unsigned IP_pref_b2_space(ip_prefix_t *prefix) {
  return IP_addr_b2_space( &(prefix->ip) );
}

unsigned IP_pref_b2_len(ip_prefix_t *prefix) {
  return prefix->bits;
}

unsigned IP_pref_b2v4_addr(ip_prefix_t *prefix) {
  return IP_addr_b2v4_addr( &(prefix->ip) );
}

/* range */

unsigned IP_rang_b2_space(ip_range_t *myrang) {
  /* hardwire to IPV4 for now */
  return IP_V4;
}

/* 
 * complex conversions (return void, set values through pointers *
 */
void IP_addr_b2v4(ip_addr_t *addrptr, unsigned *address) {
  *address = IP_addr_b2v4_addr(addrptr);
}

void IP_pref_b2v4(ip_prefix_t *prefptr, 
		   unsigned int *prefix, 
		   unsigned int *prefix_length) 
{
  *prefix        = IP_addr_b2v4_addr( &(prefptr->ip));
  *prefix_length = IP_pref_b2v4_len(prefptr);
}



void IP_pref_b2v6(ip_prefix_t *prefptr, 
		  ip_v6word_t *high, 
		  ip_v6word_t *low, 
		  unsigned int *prefix_length) 
{
  *high          = IP_addr_b2v6_hi( &(prefptr->ip));
  *low           = IP_addr_b2v6_lo( &(prefptr->ip));
  *prefix_length = IP_pref_b2v6_len(prefptr);
}


void IP_rang_b2v4(ip_range_t *myrang,
		  unsigned *begin, 
		  unsigned *end)
{
  *begin = IP_addr_b2v4_addr( &(myrang->begin));
  *end   = IP_addr_b2v4_addr( &(myrang->end));
}



/******** construct from raw values **********/

/******** address **********/
int IP_addr_v4_mk(ip_addr_t *addrptr,
		       unsigned addrval) {
  addrptr->space = IP_V4;
  addrptr->words[0] = addrval;
  addrptr->words[1] = addrptr->words[2] = addrptr->words[3] = 0;
  
  /* no real possibility of checking the syntax */
  return IP_OK;
}

int IP_addr_v6_mk(ip_addr_t *addrptr,
		       ip_v6word_t high,
		       ip_v6word_t low) {
  
  ip_v6word_t ff = 0xffffffff;

  addrptr->space = IP_V6;
  (addrptr->words[0]) =  (high >> 32) & ff;
  (addrptr->words[1]) =   high & ff ;
  (addrptr->words[2]) =  (low >> 32) & ff;
  (addrptr->words[3]) =   low  & ff;

  /* no real possibility of checking the syntax */
  return IP_OK;
}

/******** prefix **********/
int IP_pref_v4_mk(ip_prefix_t *prefix,
		       unsigned prefval, 
		       unsigned preflen) 
{
  if( preflen > 32 ) {
    die;
  }
  IP_addr_v4_mk(&(prefix->ip), prefval);
  prefix->bits = preflen;
  
  IP_pref_bit_fix( prefix ); /* never produce inconsistent prefixes */

  return IP_OK;
}

/******** range **********/
int IP_rang_v4_mk(ip_range_t *rangptr, 
		       unsigned addrbegin,
		       unsigned addrend)
{
  int err;

  if( (err=IP_addr_v4_mk( &(rangptr->begin), addrbegin)) == IP_OK ) {
    err=IP_addr_v4_mk( &(rangptr->end), addrend);
  }
  return err;
}

/**************************************************************************/


/**************************************************************************/
/*+ a2v4 == functions to convert the ascii representation into binary, 
 *          and then set the unsigned values at the pointers provided.
 *          
 +*/

/* Convert route string into numbers */
/* ipv4 */
int 
IP_pref_a2v4(const char *avalue, ip_prefix_t *pref,
	     unsigned *prefix, unsigned *prefix_length)
{
  
  int ret;
  
  if(NOERR(ret = IP_pref_e2b(pref, avalue))) {
    IP_pref_b2v4(pref, prefix, prefix_length);
  }
  return(ret);
}

/* ipv6 */
int 
IP_pref_a2v6(const char *avalue, ip_prefix_t *pref,
	     ip_v6word_t *high, ip_v6word_t  *low,
	     unsigned *prefix_length)
{
  int ret;
  
  if(NOERR(ret = IP_pref_e2b(pref, avalue))) {
    IP_pref_b2v6(pref, high, low, prefix_length);
  }
  return(ret);
}

/* Convert reverse domain string into numbers */
int 
IP_revd_a2v4(const char *avalue, ip_prefix_t *pref,
	     unsigned int *prefix, unsigned int *prefix_length)
{
  int ret;
  
  if(NOERR(ret = IP_revd_e2b(pref, avalue))) {
    IP_pref_b2v4(pref, prefix, prefix_length);
  }
  return(ret);
}

/* Convert ip addr string into numbers */
int 
IP_addr_a2v4(const char *avalue,ip_addr_t *ipaddr, unsigned int *address)
{
int ret;

 if(NOERR(ret = IP_addr_e2b(ipaddr, avalue))) {
   IP_addr_b2v4(ipaddr, address);
 }
 return(ret);
}

/* Convert inetnum attribute into numbers */
int 
IP_rang_a2v4(const char *rangstr, ip_range_t *myrang,
	     unsigned int *begin_in, unsigned int *end_in)
{
  int ret;
  
  if( NOERR(ret=IP_rang_e2b(myrang, rangstr))) {
#if 0    /* no IPv4 classful ranges anymore */
    if( IP_addr_e2b( &(myrang->begin), rangstr ) == IP_OK )
      if ((ret=IP_rang_classful( myrang , &(myrang->begin))) == IP_OK )
	;
#endif
    IP_rang_b2v4(myrang, begin_in, end_in);
  }
  
  return (ret);
}


/* *********************************************************************
   f2b - free numbers represented in ascii into a binary struct
********************************************************************* */

int
IP_addr_f2b_v4(ip_addr_t *addrptr, const char *adrstr) 
{
  unsigned address;

  if( ut_dec_2_uns(adrstr, &address) < 0 ) {
    return IP_INVARG;
  }
  
  return IP_addr_v4_mk(addrptr, address);
}

int
IP_rang_f2b_v4(ip_range_t *rangptr, const char *beginstr,  const char *endstr) 
{
  if(    IP_addr_f2b_v4( &(rangptr->begin), beginstr) != IP_OK
      || IP_addr_f2b_v4( &(rangptr->end),   endstr)   != IP_OK) {
    return IP_INVARG;
  }
  else {
    return IP_OK;
  }
}

int
IP_pref_f2b_v4(ip_prefix_t *prefptr, const char *prefixstr, const char *lengthstr) 
{
  if( IP_addr_f2b_v4( &(prefptr->ip), prefixstr) != IP_OK 
      || ut_dec_2_uns(lengthstr, &(prefptr->bits) ) < 0
      || prefptr->bits > IP_sizebits(prefptr->ip.space)) {
    return IP_INVARG;
  }
  IP_pref_bit_fix(prefptr); /* never create broken binary prefixes. */
  return IP_OK;
}


int
IP_addr_f2b_v6(ip_addr_t *addrptr, const char *msbstr, const char *lsbstr )
{
  ip_v6word_t high, low;
  char *endptr;
  
  errno = 0;
  high = strtoull(msbstr, &endptr, 10);
  if (((high == 0) || (high == ULLONG_MAX)) && (errno != 0)) {
      return IP_INVARG;
  }
  if (*endptr != '\0') {
      return IP_INVARG;
  }
  errno = 0;
  low = strtoull(lsbstr, &endptr, 10);
  if (((low == 0) || (low == ULLONG_MAX)) && (errno != 0)) {
      return IP_INVARG;
  }
  if (*endptr != '\0') {
      return IP_INVARG;
  }
 
  return IP_addr_v6_mk(addrptr, high, low);
}


int
IP_pref_f2b_v6(ip_prefix_t *prefptr, const char *msbstr, const char *lsbstr, const char *lengthstr) 
{
  if( IP_addr_f2b_v6( &(prefptr->ip), msbstr, lsbstr ) != IP_OK 
      || ut_dec_2_uns(lengthstr, &(prefptr->bits) ) < 0 
      || prefptr->bits > IP_sizebits(prefptr->ip.space)) {
    return IP_INVARG;
  }
  IP_pref_bit_fix(prefptr); /* never create broken binary prefixes. */
  return IP_OK;
}


/**************************************************************************/
/*+ convert the socket's idea of address into a binary range struct. 

  space    select the address type (and consequently struct type)
*/

int
IP_addr_s2b(ip_addr_t *addrptr, 
	    void      *addr_in, 
            int       addr_len)
{
  if( addr_len == sizeof(struct sockaddr_in) 
      && ((struct sockaddr_in *)addr_in)->sin_family == AF_INET ) {
    addrptr->space = IP_V4;
    addrptr->words[0] = 
      ntohl( ((struct sockaddr_in*)addr_in)->sin_addr.s_addr);
    
    /* set remaining limbs to zero */
    addrptr->words[1] = addrptr->words[2] = addrptr->words[3] = 0; 
    
  }
  else { /* unsupported family or invalid struct */
    die;
  }
  return IP_OK;
}

/**************************************************************************/
/*+converts the IP binary address (binaddr) to a string (ascaddr) 
   of at most strmax characters. Independent of the result
   (success or failure) it messes up the string.
+*/
int
IP_addr_b2a( ip_addr_t *binaddr, char *ascaddr, unsigned strmax ) 
{
  
  if(binaddr->space == IP_V4) {
    if (snprintf(ascaddr, strmax, "%d.%d.%d.%d",
		 ((binaddr->words[0]) & ((unsigned)0xff<<24))>>24,
		 ((binaddr->words[0]) & (0xff<<16))>>16,
		 ((binaddr->words[0]) & (0xff<<8))>>8,
		 ((binaddr->words[0]) & (0xff<<0))>>0
		 ) >= strmax) {
      /*die;  */ /* string too short */
      return IP_TOSHRT;
    }
  }
  else {
    /* IPv6 */
    unsigned tmpv6[4];
    int i;

    /* inet_* operates on network byte format numbers, so we need
   	to prepare a tmp. data with it */

    for(i=0; i<4; i++) {
	tmpv6[i] = htonl(binaddr->words[i]);
    }

    if( inet_ntop(AF_INET6, tmpv6, ascaddr, strmax) 
	== NULL ) {
      return IP_TOSHRT;
    }
  }
  return IP_OK;
}

/**************************************************************************/

/*+ convert a binary prefix back into ascii string at most strmax chars long 
+*/
int
IP_pref_b2a(ip_prefix_t *prefptr, char *ascaddr, unsigned strmax) 
{
  int strl;
  int err;

  if( (err=IP_addr_b2a (&(prefptr->ip), ascaddr, strmax)) != IP_OK) {
    /*die;  */ /* what the hell */
    return err;
  }
  strl = strlen(ascaddr);
  strmax -= strl;

  /* now strmax holds the space that is left */

  if( snprintf(ascaddr+strl, strmax, "/%d", prefptr->bits) >= strmax) {
    /* die;  */ /* error: string too short */
    return IP_TOSHRT;
  }
  return IP_OK;
}



/**************************************************************************/
/*+ convert a binary range back into ascii string at most strmax chars long 
+*/
int
IP_rang_b2a(ip_range_t *rangptr, char *ascaddr, unsigned strmax) 
{
  int strl=0;
  unsigned strleft;
  int err;

  strleft = strmax - strl;
  if( (err=IP_addr_b2a (&(rangptr->begin), ascaddr, strleft)) != IP_OK) {
    return err;
  }
  strl = strlen(ascaddr);
  
  strleft = strmax - strl;
  if( strleft < 5 ) {
    return IP_TOSHRT; 
  }
  strcat( ascaddr, " - " );
  strl += 3;

  strleft = strmax - strl;
  if( (err=IP_addr_b2a (&(rangptr->end), ascaddr+strl, strleft)) != IP_OK) {
    return err;
  }

  return IP_OK;
}

/**************************************************************************/
/*+ return the bitnum bit of the address, 
   COUNTING FROM THE TOP !!!!! , 
   starting with 0 for the *most significant bit*.
+*/
int
IP_addr_bit_get(ip_addr_t *binaddr, unsigned bitnum) {
  int bitval;
  int w,c;
  
  /* avoid unnecessary division */
  if( binaddr->space == IP_V4 ) {
    w = 0;
    c = bitnum;
  }
  else {
    w = bitnum / 32;
    c = bitnum % 32;
  }

  bitval = (binaddr->words[w] & (0x80000000 >> (c)));

  return (bitval != 0);

}

/**************************************************************************/
/*+ set the bitnum bit of the address to bitval, 
   COUNTING FROM THE TOP !!!!! , 
   starting with 0 for the *most significant bit*.
   Returns 1 in case the bit was changed and 0 in case no change occured
+*/
int
IP_addr_bit_set(ip_addr_t *binaddr, unsigned bitnum, unsigned bitval) {
  int w,c;
  int ret = 0;
  
  /* avoid unnecessary division */
  if( binaddr->space == IP_V4 ) {
    w = 0;
    c = bitnum;
  }
  else {
    w = bitnum / 32;
    c = bitnum % 32;
  }

  /* try to set the bit and indicate that bit was set or not */ 
  if ( bitval == 1 ) {
    if (binaddr->words[w] ^ (0x80000000 >> (c))) ret = 1;
    binaddr->words[w] |= (0x80000000 >> (c));
  }  
  else {
    if (binaddr->words[w] & (0x80000000 >> (c))) ret = 1;
    binaddr->words[w] &=  ~(0x80000000 >> (c));
  }  
  return(ret);  
}
/**************************************************************************/

/*+ this fixes a prefix by setting insignificant bits to 0 +*/
int
IP_pref_bit_fix( ip_prefix_t *prefix ) 
{
int err = IP_OK;


  if( prefix->ip.space == IP_V4 ) {
    ip_limb_t mask = 0xffffffff;
    
    /* shorthand for ipv4 */
    
    /* Shifting out by 32 bits does NOT turn all bits into 0... */
    if( prefix->bits < 32 ) {
      /* if bits outside the prefix length exist - this is an error */
      if ( prefix->ip.words[0] & (mask >> prefix->bits)) err = IP_INVBIT;
      /* correct the prefix anyway */
      prefix->ip.words[0] &= ~(mask >> prefix->bits);
    }
  }
  else {
    unsigned i;
    for(i=prefix->bits; i < IP_sizebits(prefix->ip.space) ; i++) {
      if(IP_addr_bit_set( & prefix->ip, i, 0)) err = IP_INVBIT;
    }
  }
  return err;
}


/**************************************************************************/

/*+ compares two IP addresses up to the bit # len, 
   returns 0 if equal, 1 if ptra greater, -1 if ptrb greater.
   
   It is the responsility of the caller to ensure that both addresses
   are from the same IP space.

   This is pretty slow; it is used in the searches of the radix tree,
   so it might be good to optimise this.
+*/

int 
IP_addr_cmp(ip_addr_t *ptra, ip_addr_t *ptrb, unsigned len)
{
  unsigned a,b,i;

  for(i=0; i<len; i++) {
    a=IP_addr_bit_get(ptra, i);
    b=IP_addr_bit_get(ptrb, i);
    if( a != b ) {
      if( a > b ) return 1;
      else return -1;
    }
  }
  return 0;
}


/*+ checks if an IP address is contained within the prefix 
  returns 1 if it is, 0 otherwise

  It is the responsility of the caller to ensure that both address
  and prefix are from the same IP space.
+*/
int 
IP_addr_in_pref(ip_addr_t *ptra, ip_prefix_t *prefix)
{
  return (IP_addr_cmp( ptra, & prefix->ip, prefix->bits) == 0);
}

/*+ checks if an IP address is contained within the range
  returns 1 if it is, 0 otherwise

  It is the responsility of the caller to ensure that both address
  and range are from the same IP space.
  
  works only for IPv4
+*/

int IP_addr_in_rang(ip_addr_t *ptra, ip_range_t *rangptr)
{
/*  if( rangptr->end.space == IP_V4 ) {
    return (  rangptr->begin.words[0]  <=  ptra->words[0]
	      && rangptr->end.words[0]  >=  ptra->words[0] );
  }
  else {
*/
    return( IP_addr_cmp(ptra, &rangptr->begin, 
			IP_sizebits(rangptr->end.space)) >= 0 /* adr >= begin */
	    && IP_addr_cmp(ptra, &rangptr->end, 
			   IP_sizebits(rangptr->end.space)) <= 0  /* adr <= end */
	    );
/*  }*/
}

/**************************************************************************/

/*+ calculate the span of a range == size - 1 +*/

ip_rangesize_t 
IP_rang_span( ip_range_t *rangptr )
{
  /* IPv4: */
  dieif( rangptr->end.space != IP_V4 );
  
  return rangptr->end.words[0] - rangptr->begin.words[0];
}


/**************************************************************************/

/*+ 
this is a shorthand notation to pull out the first word of the address.
it is defined for the scope od the following functions
+*/
#define ad(which) (rangptr->which)

/**************************************************************************/
/*+ Decomposes a binary range into prefixes and appends them to the list.
   Allocates prefix structures and list elements, they must be freed 
   after use.

   returns a bitmask of prefix lengths used.
+*/
unsigned
IP_rang_decomp(ip_range_t *rangptr, GList **preflist)
{
unsigned            prefmask=0;
register int        slash=0;
register unsigned   c_dif, blk, ff;
ip_range_t  workrange;
ip_addr_t   workbegin;
ip_addr_t   workend;
ip_prefix_t *prefptr;

 dieif( rangptr->begin.space != IP_V4 );
 
 if( ad(begin).words[0] > ad(end).words[0] ) {   /* has gone too far */
   return 0; 
 }
 
 if( ad(begin).words[0] == ad(end).words[0] ) { /* an IP == a /32 (IPv4) */
   prefmask |= 1;
   prefptr = (ip_prefix_t *)UT_calloc(sizeof(ip_prefix_t), 1);
   prefptr->ip = ad(begin);
   prefptr->bits = 32;
   
   *preflist = g_list_append( *preflist, prefptr );
   
   return prefmask;
 }
 
  c_dif = ad(end).words[0] - ad(begin).words[0];
  
  /* initialize work vars */
  
  workbegin = ad(begin);
  workend = ad(end);
  
  /* now find the biggest block fitting in this range */
  /* i.e. the first 2^n number smaller than c_dif */
  
  /* the loop would not work for /0 (some stupid queries may have that) */
  /* so this must be checked for separately */
  
  if( c_dif == 0xffffffff ) {
    /* they are already set to 0.0.0.0 - 255.255.255.255 */
    /* leave them alone.  */
    blk = 0;
    slash = 0;
  }
  else {
    
    c_dif += 1;     /* was not done earlier to protect from overflow */

    for(slash=1; 
	slash<32 && ((blk=((unsigned)0x80000000>>(slash-1))) & c_dif) == 0; 
	slash++) {}

    /* clear all digits in a and b under the blk one. */
    ff=blk-1;

    workbegin.words[0] = (workbegin.words[0] + ff) & ~ff;
    
    workend.words[0] = (workend.words[0] + 1) & ~ff;
  }
  
  if( workbegin.words[0] != workend.words[0] ) {
    prefmask |= blk;
    prefptr = (ip_prefix_t *)UT_malloc(sizeof(ip_prefix_t));
    prefptr->ip = workbegin;
    prefptr->bits = slash;
    
    *preflist = g_list_append( *preflist, prefptr );
  }

  if( ad(begin).words[0] != workbegin.words[0] ) {
    workrange.begin = ad(begin);

    workbegin.words[0] -= 1;
    workrange.end   = workbegin;

    prefmask |= IP_rang_decomp( &workrange, preflist );
  }
  
  /* here we must protect from decomposition of  
   * 255.255.255.255 - 255.255.255.255 in case the range 
   * 0.0.0.0 - 255.255.255.255 is considered. Hence the slash>0 condition. 
   */
  
  if( workend.words[0] <= ad(end).words[0] && slash > 0) {
    workrange.begin = workend;
    workrange.end   = ad(end);

    prefmask |= IP_rang_decomp( &workrange, preflist );
  }
  
  return prefmask;
 
}


/***************************************************************************/

/*+ Similar name, slightly different code, totally different functionality.

   finds the smallest canonical block encompassing the whole given range, 
   then MODIFIES the range pointed to by the argument 
   so that it's equal to this block.

+*/

void IP_rang_encomp(ip_range_t *rangptr)
{
  int         slash=0;
  unsigned    c_dif, blk, ff, t_dif;
  ip_addr_t   workbegin;
  ip_addr_t   workend;

  dieif( rangptr->begin.space != IP_V4 );

    c_dif = ad(end).words[0] - ad(begin).words[0];
    
    /* now find the biggest block fitting in this range */
    /* i.e. the first 2^n number smaller than c_dif */
    
    /* the loop would not work for /0 (some stupid queries may have that) */
    /* so this must be checked for separately */
    
    if( c_dif > 0x80000000 ) {
      slash = 0;
      ff = 0xffffffff;
      blk = 0;
      
      workbegin = workend = ad(begin);
      workbegin.words[0] = 0;
      workend.words[0] = ff;
    }
    else {
      
      do {
	c_dif += 1;
	
	/* find the smallest block ENCOMPASSING c_dif. */
	/* this implies a loop from the bottom up */
	
	for(slash=32; 
	    slash>1 && (blk=((unsigned)0x80000000>>(slash-1))) < c_dif; 
	    slash--) {}
	
	ff=blk-1;
	
	/* clear all digits in workbegin under the blk one. */
	
	workbegin = ad(begin);
	workbegin.words[0] = workbegin.words[0] & ~ff;
	
	/* see if it has not made the difference larger than blk,  */
	/* retry if so */
	
	t_dif = c_dif;
	c_dif = ad(end).words[0] - workbegin.words[0];
	
      } while( c_dif >= t_dif );
      
      /* set the endpoint to workbegin + blocksize - 1 */
      /* which amounts to + ff */
      
      workend = ad(begin);
      workend.words[0] = workbegin.words[0] + ff;
    }
    
    
    /* set the range to new values */
    
    rangptr->begin = workbegin;
    rangptr->end   = workend;
}

/***************************************************************************/
/*+ sets a range equal to a prefix +*/

int
IP_pref_2_rang( ip_range_t *rangptr, ip_prefix_t *prefptr )
{
  int shift;
  int i;
    
  ad(begin) = ad(end) = prefptr->ip;
  
  /* IPv6 is a bit more complicated, as four words are involved */
  
  /* additional problem: shifting right by >=32 is equal to shifting by 0,
     so it does not change any bits */
  /* solution: don't touch those words */
  
  for(i=0; i<4; i++) {
    
    if( prefptr->bits < 32*(1+i) ) {
      shift = prefptr->bits < 32 + (i-1) * 32 
	? 0 : (prefptr->bits % 32) ;
      ad(end).words[i] |= (0xffffffffU >> shift);
    }
    
    if( prefptr->ip.space == IP_V4) {
      break; /* do only first word for IPv4 */
    }	
  }
  return IP_OK;
}

#undef ad

/* IP_OK assumes a classful range!!! */
int
IP_rang_2_pref( ip_range_t *rangptr, ip_prefix_t *prefptr )
{
  /* begin & end = mask */
  /* count contiguous '1' bits starting from the highest digit -> prefix length */
  /* IP address = begin */

  ip_addr_t res;
  int i = 0;
  int k = 0;
  unsigned len = 0;

  prefptr->ip = rangptr->begin;

  for (i=3; i >=0; i--)
  {
    res.words[i] = rangptr->begin.words[i] & rangptr->end.words[i];
  }

  for (i=3; i >=0; i--) 
  {
    for (k = (1 << 31) ; k >= 1; ( k >> 1) ) 
    {
      if ( (res.words[i] & k ) == k ) 
      {
        len ++;
      } 
      else 
      {
        break;
      }
    }
  }

  prefptr->bits = len;
  return IP_OK;

}

/***************************************************************************/

/*+ 
   This is to parse a classfull address into a range.

   Takes the address by pointer from addrptr and puts the result
   at rangptr. 

   Throws error if the address does not fall into any of the 
   classfull categories 

+*/

int
IP_rang_classful( ip_range_t *rangptr, ip_addr_t *addrptr)
{
int i;
unsigned b[4];

  if( addrptr->space != IP_V4 ) {
    /* it's IPv6. There are no classful ranges or anything like that. */
    die;
  }
  
  rangptr->begin = *addrptr;
  rangptr->end.space = IP_V4;

  /* initisalise end to zero */
  for(i=0; i<IPLIMBNUM; i++) {
    rangptr->end.words[i] = 0;
  }
  
  /* assume it's at least a valid IP. let's try different classes now */
	      
  /* we could have used a union here, but it would not work on */
  /* low endians. So byte by byte copying to and from an array. */
  
  for(i=0; i<4; i++) {
    b[i] = ( rangptr->begin.words[0] & (0xFF << i*8) ) >> i*8;
  }
  
  if( b[3] >= 1 && b[3] < 128 
      && b[2] == 0 && b[1] == 0 && b[0] == 0 ) {
    b[2]=b[1]=b[0]=255;
  }
  else if( b[3] >= 128 && b[3] < 192 
	   && b[1] == 0 && b[0] == 0 ) {
    b[1]=b[0]=255;
  }
  else if( b[3] >= 192 && b[3] < 224 
	   &&  b[0] == 0 ) {
    b[0]=255;
  }
  else if( b[3] >= 224 && b[3] < 255 ) {
    /* just leave it, make it a /32, i.e. begin == end */
    /* EMPTY */;
  }
  else {
    /* Leave it and make it a /32 */
    /* This is AGAINST the rule! but we have some junk  */
    /* so we have to compensate for it. */
    /* EMPTY */;
  }
  
  /* copy the (now - modified) bytes into the end of range */
  for(i=0; i<4; i++) {
    rangptr->end.words[0] |= (b[i] << i*8);
  }
  
  return IP_OK;
}


/***************************************************************************/
/*+ 
  Trying to be smart :-) and convert a query search term into prefix(es),
  regardless of whether specified as IP address, prefix or range.
 
  justcheck - if just checking the syntax (justcheck == 1), 
     then the prefixes are freed before the function returns,
     otherwise it is the responsibility of the caller to free the list.
     
+*/

int
IP_smart_conv(char *key, 
	      int justcheck, 
	      int encomp, 
	      GList **preflist, 
	      ip_exp_t expf,
	      ip_keytype_t *keytype
	      )
{
  int free_it;
  int err=IP_OK;      /* let's be optimistic :-) */
  ip_prefix_t *querypref;

  /* if just checking the syntax (justcheck == 1), 
     then free_it = 1, 
     else 0, but may be modified later (in range conversion)
  */

  free_it = justcheck;
  
  querypref = (ip_prefix_t *)UT_malloc(sizeof(ip_prefix_t));
  
  /*XXX inconsistent perfix/length is excused. Later we will warn people */
  err = IP_pref_t2b(querypref, key, expf);
  if (NOERR(err)) {
    *keytype = IPK_PREFIX;

    if( justcheck == 0) {
      *preflist = g_list_append(*preflist, querypref);
    }
  } 
  else {
    /* not a prefix.  */
    /* Maybe an IP ? */
    err = IP_addr_t2b( &(querypref->ip), key, expf);
    if (NOERR(err)) {
      
      *keytype = IPK_IP;

      /*convert to a /32 or /128*/
      querypref->bits =  IP_sizebits(querypref->ip.space);

      if( justcheck == 0) {
	*preflist = g_list_append(*preflist, querypref);
      }
    }
    else {    
      /* hm, maybe a range then ? */
      ip_range_t myrang;
      
      /* won't use the querypref anymore, mark it for freeing later */
      free_it = 1;
      
      err = IP_rang_t2b(&myrang, key, expf);
      if( NOERR(err)) {
	/* Wow. Great.  */
	
	*keytype = IPK_RANGE;

	/* sometimes (exless match) we look for the first bigger(shorter)  */
	/* prefix containing this range. */
	
	if( encomp ) {
	  IP_rang_encomp(&myrang);
	}
	/* OK, now we can let the engine happily find that there's just one */
	/* prefix in range */
	
	if( justcheck == 0) {
	  IP_rang_decomp(&myrang, preflist);
	}
      }
      else {
	*keytype = IPK_UNDEF;
	err = IP_INVARG; /* "conversion error" */
      }
    }
  }
  
  if( free_it ) {
    UT_free(querypref);
  }
  
  return err;
}


/* convert whatever comes into a range */
int
IP_smart_range(char *key,
	       ip_range_t *rangptr, 
	       ip_exp_t expf,
	       ip_keytype_t *keytype
	       )
{
  int  err=IP_OK;  
  GList    *preflist = NULL;

  /* first : is it a range ? */

  if( NOERR(err = IP_rang_t2b(rangptr, key, expf))) {
      *keytype = IPK_RANGE;
  }
  else {
      /* OK, this must be possible to convert it to prefix and from there
	 to a range. */
      if( NOERR(err = IP_smart_conv(key, 0, 0, &preflist, expf, keytype))) {
	  
	  dieif( g_list_length(preflist) != 1 );
	  
	  dieif(!NOERR(IP_pref_2_rang( rangptr, g_list_first(preflist)->data )));
      }
  }
  
  wr_clear_list( &preflist );

  return err;
}

/* determine if the prefix is contained in the prefix range */
int IP_pref_in_prefrang(ip_prefix_t *prefix, ip_prefix_range_t *prefixrange)
{
  /* prefix is contained in the prefix range, if 
       prefix is child of prefix.range (prefix) 
  AND  preifxrange.n <= prefix.bits <= prefixrange.m    
  */

  ip_range_t temp_range; 

  if (IP_pref_2_rang(&temp_range, prefix) == IP_OK )
  {
    return ( IP_addr_in_pref(&(temp_range.begin), &(prefixrange->prefix))
      && IP_addr_in_pref(&(temp_range.end), &(prefixrange->prefix))
      && (prefix->bits >= prefixrange->n)
      && (prefix->bits <= prefixrange->m)
      && (prefixrange->n <= prefixrange->m)
      && (prefixrange->prefix.bits <= prefixrange->n)
      && (prefixrange->prefix.bits <= prefixrange->m)
     );
    /* the last three conditions are a temporary workaround to catch invalid prefix ranges */
  }
  return IP_INVPRN;
}

/* true if overlap, false otherwise */
int IP_rang_overlap(ip_range_t *range1, ip_range_t *range2)
{
  int begin1, end1, begin2, end2;

  IP_rang_b2v4(range1, &begin1, &end1);
  IP_rang_b2v4(range2, &begin2, &end2);
  
  return ( ( (begin1 < begin2) && (end1 < end2) && (end1 >= begin2) )
         || ((begin1 > begin2) && (end1 > end2) && (begin1 <= end2) )
         );

}

