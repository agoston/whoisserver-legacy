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

#ifndef DBUPDATE_H
#define DBUPDATE_H


#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h> 
#include <stubs.h>
#include <glib.h>

#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <errno.h> 
#include <unistd.h>

#include "rip.h"
#include <config.h>
#include "ca_configFns.h"
#include "ca_dictionary.h"
#include "ca_macros.h"
#include "ca_srcAttribs.h"
#include "lu.h"
#include "rt_dbupdate.h"


#define DEBUG_LEVEL  LG_ALL
#define RT_PERROR    32  /* used to determine error messages to be logged to dbuperr log file */
#define UPDATE_QUERY_TIMEOUT 60
#define RIP_TIMEOUT  10
#define COUNTRY_LIST_SIZE 400
#define NIC_SUFFIX_LIST_SIZE 50
#define AUTO_KEY_LENGTH  32  /* for variables malloc'd when processing AUTO- nic-hdls */
#define DB_MAXDATASIZE 1023
#define OPENSSL_CMD "openssl "

/******************* TEMP DEFINITIONS ****************/
#define LG_PERROR 128
/*****************************************************/

/* structure containing the information from the update mail header,
   if it was a mail update */
/*
Now moved to rt_dbupdate.h
typedef struct
{
  char *from;
  char *from_email;
  char *cc;
  char *subject;
  char *date;
  char *replyto;
  char *msgid;
} mail_hdr_t;
*/

/* structure containing the input flags and other data that needs to be
   used at many levels withing dbupdate 
   ***WARNING - IF THIS IS CHANGED YOU MUST CHANGE THE CODE IN DBUPDATE:MAIN
   WHERE THIS STRUCTURE IS INITIALISED */
typedef struct
{
  char *config_file_name;
  char *input_file_name;
  char *time_str;            /* start time for this dbupdate */
  int test_mode;             /* flag set from command line or config value */
  int mail_input;            /* flag set from command line */
  int debug;                 /* flag set from command line */
  int debug_level;           /* output level for debug */
  int redirect;              /* flag set from command line */
  int print;                 /* flag set from command line */
  char *keywords;            /* keywords passed by syncupdates as command option */
  char *X509cert_file;       /* file name containing X509 certificate passed by syncupdates as command option */
  char *X509certificate;     /* file name containing X509 certificate passed by syncupdates as command option */
  char *origin;              /* IP addess passed by syncupdates as command option */
  int enforced_new;          /* set if valid new keyword found */
  int notif_diff;            /* set if valid diff keyword found */
  int help;                  /* set if valid help keyword found */
  int count_successful;      /* number of objects successfuly processed */
  int count_unsuccessful;    /* number of objects unsuccessfuly processed */
  int count_noop;            /* number of objects processed without change */
  FILE *state;               /* file pointer to state log file */
  mail_hdr_t mail_hdr_data;  /* contains the data from the update mail header */
  GHashTable *AUTO_key_hash;  /* stores details of auto generated keys */
  GHashTable *ntfy_hash;     /* stores email addresses for notification messages */
  GHashTable *forw_hash;     /* stores email addresses for forward messages */
} options_struct_t;


/* structure containing data associated with the current source.
   the current source is determined from the source attribute of the 
   object currently being processed. 
   the data is taken from a list compiled from reading the config file */
typedef struct
{
  char *current_source;
  char *update_host;
  char *query_host;
  unsigned int  update_port;
  int  query_port;
  char *DBhost;
  unsigned int  DBport;
  char *DBuser;
  char *DBname;
  char *DBpasswd;
  LU_server_t *current_server;
} source_data_t;


/* return codes used between functions within dbupdate 
   set to bit values, 0,1,2,4,etc */
typedef enum
{
  UP_OK      =0, /* update succeded for this object */
  UP_FAIL    =1, /* update failed for this object, move onto the next object (if any) */
  UP_FATAL   =2, /* fatal error occured, don't process any more objects */
  UP_NOOP    =4, /* no change on modify */
  UP_NOOBJECT=8, /* no object found in submission */
  UP_FWD     =16 /* update forwarded for human processing */
} return_codes;

/* convert the enum to a printable string */
#define UP_ret2str(r) \
  (((r) == UP_OK)        ? "UP_OK" : \
   ((r) == UP_FAIL)      ? "UP_FAIL" : \
   ((r) == UP_FATAL)     ? "UP_FATAL" : \
   ((r) == UP_NOOP)      ? "UP_NOOP" : \
   ((r) == UP_NOOBJECT)  ? "UP_NOOBJECT" : \
   ((r) == UP_FWD)       ? "UP_FWD" : \
                            "UNKNOWN" )

/* codes defining the operations performed on objects by dbupdate 
   alligned with the same values used by the AU module*/
typedef enum
{
  UP_CREATE,
  UP_MODIFY,
  UP_DELETE,
  UP_NOOP_OP,
  UP_SYNTAX_ERR,
  UP_FWD_OP_CREATE,
  UP_FWD_OP_MODIFY,
  UP_FWD_OP_DELETE,
  UP_FWD_OP_POLICY
} op_codes;

/* convert the enum to a printable operation string */
#define UP_op2str(r) \
  (((r) == UP_CREATE)     ? "UP_CREATE" : \
   ((r) == UP_MODIFY)     ? "UP_MODIFY" : \
   ((r) == UP_DELETE)     ? "UP_DELETE" : \
   ((r) == UP_NOOP_OP)    ? "UP_NOOP"   : \
   ((r) == UP_FWD_OP_CREATE)     ? "UP_FWD_OP_CREATE"   : \
   ((r) == UP_FWD_OP_MODIFY)     ? "UP_FWD_OP_MODIFY" : \
   ((r) == UP_FWD_OP_DELETE)     ? "UP_FWD_OP_DELETE" : \
   ((r) == UP_FWD_OP_POLICY)     ? "UP_FWD_OP_POLICY" : \
                            "UNKNOWN" )

/* convert the enum to a printable ripupdate operation string */
#define UP_op2ripopstr(r) \
  (((r) == UP_CREATE)     ? "ADD" : \
   ((r) == UP_MODIFY)     ? "UPD" : \
   ((r) == UP_DELETE)     ? "DEL" : \
                            "UNKNOWN" )

/* convert the operation to an AU operation */
#define op2au_op(op) ( \
    ((op) == UP_CREATE)   ? AU_CREATE : \
    ((op) == UP_MODIFY)   ? AU_MODIFY : \
    ((op) == UP_DELETE)   ? AU_DELETE : \
                            -1 )

/* convert the operation to a forward operation */
#define op2fwd_op(op) ( \
    ((op) == UP_CREATE)   ? UP_FWD_OP_CREATE : \
    ((op) == UP_MODIFY)   ? UP_FWD_OP_MODIFY : \
    ((op) == UP_DELETE)   ? UP_FWD_OP_DELETE : \
                            -1 )

/* convert the operation to a printable forward operation */
#define op2fwd_op_str(op) ( \
    ((op) == UP_CREATE)   ? "CREATION" : \
    ((op) == UP_MODIFY)   ? "MODIFICATION" : \
    ((op) == UP_DELETE)   ? "DELETION" : \
                            "UNKNOWN OPERATION" )

/* convert the operation to an RT operation */
#define op2rt_upd_op(op) ( \
    ((op) == UP_CREATE)            ? RT_UPD_ADD : \
    ((op) == UP_MODIFY)            ? RT_UPD_UPD : \
    ((op) == UP_DELETE)            ? RT_UPD_DEL : \
    ((op) == UP_NOOP_OP)           ? RT_UPD_NOOP : \
    ((op) == UP_SYNTAX_ERR)        ? RT_UPD_SYNTAX_ERR : \
    ((op) == UP_FWD_OP_CREATE)     ? RT_UPD_FWD_CREATE : \
    ((op) == UP_FWD_OP_MODIFY)     ? RT_UPD_FWD_MODIFY : \
    ((op) == UP_FWD_OP_DELETE)     ? RT_UPD_FWD_DELETE : \
    ((op) == UP_FWD_OP_POLICY)     ? RT_UPD_FWD_POLICY : \
                              -1 )

/* structure containing data about a single command line option */
typedef struct
{
  char option;
  char *long_opt;
  int  arg_required;  /* 0 for no arg, 1 for mandatory arg, 2 for optional arg */
} opt;

typedef enum
{
  UP_HELP=0,
  UP_HOWTO,
  UP_NEW,
  UP_NOTIF_DIFF,
  NUMBER_OF_KEYWORDS     /* this must be the last entry in the list  */
} key_word_index;


typedef struct
{
  char *word;
  int  index;
} key_words;


/* function prototypes */



#endif
