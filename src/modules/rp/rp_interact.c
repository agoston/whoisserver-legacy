#include <ctype.h>
#include "rip.h"

void destroy_list_entry(rx_datcpy_t *data, void* nullptr) {
  if (data != NULL) {
    if (data->leafcpy.data_ptr != NULL) {
      UT_free(data->leafcpy.data_ptr);
    }
    UT_free(data);
  }
}

void print_answers(rx_datcpy_t *data, sk_conn_st *condat) {
  if (condat == NULL) return;

  if (data != NULL) {
    SK_cd_printf(condat, "%ld\n", (long)data->leafcpy.data_key);
  } else {
    SK_cd_printf(condat, "# bad entry\n");
  }
}

/************
 * Allows incoming queries for the internal radix tree structures
 ***********/

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
  if (SK_cd_gets(&condat, input, MAX_INPUT_SIZE) <= 0) return;

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
    SK_cd_printf(&condat, ca_get_qc_fmt_badsource());
    return;
  }

  /* check/convert the attribute string */
  if ((attrid=DF_attribute_name2type(attr_str)) == -1)
    goto invalid_input;

  /* check the key string */
  if (key_str == NULL || strlen(key_str) == 0) {
    SK_cd_printf(&condat, ca_get_pw_err_nokey());
    return;
  }

  /* make sure the attribute has a lookup tree */
  /* no tree = null response */
  if (!DF_attrcode_has_radix_lookup(attrid)) {
    // for now we send an error - maybe use invalid input error
    SK_cd_printf(&condat, "%%ERROR:xxx: invalid attribute\n\n");
    return;
  }

  if (!NOERR(RP_asc_search(search_mode, search_depth, 0, key_str, regid, attrid, &answers, RX_ANS_ALL))) {
    SK_cd_printf(&condat, "%%ERROR:xxx: bad search\n\n");
    return;
  }
  
  if (answers == NULL) {
    SK_cd_printf(&condat, ca_get_pw_notfound());
    return;
  }

  g_list_foreach(answers, print_answers, &condat);

  /* free the list */
  g_list_foreach(answers, destroy_list_entry, NULL);
  g_list_free(answers);

  SK_cd_printf(&condat, "\n\n# Done!\n");

#if 0
  /***** testing/debug stuff ******/
  SK_cd_printf(&condat, "Input parsed ok\n");
  SK_cd_printf(&condat, "Search Mode: %s\n", RX_text_srch_mode(search_mode));
  SK_cd_printf(&condat, "Search Depth: %d\n", search_depth);
  SK_cd_printf(&condat, "Source String: %s\n", regid->name);
  SK_cd_printf(&condat, "Attribute String: %s\n", DF_get_attribute_name(attrid));
  SK_cd_printf(&condat, "Key string: [%s]\n", key_str);
  SK_cd_printf(&condat, "Other details:\n");
  SK_cd_printf(&condat, "  Has radix lookup: %d\n", DF_attrcode_has_radix_lookup(attrid));
#endif

  return;

invalid_input:
  SK_cd_printf(&condat, "%%ERROR:405: syntax error\n");
  return;
}

