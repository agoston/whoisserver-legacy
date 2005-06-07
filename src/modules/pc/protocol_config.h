#ifndef READ_PROTOCOL_CONFIG
#define READ_PROTOCOL_CONFIG

/***************************************
  $Revision: 1.1 $

  Protocol config module (pc).  This is the protocol that the admin uses to
  talk to the server.

  Status: NOT REVUED, TESTED

  
  
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

#define DEFAULT_USER_NAME "user"

/*+ Log message +*/
#define LOG_CONFIG "[%d] %s %s %s\n"

/*+ This propmt is sent to the user. +*/
/*
#define PROMPT  "\nwhois R.I.P. config> "
#define PROMPT  "\nwhois \033[0;1;33;40mR.I.P.\033[0m config> "
*/

/*+ This login propmt is sent to the user. +*/
#define LOGIN_PROMPT  "\nlogin: "

/*+ This password propmt is sent to the user. +*/
#define PASSWD_PROMPT  "password: "

/* XXX Why doesn't invisible work ???!?!?!

http://www.dee.ufpb.br/~bob/tools/ansi.html
http://cs.wwc.edu/~bellmi/ANSI_escape_codes.html

#define ECHO_ON "\033[8m"
*/

/* Use reverse video instead - grrr. */
#define ECHO_ON "\033[33;43m"

#define ECHO_OFF "\033[0m"

#define CLEAR_SCREEN "\033[2J"

/*+ This welcome message is sent to the user, upon connecting. +*/
/*
#define WELCOME  "Welcome to the whois R.I.P. server.\n"
*/

#define HELP_ERROR        "Invalid help command: "
#define HELP_HELP         "Print help information."
#define HELP_QUIT         "Quit the server."
#define HELP_SAVE         "Save a structure/object."
#define HELP_SAVE_ACCESS_TREE "Saves the access control tree."
#define HELP_SHOW         "Show the values of: (type show for a list)"
#define HELP_SHOW_ERROR   "Invalid show command: "
#define HELP_SHOW_CONST   "The values of a constant used in the server."
#define HELP_SHOW_CONSTS  "The values of constants used in the server.\n"\
                          "(This is set from values in properties object.)"
#define HELP_SHOW_PROPS   "The values of properties in the properties file."
#define HELP_SHOW_THREADS "The properties of the client's thread."
#define HELP_SHOW_WHOIS   "The values of the whois query module."
#define HELP_SHOW_ACCESS  "The access to server."
#define HELP_SHOW_ACL     "The access control list for server."
#define HELP_SHOW_AUTO_SAVE "The access control list auto save status."
#define HELP_SHOW_ERR     "Show the setup of the error path system"
#define HELP_SHOW_MACROS  "Show the currently defined macros of the error path system"
#define HELP_SHOW_UPTIME  "Show system uptime statistics"
#define HELP_SHOW_DYNAMIC "Show status of dynamic server tag"

#define HELP_SET          "Set the values of: (type set for a list)"
#define HELP_SET_ERROR    "Invalid set command: "
#define HELP_SET_CONST    "Set the values of a constants: (type set const name=value)"
#define HELP_SET_CONSTS   "Set the values of constants used in the server.\n"\
                          "(This is set from values in properties object.)"
#define HELP_SET_PROPS    "Set the values of properties in the properties file."
#define HELP_SET_AUTO_SAVE "Sets the auto saving of the access tree to SQL."
#define HELP_SET_ACL      "Create/modify acl entries[A for a given address or prefix.\n" \
"\tSyntax:  set acl ip[/prefixlength] column=value,column=value...\n" \
"\tColumn names as in acl display. Unset columns are inherited."
#define HELP_SET_NODENY   "Set the deny counter in the access tree to 0\n"\
                          "\tSyntax:  set nodeny ip"
#define HELP_STOP         "Stop activities (type stop for a list)"
#define HELP_STOP_QUERY   "Stop a query thread (arguments are: <socket #> <thread #>)"
#define HELP_SET_UPDATES  "Pause/resume all update threads: \n"\
                          "\tSyntax: set updates {pause|resume}"
#define HELP_SET_QUERIES  "Pause/resume all queries: \n"\
                          "\tSyntax: set queries {pause|resume}"
#define HELP_SET_INITRX   "Reload radix trees for a given source: \n"\
			  "\tSyntax: set initrx <source>" 
#define HELP_SET_ERR      "Modify the error path system.\n"\
                          "\tSyntax: set err <errspec>\n"\
                          "\tFor details of <errspec> please see the error specification document\n"

#define HELP_SET_MACRO    "Set a new macro in the error path system"
#define HELP_SET_COUNTER "Reset the counter for the given thread. Argument: thread_id"
#define HELP_SET_DYNAMIC "Set server dynamic status"


#define PC_RET_QUIT     (0xabcdef) /* make it really uniq */
#define PC_RET_ERR      (1)


void PC_interact(int sock);
void PC_init (LG_context_t *ctx);


/*+ Each command has a +*/
typedef struct _command {
  const char *name;                          /*+ Name to be invoked. +*/
  int (*function)(char *, GString *, sk_conn_st *);/*+ Function to be invoked. +*/
  const char *help;                                /*+ Command help. +*/
} Command;

int show_commands(Command *comm, char *comm_name, GString *output);

#ifdef PC_IMPL

/*+
 * Contains the command definitions
+*/
struct _command command[] = {
  {"help"   , command_help   , HELP_HELP   },
  {"quit"   , command_quit   , HELP_QUIT   },
  {"save"   , command_save   , HELP_SAVE   },
  {"show"   , command_show   , HELP_SHOW   },
  {"stop"   , command_stop   , HELP_STOP   },
  {"set"    , command_set    , HELP_SET    },
  {"purify" , command_purify , "trigger a new memory-in-use report" },
  {NULL     , NULL           , NULL        }
};

/*+
 * Contains the save commands
+*/
struct _command save[] = {
  {"access_tree", save_access_tree, HELP_SAVE_ACCESS_TREE },
  {NULL         , NULL            , NULL                  }
};

/*+
 * Contains the show commands
+*/
struct _command show[] = {
  {"const"    , show_const    , HELP_SHOW_CONST     },
  {"consts"   , show_consts   , HELP_SHOW_CONSTS    },
  {"props"    , show_props    , HELP_SHOW_PROPS     },
  {"threads"  , show_threads  , HELP_SHOW_THREADS   },
  {"whois"    , show_whois    , HELP_SHOW_WHOIS     },
  {"access"   , show_access   , HELP_SHOW_ACCESS    },
  {"acl"      , show_acl      , HELP_SHOW_ACL       },
  {"auto_save", show_auto_save, HELP_SHOW_AUTO_SAVE },
/*  {"err"      , show_err      , HELP_SHOW_ERR       },*/
/*  {"macros"   , show_macros   , HELP_SHOW_MACROS    },*/
  {"uptime"   , show_uptime   , HELP_SHOW_UPTIME    },
  {"dynamic"  , show_dynamic  , HELP_SHOW_DYNAMIC   },
  {NULL       , NULL          , NULL                }
};
 
/*+
 * Contains the set commands
+*/
struct _command set[] = {
  {"acl"      , set_acl       , HELP_SET_ACL       },
  {"nodeny"   , set_nodeny    , HELP_SET_NODENY    },
  {"updates"  , set_updates   , HELP_SET_UPDATES   },
  {"queries"  , set_queries   , HELP_SET_QUERIES   },
/*  {"err"      , set_err       , HELP_SET_ERR       },*/
/*  {"macro"    , set_macro     , HELP_SET_MACRO     }, */
/*  {"counter", set_counter   , HELP_SET_COUNTER   },*/
  {"auto_save",  set_auto_save, HELP_SET_AUTO_SAVE },
  {"initrx"   ,  set_initrx   , HELP_SET_INITRX    },
  {"dynamic"  ,  set_dynamic  , HELP_SET_DYNAMIC   },
  {NULL       , NULL          , NULL               }
};

struct _command stop[] = {
/*  {"query"  , stop_query  , HELP_STOP_QUERY  },*/
  {NULL     , NULL        , NULL            }
};

#else

extern struct _command command[], save[], show[], set[], stop[]; 

#endif /* PC_IMPL */

int command_execute(Command *comm, char *comm_name,
		    char *input, GString *output, sk_conn_st *condat) ;
void
PC_init (LG_context_t *ctx);

#endif /* READ_PROTOCOL_CONFIG */
