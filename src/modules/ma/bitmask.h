/***************************************
  $Revision: 1.1 $

  bitmask (ma) - bitmask.h - header file for bitmask library

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


#ifndef READ_BITMASK
#define READ_BITMASK

#define MASK_BITS_BASETYPE unsigned int
#define MASK_BITS_BASESIZE (8 * sizeof (MASK_BITS_BASETYPE))

/* size of the mask: to keep things simple, there is only one size
   that must suit everybody. The requirements are listed below.

   Marek: Aspects. Not many so far, about 30-40 possible.
   Chris: Query types, 78 at the moment, probably less than 128 at the end
*/


#define MASK_MAX 128
#define MASK_BITS_WORDS    (MASK_MAX / MASK_BITS_BASESIZE + 1)

typedef struct {
  MASK_BITS_BASETYPE  data[MASK_BITS_WORDS];
} mask_t;


#define MA_END (-1)

#ifdef __cplusplus
extern "C" {
#endif

void MA_free(mask_t *m);
int MA_isset(mask_t d, unsigned b);
void MA_set(mask_t *m_ptr, unsigned b, unsigned v);
void MA_clear(mask_t *m_ptr);
mask_t MA_new(int n,...);
void MA_prt(mask_t m);
mask_t MA_and(mask_t a, mask_t b);
mask_t MA_xor(mask_t a, mask_t b);
mask_t MA_or(mask_t a, mask_t b);
mask_t MA_not(mask_t a);
int MA_bitcount(mask_t m);
char *MA_to_string(mask_t mask, char * const *tokens);

#define MA_eq(a,b) (memcmp( &(a), &(b), sizeof(mask_t))==0)

#ifdef __cplusplus
}

#endif


#endif /* READ_BITMASK */
