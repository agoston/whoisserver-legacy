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

#ifndef CLASS_H
#define CLASS_H

#include <glib.h>

typedef enum {
    ATTR_MANDATORY,
    ATTR_OPTIONAL,
    ATTR_GENERATED
} attr_choice_t;

typedef enum {
    ATTR_SINGLE,
    ATTR_MULTIPLE
} attr_number_t;

/* info about attributes used by this class
 *  
 * FIXME: if we got into this structure by class_attr_t attr, we have no easy way to find out
 * the name of the attribute :(
 * agoston, 2007-11-12 */
typedef struct {
    int offset;
    attr_choice_t choice;
    attr_number_t number;
} class_attr_t;

/* maximum number of attributes in a class */
#define MAX_CLASS_ATTR 50

typedef struct {
    char *name;                         /* name of class */
    int id;                             /* identifier for this class, 
                                           e.g. C_MT, C_RS, etc. */
    int num_attr;                       /* number of attributes */
    class_attr_t attr[MAX_CLASS_ATTR];  /* attribute information (in-order) */
    GHashTable *attr_hash;              /* hash of attribute name -> 
                                            class_attr_t */
} class_t;

/* functions */
const char* const *get_class_names();
const class_t *class_lookup(const char *name);
const class_t *class_lookup_id(const int id);
const class_attr_t *class_attr_lookup(const class_t *class, 
                                      const char *attr_name);

#endif /* CLASS_H */
