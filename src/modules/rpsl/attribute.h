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

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

typedef struct {
    char *name;              /* "person", "descr", etc. */
    int id;                  /* identifier for this attribute, e.g. A_RT, 
                                A_NA, etc. */
    char *altname;           /* NULL, or "localas" for "local-as" */
    char *xmlname;           /* unique identifier for ambiguous attributes,
                                e.g. "members", "member-of", "status" */
    char *code;              /* "pn", "de", etc. */
    int syntax_offset;       /* index into syntax_tab[] of syntax */
    int is_lookup;           /* flag whether is a lookup attribute or not */
    int is_inverse;          /* flag whether is an inverse attribute or not */
    int is_primary;          /* flag whether is a primary key or not */
    int is_list;             /* flag whether is a list-valued attribute */
    int is_ripe_list;        /* flag whether is a RIPE-styled attribute */

    // unused; commented out by agoston, 2009-08-07
    //char *foreign_code;      /* code of attribute referenced, e.g. "mt" for
    //                            "mnt-by" or "mnt-lower" */

    int foreignkey_class_offset;       /* index into class_tab[] for class definition */
} attribute_t;

/* functions */
const attribute_t *attribute_lookup(const char *name, gboolean *is_ambiguous);
const attribute_t *attribute_lookup_by_offset(int offset);

#endif /* ATTRIBUTE_H */
