/***************************************
  $Revision: 1.1 $

  Radix payload (rp) - user level functions for storing data in radix trees

  rp_convert = conversion helpers for RX_asc_node and UD module.

  Status: NOT REVIEWED, TESTED
  
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

#include "rip.h"

/* logging */
extern LG_context_t *rp_context;

/* return the family of tree to be used for the given attribute.
   die if not defined.
*/
rx_fam_t RP_attr2fam( rp_attr_t type )
{
  rx_fam_t res = DF_attrcode_radix_family( type );

  dieif(res == -1);
  
  return res;
}
  


/* 
   returns 1 if the given space may appear for a given attribute 
*/
int RP_attr2spc(rp_attr_t type, ip_space_t space)
{
  char *loadv4 = DF_attrcode_radix_load_v4( type );
  char *loadv6 = DF_attrcode_radix_load_v6( type );
  char *loadqry = ( space == IP_V4 ) ? loadv4 : loadv6;

  return ( loadqry != NULL ); /* 1 if defined, 0 otherwise */
}


int
RP_asc2uni(const char *astr,       /*+ string prefix/range/IP/inaddr +*/
	   rp_attr_t  attr,
	   rp_uni_t   *uni)        /* destination pointer */
{
  int   conv;
  rx_fam_t   fam_id = RP_attr2fam( attr );
  switch( attr ) {
  case A_IN:
    conv = IP_rang_e2b(&(uni->u.in), astr);
    break;
  case A_RT:
  case A_I6: 
  case A_R6:
    conv = IP_pref_e2b(&(uni->u.rt), astr);  
    break;
  case A_DN:
    conv = IP_revd_e2b(&(uni->u.rt), astr);
    break;
  default:
    /*    die; / * shouldn't have got here */
    conv = IP_INVARG;
  }

  if(NOERR(conv)) {
    uni->fam = fam_id;
    
    if( fam_id == RX_FAM_RT ) {
      uni->space = IP_pref_b2_space( &(uni->u.rt) );
    } else {  /* RX_FAM_IN */
      uni->space = IP_rang_b2_space( &(uni->u.in) );
    }
  }

  return conv;
}



/* Function to fill data for radix tree */
/* returns error if string is not valid, also for reverse domains */
int
RP_asc2pack(rp_upd_pack_t *pack, rp_attr_t type, const char *string)
{
  int err;

  pack->type = type;
 
  LG_log(rp_context, LG_DEBUG,
	    "RP_asc2pack: converted attr %s: %s to pack at %08x",
	    DF_get_attribute_code(type), string, pack );
				  
 
  err = RP_asc2uni(string, type, &(pack->uni) );
  
  if( type == A_DN && err == IP_OK) {
    /* Check if it is an in-addr.arpa domain, set domain ptr only then */
    pack->d.domain = string;
  }

  return err;
}


/* construct -K contents 
 *
 * MT-NOTE: returns POITNER TO STATIC MEMORY !!!
 * 
 * ASSUMES ONLY ONE UPDATE THREAD RUNNING.
 */
void rp_make_short(rp_upd_pack_t *pack, char **ptr, unsigned *len) 
{
#undef STR_L
#define STR_L 2048
  static char buf[STR_L];
  char prefstr[IP_PREFSTR_MAX];
  char rangstr[IP_RANGSTR_MAX];

  switch( pack->type ) {
  case A_R6:
    dieif( IP_pref_b2a( &(pack->uni.u.rt), prefstr, IP_PREFSTR_MAX) != IP_OK );
    snprintf(buf, STR_L, "route6:   \t%s\norigin:   \t%s\n", prefstr, pack->d.origin);
    break;
  case A_RT: 
    dieif( IP_pref_b2a( &(pack->uni.u.rt), prefstr, IP_PREFSTR_MAX) != IP_OK );
    snprintf(buf, STR_L, "route:    \t%s\norigin:   \t%s\n", prefstr, pack->d.origin);
    break;
  case A_I6:
    dieif( IP_pref_b2a( &(pack->uni.u.rt), prefstr, IP_PREFSTR_MAX) != IP_OK );
    snprintf(buf, STR_L, "inet6num: \t%s\n", prefstr);
    break;
  case A_IN:
    dieif( IP_rang_b2a( &(pack->uni.u.in), rangstr, IP_RANGSTR_MAX) != IP_OK );
    snprintf(buf, STR_L, "inetnum:  \t%s\n", rangstr);
    break;
  case A_DN:
    snprintf(buf, STR_L, "domain:   \t%s\n", pack->d.domain );
    break;
  default:
    /* FALLTHROUGH */
    ;
  }

  *ptr = buf;
  *len = strlen(buf);
}

/***************** set the values in rx_*_data thingies ***************/
int RP_pack_set_orig(rp_attr_t  attr, rp_upd_pack_t *pack, const char *origin)
{
  pack->d.origin = origin;
  return(IP_OK);
  /* ignore attr */
}

/* those are just interfacing to 
 * functions to convert to IP binary format and retain raw values 
 */
int RP_pack_set_type(rp_attr_t  attr, rp_upd_pack_t *pack)
{
  pack->type = attr;
  pack->uni.fam = RP_attr2fam( attr );
  return(IP_OK);
}

int RP_pack_set_pref4(rp_attr_t  attr, const char *avalue, rp_upd_pack_t *pack,
		       unsigned *prefix, unsigned *prefix_length)
{
int ret;
  if(NOERR(ret = IP_pref_a2v4(avalue, &(pack->uni.u.rt), prefix, prefix_length))){
    pack->uni.space = IP_V4;
    RP_pack_set_type(attr, pack);
  }
  return(ret);
}

int RP_pack_set_revd(rp_attr_t  attr, const char *avalue, rp_upd_pack_t *pack)
{
  dieif(IP_revd_a2b(&(pack->uni.u.rt), avalue) != IP_OK); /* assuming correctness checked */
  pack->d.domain = avalue;
  pack->uni.space = IP_pref_b2_space( &(pack->uni.u.rt) );
  RP_pack_set_type(attr, pack);
  return(IP_OK);
}


int RP_pack_set_pref6(rp_attr_t  attr, const char *avalue, rp_upd_pack_t *pack,
		       ip_v6word_t *high, ip_v6word_t *low, unsigned *prefix_length)
{
int ret;
  if(NOERR(ret = IP_pref_a2v6(avalue, &(pack->uni.u.rt), high, low, prefix_length))){
    pack->uni.space = IP_V6;
    RP_pack_set_type(attr, pack);
  }
  return(ret);
}

int RP_pack_set_rang(rp_attr_t  attr, const char *avalue, rp_upd_pack_t *pack,
		      unsigned *begin_in, unsigned *end_in)
{
int ret;
  if(NOERR(ret = IP_rang_a2v4(avalue, (ip_range_t *) &(pack->uni.u.in),
	       begin_in, end_in))) {
    pack->uni.space = IP_V4;
    RP_pack_set_type(attr, pack);
  }
  return(ret);
}


/***************************************************************************/

/***************************************************************************/
