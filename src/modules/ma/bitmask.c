/***************************************
  $Revision: 1.1 $

  bitmask (ma) - bitmask.c - library for manipulating fixed size bitmasks.

  Status: NOT REVUED, TESTED, INCOMPLETE

  Design and implementation by: Marek Bukowy, Chris Ottrey.

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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>

#include <string.h>

int MA_isset(mask_t d, unsigned b) {
  return( 
 (d.data[b / MASK_BITS_BASESIZE] &(1 <<(b % MASK_BITS_BASESIZE)))
   > 0);
}

void MA_set(mask_t *m_ptr, unsigned b, unsigned v) {
  assert(b < MASK_MAX);
  assert(v == 0 || v == 1);

  if(v == 1)
    m_ptr->data[b / MASK_BITS_BASESIZE] |=(1 <<(b % MASK_BITS_BASESIZE));
  else
    m_ptr->data[b / MASK_BITS_BASESIZE] &= ~(1 <<(b % MASK_BITS_BASESIZE));
}

void MA_clear(mask_t *m_ptr) {
  memset(m_ptr, 0, sizeof(mask_t));
} /* MA_clear() */


/* set bits in dptr. variable number of args, terminated by MA_END
   
   MA_END is -1, so the arguments are of type signed int
 */
mask_t MA_new(int n,...) {
  va_list         ap;
  mask_t d;

  MA_clear(&d);

  /* cover the "empty mask" case => MA_new(MA_END) invoked */
  for ( va_start(ap, n); n != MA_END; n = va_arg(ap, int) ) {
      MA_set(&d, (unsigned) n, 1);
  }
  va_end(ap);

  return d;
}


void MA_prt(mask_t m) {
  unsigned i;

  for (i = 0; i < MASK_MAX; i++) {
    printf("%d", MA_isset(m, i));
  }

  printf("\n");

  for (i = 0; i < MASK_BITS_WORDS; i++) {
    printf("0x%0*X ", 2 * (int)sizeof(MASK_BITS_BASETYPE), m.data[i]);
  }

  printf("\n");
}

/* Perform a logical AND on two masks.
   Author: ottrey
   Date: Tue Jul  6 13:28:24 CEST 1999
   NB: This operation could/should be done a word at a time?
*/
mask_t MA_and(mask_t a, mask_t b) {
  mask_t c;
  unsigned i;

  for (i=0; i < MASK_BITS_WORDS; i++ )  {
    c.data[i] = a.data[i] & b.data[i];
  }
  return c;
} /* MA_and() */

/* Perform a logical XOR on two masks.
   Author: ottrey
   Date: Thu Jul  8 14:50:14 CEST 1999
   NB: This operation could/should be done a word at a time?
*/
mask_t MA_xor(mask_t a, mask_t b) {
  mask_t c;
  unsigned i;

  for (i=0; i < MASK_BITS_WORDS; i++ )  {
    c.data[i] = a.data[i] ^ b.data[i];
  }

  return c;

} /* MA_xor() */

/* Perform a logical OR on two masks.
   Author: ottrey
   Date: Thu Jul  8 16:34:34 CEST 1999
   NB: This operation could/should be done a word at a time?
*/
mask_t MA_or(mask_t a, mask_t b) {
  mask_t c;
  unsigned  i;

  for (i=0; i < MASK_BITS_WORDS; i++ )  {
    c.data[i] = a.data[i] | b.data[i];
  }

  return c;

} /* MA_or() */

/* Perform a logical NOT operation on a mask.
   Author: marek
   Date: Fri Jan 14 17:15:00 MET 2000
   NB: This operation is done a word at a time.
*/
mask_t MA_not(mask_t a) {
  mask_t c;
  unsigned i;
  
  for (i=0; i < MASK_BITS_WORDS; i++ )  {
    c.data[i] = ~a.data[i];
  }
  return c;
}

/* Counts the number of bits set. */
int MA_bitcount(mask_t m) {
  unsigned i;
  int count=0;

  for (i=0; i < MASK_MAX; i++) {
    count += MA_isset(m, i);
  }

  return count;

} /* MA_bitcount() */

/* Convert the bitmap to a string comprising of tokens from an array of tokens.
   Author: ottrey
   Date: Tue Jul  6 13:28:24 CEST 1999
*/
char *MA_to_string(mask_t mask, char * const *tokens) {
  char str_buf[STR_L];
  int count;
  unsigned i;
  int length;

  count=0;
  strcpy(str_buf, "{");
  for (i=0;  tokens[i] != NULL; i++) {
    if ( MA_isset(mask, i) != 0) {
      strcat(str_buf, tokens[i]);
      strcat(str_buf, ",");
      count++;
    }
  }
  if (count == 0) {
    strcat(str_buf, "NULL ");
  }
  length = strlen(str_buf);
  str_buf[length-1] = '}';

  return UT_strdup(str_buf);

} /* MA_to_string() */

#ifdef MODULE_TEST
void 
main() {
  mask_t  d;

  d = MA_new(1, 4, 56, 3, 5, 7, 19, MA_END);

  MA_prt(d);

  MA_set(&d, 3, 0);
  MA_set(&d, 7, 0);
  MA_set(&d, 9, 0);
  MA_set(&d, 19, 0);

  MA_prt(d);
}
#endif
