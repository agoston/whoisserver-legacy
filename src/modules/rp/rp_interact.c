#include <ctype.h>
#include "rip.h"

/****************************
 * The intention here is to provide a means of externally
 * accessing the radix tree of the server.
 *
 * Later on this can be expanded into a variety of uses
 *
 ****************************/


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

void RP_interact(int socket) {
  sk_conn_st condat;
  char input[MAX_INPUT_SIZE + 1];
  char *pos, *search_str, *source_str, *attr_str, *key_str;
  rx_srch_mt search_mode;
  rp_regid_t regid;
  rp_attr_t attrid;
  int search_depth;
  GList *answers;

  /* get the connection data */
  SK_cd_make(&condat, socket, (unsigned) ca_get_keepopen);

  /* grab the input string - leave if no input*/
  if (SK_cd_gets(&condat, input, MAX_INPUT_SIZE) <= 0)
    goto cleanup;

  /* break the input into it's seperate parts */
  pos = input;				// start at the begining
  while (isspace(*pos)) pos++;		// skip any starting whitespace
  if (!*pos) goto invalid_input;	// check string terminator
  search_str = pos;			// the search mode starts here
  while (!isspace(*pos)) pos++;		// cover all non-whitespace
  if (!*pos) goto invalid_input;	// check string terminator
  *pos = 0;				// mark the end of the search mode string
  pos++;				// and move on
  while (isspace(*pos)) pos++;		// skip extra whitespace
  if (!*pos) goto invalid_input;	// check string terminator
  source_str = pos;			// the source string starts here
  while (!isspace(*pos)) pos++;		// cover all non-whitespace
  if (!*pos) goto invalid_input;	// check string terminator
  *pos = 0;				// mark the end of the source string
  pos++;				// and move on
  while (isspace(*pos)) pos++;		// skip extra whitespace
  if (!*pos) goto invalid_input;	// check string terminator
  attr_str = pos;			// the attribute string starts here
  while (!isspace(*pos)) pos++;		// cover all non-whitespace
  if (!*pos) goto invalid_input;	// check string terminator
  *pos = 0;				// mark the end of the attribute string
  pos++;				// and move on
  while (isspace(*pos)) pos++;		// skip extra whitespace
  if (!*pos) goto invalid_input;	// check string terminator
  key_str = pos;			// the key string starts here
  pos = key_str + strlen(key_str) - 1;	// jump to the end of the input string
  while (pos>key_str			// while we haven't gone back too far
    && (isspace(*pos)			// skip spaces
      || *pos == '\n' || *pos == '\r'))	// and new lines
        pos--;				// go back, skipping the whitespace
  pos++;				// went one too far
  *pos = 0;				// mark the end of the key string

  /* check/convert the search mode string */
  // i use the more code-complex switch statements
  // to save time doing 6 string compares
  // todo - break into another func for easy removal/fixup
  search_depth = 0;
  pos = search_str + 1;
  switch(*search_str) {
    case '.':
      search_mode = RX_SRCH_EXLESS;
      break;
    case '=':
      search_mode = RX_SRCH_EXACT;
      break;
    case '<':
      search_mode = RX_SRCH_LESS;
      if (*pos == '<') {
	search_depth = RX_ALL_DEPTHS;
	pos ++;
      } else {
	search_depth = 1;
      }
      break;
    case '>':
      search_mode = RX_SRCH_MORE;
      if (*pos == '>') {
	search_depth = RX_ALL_DEPTHS;
	pos ++;
      } else {
	search_depth = 1;
      }
      break;
    default:
      goto invalid_input; // mainly to trap null strings
  }
  if (*pos) goto invalid_input;

  /* check/convert the source string */
  if ((regid=ca_get_SourceHandleByName(source_str)) == NULL) {
    //SK_cd_printf(&condat, ca_get_qc_fmt_badsource );
    SK_cd_printf(&condat, "%%ERROR:102: unknown source\n" );
    goto cleanup;
  }

  /* check/convert the attribute string */
  if ((attrid=DF_attribute_name2type(attr_str)) == -1) {
    SK_cd_printf(&condat, "%%ERROR:104: unknown attribute\n");
    goto cleanup;
  }

  /* check the key string */
  if (key_str == NULL || strlen(key_str) == 0) {
    //SK_cd_printf(&condat, ca_get_pw_err_nokey );
    SK_cd_printf(&condat, "%%ERROR:106: no search key specified\n" );
    goto cleanup;
  }

  /* make sure the attribute has a lookup tree */
  /* no tree = null response */
  if (!DF_attrcode_has_radix_lookup(attrid)) {
    // for now we send an error - maybe use invalid input error
    SK_cd_printf(&condat, "%%ERROR:105: attribute is not searchable\n");
    goto cleanup;
  }

  /* perform the radix tree lookup */
  if (!NOERR(RP_asc_search(search_mode, search_depth, 0, key_str, regid, attrid, &answers, RX_ANS_ALL))) {
    SK_cd_printf(&condat, "%%ERROR:509: error during search\n");
    goto cleanup;
  }
  
  /* double check we actually have answers */
  if (answers == NULL) {
    //SK_cd_printf(&condat, ca_get_pw_notfound );
    SK_cd_printf(&condat, "%%ERROR:101: no entries found\n" );
    goto cleanup;
  }

  /* print the output list to the user         */
  g_list_foreach(answers, print_answers, &condat);  

  /* free the list */
  wr_clear_list(&answers);

  /* and we're done */
  SK_cd_printf(&condat, "\n\n");

  /* cleanup the connection structure and leave */
cleanup:
  SK_cd_free(&condat);
  return;

  /* oops - the input has bad syntax */
invalid_input:
  SK_cd_printf(&condat, "%%ERROR:405: syntax error\n");
  goto cleanup;
}
