/******************
  Copyright (c) 2002                              RIPE NCC

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
#ifndef AA_H
#define AA_H

#include "iproutines.h"

/* DENY = can't get nrtm stream
 * PUBLIC = can get public nrtm stream (person, role, mntner, org, irt objects removed)
 * FULL = can get full nrtm stream */
typedef enum {
	AA_MIRROR_DENY,
	AA_MIRROR_PUBLIC,
	AA_MIRROR_FULL
} aa_mirror_right;

void AA_init();
void AA_load();
int AA_can_ripupdate(ip_addr_t *address, char *source);
aa_mirror_right AA_can_mirror(ip_addr_t *address, char *source);

#endif
