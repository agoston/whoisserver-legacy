/***************************************
  $Revision: 1.1 $

  Properties module (pr) - this _should_ eventually get merged in with the

  Status: NOT REVUED, NOT TESTED

  +html+ <DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+   <LI><A HREF=".properties">.properties</A>
  +html+ </UL>
  +html+ </DL>
  +html+ <PRE>
  Instructions for use:

    To get a property:
      use the PR_get_property("Property.name") function from your other code.
  +html+ </PRE>
 
  ******************/ /******************
  Filename            : properties.c
  Description         : Provides a hash table of tokens and their values.
  Author              : ottrey@ripe.net
  Date                : 04/03/1999
  OSs Tested          : Solaris, BSDI, Linux
  Input Files         : .properties
  Related Modules     : Used in conjunction with the constants module.
  Problems            :
  To Do               : Fix up handling multi-lined properties.
                      : PR_set() could be cleaned up a little.
  Comments            :
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
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#define COMMENT_CHARACTER #
#define MAX_PROPERTIES    1024


/*
 * Type defs
 */
/*+ Each property has a +*/
typedef struct _Property {
  char *token;                  /*+ Token to be found in properties file. +*/
  char *value;                  /*+ Value to be found in properties file. +*/
} *Property;


/*
 * Global Variables
 */
/*+ Array of Properties +*/
Property Properties[MAX_PROPERTIES];

/*+ The number of properties. +*/
int Prop_count = 0;

/*+ The name of properties file. +*/
char  *Prop_file_name;



/* PR_to_string() */
/*++++++++++++++++++++++++++++++++++++++
  Returns the properties as a string.

  More:
  +html+ <PRE>
  Authors:
        ottrey

  Pre-Conditions:
        The properties must be loaded first with load_properties().

  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *PR_to_string(void) {
  char *props;
  GString *tmp;
  int i=0;

  tmp = g_string_new("Properties = { ");
  for(i=0; i< Prop_count; i++) {
    g_string_sprintfa(tmp, "[%s]=\"%s\" ", 
                     Properties[i]->token, Properties[i]->value );
  }
  g_string_append_c(tmp, '}');

  props = UT_strdup(tmp->str);
  g_string_free(tmp, TRUE);

  return props;
} /* PR_to_string() */

/* purge_properties() */
/*++++++++++++++++++++++++++++++++++++++
  Purges the old properties.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+   <LI><A HREF="../src/.properties">.properties</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
static void purge_properties(void) {
  int i;

  for(i=0; i < Prop_count; i++) {
    UT_free(Properties[i]->value);
    UT_free(Properties[i]->token);
    UT_free(Properties[i]);
  }

  Prop_count = 0;
} /* purge_properties() */


/* add_property() */
/*++++++++++++++++++++++++++++++++++++++
  Adds a new property to the Properties array.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+   <LI><A HREF=".properties">.properties</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
static void add_property(const char *token, const char *value) {
  Property prop;

  prop = (Property)UT_calloc(1, sizeof(struct _Property));
	   
  prop->token = UT_strdup(token);
  prop->value = UT_strdup(value);

  Properties[Prop_count] = prop;

  Prop_count++;
  Properties[Prop_count] = NULL;
} /* add_property() */


/* PR_set() */
/*++++++++++++++++++++++++++++++++++++++
  Sets the properties from the properties file.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+   <LI><A HREF=".properties">.properties</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *PR_set() {
  FILE    *prop_file;
  char    prop_line[1024];
  char    prop_line_more[1024];
  char    *eql_ptr;
  char    *token_ptr;
  char    *token_e_ptr;
  char    *value_ptr;
  char    *value_more_ptr;
  char    *value_e_ptr;
  int     token_l, value_l;
  int     more_lines;
  char    the_token[64];
  char    the_value[1024];
  char    result_buff[256];

  prop_file = fopen(Prop_file_name, "r");
  if (prop_file == NULL) {
    fprintf(stderr, "Error: Can't find properties file: %s\n", Prop_file_name);
    sprintf(result_buff, "Error: Can't find properties file: %s", Prop_file_name);
  }
  else {
    purge_properties();

    while (fgets(prop_line, 1024, prop_file) != 0) {
      if ( (eql_ptr = strchr(prop_line, '=')) != NULL) {
        /* An "=" was found  */

        token_ptr = prop_line;
        token_e_ptr = eql_ptr-1;

        /* Trim the trailing spaces/tabs off the token. */
        while (( *(token_e_ptr) == ' ') || ( *(token_e_ptr) == '\t')) {
          token_e_ptr--;
        }

        /* Trim the leading spaces/tabs off the token. */
        while (( *(token_ptr) == ' ') || ( *(token_ptr) == '\t')) {
          token_ptr++;
        }

        /* Skip if it's a comment line. */
        if (token_ptr[0] == '#' ) {
          continue;
        }

        /* Assign the token */
        token_l = (token_e_ptr - token_ptr) + 1;
        strncpy(the_token, token_ptr, token_l);
        the_token[token_l] = '\0';

        value_ptr = eql_ptr+1;
        value_e_ptr = strchr(prop_line, '\n')-1;

        /* Trim the leading spaces/tabs off the value. */
        while (( *(value_ptr) == ' ') || ( *(value_ptr) == '\t')) {
          value_ptr++;
        }

        /* Trim the trailing spaces/tabs off the value. */
        while (( *(value_e_ptr) == ' ') || ( *(value_e_ptr) == '\t')) {
          value_e_ptr--;
        }

        /* Assign the value */
        value_l = (value_e_ptr - value_ptr) + 1;
        strncpy(the_value, value_ptr, value_l);
        the_value[value_l] = '\0';

        /* If the value goes over the line */
        if ((value_e_ptr = strrchr(the_value, '\\')) != NULL) {
          *value_e_ptr = ' ';
          more_lines = 0;
          do {
            if (fgets(prop_line_more, 1024, prop_file) != 0) {

              /* Trim the leading spaces/tabs off the line_more. */
              value_more_ptr = prop_line_more;
              while (( *(value_more_ptr) == ' ') || ( *(value_more_ptr) == '\t')) {
                value_more_ptr++;
              }

              /* Trim the trailing spaces/tabs off the value. */
              if ((value_e_ptr = strrchr(prop_line_more, '\\')) != NULL) {
                more_lines = 1;
                *value_e_ptr = ' ';
              }
              else {
                more_lines = 0;
              }
              value_e_ptr = strchr(prop_line_more, '\n');
              *value_e_ptr = ' ';
              while ((*value_e_ptr == ' ') || (*value_e_ptr == '\t')) {
                value_e_ptr--;
              }

              *(value_e_ptr+1) = '\0';
              strcat(the_value, value_more_ptr);
              
            }
          } while (more_lines == 1);

          value_l = strlen(the_value);
          the_value[value_l] = '\0';
        }

        add_property(the_token, the_value);
      } else {
        /* Skip this line */
        ;
      }
    }

    /*
    printf("%s\n", PR_to_string() );
    */

    fclose(prop_file);

    sprintf(result_buff, "Properties successfully set from %s file.  (%d properties)", Prop_file_name, Prop_count);
  }

  return UT_strdup(result_buff);
} /* PR_set() */


/* PR_load() */
/*++++++++++++++++++++++++++++++++++++++
  Sets the properties file name.  Then sets the properties with a call to set_properties().

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+   <LI><A HREF=".properties">.properties</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
void PR_load(const char *prop_file_name) {
  
  Prop_file_name = UT_strdup(prop_file_name);
  UT_free(PR_set());

} /* PR_load() */


/* PR_get_property() */
/*++++++++++++++++++++++++++++++++++++++
  Sets the properties file name.  Then sets the properties with a call to set_properties().

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+   <LI><A HREF=".properties">.properties</A>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *PR_get_property(const char *token, const char *default_value) {
  char *value;
  int   i = 0;

  /* Search through the Properties until the token is found */
  while (i < Prop_count) {
    if (strcmp(token, Properties[i]->token) == 0) {
      break;
    }
    i++;
  }
  
  if (i == Prop_count) {
    /* If token not found return the default value */
    if (default_value == NULL) {
      value = UT_strdup("");
    } else {
      value = UT_strdup(default_value);
    }
  } else {
    /* Return the found value */
    value = UT_strdup(Properties[i]->value);
  }

  return value;

} /* PR_get_property() */
