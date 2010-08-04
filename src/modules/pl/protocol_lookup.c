#include <ctype.h>
#include "rip.h"

/****************************
 * The intention here is to provide a means of externally
 * accessing the radix tree of the server.
 *
 * Later on this can be expanded into a variety of uses
 *
 ****************************/

typedef struct {
  sk_conn_st  condat;
  char       *attrib;
} pl_answer_t;

/**********************
 * Gather the sources set as default lookups into one list
 *********************/
GList *pl_default_sources(void) {
  GList *sources = NULL;
  ca_dbSource_t *handle;
  char *deflook;
  int	i;

  for (i=0; (handle=ca_get_SourceHandleByPosition(i)) != NULL; i++) {
    deflook = ca_get_srcdeflook(handle);
    if (strcmp(deflook, "y") == 0) {
      sources = g_list_append(sources, handle);
    }
    UT_free(deflook);
  }

  return sources;
}

/****************************
 * Gather the attributes with radix lookups into one list
 ***************************/
GList *pl_default_attributes(void) {
  GList *attrs = NULL;
  int i;
  
  for (i=0; i < A_END; i++) {
    if (DF_attrcode_has_radix_lookup(i)) {
      attrs = g_list_append(attrs, GINT_TO_POINTER(i));
    }
  }

  return attrs;
}

/****************************
 * pl_print_answers is called for each item of an answer list
 * returned by the radix search function (RP_asc_search).
 *
 * It's task is to output the object_id from the radix node
 * to the user's connection.
 *
 ****************************/
void pl_print_answers(rx_datcpy_t *data, pl_answer_t *info) {
  char buf[256];
  
  if (data == NULL || info == NULL) return;

  // stringify the range
  //buf[0] = '\0';
  if (IP_rang_b2a(&data->leafcpy.iprange, buf, 256) != IP_OK) {
    strcpy(buf, "<invalid range>");
  }

  // output the answer
  SK_cd_printf(&info->condat, "%ld, %s, %s\n", (long)data->leafcpy.data_key, info->attrib, buf);

  // now we're finished with the node copy
  // destroy any extra node pointers
  if (data->leafcpy.data_ptr != NULL) {
    UT_free(data->leafcpy.data_ptr);
    data->leafcpy.data_ptr = NULL;
  }
}

/****************************
 * RP_interact handles a single server connection by taking/parsing
 * input, performing a radix tree search using the provided information,
 * and returning a list of object_id's as a result.
 ****************************/

/* The maximum input buffer size */
#define MAX_INPUT_SIZE	1024

void PL_interact(int socket) {
  pl_answer_t      answer_info;
  char             input[MAX_INPUT_SIZE + 1];
  char            *str, *search_key;
  int              rc, err, argcount, entries_found;
  getopt_state_t  *opt_state = NULL;
  char           **arglist;
  int              persistent_connection = 0;
  int              k_flag, extra_flag, search_flag_used, dup_search_flag, search_depth;
  GList           *source_list = NULL, *cur_source;
  GList           *attr_list = NULL, *cur_attr;
  GList           *answers = NULL;
  rx_srch_mt       search_mode;

  //g_mem_profile();

  // start by getting the connection information
  SK_cd_make(&answer_info.condat, socket, (unsigned) ca_get_keepopen);

  // allow multiple queries - similar to the PW_interact function
  do {
    // produce the banner ??
    //str = ca_get_pw_banner;
    //rc = SK_cd_printf(&condat, "%s\n", str);
    //UT_free(str);

    // grab the input string
    rc = SK_cd_gets(&answer_info.condat, input, MAX_INPUT_SIZE);
    dieif( rc > MAX_INPUT_SIZE );

    // if the string is too long let them know
    if (rc > 0 && input[rc-1] != '\n') {
      str = ca_get_pw_err_linetoolong;
      SK_cd_printf(&answer_info.condat, "%s\n\n", str);
      UT_free(str);
      // we should exit out of any persistent connection
      // otherwise we will have to slurp up the rest of
      // the input line to prevent the rest of it being
      // processed in the next loop iteration
      // (whois query loop does the same, but also bans the connection)
      err = 1;
    }
    else // it's valid input
    {
      // chomp and count
      g_strchomp(input);
      arglist = g_strsplit_set(input, " \t", -1);
      for (argcount=0; arglist[argcount]; argcount++) ;

      // DEBUG
//      rc = 0;
//      while (arglist[rc] != NULL) {
//	SK_cd_printf(&answer_info.condat, "Arg %d: [%s]\n", rc+1, arglist[rc]);
//	rc++;
//      }
      // END-DEBUG
      
      // setup and double-check option state
      opt_state = mg_new(0);
      dieif( opt_state == NULL );
      
      // clear the loop state
      err = 0;
      k_flag = 0;
      extra_flag = 0;
      search_flag_used = 0;
      dup_search_flag = 0;
      search_mode = RX_SRCH_EXLESS;
      search_depth = 0;
      
      // process the options
      while (!err && ((rc=mg_getopt(argcount, arglist, "ks:T:xlLmM", opt_state)) != EOF)) {
	switch (rc) {
	  case 'k':
	    //SK_cd_printf(&answer_info.condat, "k flag : [%s]\n", opt_state->optarg);
	    k_flag = 1;
	    break;
	  case 's':
	    //SK_cd_printf(&answer_info.condat, "s flag : [%s]\n", opt_state->optarg);

	    extra_flag = 1;	    
	    // should always get an argument
	    if ( opt_state->optarg == NULL ) {
	      str = ca_get_pl_err_nosource;
	      SK_cd_printf(&answer_info.condat, "%s\n", str);
	      UT_free(str);
	      err = 1;
	    } else {
	      gchar **source_strings;
	      ca_SrcHdl_t *source_handle;
	      
	      g_strdown( opt_state->optarg );
	      source_strings = g_strsplit(opt_state->optarg, ",", -1);

	      for (rc = 0; source_strings[rc] != NULL; rc++) {
		source_handle = ca_get_SourceHandleByName(source_strings[rc]);
		if (source_handle == NULL) {
		  // Ooops - unknown source handle
		  // Hardcoded error for now - note that using
		  // the whois query message for error 102
		  // will display incorrect lookup query protocol
		  // (ie, -q sources)
		  str = ca_get_pl_err_badsource;
		  SK_cd_printf(&answer_info.condat, str, source_strings[rc]);
		  UT_free(str);
		  err = 1;
		} else {
		  source_list = g_list_append(source_list, source_handle);
		}
	      }
	      g_strfreev(source_strings);
	    }
	    break;
	  case 'T':
	    //SK_cd_printf(&answer_info.condat, "T flag : [%s]\n", opt_state->optarg);
	    
	    extra_flag = 1;
	    // should always get an argument
	    if ( opt_state->optarg == NULL ) {
	      str = ca_get_pl_err_noclass;
	      SK_cd_printf(&answer_info.condat, "%s\n", str);
	      UT_free(str);
	      err = 1;
	    } else {
	      gchar     **class_strings;
	      rp_attr_t   attr_type;

	      g_strdown( opt_state->optarg );
	      class_strings = g_strsplit(opt_state->optarg, ",", -1);

	      for (rc=0; class_strings[rc] != NULL; rc++) {
		// sorting out the object class is not fun
		// we first make sure we have been given a class type
		if (DF_class_name2type(class_strings[rc]) == -1) {
		  // It's not a class type
		  str = ca_get_pl_err_badclass;
		  SK_cd_printf(&answer_info.condat, str, class_strings[rc]);
		  UT_free(str);
		  err = 1;
		} else {
		  // So it's a class type, but is it an attribute
		  // (should always be true - so borrow bad class error)
		  if ((attr_type=DF_attribute_name2type(class_strings[rc])) == -1) {
		    // It's not an attribute type
		    str = ca_get_pl_err_badclass;
		    SK_cd_printf(&answer_info.condat, str, class_strings[rc]);
		    UT_free(str);
		    err = 1;
		  } else {
		    // So it's an attribute, but does it have a radix tree
		    if (!DF_attrcode_has_radix_lookup(attr_type)) {
		      // Oops, it's not a searchable class type
		      str = ca_get_pl_err_nosearchclass;
		      SK_cd_printf(&answer_info.condat, str, class_strings[rc]);
		      UT_free(str);
		      err = 1;
		    } else {
		      // it's a searchable attribute/class name !!
		      attr_list = g_list_append(attr_list, GINT_TO_POINTER(attr_type));
		    }
		  }
		}
	      }
	      g_strfreev(class_strings);
	    }
	    break;
	  case 'x':
	    //SK_cd_printf(&answer_info.condat, "x flag : [%s]\n", opt_state->optarg);
	    if (search_flag_used == 0) {
	      search_flag_used = 'x';
	      search_mode = RX_SRCH_EXACT;
	    } else {
	      dup_search_flag = 1;
	    }
	    break;
	  case 'l':
	    //SK_cd_printf(&answer_info.condat, "l flag : [%s]\n", opt_state->optarg);
	    if (search_flag_used == 0) {
	      search_flag_used = 'l';
	      search_mode = RX_SRCH_LESS;
	      search_depth = 1;
	    } else {
	      dup_search_flag = 1;
	    }
	    break;
	  case 'L':
	    //SK_cd_printf(&answer_info.condat, "L flag : [%s]\n", opt_state->optarg);
	    if (search_flag_used == 0) {
	      search_flag_used = 'L';
	      search_mode = RX_SRCH_LESS;
	      search_depth = RX_ALL_DEPTHS;
	    } else {
	      dup_search_flag = 1;
	    }
	    break;
	  case 'm':
	    //SK_cd_printf(&answer_info.condat, "m flag : [%s]\n", opt_state->optarg);
	    if (search_flag_used == 0) {
	      search_flag_used = 'm';
	      search_mode = RX_SRCH_MORE;
	      search_depth = 1;
	    } else {
	      dup_search_flag = 1;
	    }
	    break;
	  case 'M':
	    //SK_cd_printf(&answer_info.condat, "M flag : [%s]\n", opt_state->optarg);
	    if (search_flag_used == 0) {
	      search_flag_used = 'M';
	      search_mode = RX_SRCH_MORE;
	      search_depth = RX_ALL_DEPTHS;
	    } else {
	      dup_search_flag = 1;
	    }
	    break;
	  default:
	    err = 1;
	    break;
	}
      }

      if (dup_search_flag) {
	// only one search flag is allowed
	str = ca_get_pl_err_dupsearchflag;
	SK_cd_printf(&answer_info.condat, "%s\n", str);
	UT_free(str);
	err = 1;
      }
      
      // we MUST have a source
      if (source_list == NULL) {
	// so we add the default sources
	source_list = pl_default_sources();
	if (source_list == NULL) {
	  str = ca_get_pl_err_nosource;
	  SK_cd_printf(&answer_info.condat, "%s\n", str);
	  UT_free(str);
	  err = 1;
	}
      }
      
      // and we MUST have an attribute/class type
      if (attr_list == NULL) {
	attr_list = pl_default_attributes();
	if (attr_list == NULL) {
	  str = ca_get_pl_err_noclass;
	  SK_cd_printf(&answer_info.condat, "%s\n", str);
	  UT_free(str);
	  err = 1;
	}
      }
      
      // The leftover arguments make the search key
      search_key = g_strjoinv(" ", &arglist[opt_state->optind]);
      //SK_cd_printf(&answer_info.condat, "Search: [%s]\n", search_key); // DEBUG

      // check our persistent connection request
      if (k_flag) {
	if (extra_flag || search_flag_used || (search_key != NULL && strlen(search_key) > 0)) {
	  // The -k flag was used with other search options
	  // So we enable persistent connection
	  persistent_connection = 1;
	} else {
	  // The -k flag was used by itself,
	  // close the persistent connection
	  persistent_connection = 0;
	  // note - it's not really an error
	  // but this prevents us going through the search/answer loop with no key
	  err = 1;
	}
      }

      if (!err) {
	// no errors - let's prep the search

	// perform search
	for (cur_source = g_list_first(source_list); cur_source != NULL; cur_source = g_list_next(cur_source)) {
	  rp_regid_t reg_id = (rp_regid_t)cur_source->data;
	  
	  SK_cd_printf(&answer_info.condat, "%%%%SOURCE: %s\n", reg_id->name);

	  entries_found = 0;
	  for (cur_attr = g_list_first(attr_list); cur_attr != NULL; cur_attr = g_list_next(cur_attr)) {
	    rp_attr_t attr_id = (rp_attr_t)GPOINTER_TO_INT(cur_attr->data);

	    /* DEBUG
	    SK_cd_printf(&answer_info.condat, "#Search mode: %s\n", RX_text_srch_mode(search_mode));
	    SK_cd_printf(&answer_info.condat, "#Search depth: %d\n", search_depth);
	    SK_cd_printf(&answer_info.condat, "#Search key: [%s]\n", search_key);
	    SK_cd_printf(&answer_info.condat, "#Source: [%s]\n", reg_id->name);
	    SK_cd_printf(&answer_info.condat, "#Attribute: [%s]\n", DF_get_attribute_name(attr_id));
	    */

	    rc = RP_asc_search(search_mode, search_depth, 0, search_key, reg_id, attr_id, &answers, RX_ANS_ALL);
	    
	    if (rc == IP_INVARG) {
	      // the search key cannot be converted to an ip range
	      // theoretically this trashes the entire query
	      str = ca_get_pl_err_badsearchkey;
	      SK_cd_printf(&answer_info.condat, "%s\n", str);
	      UT_free(str);
	      err = 1;
	      cur_attr = NULL;
	      cur_source = NULL;
	    } else if (rc == RP_NOTREE) {
	      // because there is no tree for this source/attribute combination
	      // (but there should be), there are no entries to be found
	    } else if (rc != 0) {
	      // this should never happen, but future error codes might appear
	      SK_cd_printf(&answer_info.condat, "%%ERROR:999: unknown error");
	      // unknown error stops everything (but I don't think we need to die)
	      err = 1;
	      cur_attr = NULL;
	      cur_source = NULL;
	    } else {
	      if (answers != NULL) {
		// woohoo - we found some answers
		entries_found = 1;
		// show the answers
		answer_info.attrib = DF_get_attribute_name(attr_id);
		g_list_foreach(answers, (GFunc)pl_print_answers, &answer_info);
		// and clear the list
		wr_clear_list(&answers);
	      }
	    }
	    
	  }
	  // all the attribute trees have been searched,
	  // did we find any entries for this source
	  if (!err && !entries_found) {
	    str = ca_get_pl_err_noentries;
	    SK_cd_printf(&answer_info.condat, "%s\n", str);
	    UT_free(str);
	  }
	}
	// all the sources are done so write a blank line for termination
	SK_cd_printf(&answer_info.condat, "\n");

	g_free(search_key);

      } /* no error after processing options */

      // cleanup the loop memory
      UT_free(opt_state);
      
      g_strfreev(arglist);

      if (source_list != NULL) {
	g_list_free(source_list);
	source_list = NULL;
      }
      if (attr_list != NULL) {
	g_list_free(attr_list);
	attr_list = NULL;
      }

    } /* process valid input */

  } while (persistent_connection && answer_info.condat.rtc == 0
    && CO_get_whois_suspended() == 0);

  SK_cd_free(&answer_info.condat);
  
  //g_mem_profile();
}
