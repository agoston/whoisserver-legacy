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
#include "class.h"
#include "class_tab.h"
#include "attribute.h"

/* defined in attribute_tab.h */
/*extern attribute_t attribute_tab[];*/

#define CLASS_TAB_LEN (sizeof(class_tab)/sizeof(class_tab[0]))

/* array with only lists of class names */
static char** class_names;

/* hash mapping class name to entry in class_tab[] */
static GHashTable *class_hash_name;

/* hash mapping class id to entry in class_tab[] */
static GHashTable *class_hash_id;

static guint my_g_str_hash(gconstpointer v) {
	gchar *s;
	guint hash;

	s = g_strdup(v);
	g_strdown(s);
	hash = g_str_hash(s);
	g_free(s);
	return hash;
}

static gint my_g_strcasecmp(gconstpointer a, gconstpointer b) {
	return (g_strcasecmp(a, b) == 0);
}

static void class_init() {
	int i, j;
	char *attribute_name;
	const attribute_t *attr_info;
	class_attr_t *class_attr_info;

	/* initialize table of class names */
	class_names = g_new(char*, CLASS_TAB_LEN+1);
	for (i=0; i<CLASS_TAB_LEN; i++) {
		class_names[i] = g_strdup(class_tab[i].name);
	}
	class_names[i] = NULL;

	/* initialize hash to class information */
	class_hash_name = g_hash_table_new(my_g_str_hash, my_g_strcasecmp);
	class_hash_id = g_hash_table_new(g_int_hash, g_int_equal);
	
	for (i=0; i<CLASS_TAB_LEN; i++) {
		class_tab[i].attr_hash = g_hash_table_new(my_g_str_hash, my_g_strcasecmp);
		for (j=0; j<class_tab[i].num_attr; j++) {
			attr_info = attribute_lookup_by_offset(class_tab[i].attr[j].offset);
			attribute_name = attr_info->name;
			class_attr_info = &(class_tab[i].attr[j]);
			g_hash_table_insert(class_tab[i].attr_hash, attribute_name, class_attr_info);
		}
		g_hash_table_insert(class_hash_name, class_tab[i].name, &class_tab[i]);
		g_hash_table_insert(class_hash_id, &class_tab[i].id, &class_tab[i]);
	}
}

static pthread_once_t init_control= PTHREAD_ONCE_INIT;

/* FIXME: This is buggy as init_control will indeed be executed only once, but if 2 or more threads start executing that
 * function, the second one will jump over the pthread_once and crash the server because the initialization is
 * not yet done. - agoston, 2007-10-26 */
const char* const* get_class_names() {
	pthread_once(&init_control, class_init);
	return (const char* const*)class_names;
}

const class_t *class_lookup(const char *name) {
	pthread_once(&init_control, class_init);
	return g_hash_table_lookup(class_hash_name, name);
}

const class_t *class_lookup_id(const int id) {
	pthread_once(&init_control, class_init);
	return g_hash_table_lookup(class_hash_id, &id);
}

const class_attr_t * class_attr_lookup(const class_t *class, const char *attr_name) {
	pthread_once(&init_control, class_init);
	return g_hash_table_lookup(class->attr_hash, attr_name);
}

