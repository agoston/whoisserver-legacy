/***************************************
  $Revision: 1.1 $

  stubs.h - header file with things defined just to provisionally fix the TBDs.

  Status: NOT REVUED, TESTED, INCOMPLETE

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

/* stubs: just defined to let the other thing compile. Use #define
NOSTUBS to disable it. */

#ifndef _STUBS_H
#define _STUBS_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* config.h - comes from configure. */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* this is to allow user stubs in the code */
/* it will trigger an error when NOSTUBS is set */

#ifndef NOSTUBS
#define STUB(a) do{a;}while(0)
#else
#define STUB(a) hey_here_is_a_stub_left=%^&*&@@@;
#endif

/* now these are predefined ones... */

#ifndef NOSTUBS

/* for memcmp and strcmp */
#define CMP_EQUAL 0

/* stub for those compilers (wrrrrrrr....) that can't do (struct != struct) */
#define STRUCT_EQUAL(a,b) (memcmp(&a, &b, sizeof(a)) == CMP_EQUAL)

/* this prints the filename and line number where it was placed,
   and then commits suicide (read: segfault) by writing to address NULL.
   This in normal conditions generates a coredump for later analysis.
   
   isdigit() is used to avoid warnings from lint. 
   do..while is to allow treating this macro as a single instruction 
   (eg in an if).
*/

#define die   do{ \
	fprintf(stderr,"died: +%d %s\n",__LINE__, __FILE__);\
	*((int*)NULL)=0; \
	} while(isdigit('a'))		

#define dieif(a) if(a) { \
        fprintf(stderr,"died on "#a" in: +%d %s\n",__LINE__, __FILE__);\
	*((int*)NULL)=0; \
	}


#define SQL_TBLNAM_MAX 32 


/* struct to hold the table name. 
   Wrapped in a struct to make it behave like a normal object (ref by
value)

   in fact, this type should be made engine-dependent.
*/

typedef struct {              
  char val[SQL_TBLNAM_MAX];
} sql_tblnam_t;

/* who knows what key type we will really have in other engines */
typedef long sql_key_t;

#define SQ_NOKEY 0L


#ifdef HAVE_STRSEP
/* good */
#else 
#  if defined(HAVE_STRTOK_R) && !defined(HAVE_STRSEP)
/* emulate strsep with strtok_r 
   by making first arg to strtok_r point to the last 

char *
strsep(char **stringp, const char *delim)
{
  return strtok_r( *stringp, delim, stringp);
}
*/
#define strsep(a,b) strtok_r( *(a), (b), (a) )

#  else
#  error "must have strsep or strtok_r"
#  endif
#endif


#endif /* NOSTUBS */
#endif /* _STUBS_H */
