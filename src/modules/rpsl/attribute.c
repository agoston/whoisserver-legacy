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

#include <stdio.h>

#include <glib.h>
#include <string.h>
#include <pthread.h>
#include "attribute.h"
#include "attribute_tab.h"

#define ATTRIBUTE_TAB_LEN (sizeof(attribute_tab)/sizeof(attribute_tab[0]))

/* hash mapping attribute name to TRUE (only present if ambiguous) */
static GHashTable *ambiguous_names;

/* hash mapping attribute name to entry in attribute_tab[] */
static GHashTable *name_to_attribute;

static guint 
my_g_str_hash (gconstpointer v)
{
    gchar *s;
    guint hash;

    s = g_strdup(v);
    g_strdown(s);
    hash = g_str_hash(s);
    g_free(s);
    return hash;
}

static gint
my_g_strcasecmp (gconstpointer a, gconstpointer b)
{
    return (g_strcasecmp(a, b) == 0);
}

static void
attribute_init()
{
    int i;
    gchar *name;
    attribute_t *attr;
    static gboolean true = TRUE;

    name_to_attribute = g_hash_table_new(my_g_str_hash, my_g_strcasecmp);
    ambiguous_names = g_hash_table_new(my_g_str_hash, my_g_strcasecmp);
    for (i=0; i<ATTRIBUTE_TAB_LEN; i++) {
        name = attribute_tab[i].name;
        /* don't do anything if we've already discovered this name is 
           ambiguous (i.e. appears more than once in the table) */
        if (g_hash_table_lookup(ambiguous_names, name) == NULL) {
            /* otherwise, see if it is already in the table */
            attr = g_hash_table_lookup(name_to_attribute, name);
            if (attr == NULL) {
                /* if it has not appeared, add it */
                g_hash_table_insert(name_to_attribute, name, &attribute_tab[i]);
            } else {
                /* if it has appeared, mark this as ambiguous */
                g_hash_table_insert(ambiguous_names, name, &true);
            }
        }
    }
}

static pthread_once_t init_control = PTHREAD_ONCE_INIT;

/* returns attribute information if found and unambiguous, else NULL */
const attribute_t *
attribute_lookup (const char *name, gboolean *is_ambiguous)
{
    pthread_once(&init_control, attribute_init);

    if (g_hash_table_lookup(ambiguous_names, name)) {
        *is_ambiguous = TRUE;
    } else {
        *is_ambiguous = FALSE;
    }
    return g_hash_table_lookup(name_to_attribute, name);
}

const attribute_t *
attribute_lookup_by_offset (int offset)
{
    pthread_once(&init_control, attribute_init);

    if ((offset < 0) || (offset >= ATTRIBUTE_TAB_LEN)) {
        return NULL;
    } else {
        return &attribute_tab[offset];
    }
}

