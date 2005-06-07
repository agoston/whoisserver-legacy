/******************
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

/* this is for purify - to display the memory allocation records */
extern void purify_new_inuse(void);


/*++++++++++++++++++++++++++++++++++++++
  
  All functions in this file share the same interface: they take the
  arguments to the command given by the user, pointer to a dynamic
  GString to which the command output should be appended and the
  connection data, so that some things can be displayed directly to it,
  bypassing the GString.
  
  int <command_something>     return code. 0 indicates success.
                              PC_RET_QUIT is a reserved code
                              that indicates that the connection
			      should be closed. 

  char *input                 command arguments

  GString *output             (dynamic) output string

  sk_conn_st *condat          connection data

  ++++++++++++++++++++++++++++++++++++++*/

/*++++++++++++++++++++++++++++++++++++++
      
  Relay functions for composed commands (eg. "set counter").

  They run the second word as a command from a specific array
  (show/set/stop/whatever). The hardcoded text is used only for help
  messages, printed in case the command is wrong as 

 "<hardcoded> commands are: <list of possible commands>".

 ++++++++++++++++++++++++++++++++++++++*/
int command_save(char *input, GString *output, sk_conn_st *condat) {
  return command_execute(save, "save ", input, output, condat);
}

int command_show(char *input, GString *output, sk_conn_st *condat) {
  return command_execute(show, "show ", input, output, condat);
}

int command_set( char *input, GString *output, sk_conn_st *condat) {
  return command_execute(set,  "set ",  input, output, condat);
}

int command_stop(char *input, GString *output, sk_conn_st *condat) {
  return command_execute(stop, "stop ", input, output, condat);
}


/*++++++++++++++++++++++++++++++++++++++
  
  Display available commands.
  
  ++++++++++++++++++++++++++++++++++++++*/
int command_help(char *input, GString *output, sk_conn_st *condat) 
{
  /* by the time it came here, the "help" bit is already taken away. */
  return show_commands(command, "", output);

}


/*++++++++++++++++++++++++++++++++++++++
  
  Quit the config session.

  ++++++++++++++++++++++++++++++++++++++*/
int command_quit(char *input, GString *output, sk_conn_st *condat) {
    /* Administrator wishes to quit. */
  return PC_RET_QUIT;
} /* command_quit() */

/*++++++++++++++++++++++++++++++++++++++
  
  Display the memory allocation records of purify(tm).
  The #define must be changed to activate this.
  The program will link only with purify.

  ++++++++++++++++++++++++++++++++++++++*/
int command_purify(char *input, GString *output, sk_conn_st *condat)
{
#if 0
  purify_new_inuse();
#else 
  g_string_append(output, "NOP");
#endif
  
  return 0;
}

int save_access_tree(char *input, GString *output, sk_conn_st *condat) {
  int ret_err;

  ret_err = AC_persistence_save();

  switch (ret_err) {
  case AC_OK:
    g_string_append(output, "Save successful");
    break;
  case AC_SAVING:
    g_string_append(output, "Already saving");
    break;
  default:
    g_string_append(output, "Unknown error");
    return PC_RET_ERR;
  }
  return 0;
}

/*++++++++++++++++++++++++++++++++++++++
  
  Display a specific constant of the CO module.

  Argument: name of the constant.

  ++++++++++++++++++++++++++++++++++++++*/
int show_const(char *input, GString *output, sk_conn_st *condat) {
  /* Administrator wishes to show constants. */
  char *result, *name, *cursor;
  int res = 0;
  
  if( strlen(input) > 0 ) {
    cursor = input;
    name = (char *)strsep(&cursor, " ");

    if( (result = CO_const_to_string(name)) != NULL ) {
      g_string_append(output, result);
      UT_free(result);
    }
    else {
      g_string_append(output,  "unknown constant");
      res = PC_RET_ERR;
    }
  }
  else {
    g_string_append(output,  "name required");
    res = PC_RET_ERR;
  }
 
  return res;

} /* show_const() */


/*++++++++++++++++++++++++++++++++++++++
  
  Display all the constants of the CO module.

  ++++++++++++++++++++++++++++++++++++++*/
int show_consts(char *input, GString *output, sk_conn_st *condat) 
{
  /* Administrator wishes to show constants. */
  char *s =  CO_to_string();
  g_string_append(output, s);
  UT_free(s);
  return 0;
} /* show_consts() */

/*++++++++++++++++++++++++++++++++++++++
  
  Display dynamic status of the server

  ++++++++++++++++++++++++++++++++++++++*/
int show_dynamic(char *input, GString *output, sk_conn_st *condat) 
{
  int res = 0;
  char *result;

  /* Administrator wants to see if the server is static or dynamic */
  if ( (result = CO_const_to_string("SV.dynamic")) != NULL ) {
    g_string_append(output,result);
    UT_free(result);
  } else {
    g_string_append(output, "error getting value");
    res = PC_RET_ERR;
  }

  return res;
} /* show_dynamic() */

/*++++++++++++++++++++++++++++++++++++++
  
  Display all the properties of the PR module.

  ++++++++++++++++++++++++++++++++++++++*/
int show_props(char *input, GString *output, sk_conn_st *condat) 
{
  /* Administrator wishes to show properties. */
  char *s =  PR_to_string();
  g_string_append(output, s);
  UT_free(s);  
  return 0;
} /* show_props() */


/*++++++++++++++++++++++++++++++++++++++
  
  Display all running threads registered with the TA module.

  ++++++++++++++++++++++++++++++++++++++*/
int show_threads(char *input, GString *output, sk_conn_st *condat) 
{
  /* Administrator wishes to show thread information. */
  char *s = TA_tostring();
  g_string_append(output, s);
  UT_free(s);  
  return 0;
} /* show_thread() */


/*++++++++++++++++++++++++++++++++++++++
  
  Switch the session to a whois session.

  ++++++++++++++++++++++++++++++++++++++*/
int show_whois(char *input, GString *output, sk_conn_st *condat) 
{
  /*  Go to whois mode */
  PW_interact(condat->sock);
  return 0;
} /* show_whois() */


/*++++++++++++++++++++++++++++++++++++++
  
  Display the statistics about the server.

  ++++++++++++++++++++++++++++++++++++++*/
int show_uptime(char *input, GString *output, sk_conn_st *condat) 
{
  char timestring[26];
  extern time_t SV_starttime;
  
  ctime_r(&SV_starttime, timestring); 
  SK_cd_printf( condat, 
	       "System running since %sUptime in seconds: %ld \n\n",
	       timestring,		  
	       time(NULL) - SV_starttime);
  
  return 0;
} 

/*++++++++++++++++++++++++++++++++++++++
  
  Display the whois access statistics from the AC module.

  ++++++++++++++++++++++++++++++++++++++*/
int show_access(char *input, GString *output, sk_conn_st *condat) 
{  
  int cnt = AC_print_access(output);
  
  g_string_sprintfa(output, "Found %d nodes\n", cnt);

  return 0;
} /* show_access() */


/*++++++++++++++++++++++++++++++++++++++
  
  Display the whois access control list from the AC module.

  ++++++++++++++++++++++++++++++++++++++*/
int show_acl(char *input, GString *output, sk_conn_st *condat) 
{
  int cnt = AC_print_acl(output);

  g_string_sprintfa(output, "Found %d nodes\n", cnt);

  return 0;
} /* show_acl() */

/*++++++++++++++++++++++++++++++++++++++
  
  Display the access tree auto save state.

  ++++++++++++++++++++++++++++++++++++++*/
int show_auto_save(char *input, GString *output, sk_conn_st *condat) 
{
  g_string_sprintfa(output, "Auto save is %d\n", ac_auto_save);

  return 0;
} /* show_auto_save() */


/*++++++++++++++++++++++++++++++++++++++
  
  Modify the whois access control list in the AC module.

  Arguments: IP[/prefixlength] column=value,column=value...

  Column names as in acl display. Unset columns are inherited.

  ++++++++++++++++++++++++++++++++++++++*/
int set_acl(char *input, GString *output, sk_conn_st *condat)
{
  int res = 0;
  
  /* first 8 characters ("set acl ") are already skipped */
  if( ! NOERR( AC_asc_acl_command_set( input, "Manual"))) {
    g_string_append(output, "Error!\n");
    res = PC_RET_ERR;
  }
  return res;
}

/*++++++++++++++++++++++++++++++++++++++
  
  Sets the auto save status of the access tree.

  Arguments: 0 = don't auto save, 1 = auto save

  ++++++++++++++++++++++++++++++++++++++*/
int set_auto_save(char *input, GString *output, sk_conn_st *condat)
{
  int res = 0;
  
  /* Lame */
  switch (input[0]) {
  case '0':
    ac_auto_save  = 0;
printf("0\n");
    break;
  case '1':
    ac_auto_save  = 1;
printf("1\n");
    break;
  default:
printf("X\n");
    res = PC_RET_ERR;
  }
  return res;
}

/*++++++++++++++++++++++++++++++++++++++
  
  Reset the deny counter in the access tree to 0 (after reenabling) 
  (AC module).

  Argument: IP address.

  ++++++++++++++++++++++++++++++++++++++*/
int set_nodeny(char *input, GString *output, sk_conn_st *condat) {
  
  /* first 11 characters ("set nodeny ")  are already skipped */

  if( ! NOERR( AC_asc_set_nodeny(input) )) {
    g_string_append(output, "Error\n");
    return PC_RET_ERR;
  }
  else {
    return 0;
  }
  
} /* set_nodeny() */


/*++++++++++++++++++++++++++++++++++++++
  
  Pause/resume update capability of the UD module.

  Argument: the word "pause" or "resume".

  ++++++++++++++++++++++++++++++++++++++*/
int set_updates(char *input, GString *output, sk_conn_st *condat) 
{
  char argstr[17];
  int pause=0, resume=0;
  int res = 0;
 
  if( sscanf(input, "%16s", argstr) == 1) {
    pause = (strcmp(argstr,"pause") == 0);
    resume = (strcmp(argstr,"resume") == 0);
  }
  
  if( !pause && !resume ) {
    g_string_append(output,  "syntax error.");
    res = PC_RET_ERR;
  }
  else {
    /* all params ok. just set the property */
    char *value = pause ? "0" : "1";
    
    if (CO_set_const("UD.do_update", value) == 0) {
      g_string_append(output, "Constant successfully set\n");
    }
    else {
      g_string_append(output, "Could not set\n");
      res = PC_RET_ERR;
    }
  }
  return res;
}

/*++++++++++++++++++++++++++++++++++++++
  
  Stop/Start dynamic sources.

  Argument: the word "stop" or "start".

  ++++++++++++++++++++++++++++++++++++++*/
int set_dynamic(char *input, GString *output, sk_conn_st *condat) 
{
  char argstr[17];
  int stop_dynamic=0, start_dynamic=0;
  int res = 0;
 
  if( sscanf(input, "%16s", argstr) == 1) {
    stop_dynamic = (strcmp(argstr,"stop") == 0);
    start_dynamic = (strcmp(argstr,"start") == 0);
  }
  
  if( !stop_dynamic && !start_dynamic ) {
    g_string_append(output,  "syntax error.");
    res = PC_RET_ERR;
  } else {
    char *value = stop_dynamic ? "0" : "1";
    if (CO_set_const("SV.dynamic", value) == 0) {
      g_string_append(output, "Constant successfully set\n");
    } else {
      g_string_append(output, "Could not set\n");
      res = PC_RET_ERR;
    }
  }
  if (res != PC_RET_ERR) {
    SV_switchdynamic();
  }

  return res;
}

/*++++++++++++++++++++++++++++++++++++++
  
  Pause/resume queries.

  Argument: the word "pause" or "resume".

  ++++++++++++++++++++++++++++++++++++++*/
int set_queries(char *input, GString *output, sk_conn_st *condat) 
{
  char argstr[17];
  int pause=0, resume=0;
  int res = 0;
 
  if( sscanf(input, "%16s", argstr) == 1) {
    pause = (strcmp(argstr,"pause") == 0);
    resume = (strcmp(argstr,"resume") == 0);
  }
  
  if( !pause && !resume ) {
    g_string_append(output,  "syntax error.");
    res = PC_RET_ERR;
  }
  else {
   
    if(pause){
	    PW_stopqueries();
	    g_string_append(output, "Queries are stopped\n");
    }else {
	    PW_startqueries();
	    g_string_append(output, "Queries are unblocked\n");
    }
  }
  return res;
}


/*++++++++++++++++++++++++++++++++++++++
  
  Reset the source.

  Reloads the radix tree.

  Argument: the source name.

  ++++++++++++++++++++++++++++++++++++++*/
int set_initrx(char *input, GString *output, sk_conn_st *condat) 
{
ca_dbSource_t *source_hdl;
int res = 0;
 
	source_hdl = ca_get_SourceHandleByName(input); 
        if (source_hdl == NULL){
		g_string_append(output,  "Unknown source");
		res = PC_RET_ERR;
	}
	else if(RP_init_trees( source_hdl ) != RP_OK ) {
		g_string_append(output, "Could not re-initialize radix trees");
                res = PC_RET_ERR;
	}
	else if(RP_sql_load_reg( source_hdl ) != RP_OK ) {
		g_string_append(output, "Could not load radix trees");
                res = PC_RET_ERR;
	}
        else {
         g_string_append(output, "radix trees reloaded successfully\n");		
        }
 return res;
}
/*++++++++++++++++++++++++++++++++++++++
  
  Reset the "session time" and "# of tasks" 
  of a specific thread registered with the TA module.

  ++++++++++++++++++++++++++++++++++++++*/
#if 0

/*
XXX:
I've removed this function because it is supposed to pass a pthread_t 
to the TA_reset_counters() function.  But pthread_t is an opaque
type - on FreeBSD it is a pointer to a structure, so you can't simply
use sscanf() to get one!

Shane
2001-09-05

int set_counter(char *input, GString *output, sk_conn_st *condat) 
{
  unsigned thr_id;
  
  if( sscanf(input, "%d", &thr_id) == 1) {
    TA_reset_counters(thr_id);
  }
  return 0;
}
*/
#endif /* 0 */



/* NO LONGER SUPPORTED AFTER ER to LG transition */
#if 0
/*++++++++++++++++++++++++++++++++++++++
  
  Execute a command in the ER path processor of the ER module.
  (first subject to macro expansion of the first word).

  Argument is passed entirely to ER_macro_spec().

  ++++++++++++++++++++++++++++++++++++++*/
int set_err(char *input, GString *output, sk_conn_st *condat) 
{
  char *erret = NULL;
  int res;

  res = ER_macro_spec(input, &erret);
  g_string_append(output, erret);
  UT_free(erret);

  return res;
}


/*++++++++++++++++++++++++++++++++++++++
  
  Show the current setup of the ER path system of the ER module.
  
  ++++++++++++++++++++++++++++++++++++++*/
int show_err(char *input, GString *output, sk_conn_st *condat) 
{
  char *erret = NULL;

  er_print_paths(&erret);
  g_string_append(output, erret);
  UT_free(erret);

  return 0;
}


/*++++++++++++++++++++++++++++++++++++++
  
  Show the currently defined macros for the ER path system of the ER module.

  ++++++++++++++++++++++++++++++++++++++*/
int show_macros(char *input, GString *output, sk_conn_st *condat)
{
  ER_macro_list(condat);
  return 0;
}



/*++++++++++++++++++++++++++++++++++++++
  
  (re)define a macro for the ER path processor.

  Arguments: The first word is treated as a macro name. 
  The rest of the line is treated as a macro definition.

  ++++++++++++++++++++++++++++++++++++++*/
int set_macro(char *input, GString *output, sk_conn_st *condat)
{
  char *name, *body;
  
  if( strlen(input) > 0 ) {
    body = input;
    name = (char *)strsep(&body, " "); 
    
    ER_make_macro( name, body );
  }

  return 0;
}
#endif /* 0 */




/*++++++++++++++++++++++++++++++++++++++
  
  Trigger running of the socket watchdog actions for a specific thread
  (typically resulting in shutting down of a query thread). 
  
  Arguments are "<socket_id> <thread_id>" as in the output of "show threads".

  Assumes the command is like "stop query 11 17". 
  This is to limit ambiguities (a new thread on the same socket, for example).
. 
  ++++++++++++++++++++++++++++++++++++++*/
#if 0
/*
XXX:
I've removed this function because it is supposed to pass a pthread_t 
to the TA_trigger() function.  But pthread_t is an opaque
type - on FreeBSD it is a pointer to a structure, so you can't simply
use sscanf() to get one!

Shane
2001-09-05

int stop_query(char *input, GString *output, sk_conn_st *condat) 
{
  int fd;
  unsigned thr;

  
  if( sscanf(input, "%d %ud", &fd, &thr) < 2 ) {
    g_string_append(output,"error!!");
    return PC_RET_ERR;
  }
  else {
    TA_trigger("whois", fd, thr);
    return 0;
  }
}
*/
#endif /* 0 */
