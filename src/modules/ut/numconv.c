/***************************************
  $Revision: 1.9 $

  Utilities (ut). numconv.c  - library for fast numerical conversions
                               (to[?]/from string).

  Status: NOT REVUED, TESTED, 

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
#include <ctype.h>

/* converts QUICKLY a string to integer value, stops when encounters a 
   whitespace. All it expects is an UNSIGNED INTEGER value, 
   so in case it gets a non-digit (and non-whitespace) character it returns -1 
   
   if everything looks fine, it return 0. The converted unsigned int is placed
   into *result;
*/

int
ut_dec_2_uns(const char *cpy, unsigned *result) 
{
register unsigned val=0;
register const char *ch=cpy;

  while( *ch != '\0') {

    if ( ! isdigit(* (unsigned char *) ch)) {       /* make the &*^%#@$%^ */
      if ( isspace(* (unsigned char *) ch)) {       /* gcc happy */
        break;
      }
      else {
        return -1;
      }
    }

    val *= 10;
    val += ( *ch - '0' );
    ch++;
  }

 *result = val;
 return 0;
}
