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
	//dmalloc_shutdown();
//#if 0
//  purify_new_inuse();
//#else 
//  g_string_append(output, "NOP");
//#endif
  
  return 0;
}

int save_access_tree(char *input, GString * output, sk_conn_st * condat)
{
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

int save_acl_tree(char *input, GString * output, sk_conn_st * condat)
{
	g_string_append(output, "No need to save ACL tree. It's always done right after any change "
		"(which are automatic ban and the 'set acl' console command).");
	return 0;
}

/** reload the acl tree */
int set_acl_tree(char *input, GString * output, sk_conn_st * condat)
{
	int i;

	for (i = MIN_IPSPACE_ID; i <= MAX_IPSPACE_ID; i++) {
		TH_acquire_write_lock(&(act_acl[i]->rwlock));
		rx_delete_tree(act_acl[i], act_acl[i]->top_ptr, 16777216, 0, 0, NULL);
	}

	AC_acc_load();

	for (i = MIN_IPSPACE_ID; i <= MAX_IPSPACE_ID; i++) {
		TH_release_write_lock(&(act_acl[i]->rwlock));
	}

	g_string_append(output, "ACL tree reload successful\n");

	return 0;
}

/** reload the access tree */
int set_access_tree(char *input, GString * output, sk_conn_st * condat)
{
	int i;

	for (i = MIN_IPSPACE_ID; i <= MAX_IPSPACE_ID; i++) {
		TH_acquire_write_lock(&(act_runtime[i]->rwlock));
		if (act_runtime[i]->top_ptr) {
			rx_delete_tree(act_runtime[i], act_runtime[i]->top_ptr, 16777216, 0, 0, NULL);
		}
	}

	AC_persistence_load();

	for (i = MIN_IPSPACE_ID; i <= MAX_IPSPACE_ID; i++) {
		TH_release_write_lock(&(act_runtime[i]->rwlock));
	}

	g_string_append(output, "Access tree reload successful\n");

	return 0;
}

/** reloads the aaa cache */
int set_aaa_cache(char *input, GString * output, sk_conn_st * condat)
{
	AA_load();

	/* if the server is still up, AA_load() didn't crash, so we're OK */
	g_string_append(output, "AAA cache reload successful\n");

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
  /*  deprecated */
  return 0;
} /* show_whois() */


/*++++++++++++++++++++++++++++++++++++++
  
  Display the statistics about the server.

  ++++++++++++++++++++++++++++++++++++++*/
int show_uptime(char *input, GString *output, sk_conn_st *condat) 
{
  char timestring[26];
  extern time_t SV_starttime;
  time_t uptime = time(NULL) - SV_starttime;
  
  ctime_r(&SV_starttime, timestring); 
  SK_cd_printf( condat, 
	       "System running since %sUptime in seconds: %ld (%.1f days)\n\n",
	       timestring,		  
	       uptime,
	       uptime/86400.0);
  
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

int set_access(char *input, GString *output, sk_conn_st *condat)
{
  int res = 0;
  
  /* first 8 characters ("set acl ") are already skipped */
  if( ! NOERR( AC_set_access_command(input))) {
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
int set_initrx(char *input, GString *output, sk_conn_st *condat) {
    ca_dbSource_t *source_hdl;
    SQ_connection_t *con = NULL;
    int res = 0;

    source_hdl = ca_get_SourceHandleByName(input);

    if (source_hdl == NULL ) {
        g_string_append(output, "Unknown source");
        res = PC_RET_ERR;
    } else {
        // set global update lock
        con = SQ_get_connection_by_source_hdl(source_hdl);
        if (SQ_execute_query(con, "SELECT global_lock FROM update_lock WHERE global_lock = 0 FOR UPDATE", NULL )) {
            fprintf(stderr, "SQL ERROR %d: %s\n", SQ_errno(con), SQ_error(con));
            die;
        }

        if (RP_init_trees(source_hdl) != RP_OK) {
            g_string_append(output, "Could not re-initialize radix trees");
            res = PC_RET_ERR;
        } else if ((RP_sql_load_reg(source_hdl) != RP_OK) || (RP_sql_load_start() != RP_OK) || (RP_sql_load_wait_until_finished() != RP_OK)) {
            g_string_append(output, "Could not load radix trees");
            res = PC_RET_ERR;
        } else {
            g_string_append(output, "radix trees reloaded successfully\n");
        }

        // release global update lock
        if (con)
            SQ_close_connection(con);

        return res;
    }
}
