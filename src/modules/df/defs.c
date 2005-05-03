/***************************************
  $Revision: 1.1 $

  Definitions module (df)

  Status: NOT REVUED, NOT TESTED

  ******************/ /******************
  Filename            : defs.c
  Authors             : ottrey@ripe.net
                        marek@ripe.net
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
#define  DEFS_IMPL
#include "rip.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <strings.h>
#include <glib.h>
#include <pthread.h>

#include "DF_class_names.def"
#include "DF_class_codes.def"
#include "DF_class_aliases.def"
#include "DF_class_aliases_map.def"
#include "DF_class_dbase_code_map.def"
#include "DF_class_templates.def"
#include "DF_class_templates_v.def"

#include "DF_attribute_names.def"
#include "DF_attribute_codes.def"
#include "DF_attribute_aliases.def"
#include "DF_attribute_aliases_map.def"

#include "UD_queries.def"


/* Filtered names of classes (only "public" objects, no person or role).

   XXX this also should be generated from XML... 
*/
char * const  Filter_names[] = {
  "as-block",
  "as-set",
  "aut-num",
  "domain",
  "inet-rtr",
  "inet6num",
  "inetnum",
  "key-cert",
  "limerick",
  "mntner",
  "route",
  "route6",
  "origin",
  "route-set",   
  "members",
  "filter-set",
  "peering-set",
  "rtr-set",
  "irt",
  "poem",
  "poetic-form",
  NULL
}; /* Filter_names */

char * const *DF_get_filter_names(void) {
  return Filter_names;
} /* DF_get_filter_names() */

char * const *DF_get_class_names(void) {
  return Class_names;
} /* DF_get_class_names() */

char * const *DF_get_class_aliases(void) {
  return Class_aliases;
} /* DF_get_class_aliases() */

int DF_get_class_index(int alias_index) {
  return Class_aliases_map[alias_index];
} /* DF_get_class_index() */

char * const DF_get_class_code(C_Type_t index) {
  if( index == C_ANY ) {
    return "*";
  }
  else {
    return Class_codes[index];
  }
} /* DF_get_class_code() */

int DF_get_class_dbase_code(int class_index) {
  return Class_dbase_code_map[class_index];
} /* DF_get_class_dbase_code() */

/* Main tables names for object types */
/*XXX this should be modified according to class order */
char * const Type2main[] = {
"as_block",
"as_set",
"aut_num",
"domain",
"inet_rtr",
"inet6num",
"inetnum",
"key_cert",
"limerick",
"mntner",
"person_role",
"person_role",
"route",
"route_set",
"filter_set",
"peering_set",
"rtr_set",
"irt",
"organisation",
"route6",
"poem",
"poetic_form",
NULL
};

char * const DF_get_class_sql_table(C_Type_t index) {
 return Type2main[index];
} /* DF_get_class_sql_table() */ 



char * const *DF_get_attribute_aliases(void) {
  return Attribute_aliases;
} /* DF_get_attribute_aliases() */

const char *DF_get_attribute_name(A_Type_t index) {
  return Attribute_names[index];
} /* DF_get_attribute_name() */

const char *DF_get_attribute_code(A_Type_t index) {
  return Attribute_codes[index];
} /* DF_get_attribute_code() */

char * const *DF_get_attribute_names(void) {
  return Attribute_names;
} /* DF_get_attribute_names() */

int DF_get_attribute_index(int alias_index) {
  return Attribute_aliases_map[alias_index];
} /* DF_get_attribute_index() */

const char *DF_get_class_template(C_Type_t index) {
  return Templates[index];
} /* DF_get_class_template() */

const char *DF_get_class_template_v(C_Type_t index) {
  return Templates_v[index];
} /* DF_get_class_template_v() */

const char *DF_get_update_query(A_Type_t index){
  return Update[index].qry;
} /* DF_get_update_query() */

UD_qtype DF_get_update_query_type(A_Type_t index){
  return Update[index].qtype;
} /* DF_get_update_query_type() */

const char *DF_get_insert_query(A_Type_t index){
  return Insert[index].qry;
} /* DF_get_insert_query() */
  
UD_qtype DF_get_insert_query_type(A_Type_t index){
  return Insert[index].qtype;
} /* DF_get_insert_query_type() */
    
const char *DF_get_select_query(A_Type_t index){
  return Select[index].qry;
} /* DF_get_select_query() */
  
UD_qtype DF_get_select_query_type(A_Type_t index){
  return Select[index].qtype;
} /* DF_get_select_query_type() */
        
const char *DF_get_dummy_query(A_Type_t index){
  return Dummy[index].qry;
} /* DF_get_dummy_query() */
  
UD_qtype DF_get_dummy_query_type(A_Type_t index){
  return Dummy[index].qtype;
} /* DF_get_dummy_query_type() */


/* XXX This could be done MUCH more efficiently (with a hash) */
A_Type_t DF_attribute_code2type(const gchar *token) {
  A_Type_t result=-1;

  int i;
  for (i=0; Attribute_aliases[i] != NULL; i++) {
    if (strcmp(Attribute_aliases[i], token) == 0) {
      result = Attribute_aliases_map[i];
      break;
    }
  }

  return result;
} /* DF_attribute_code2type() */

/* 
Description:

  Find the type identifier for the given long attribute name.  This can 
  be used to get the attribute code via the DF_get_attribute_code()
  function.
 
Arguments:

  const gchar *token; attribute name, e.g. "person", "aut-num", or "limerick"

Returns:
 
  A_Type_t with the attribute's code, or -1 on error (bad attribute name).

Notes:
 
  Uses a hash table for speedy conversion.  The first time this is called, 
  the hash table will be built.  Subsequent calls use that table.
   
  It might be better to provide a single function to translate from an 
  attribute name to the attribute code, but for now, just use 
  DF_get_attribute_code() with the value returned here.  - SK
*/
static GHashTable *attribute_name2type_hash = NULL;

static void 
init_attribute_name2type_hash ()
{
    A_Type_t *val;
    int i;

    attribute_name2type_hash = g_hash_table_new(g_str_hash, g_str_equal);
    for (i=0; Attribute_aliases[i] != NULL; i++) {
        val = UT_malloc(sizeof(A_Type_t));
        *val = Attribute_aliases_map[i];
        g_hash_table_insert(attribute_name2type_hash, 
                            Attribute_aliases[i], val);
    }
}

A_Type_t 
DF_attribute_name2type (const gchar *token) 
{
    static pthread_once_t once_control = { PTHREAD_ONCE_INIT };
    A_Type_t *result;

    /* build table on first call */
    pthread_once(&once_control, init_attribute_name2type_hash);

    /* find the type in our has table, returning if found */
    result = g_hash_table_lookup(attribute_name2type_hash, token);
    if (result != NULL) {
        return *result;
    } else {
        return -1;
    }
} /* DF_attribute_name2type() */

static GHashTable *class_name2type_hash = NULL;

static void 
init_class_name2type_hash ()
{
    C_Type_t *val;
    int i;

    class_name2type_hash = g_hash_table_new(g_str_hash, g_str_equal);
    for (i=0; Class_aliases[i] != NULL; i++) {
        val = UT_malloc(sizeof(C_Type_t));
        *val = Class_aliases_map[i];
        g_hash_table_insert(class_name2type_hash, 
                            Class_aliases[i], val);
    }
}

C_Type_t 
DF_class_name2type (const gchar *token) 
{
    static pthread_once_t once_control = { PTHREAD_ONCE_INIT };
    C_Type_t *result;

    /* build table on first call */
    pthread_once(&once_control, init_class_name2type_hash);

    /* find the type in our has table, returning if found */
    result = g_hash_table_lookup(class_name2type_hash, token);
    if (result != NULL) {
        return *result;
    } else {
        return -1;
    }
} /* DF_class_name2type() */

/* Returns class name for a given type */
const char *DF_class_type2name(C_Type_t class) {
	return(Class_names[class]);
}/* DF_class_type2name() */


/* check in the queries if this attribute can trigger a radix lookup */
int DF_attrcode_has_radix_lookup(A_Type_t attr) 
{
  int i;
  
  for (i=0; Query[i].query != NULL; i++) {
    if( Query[i].refer == R_RADIX &&
	Query[i].attribute == attr ) {
      return 1;
    }
  }
  return 0;
}

/* return the sql query to load the radix ipv4 tree for this attribute
   or NULL if no ipv4 radix is used for this attribute */
char * DF_attrcode_radix_load_v4(A_Type_t attr) 
{
    int i;
    
    for(i=0; 
	DF_radix_load[i].attr != -1 && DF_radix_load[i].family != -1;	
	i++) {
	
	if( DF_radix_load[i].attr == attr ) {
	    return DF_radix_load[i].ipv4_load;
	}
    }
    return NULL;
}

/* return the sql query to load the radix ipv4 tree for this attribute
   or NULL if no ipv4 radix is used for this attribute */
char * DF_attrcode_radix_load_v6(A_Type_t attr) 
{
    int i;
    
    for(i=0; 
	DF_radix_load[i].attr != -1 && DF_radix_load[i].family != -1;	
	i++) {
	
	if( DF_radix_load[i].attr == attr ) {
	    return DF_radix_load[i].ipv6_load;
	}
    }
    return NULL;
}

/* return the family of the radix tree(s) used for this attribute
   or -1 if no radix is used for this attribute */
rx_fam_t DF_attrcode_radix_family(A_Type_t attr) {
    int i;
    
    for(i=0; 
	DF_radix_load[i].attr != -1 && DF_radix_load[i].family != -1;	
	i++) {
	
	if( DF_radix_load[i].attr == attr ) {
	    return DF_radix_load[i].family;
	}
    }
    return -1;
}
