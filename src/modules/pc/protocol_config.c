/***************************************
  $Revision: 1.1 $

  Protocol config module (pc).  This is the protocol that the admin uses to
  talk to the server.

  Status: NOT REVUED, NOT TESTED

  ******************/ /******************
  Filename            : protocol_config.c
  Authors             : ottrey@ripe.net - initial design
                        marek@ripe.net - restructured and rewritten

  To Do               : Add a facility to take callbacks instead of
                        hard-coding menu options.
                        Add in all the menu support provided by the GLib
                        libraries.
                        (Remove strtok if multiple threads are to be used.)
			use gnu readline with expansion and history
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

#define PC_IMPL
#include "lg.h"
#include "rip.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ioctl.h>
#include <glib.h>

/* logging context */
LG_context_t *protocol_config_ctx;

/*+ Maximum size of input that can be recieved from the client. +*/
#define MAX_INPUT_SIZE  1024

extern char* crypt(const char *, const char *);   /* crypt stuff */

void
PC_init (LG_context_t *ctx)
{
  protocol_config_ctx = ctx;
}


/*++++++++++++++++++++++++++++++++++++++
  
  Finds a command by name in the array of command function structures.

  int find_command     returns the index to the correct row of the array, or -1
                       if the command could not be found.

  char *comm_name      name of the command

  Command *comm        array of command function structures.
  ++++++++++++++++++++++++++++++++++++++*/
static 
int find_command(char *comm_name, Command *comm) 
{
  int i;
  char *comm_buffer = UT_strdup(comm_name);
  char *token, *cursor;
  int index = -1;
  
  cursor = comm_buffer;
  if( (token = strsep(&cursor, " \t")) != NULL) {
    for (i=0; comm[i].name != NULL; i++) {
      if ( strcmp(token, comm[i].name) == 0) {
	index = i;
	break;
      }
    }
  }
  
  UT_free(comm_buffer);

  return index; /* returns -1 when command not found */
} /* find_command() */


int show_commands(Command *comm, char *comm_name, GString *output) 
{
  int i = 0;

  g_string_sprintfa(output, "%scommands are:\n\n", comm_name);
  while (comm[i].name != NULL) {
    g_string_sprintfa(output, "%s\t%s\n", comm[i].name, comm[i].help);
    i++;
  }

  return 1;
} /* show_commands() */


/*++++++++++++++++++++++++++++++++++++++

  int command_execute 
                      executes a command from the given array, matching
		      given name. Passes input, output and condat to the
		      function found in the array. Command name is 
		      removed from the input line so only next words are
		      passed over to the function. 

		      returns the code of the last command. Code 
		      PC_RET_QUIT is reserved to indicate that the connection
		      should be closed.

  Command *comm       array of command function structures (defined in
                      protocol_config.h)

  char *comm_name     name of the command to be run

  char *input         rest of the command line

  GString *output     dynamically built output string

  sk_conn_st *condat  socket structure for this connection (some commands
                      use it directly instead of storing the output)

  ++++++++++++++++++++++++++++++++++++++*/
int command_execute(Command * comm, char *comm_name, char *input, GString * output, sk_conn_st * condat)
{
	char *name, *next_word, *tmp_input;
	int index, result = 0;

	/* find the command in the string - first whitespace delimited word */
	/* make a copy of the input */
	tmp_input = UT_strdup(input);
	next_word = tmp_input;

	/* find the first word and set the pointer to the rest of the string */
	name = strsep(&next_word, " \t");

	if (name != NULL && strlen(name) != 0) {
		index = find_command(name, comm);
		if (index != -1) {
			if (next_word != NULL) {
				/* advance the input pointer to the next word */
				while (*next_word != '\0' && isspace(*(unsigned char *)next_word)) {
					next_word++;
				}
			} else {
				next_word = "";
			}

			/* run, Forrest, run... */
			result = comm[index].function(next_word, output, condat);
		} else {
			g_string_sprintfa(output, "invalid %scommand: %s\n", comm_name, name);
			show_commands(comm, comm_name, output);
			result = 2;
		}
	} else {
		show_commands(comm, comm_name, output);
		result = 2;
	}

	UT_free(tmp_input);

	return result;
}								/* command_execute() */

/* proces_input() */
/*++++++++++++++++++++++++++++++++++++++

  Process the input. Finds the proper command in the top level command
  array and invokes the function associated with it with the input and
  output data as arguments.

  int process_input          returns 1 if the connection is to be kept
                             or 0 when it should be finished - that is, 
			     when command_execute() returns PC_RET_QUIT.

  char *input                input (presumably a command)

  sk_conn_st *condat         connection data    

  More:
  +html+ <PRE>
  Author:
        ottrey
	marek - changes and documentation.
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
static 
int process_input(char *input, sk_conn_st *condat) 
{
  int  index;
  int res=0;
  GString *output = g_string_new("");

  index = find_command(input, command);

  switch (index) {
  case -1:
    /* Command not found */
    command_help(NULL, output, condat);
    break;
    
  default: 
    res = command_execute(command, "", input, output, condat);
  }
  
  if(res != PC_RET_QUIT) {
    /*
      printf("thread output=\n%s\n", output);
    */
    if ( CO_get_clear_screen() == 1 ) {
      SK_cd_puts(condat, CLEAR_SCREEN);
    }
    SK_cd_puts(condat,  output->str);
    SK_cd_printf(condat, "\n\n=%d= %s", res, CO_get_prompt());
    
  }
  
  g_string_free( output, TRUE );

  /* the return value is the connection state: 1=still open, 0=to be closed
   */

  return (res != PC_RET_QUIT);
} /* process_input() */


/*++++++++++++++++++++++++++++++++++++++
  
  Authenticates the user - asks for password and checks it. The password is
  echoed by the tcp stack, to disable that one would have to attach a tty
  to this connection and switch to raw mode or try the hard way - renegotiate
  the telnet connection to switch to character mode (and, possibly, back).
  The latter has the drawback that to do it right it has to be able to check
  whether there's telnet on the other side - otherwise, if the connection
  is made by a program just connecting to the socket, garbage will result.
  However, in such case password checking might be not a good idea.

  sk_conn_st *condat

  More:
  +html+ <PRE>
  Author:
        ottrey
	marek - slight changes and documentation.
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
static 
char *authenticate_user(sk_conn_st *condat) 
{
  char *user = NULL;
  const char Salt[2] = "DB";
  char input[MAX_INPUT_SIZE];
  int read_result;
  char *password=NULL;
  char *user_password=NULL;
  char user_buf[10];

  SK_cd_puts(condat, LOGIN_PROMPT);
  read_result = SK_cd_gets(condat, input, MAX_INPUT_SIZE);

  strncpy(user_buf, input, 10);

  SK_cd_puts(condat, PASSWD_PROMPT);
  /* XXX These aren't working.
  SK_puts(sock, ECHO_ON);
  echo_off(sock);
  */
  read_result = SK_cd_gets(condat, input, MAX_INPUT_SIZE);
  /* XXX These aren't working.
  echo_on(sock);
  SK_puts(sock, ECHO_OFF);
  */

  password = crypt(input, Salt);

  user_password = PR_get_property(user_buf, DEFAULT_USER_NAME);

  if (user_password != NULL) {
    if (strcmp(password, user_password) == 0) {
      user = UT_strdup(user_buf);
    }
  }

  
  return user;

} /* authenticate_user() */



/*++++++++++++++++++++++++++++++++++++++
  
  Main function that talks to the user connected on the given socket.
  Starts by authenticating the user (if this mode is active)
  and greeting her with the uptime data. Then it loops reading and executing
  commands until the "quit" command (or any other command that causes
  process_input to return 0).

  int sock        connected client socket

  ++++++++++++++++++++++++++++++++++++++*/
void PC_interact(svr_args *args) {
	char input[MAX_INPUT_SIZE];
	int connected = 1;
	char *user = NULL;
	sk_conn_st condat;

	/* init the connection structure, set timeout for reading the query */
    SK_cd_make(&condat, args->conn_sock, 0);

	/* Welcome the client */
	SK_cd_puts(&condat, CO_get_welcome());

	/* Authenticate the user */
	if (CO_get_authenticate() == 1) {
		user = authenticate_user(&condat);

		if (user == NULL) {
			LG_log(protocol_config_ctx, LG_INFO, "unsuccesful login attempt from %s", condat.rIPs);
		}
	} else {
		user = "nobody";
	}

	if (user != NULL) {

		/* Log admin logging on */
		LG_log(protocol_config_ctx, LG_INFO, "user %s from %s logged on", user, condat.rIPs);

		{
			show_uptime("", NULL, &condat);
		}

		SK_cd_printf(&condat, "=0= %s", CO_get_prompt());

		while (condat.rtc == 0 && connected) {
			char *icopy;

			/* Read input. Quit if no input (socket closed) */
			if (SK_cd_gets(&condat, input, MAX_INPUT_SIZE) <= 0) {
				break;
			}

			/* filter junk out: leading/trailing/redundant whitespaces */
			icopy = ut_string_compress(input);

			/* set thread accounting */
			TA_setactivity(icopy);
			TA_increment();

			/*      if( strlen(icopy) > 0 ) { */
			{
				LG_log(protocol_config_ctx, LG_DEBUG, "%s", icopy);

				connected = process_input(icopy, &condat);
			}

			TA_setactivity("");

			UT_free(icopy);
		}

		/* Log admin logging off */
		LG_log(protocol_config_ctx, LG_INFO, "user %s from %s logged off", user, condat.rIPs);
	}

	SK_cd_free(&condat);
}
