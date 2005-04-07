/***************************************
  $Revision: 1.1 $

  Query command module (qc).  This is what the whois query gets stored as in
  memory.

  Status: NOT REVUED, TESTED

  ******************/ /******************
  Filename            : query_command.c
  Author              : ottrey@ripe.net
  Modifications by    : marek@ripe.net
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

#define QC_IMPL
#include "rip.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* logging */
LG_context_t *qc_context;

/* 
  note on errors: 
     QC_SYNERR returns a full help text
     QC_PARERR returns only any messags
 */

/* only those are allowed in query strings*/
#define ALLOWED_QUERY_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZ"\
                            "abcdefghijklmnopqrstuvwxyz"\
                            "0123456789-_:+=.,@/?' \n"

extern char *suboptarg;
extern int getsubopt(char **optionp, char * const *tokens, char **valuep);

/*++++++++++++++++++++++++++++++++++++++
  make a list of sources. expects list to hold source handles

  char *
  qc_sources_list_to_string     returns an allocated string, must be freed
  
  GList *list                   list of source handles (as defined by CA)

  ++++++++++++++++++++++++++++++++++++++*/
char *
qc_sources_list_to_string(GList *list)
{
    GString *tmp;
    GList *qitem;
    char *result;

    /* use GString for the result, because it resizes magically */
    tmp = g_string_sized_new(STR_L);

    /* loop through the list, creating a string with the names of the
       sources separated by commas */
    qitem = g_list_first(list);
    if (qitem != NULL) {
        for (;;) {
            ca_dbSource_t *source_hdl = (ca_dbSource_t *) ( qitem->data );
            char *srcname = ca_get_srcname( source_hdl );
            g_string_append(tmp, srcname);

            qitem = g_list_next(qitem);
            if (qitem == NULL) {
                break;
            }

            g_string_append_c(tmp, ',');
        }
    }
      
    /* move to a new buffer for the return */
    result = UT_strdup(tmp->str);
    g_string_free(tmp, TRUE);
    return result;
}

/* QC_environ_to_string() */
/*++++++++++++++++++++++++++++++++++++++
  Convert the query_environ to a string.

  Query_environ *query_environ The query_environ to be converted.
   
  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *
QC_environ_to_string(Query_environ qe) 
{
  char *sources;
  char passed_addr[IP_ADDRSTR_MAX];
  GString *tmp;
  char *result;

  /* convert the sources and the passed address (if any) to printable strings */
  sources = qc_sources_list_to_string(qe.sources_list);
  if( IP_addr_b2a( &(qe.pIP), passed_addr, IP_ADDRSTR_MAX) != IP_OK ) { 
    passed_addr[0] = '\0';
  }
  
  /* format the environment info */
  tmp = g_string_sized_new(STR_L);
  g_string_sprintf(tmp, 
                   "host=%s, keep_connection=%s, sources=%s, version=%s%s%s", 
                   qe.condat.ip, 
                   qe.k ? "on" : "off", 
                   sources, 
                   (qe.version == NULL) ? "?" : qe.version,
                   passed_addr[0] == '\0' ? "" : ", passedIP=",
                   passed_addr);
  
  /* move result to return buffer, and free up memory */
  result = UT_strdup(tmp->str);
  g_string_free(tmp, TRUE);
  UT_free(sources);

  return result;
  
} /* QC_environ_to_string() */

/* QC_query_command_to_string() */
/*++++++++++++++++++++++++++++++++++++++
  Convert the query_command to a string.

  Query_command *query_command The query_command to be converted.
   
  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
char *QC_query_command_to_string(Query_command *query_command) {
  char result_buf[STR_XL];
  char *str1;
  char *str2;
  char *str3;

  str1 = MA_to_string(query_command->inv_attrs_bitmap, DF_get_attribute_names());
  str2 = MA_to_string(query_command->object_type_bitmap, DF_get_class_names());
  str3 = WK_to_string(query_command->keytypes_bitmap);
  
  sprintf(result_buf, "Query_command : inv_attrs=%s, recursive=%s, object_type=%s, (c=%s,G=%s,B=%s,b=%s,g=%d,l=%d,m=%d,q=%d,t=%d,v=%d,x=%d,F=%d,K=%d,L=%d,M=%d,R=%d), possible keytypes=%s, keys=[%s]",
          str1,
	  query_command->recursive?"y":"n",
          str2,
          query_command->c_irt_search ? "TRUE" : "FALSE",
          query_command->G_group_search ? "TRUE" : "FALSE",
          query_command->B ? "TRUE" : "FALSE",
          query_command->b ? "TRUE" : "FALSE",
          query_command->g,
          query_command->l,
          query_command->m,
          query_command->q,
          query_command->t,
          query_command->v,
          query_command->x,
          query_command->fast,
          query_command->filtered,
          query_command->L,
          query_command->M,
          query_command->R,
          str3,
          query_command->keys);
  UT_free(str1);
  UT_free(str2);
  UT_free(str3);

  return UT_strdup(result_buf);
  
} /* QC_query_command_to_string() */

/* log_command() */
/*++++++++++++++++++++++++++++++++++++++
  Log the command.
  This is more to do with Tracing.  And should/will get merged with a tracing
  module (when it is finalized.)

  char *query_str
  
  Query_command *query_command
   
  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
static void 
log_command (const char *query_str, Query_command *query_command) 
{
  char *str;

  str = QC_query_command_to_string(query_command);
  LG_log(qc_context, LG_DEBUG, "query=[%s]   %s", query_str, str);
  UT_free(str);
} /* log_command() */

/* QC_environ_free() */
/*++++++++++++++++++++++++++++++++++++++
  Free the query_environ.

  Query_command *qc query_environ to be freed.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
void QC_environ_free(Query_environ *qe) {
  if (qe != NULL) {
    UT_free(qe->version);

    if (qe->sources_list != NULL) {
      g_list_free(qe->sources_list); 
      qe->sources_list=NULL;
    }
    UT_free(qe);
  }
} /* QC_environ_free() */

/* QC_free() */
/*++++++++++++++++++++++++++++++++++++++
  Free the query_command.

  Query_command *qc query_command to be freed.

  XXX I'm not sure the bitmaps will get freed.
  qc->inv_attrs_bitmap
  qc->object_type_bitmap
  qc->keytypes_bitmap

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
void 
QC_free (Query_command *qc) {
    GList *message_node;

    if (qc != NULL) {
        if (qc->keys != NULL) {
            g_free(qc->keys);
        }
        message_node = qc->parse_messages;
        while (message_node != NULL) {
            UT_free(message_node->data);
            message_node = g_list_next(message_node);
        }
        g_list_free(qc->parse_messages);
        UT_free(qc);
    }
} /* QC_free() */

void 
QC_init_struct (Query_command *query_command)
{
    query_command->query_type = QC_SYNERR;
    query_command->c_irt_search = FALSE;
    query_command->G_group_search = TRUE; /* grouping is on by default */
    query_command->B = FALSE; /* "original" output is off by default */
    query_command->b = FALSE;
    query_command->d = 0;
    query_command->fast = 0;
    query_command->g = 0;
    query_command->inv_attrs_bitmap = MA_new(MA_END);
    query_command->recursive = 1;  /* Recursion is on by default. */
    query_command->l = 0;
    query_command->m = 0;
    query_command->q = -1;
    query_command->t = -1;
    query_command->v = -1;
    query_command->x = 0;
    query_command->filtered = 0;
    query_command->L = 0;
    query_command->M = 0;
    query_command->R = 0;
    /* XXX UGLY - "all zeros" in object_type_bitmap means the same as 
      "all ones". To limit the inconsistency, this is changed at the end 
      of this function, so outside "all zeros" is an illegal value. */
    query_command->object_type_bitmap = MA_new(MA_END);
    query_command->keytypes_bitmap = MA_new(MA_END);
    query_command->keys = NULL;
    query_command->parse_messages = NULL;
}


/* QC_fill() */
/*++++++++++++++++++++++++++++++++++++++
  Create a new query_command.
  Returns 0 when OK, -1 when query incorrect.
  
  char *query_str The garden variety whois query string.

  Query_environ *qe the environment

  More:
  +html+ <PRE>
  Authors:
        ottrey - original code
	marek - modified for my getopts, multiple sources;
	        and generally cleaned.
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
static
int QC_fill (const char *query_str, 
             Query_command *query_command, 
             Query_environ *qe) 
{
  
  int c;
  int synerrflg = 0;
  int badparerr = 0;
  int minusk = 0;
  mask_t inv_attr_mask;
  gchar **inv_attrs;
  int opt_argc;
  gchar **opt_argv;
  gchar **opt_argv_copy;
  int i;
  C_Type_t type;
  A_Type_t attr;
  getopt_state_t *gst = NULL;
  gchar **types;
  gchar **version_info;

  int ip_flag_used;
  int ip_flag_duplicated;
  GList *sources_specified;
  int num_flags;
  int num_client_ip;

  gboolean is_ip_key;
  gboolean fixed_lookup;
  char lookup[64];

  ip_flag_used = 0;
  ip_flag_duplicated = 0;
  sources_specified = NULL;
  num_flags = 0;
  num_client_ip = 0;

  /* split into words */
  opt_argv = ut_g_strsplit_v1(query_str, " ", -1);

  /* count number of non-empty words */
  opt_argc = 0;
  for (i=0; opt_argv[i] != NULL; i++) {
      if (opt_argv[i][0] != '\0') {
          opt_argc++;
      }
  }

  /* make a copy of the arguments with empty words removed */
  opt_argv_copy = g_new(gchar*, opt_argc+1);
  opt_argc = 0;
  for (i=0; opt_argv[i] != NULL; i++) {
      if (opt_argv[i][0] != '\0') {
          opt_argv_copy[opt_argc] = opt_argv[i];
          opt_argc++;
      } else {
          g_free(opt_argv[i]);
      }
  }
  opt_argv_copy[opt_argc] = NULL;
  g_free(opt_argv);
  opt_argv = opt_argv_copy;

  dieif( (gst = mg_new(0)) == NULL );
  
  while ((c = mg_getopt(opt_argc, opt_argv, "acdgi:klbmq:rs:t:v:xBGFKLMRST:V:", 
			gst)) != EOF) {
    num_flags++;

    switch (c) {
      case 'a':
          /* Add all the config sources to the sources list. */
          {
	      int i;
	      ca_dbSource_t *hdl;
	  
	      for (i=0; (hdl = ca_get_SourceHandleByPosition(i)) != NULL; i++) {
	          sources_specified = g_list_append(sources_specified, hdl);
	      }
	  }
      break;

      case 'c':
        query_command->c_irt_search = TRUE;
        query_command->l = 0;
        query_command->m = 0;
        query_command->x = 0;
        query_command->L = 0;
        query_command->M = 0;
        if (ip_flag_used) ip_flag_duplicated++;
        ip_flag_used = 'c';
        break;

			case 'G':
				query_command->G_group_search=FALSE;
			break;

			case 'B':
				query_command->B=TRUE;
			break;

      case 'b':
        query_command->b=TRUE;
				query_command->c_irt_search = TRUE;
        query_command->l = 0;
        query_command->m = 0;
        query_command->x = 0;
        query_command->L = 0;
        query_command->M = 0;
        if (ip_flag_used) ip_flag_duplicated++;
        ip_flag_used = 'b';
			break;

      case 'd':
        query_command->d=1;
      break;

      case 'g':
        query_command->g=1;
      break;

      case 'i':
        /* always get an argument here (according to the code) */
        dieif(gst->optarg == NULL);

        /* 
           Now a really stupid hard-coded hack to support "pn" being a
           synonym for "ac,tc,zc,ah".  I particularly object to this because
           it references attributes that should only be defined in XML - but
           I don't see a simplier more robust way of doing this hack.
           :-( - ottrey 8/12/99 

	   ** removed a memory leak - MB, 1/08/00

           ** removed the use of "ro", added "person" - shane, 2002-01-23

           ** simplified to simply set the flags rather than requiring a
              memory allocation and parse - shane, 2002-06-27

         */

        /* check each argument present */
        inv_attr_mask = MA_new(INV_ATTR_MASK);
        inv_attrs = ut_g_strsplit_v1(gst->optarg, ",", -1);
        for (i=0; inv_attrs[i] != NULL; i++) {
            g_strdown(inv_attrs[i]);
            if (!strcmp(inv_attrs[i],"pn") || !strcmp(inv_attrs[i],"person")) {
                /* person means "admin-c,tech-c,zone-c,author" */
                MA_set(&(query_command->inv_attrs_bitmap), A_AC, 1);
                MA_set(&(query_command->inv_attrs_bitmap), A_TC, 1);
                MA_set(&(query_command->inv_attrs_bitmap), A_ZC, 1);
                MA_set(&(query_command->inv_attrs_bitmap), A_AH, 1);
            } else {
                /* otherwise it should be an inverse attribute name */
                attr = DF_attribute_name2type(inv_attrs[i]);
                if ((int)attr < 0) {
                    /* ERROR:104 */
                    /* "%s" is not a known RPSL attribute. */
                    char *fmt = ca_get_qc_fmt_badattr;
                    query_command->parse_messages = 
                        g_list_append(query_command->parse_messages, 
                                      g_strdup_printf(fmt, inv_attrs[i]));
                    UT_free(fmt);
                    badparerr++;
                } else if (!MA_isset(inv_attr_mask, attr)) {
                    /* ERROR:105 */
	            /* "%s" is not an inverse searchable attribute. */
                    char *fmt = ca_get_qc_fmt_attrnotinv;
                    query_command->parse_messages = 
                        g_list_append(query_command->parse_messages, 
                                      g_strdup_printf(fmt, inv_attrs[i]));
                    UT_free(fmt);
                    badparerr++;
                } else {
                    /* Add the attr to the bitmap. */
                    MA_set(&(query_command->inv_attrs_bitmap), attr, 1);
                }
            }
        }
        g_strfreev(inv_attrs);

      break;

      case 'k':
	minusk = 1;
      break;

      case 'r':
        query_command->recursive=0;       /* Unset recursion */
      break;

      case 'l':
        query_command->c_irt_search = FALSE;
        query_command->l = 1;
        query_command->m = 0;
        query_command->x = 0;
        query_command->L = 0;
        query_command->M = 0;
        if (ip_flag_used) ip_flag_duplicated++;
        ip_flag_used = 'l';
      break;

      case 'm':
        query_command->c_irt_search = FALSE;
        query_command->l = 0;
        query_command->m = 1;
        query_command->x = 0;
        query_command->L = 0;
        query_command->M = 0;
        if (ip_flag_used) ip_flag_duplicated++;
        ip_flag_used = 'm';
      break;

      case 'q':
        /* always get an argument here (according to the code) */
        dieif(gst->optarg == NULL);

        if (strcasecmp(gst->optarg, "sources") == 0) {
            query_command->q = QC_Q_SOURCES;
        } else if (strcasecmp(gst->optarg, "version") == 0) {
            query_command->q = QC_Q_VERSION;
        } else if (strcasecmp(gst->optarg, "types") == 0) {
            query_command->q = QC_Q_TYPES;
        } else {
            synerrflg++;
        } 
      break;

      case 's':
        /* always get an argument here (according to the code) */
        dieif(gst->optarg == NULL);

        {
            gchar **sources;
	    ca_dbSource_t *hdl;

            g_strdown(gst->optarg);
	  
	    /* go through specified sources */
            sources = ut_g_strsplit_v1(gst->optarg, ",", -1);
            for (i=0; sources[i] != NULL; i++) {
                hdl = ca_get_SourceHandleByName(sources[i]);
	        if (hdl == NULL) {
                    /* ERROR:102 */
	            /* Unknown source %s requested. */
                    char *fmt = ca_get_qc_fmt_badsource;
                    g_strup(sources[i]);
                    query_command->parse_messages = 
                        g_list_append(query_command->parse_messages, 
                                      g_strdup_printf(fmt, sources[i]));
                    UT_free(fmt);
                    badparerr++;
                } else {
	            /* append */
	            sources_specified = g_list_append(sources_specified, hdl);
                }
	    }
            g_strfreev(sources);
        }
	break;
	
      case 't':
        /* always get an argument here (according to the code) */
        dieif(gst->optarg == NULL);

	/* parse the specification */
        g_strdown(gst->optarg);
        type = DF_class_name2type(gst->optarg);
        if (type < 0) {
            /* ERROR:103 */
            /* "%s" is not a known RPSL object type. */
            char *fmt = ca_get_qc_fmt_badobjtype;
            query_command->parse_messages =
                g_list_append(query_command->parse_messages,
                                  g_strdup_printf(fmt, gst->optarg));
            UT_free(fmt);
            badparerr++;
        } else {
            query_command->t = type;
        }
      break;

      case 'v':
        /* always get an argument here (according to the code) */
        dieif(gst->optarg == NULL);

	/* parse the specification */
        g_strdown(gst->optarg);
        type = DF_class_name2type(gst->optarg);
        if (type < 0) {
            /* ERROR:103 */
            /* "%s" is not a known RPSL object type. */
            char *fmt = ca_get_qc_fmt_badobjtype;
            query_command->parse_messages =
                g_list_append(query_command->parse_messages,
                                  g_strdup_printf(fmt, gst->optarg));
            UT_free(fmt);
            badparerr++;
        } else {
            query_command->v = type;
        }
      break;

      case 'x':
        query_command->c_irt_search = FALSE;
        query_command->l = 0;
        query_command->m = 0;
        query_command->x = 1;
        query_command->L = 0;
        query_command->M = 0;
        if (ip_flag_used) ip_flag_duplicated++;
        ip_flag_used = 'x';
      break;

      case 'F':
				query_command->fast=1;
				query_command->recursive=0; /* implies no recursion */
      break;

      case 'K':
        query_command->filtered=1;
				query_command->recursive=0; /* implies no recursion */
      break;

      case 'L':
        query_command->c_irt_search = FALSE;
        query_command->l = 0;
        query_command->m = 0;
        query_command->x = 0;
        query_command->L = 1;
        query_command->M = 0;
        if (ip_flag_used) ip_flag_duplicated++;
        ip_flag_used = 'L';
      break;

      case 'M':
        query_command->c_irt_search = FALSE;
        query_command->l = 0;
        query_command->m = 0;
        query_command->x = 0;
        query_command->L = 0;
        query_command->M = 1;
        if (ip_flag_used) ip_flag_duplicated++;
        ip_flag_used = 'M';
      break;

      case 'R':
        query_command->R=1;
      break;

      case 'T':
        /* always get an argument here (according to the code) */
        dieif(gst->optarg == NULL);

	/* parse the specification */
        g_strdown(gst->optarg);
        types = ut_g_strsplit_v1(gst->optarg, ",", -1);
        for (i=0; types[i] != NULL; i++) {
            type = DF_class_name2type(types[i]);
            if (type < 0) {
                /* ERROR:103 */
                /* "%s" is not a known RPSL object type. */
                char *fmt = ca_get_qc_fmt_badobjtype;
                query_command->parse_messages =
                    g_list_append(query_command->parse_messages,
                                  g_strdup_printf(fmt, types[i]));
                UT_free(fmt);
                badparerr++;
            } else {
                MA_set(&(query_command->object_type_bitmap), (unsigned)type, 1);
            }
        }
        g_strfreev(types);
      break;

      case 'V':
        version_info = g_strsplit(gst->optarg, ",", 3);
        if (version_info[1] == NULL) {
            /* single parameter */
            UT_free(qe->version);
            qe->version = UT_strdup(version_info[0]);
        } else if (version_info[2] == NULL) {
            /* two parameters */

            /* XXX we don't accept IPv6 addresses to appear in -V flag */
            /*if it contains a ':', then it might be an IPv6 address*/
            if (index(version_info[1], ':') != NULL ){
                synerrflg++;
            } else if (IP_addr_e2b(&(qe->pIP), version_info[1]) != IP_OK) {
                synerrflg++;
            } else {
                num_client_ip++;
            }
            
            UT_free(qe->version);
            qe->version = UT_strdup(version_info[0]);
        } else {
            /* more than two parameters */
            synerrflg++;
        }
        g_strfreev(version_info);
        num_flags--;  /* don't count -V flags in our total */
      break;

      /* any other flag, including '?' and ':' errors */
      default:
        synerrflg++;
    }
  }

    /* update sources list if specified */
    if (sources_specified) {
        GList *ptr, *next_ptr;

        /* free space from previous version of the list */
	g_list_free(qe->sources_list); 
	qe->sources_list = NULL;

        /* add each new source */
        ptr = sources_specified;
        while (ptr != NULL) {
            next_ptr = g_list_next(ptr);
            /* only add this if it does not appear later in the list;
               this removes duplicate source specifications */
            if (g_list_find(next_ptr, ptr->data) == NULL) {
                qe->sources_list = g_list_append(qe->sources_list, ptr->data);
            }
            ptr = next_ptr;
        }
        g_list_free(sources_specified);
    }

    /* check for duplicate IP flags */
    if (ip_flag_duplicated) { 
        /* WARNING:901 */
        char *fmt = ca_get_qc_fmt_dupipflag;
        char ip_flag_str[2];
        ip_flag_str[0] = ip_flag_used; ip_flag_str[1] = '\0';
        query_command->parse_messages = 
            g_list_append(query_command->parse_messages, 
                          g_strdup_printf(fmt, ip_flag_str));
        UT_free(fmt);
    }

    /* check for duplicate proxy IP settings */
    if (num_client_ip > 1) {
        /* ERROR:205 */
        query_command->parse_messages = 
            g_list_append(query_command->parse_messages, 
                          ca_get_qc_dupproxyipflag);
        badparerr++;
    }

    /* check for flags that should appear alone */
    if (num_flags != 1) {
        if (query_command->q != -1) {
            synerrflg++;
        }
        if (query_command->t != -1) {
            synerrflg++;
        }
        if (query_command->v != -1) {
            synerrflg++;
        }
    }

		/* check uncompatible flags */
		if ((query_command->b == 1) && (query_command->recursive == 0)) {
			/* -b -r is error, we need person/role/organisation objects */
      /* WARNING:903 */
        char *fmt = ca_get_qc_fmt_uncompflag;
        query_command->parse_messages = 
            g_list_append(query_command->parse_messages, 
                          g_strdup_printf(fmt,"-b","-r"));

				UT_free(fmt);
				badparerr++;
		}

		if ((query_command->b == 1) && (query_command->G_group_search == 0)) {
			/* -G -b is error, we need grouping */
      /* WARNING:903 */
        char *fmt = ca_get_qc_fmt_uncompflag;
        query_command->parse_messages = 
            g_list_append(query_command->parse_messages, 
                          g_strdup_printf(fmt,"-b","-G"));
				UT_free(fmt);
				badparerr++;
		}

    /* copy the key */
    query_command->keys = g_strjoinv(" ", opt_argv + gst->optind);

    /* check for using keys on queries that should not have them */
    if (query_command->keys[0] != '\0') {
        if (query_command->q != -1) {
            synerrflg++;
        }
        if (query_command->t != -1) {
            synerrflg++;
        }
        if (query_command->v != -1) {
            synerrflg++;
        }
    }

    /* if no error, process the key, otherwise don't bother */
    if (!synerrflg && !badparerr) { 
        /* convert the key to uppercase. */
        g_strup(query_command->keys);
    
        /* make the keytypes_bitmap. */
        query_command->keytypes_bitmap = WK_new(query_command->keys);

        /* fix the object type bitmap - turn "all zeros" into "all ones" */
        if (MA_bitcount(query_command->object_type_bitmap) == 0 ) {
            query_command->object_type_bitmap = 
                 MA_not(query_command->object_type_bitmap);
        }

        /* XXX: missing checks for "-i" and "-T" versus key types */

        is_ip_key = MA_isset(query_command->keytypes_bitmap, WK_IPADDRESS) ||
	            MA_isset(query_command->keytypes_bitmap, WK_IPRANGE) ||
	            MA_isset(query_command->keytypes_bitmap, WK_IPPREFIX) ||
	            MA_isset(query_command->keytypes_bitmap, WK_IP6PREFIX);

        /* check for use of IP flags on non-IP lookups */
        if ((ip_flag_used || query_command->d) && !is_ip_key) {
            /* WARNING:902, meaningless IP flag */
            query_command->parse_messages = 
                g_list_append(query_command->parse_messages, 
                              ca_get_qc_uselessipflag);
        }

        /* check for "fixed" lookups */
        fixed_lookup = 0;
        if (MA_isset(query_command->keytypes_bitmap, WK_IPADDRESS)) {
            ip_addr_t addr;
            if (NOERR(IP_addr_a2b(&addr, query_command->keys))) {
                IP_addr_b2a(&addr, lookup, sizeof(lookup));
                if (strcmp(query_command->keys, lookup) != 0) {
                    fixed_lookup = 1;
                }
            }
        } else if (MA_isset(query_command->keytypes_bitmap, WK_IPRANGE)) {
            ip_range_t rang;
            if (NOERR(IP_rang_a2b(&rang, lookup))) {
                IP_rang_b2a(&rang, lookup, sizeof(lookup));
                if (strcmp(query_command->keys, lookup) != 0) {
                    fixed_lookup = 1;
                }
            }
        } else if (MA_isset(query_command->keytypes_bitmap, WK_IPPREFIX) ||
                   MA_isset(query_command->keytypes_bitmap, WK_IP6PREFIX)) 
        {
            ip_prefix_t pref;
            int ip_pref_a2b_retval= IP_pref_a2b(&pref, query_command->keys);
            /* IP_INVBIT error seems to do no harm, but good */
            if (NOERR(ip_pref_a2b_retval) || (ip_pref_a2b_retval==IP_INVBIT)) {
                IP_pref_b2a(&pref, lookup, sizeof(lookup));
                if (strcasecmp(query_command->keys, lookup) != 0) {
                    fixed_lookup = 1;
                }
            }
        }

        if (fixed_lookup) {
            /* WARNING:905 */
            char *fmt = ca_get_qc_fmt_fixedlookup;
            query_command->parse_messages = 
                g_list_append(query_command->parse_messages, 
                            g_strdup_printf(fmt, query_command->keys, lookup));
            /* here we also need to set query_command->keys to 'lookup' 
               variable, otherwise, in some cases the object might not
               get returned from radix tree. EG 2003-08-07 */
            UT_free(query_command->keys);
            query_command->keys = g_strdup(lookup);

            UT_free(fmt);
        }

   
        /* -d handling: if the keytype is IPv4/v6 address/prefix/range, then
           exclude the domains unless -d is set 
           XXX this must be kept in sync with new types */
        /* XXX: do we want this?  what's the point?!?!? - shane */
        if (is_ip_key && !query_command->d) {
            MA_set(&(query_command->object_type_bitmap), C_DN , 0);
        }

        if (query_command->R && 
            !MA_isset(query_command->keytypes_bitmap, WK_DOMAIN))
        {
            /* WARNING:904, meaningless no-referral flag */
            query_command->parse_messages = 
                g_list_append(query_command->parse_messages, 
                              ca_get_qc_uselessnorefflag);
        }

        /* tracing */
        log_command(query_str, query_command);

        /* "keep connection" processing:
           when opening connection, -k may be alone or with a query
           later -k must appear alone (or there must be an empty line,
           or an error) for the connection to close.
        */
        if (minusk)  {
            if (qe->k == 0) { /* opening */ 
	        qe->k = 1;
            } else { /* closing, if no key; otherwise keep open */
	        if (query_command->keys[0] == '\0') {
	            qe->k = 0;
	        }
            }
        }
    
    } /* if no error */

  /* we don't need this anymore */
  UT_free(gst);
  g_strfreev(opt_argv);

  if(synerrflg > 0) { /* severe syntax error. Usage must be printed */
    return QC_SYNERR;
  }
  else if(badparerr > 0) { /* the requester has a clue. No Usage info */
    return QC_PARERR;
  }
  else {
    return 0;
  }
} /* QC_fill() */

/* QC_environ_new() */
/*++++++++++++++++++++++++++++++++++++++
  Create a new query environment.

  More:
  +html+ <PRE>
  Authors:
        ottrey
  +html+ </PRE><DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL></DL>

  ++++++++++++++++++++++++++++++++++++++*/
Query_environ *QC_environ_new(char *ip, int sock) {
  Query_environ *qe;

  qe = (Query_environ *)UT_calloc(1, sizeof(Query_environ));
  qe->condat.ip = ip;
  qe->condat.sock = sock;

  /* The source is initialized to include only the deflook sources */
  {
    int i;
    ca_dbSource_t *hdl;
    
    for (i=0; (hdl = ca_get_SourceHandleByPosition(i)) != NULL; i++) {
      char *amrmrulez = ca_get_srcdeflook(hdl);
      if( strcmp(amrmrulez, "y")==0 ) {
	qe->sources_list = g_list_append(qe->sources_list, (void *)hdl);
      }
      UT_free(amrmrulez);
    }
  }
  
  return qe;

} /* QC_environ_new() */




/* QC_create() */
/*++++++++++++++++++++++++++++++++++++++
  try to parse the query and fill in the QC struct, setting 
  qc->query_type accordingly.
  
  Query_command *QC_create    returns allocated structure
  
  char *input                 user query
  
  Query_environ *qe           query environment structure
  
  Author: 
    marek.
  
  ++++++++++++++++++++++++++++++++++++++*/

Query_command *QC_create(const char *input, Query_environ *qe)
{
  Query_command *qc;
  const char *s;
  int qt;

  qc = (Query_command *)UT_calloc(1, sizeof(Query_command));
  QC_init_struct(qc);

  /* search the string for illegal characters */
  for (s=input; *s != '\0'; s++) {
      if (!strchr(ALLOWED_QUERY_CHARS, *s)) {
          qc->query_type = QC_PARERR;
          qc->parse_messages = g_list_append(qc->parse_messages, 
                                             ca_get_qc_badinput);
          return qc;
      }
  }
  
  if (strlen(input) == 0) {
    /* An empty query (Ie return) was sent */
    qc->query_type = QC_EMPTY;
  } 
  else {        /* else <==> input_length > 0 ) */
    /* parse query */
    qt = QC_fill(input, qc, qe);

    if( qt == QC_SYNERR || qt == QC_PARERR ) {
      qc->query_type = qt;
    }
    else {
      /* Update the query environment */
      /* qe = QC_environ_update(qc, qe); */

      /* Only do a query if there are keys. */
      if (qc->keys == NULL || strlen(qc->keys) == 0 ) {
	if (( qc->q != -1 || qc->t != -1 || qc->v != -1 )) {
	  qc->query_type = QC_TEMPLATE;
	} else {
	  qc->query_type = QC_NOKEY;
	}
      } else {
	if ( strcmp(qc->keys, "HELP") == 0 ) {
	  qc->query_type = QC_HELP;
	}
	/* So, a real query */
	else if( qc->filtered ) {
	  qc->query_type = QC_FILTERED;
	}
	else {
	  qc->query_type = QC_REAL;
	}
      }
    }
  }

  return qc;
}


/*++++++++++++++++++++++++++++++++++++++

  Get the name of the given query type code.

  char *QC_get_qrytype   returns a pointer to an element of array of static strings

  qc_qtype_t qrytype     query type code

  ++++++++++++++++++++++++++++++++++++++*/

char *QC_get_qrytype(qc_qtype_t qrytype) {
  dieif(qrytype >= QC_TYPE_MAX);

  return qrytype_str[qrytype];
}

void
QC_init (LG_context_t *ctx)
{
  qc_context = ctx;
}
