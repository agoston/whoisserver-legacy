#include <ctype.h>
#include "rip.h"

/****************************
 * The intention here is to provide a means of externally
 * accessing the radix tree of the server.
 *
 * Later on this can be expanded into a variety of uses
 *
 ****************************/

// process an input query string
// NOTE: will trash the input string
int rpi_parse_input(char *input, GList **source_list, GList **type_list, char **key) {
  char *p, *s;
  int f;
  rp_regid_t source;

  p = input;
  while (isspace(*p)) p++;
  if (!*p) {}// stop

  // start with source(s)
  f = 1;
  while (f) {
    s = p;
    while (!isspace(*p) && *p != ',') p++;
    if (*p != ',') f = 0;
    *p = '\0';

    //ca_get_SourceHandleByName
  }
  
  return -1;
}

/****************************
 * print_answers is called for each item of an answer list
 * returned by the radix search function (RP_asc_search).
 *
 * It's task is to output the object_id from the radix node
 * to the user's connection.
 *
 ****************************/
void print_answers(rx_datcpy_t *data, sk_conn_st *condat) {
  if (data == NULL || condat == NULL) return;

  // print the object_id
  SK_cd_printf(condat, "%ld\n", (long)data->leafcpy.data_key);
  
  // now we're finished with the node copy
  // destroy any node pointers
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
  sk_conn_st condat;
  char input[MAX_INPUT_SIZE + 1];
  char *str, *search_key;
  int rc, err, argcount;
  getopt_state_t *opt_state = NULL;
  char **arglist;
  int persistent_connection = 0;
  int k_flag, extra_flag, search_flag_used, dup_search_flag, search_depth;
  GList *source_list = NULL;
  rx_srch_mt search_mode = RX_SRCH_EXLESS;
  
  int x;
  
  SK_cd_make(&condat, socket, (unsigned) ca_get_keepopen);

  // allow multiple queries - similar to the PW_interact function
  do {
    // produce the banner ??
    //str = ca_get_pw_banner;
    //rc = SK_cd_printf(&condat, "%s\n", str);
    //UT_free(str);

    // grab the input string
    rc = SK_cd_gets(&condat, input, MAX_INPUT_SIZE);
    dieif( rc > MAX_INPUT_SIZE );

    // if the string is too long let them know
    if (rc > 0 && input[rc-1] != '\n') {
      str = ca_get_pw_err_linetoolong;
      SK_cd_printf(&condat, "%s\n\n", str);
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
      // chomp and parse
      g_strchomp(input);

      arglist = g_strsplit_set(input, " \t", -1);
      
      for (argcount=0; arglist[argcount]; argcount++) ;
      
      // DEBUG
      rc = 0;
      while (arglist[rc] != NULL) {
	SK_cd_printf(&condat, "Arg %d: [%s]\n", rc+1, arglist[rc]);
	rc++;
      }
      // END-DEBUG
      
      // setup and double-check option state
      opt_state = mg_new(0);
      dieif(opt_state == NULL );
      
      // clear the loop state
      err = 0;
      k_flag = 0;
      extra_flag = 0;
      search_flag_used = 0;
      dup_search_flag = 0;
      
      // process the options
      while (!err && ((rc=mg_getopt(argcount, arglist, ":ks:T:xlLmM", opt_state)) != EOF)) {
	switch (rc) {
	  case 'k':
	    SK_cd_printf(&condat, "k flag : [%s]\n", opt_state->optarg);
	    k_flag = 1;
	    break;
	  case 's':
	    SK_cd_printf(&condat, "s flag : [%s]\n", opt_state->optarg);

	    extra_flag = 1;	    
	    // should always get an argument
	    if ( opt_state->optarg == NULL ) {
	      str = ca_get_pl_err_nosource;
	      SK_cd_printf(&condat, "%s\n", str);
	      UT_free(str);
	      err = 1;
	    } else {
	      gchar **source_strings;
	      rp_regid_t source_handle;
	      
	      g_strdown( opt_state->optarg );
	      
	      source_strings = ut_g_strsplit_v1(opt_state->optarg, ",", -1);
	      for (rc = 0; source_strings[rc] != NULL; rc++) {
		source_handle = ca_get_SourceHandleByName(source_strings[rc]);
		if (source_handle == NULL) {
		  // Ooops - unknown source handle
		  // Hardcoded error for now - note that using
		  // the whois query message for error 102
		  // will display incorrect lookup query protocol
		  // (ie, -q sources)
		  str = ca_get_pl_err_badsource;
		  SK_cd_printf(&condat, str, source_strings[rc]);
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
	    SK_cd_printf(&condat, "T flag : [%s]\n", opt_state->optarg);
	    
	    extra_flag = 1;
	    // should always get an argument
	    if ( opt_state->optarg == NULL ) {
	      str = ca_get_pl_err_noclass;
	      SK_cd_printf(&condat, "%s\n", str);
	      UT_free(str);
	      err = 1;
	    } else {
	      gchar **class_strings;
	      rp_attr_t class_type;
	      
	      g_strdown( opt_state->optarg );
	      
	      class_strings = ut_g_strsplit_v1(opt_state->optarg, ",", -1);
	      for (rc=0; class_strings[rc] != NULL; rc++) {
		SK_cd_printf(&condat, "Object class: [%s]\n", class_strings[rc]);
		
		x = DF_class_name2type(class_strings[rc]);
		SK_cd_printf(&condat, "Class type: %d\n", x);
		
		x = DF_attribute_name2type(class_strings[rc]);
		SK_cd_printf(&condat, "Attribute type: %d\n", x);
		
		if (x != -1) {
		  x = DF_attrcode_has_radix_lookup(x);
		  SK_cd_printf(&condat, "Attribute has radix: %d\n", x);
		}
	      }
	    }
	    break;
	  case 'x':
	    SK_cd_printf(&condat, "x flag : [%s]\n", opt_state->optarg);
	    if (search_flag_used == 0) {
	      search_flag_used = 'x';
	      search_mode = RX_SRCH_EXACT;
	    } else {
	      dup_search_flag = 1;
	    }
	    break;
	  case 'l':
	    SK_cd_printf(&condat, "l flag : [%s]\n", opt_state->optarg);
	    if (search_flag_used == 0) {
	      search_flag_used = 'l';
	      search_mode = RX_SRCH_LESS;
	      search_depth = 1;
	    } else {
	      dup_search_flag = 1;
	    }
	    break;
	  case 'L':
	    SK_cd_printf(&condat, "L flag : [%s]\n", opt_state->optarg);
	    if (search_flag_used == 0) {
	      search_flag_used = 'L';
	      search_mode = RX_SRCH_LESS;
	      search_depth = RX_ALL_DEPTHS;
	    } else {
	      dup_search_flag = 1;
	    }
	    break;
	  case 'm':
	    SK_cd_printf(&condat, "m flag : [%s]\n", opt_state->optarg);
	    if (search_flag_used == 0) {
	      search_flag_used = 'm';
	      search_mode = RX_SRCH_MORE;
	      search_depth = 1;
	    } else {
	      dup_search_flag = 1;
	    }
	    break;
	  case 'M':
	    SK_cd_printf(&condat, "M flag : [%s]\n", opt_state->optarg);
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

      g_strfreev(arglist);

      search_key = g_strjoinv(" ", arglist + opt_state->optind);
      SK_cd_printf(&condat, "Search: [%s]\n", search_key); // DEBUG

      g_free(search_key);

      if (source_list != NULL) {
	g_list_free(source_list);
	source_list = NULL;
      }

    }

  } while (persistent_connection && condat.rtc == 0
    && CO_get_whois_suspended() == 0);

  SK_cd_printf(&condat, "Done!\n");
  SK_cd_free(&condat);

}
