/***************************************
  $Revision: 1.1 $

  Constants module (co) - this _should_ eventually get merged in with the
  config module.

  Status: NOT REVUED, NOT TESTED

  +html+ <DL COMPACT>
  +html+ <DT>Online References: 
  +html+ <DD><UL>
  +html+ </UL>
  +html+ </DL>
  +html+ <PRE>
  Instructions for use:

    To add a constant:
      0. Add a default value for the constant. (string)
      1. Add the constant declaration to the _Constants struct.
      2. Add a CO_get_function()
      3. Add initializing code to init_constants()

    To access the constant:
      use the CO_get<Constant>() function from your other code.
  +html+ </PRE>
 
  ******************/ /******************
  Filename            : constants.c
  Author              : ottrey@ripe.net
  OSs Tested          : Solaris
  Related Modules     : Used in conjunction with the properties module.
  Problems            : 
  To Do               : Merge into a "config module"
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

/*+ Maximum number of constants. +*/
#define MAX_CONSTS 100

/*+ Default values for constants. +*/

#define DEFLT_AUTHENTICATE    "0"
#define DEFLT_WHOIS_SUSPENDED "0"
#define DEFLT_DO_SERVER       "1"
#define DEFLT_WELCOME         "Welcome to the whois R.I.P. server.\n"
#define DEFLT_PROMPT          "whois R.I.P. config> "
#define DEFLT_CLEAR_SCREEN    "0"
#define DEFLT_ACCOUNTING      "0"
#define DEFLT_CONFIG_FILE     "rip.config"

/*+ Each constant has a +*/
struct _constant {
  const char *token;              /*+ Token to be found in properties file. +*/
  const char *deflt;                    /*+ Default value for the constant. +*/
  int (*set_func)(void *, const char *);  /*+ Function to set the constant. +*/
  void *constant_ptr;                     /*+ Pointer to the constant value +*/
  char *(*show_func)(const void *);      /*+ Function to show the constant. +*/
};


/*+ The Constants array has a +*/
typedef struct _Constants {
  int   authenticate[1];                         /*+ Authenticate users. +*/
  int   whois_suspended[1];                /*+ Suspend the whois server. +*/
  char  welcome[1024];                  /*+ Welcome for config protocol. +*/
  char  prompt[1024];                    /*+ Prompt for config protocol. +*/
  int   clear_screen[1];         /*+ Clear screen after config commands. +*/
  int   accounting[1];          /*+ Conduct accounting on whois queries. +*/

  int   do_server[1]; /*+ turns off execution of the all servers(threads) +*/
  int   do_update[1]; /*+ switches on and off the updates +*/

} *Constants;

/*
 * Global Variables
 */
/*+ The array of Global Constants. +*/
static Constants  Global_constants=NULL;

/* 
 * Set Functions
 */
static int set_string(void *constant, const char *value) {

  strcpy((char *)constant, value);

  return 0;
} /* set_string() */

static int set_int(void *constant, const char *value) {
  int i;
  
  i = atol(value);
  ((int *)constant)[0] = i;

  return 0;
} /* set_int() */

static int set_boolean(void *constant, const char *value) {
  int result=1;
  int i;
  
  i = atol(value);

  /* If a valid boolean */
  if ( (i == 0) || (i == 1)) {
    ((int *)constant)[0] = i;
    result = 0;
  }

  return result;
} /* set_boolean() */


/* 
 * Show Functions
 */
/* AR. changed for unification with oter show funcs */
static char *show_string(const void *constant) {
  return UT_strdup((char *)constant);
} /* show_string() */

static char *show_int(const void *constant) {
  char *tmp;

  tmp = UT_malloc(32);

  sprintf(tmp, "%d", ((int *)constant)[0]);
  return tmp;
} /* show_int() */

static char *show_boolean(const void *constant) {
  char *tmp;

  tmp =  UT_malloc(32);

  sprintf(tmp, "%d", ((int *)constant)[0]);
  return tmp;
} /* show_boolean() */


/* 
 * Get Functions
 */

int CO_get_authenticate() {
  return Global_constants->authenticate[0];
}

int CO_get_whois_suspended() {
  return Global_constants->whois_suspended[0];
}

char *CO_get_welcome() {
  return Global_constants->welcome;
}

char *CO_get_prompt() {
  return Global_constants->prompt;
}

int CO_get_clear_screen() {
  return Global_constants->clear_screen[0];
}

int CO_get_accounting() {
  return Global_constants->accounting[0];
}

int CO_get_do_server() {
  return Global_constants->do_server[0];
}
  
int CO_get_do_update() {
  return Global_constants->do_update[0];
}

/*+
 * Contains the constant definitions for the Token, set_function, show_function.
 * (See: _constant)
+*/
static struct _constant constant[MAX_CONSTS];

/* init_constants() */
/*++++++++++++++++++++++++++++++++++++++
  Initialize all the constants.

  More:
  +html+ <PRE>
  Authors:
        ottrey

  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
static void init_constants(void) {
  int n=0;

  constant[n].token="SV.authenticate";
  constant[n].deflt=DEFLT_AUTHENTICATE;
  constant[n].set_func=set_boolean;
  constant[n].constant_ptr=Global_constants->authenticate;
  constant[n].show_func=show_boolean;
  n++;

  constant[n].token="SV.whois_suspended";
  constant[n].deflt=DEFLT_WHOIS_SUSPENDED;
  constant[n].set_func=set_boolean;
  constant[n].constant_ptr=Global_constants->whois_suspended;
  constant[n].show_func=show_boolean;
  n++;
  
  constant[n].token="SV.do_server";
  constant[n].deflt=DEFLT_DO_SERVER;
  constant[n].set_func=set_boolean;
  constant[n].constant_ptr=Global_constants->do_server;
  constant[n].show_func=show_boolean;
  n++;
  
  constant[n].token="UD.do_update";
  constant[n].deflt="1";
  constant[n].set_func=set_int;
  constant[n].constant_ptr=Global_constants->do_update;
  constant[n].show_func=show_int;
  n++;

  constant[n].token="PC.prompt";
  constant[n].deflt=DEFLT_PROMPT;
  constant[n].set_func=set_string;
  constant[n].constant_ptr=Global_constants->prompt;
  constant[n].show_func=show_string;
  n++;

  constant[n].token="PC.clear_screen";
  constant[n].deflt=DEFLT_CLEAR_SCREEN;
  constant[n].set_func=set_boolean;
  constant[n].constant_ptr=Global_constants->clear_screen;
  constant[n].show_func=show_boolean;
  n++;

  constant[n].token=NULL;

} /* init_constants() */


/* CO_to_string() */
/*++++++++++++++++++++++++++++++++++++++
  Returns the constants as a string.

  More:
  +html+ <PRE>
  Authors:
        ottrey

  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *CO_to_string(void) {
  char *consts;
  const char *token;
  char *value;
  GString *tmp;
  int i=0;

  tmp = g_string_new("Constants = { ");
  while(constant[i].token != NULL) {
    token = constant[i].token;
    value = constant[i].show_func(constant[i].constant_ptr);
    g_string_sprintfa(tmp, "\n[%s]=\"%s\"", token, value);
    UT_free(value); /* Otherwise we have memory leaks */
    i++;
  }
  g_string_append_c(tmp, '}');

  consts = UT_strdup(tmp->str);
  g_string_free(tmp, TRUE);

  return consts;
} /* CO_to_string() */


char *CO_const_to_string(const char *name) {
  char *result=NULL;
  int i;
  
  for (i=0; constant[i].token != NULL; i++) {
    if (strcmp(constant[i].token, name) == 0) {
      result = constant[i].show_func(constant[i].constant_ptr);
      break;
    }
  }

  return result;
} /* CO_const_to_string() */

 /* CO_set_const() */
/*++++++++++++++++++++++++++++++++++++++
  Sets the value of one constant.  Returns 0 if no error.

  More:
  +html+ <PRE>
  Authors:
        ottrey

  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
int CO_set_const(const char *name, const char *value) {
  int result=1;
  int i;
  
  for (i=0; constant[i].token != NULL; i++) {
    if (strcmp(constant[i].token, name) == 0) {
      result = constant[i].set_func((void *)constant[i].constant_ptr, value);
      break;
    }
  }

  return result;
} /* CO_set_const() */


/* CO_set() */
/*++++++++++++++++++++++++++++++++++++++
  Sets the constants from the properties module.
  Returns the number of constants set.

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
char *CO_set(void) {
  int i;
  int set_count=0;
  int set;
  char result_buff[256];
  char *result;
  char *property;

  /* Initialize if necessary */
  if (Global_constants == NULL) {
    Global_constants = (Constants)UT_calloc(1, sizeof(struct _Constants));
    
    init_constants();
  }

  for (i=0; constant[i].token != NULL; i++) {
    property = PR_get_property(constant[i].token, constant[i].deflt);
    set = constant[i].set_func((void *)constant[i].constant_ptr, property);
    UT_free(property);
    if (set == 0) {
      set_count++;
    }
  }

  sprintf(result_buff, "%d out of %d constant(s) set.", set_count, i);

  result = (char *)UT_calloc(1, strlen(result_buff)+1);
  strcpy(result, result_buff);

  return result;
} /* CO_set() */

