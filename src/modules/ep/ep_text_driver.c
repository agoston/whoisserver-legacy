#include "rip.h"
#include "ep.h"

/*
  Please see ep_text_driver first
*/

/*
  Processes a piece of PGP signed text (with signature).

  split       - GString containing lines of input with PGP content.
  credentials - Current credentials.

  return - List of authenticated_data.

Recursively calls ep_split_text.
 */
GList *ep_grab_pgp(GString *split, GList *credentials) {

  GList *return_list = NULL;
  GList *creds = NULL;
//  GList *cred_list = NULL;
  KM_key_return_t *kr;
  CR_credential_t *cred;
  gchar **gsplit;
  const gchar *unescaped_content;
  gboolean sig_ok;
  ep_authenticated_data_t *a_data;

  LG_log(ep_ctx, LG_FUNC, ">ep_grab_pgp: entered");

  kr = KM_signature_verify(KM_PGP, split->str, NULL);

  /* should we dupplicate credentials? not in this case */
  sig_ok = KM_key_return_get_signature_ok(kr);
  creds = CR_credential_list_duplicate(credentials);

  if (KM_key_return_get_status(kr) == KM_OK) {
    /* PGP signature verified OK.
       Add the signature to the credentials list
       and handle the content as a text message
       (which may contain more PGP so it has to be split again). */
    LG_log(ep_ctx, LG_DEBUG, "ep_grab_pgp: PGP signature verified OK");
    cred  = CR_credential_new(CR_PGP, KM_key_return_get_key_id(kr), sig_ok);
    creds = g_list_append(creds, cred);
    unescaped_content = KM_key_return_get_signed_text(kr);
    gsplit = ut_g_strsplit_v1((char*)unescaped_content, "\n", -1);
    return_list = ep_split_text(gsplit, creds);
    g_strfreev(gsplit);
  }
  else {
    /* The signature verified as bad.
       Do not treat this as PGP
       and handle the whole piece as text. */
    LG_log(ep_ctx, LG_DEBUG, "ep_grab_pgp: PGP signature verified bad");
    gsplit = ut_g_strsplit_v1(split->str, "\n", -1);
    gsplit[0][0] = '*'; /* fool the parser - HACK ALERT */
    return_list = ep_split_text(gsplit, creds);
    a_data = (ep_authenticated_data_t*)return_list->data;
    a_data->exp_data->data_str->str[0] = '-'; /* restate the data */
    g_strfreev(gsplit);
  }

  KM_key_return_free(kr);
  /* I think these can be freed but won't risk it this time through */
/*
  cred_list = creds;
  while (cred_list)
  {
    CR_credential_free( (CR_credential_t*)(cred_list->data) );
    cred_list = g_list_next(cred_list);
  }
  g_list_free(creds);
  if (sig_ok) {
    CR_credential_free(cred);
  }
*/
  LG_log(ep_ctx, LG_FUNC, "<ep_grab_pgp: exiting");
  return return_list;
}

/*
  Removes passwords from a text piece of an authenticated_data
  and adds them to the credentials.

  a_data       - Authenticated data with text piece.
  crendentials - Current list of credentials (which can be changed).
 */
void ep_password_clean(ep_authenticated_data_t *a_data, GList **credentials) {
  GString *dirty_string;
  GString *clean_string;
  gchar **dirty;
  gchar **current_dirty;
  CR_type type;
  gchar *password_pos;

  LG_log(ep_ctx, LG_FUNC, ">ep_password_clean: entered");

  dirty_string = a_data->exp_data->data_str;
  clean_string = g_string_new("");

  dirty = ut_g_strsplit_v1(dirty_string->str, "\n", -1);
  current_dirty = dirty;
  while (*current_dirty) {
    type = -1;
    if (strncasecmp("password:", *current_dirty, strlen("password:")) == 0) {
      type = CR_PASSWORD;
      password_pos = *current_dirty + strlen("password:");
      LG_log(ep_ctx, LG_DEBUG, "ep_password_clean: password [%s]", password_pos );
    }
    if (strncasecmp("override:", *current_dirty, strlen("override:")) == 0) {
      type = CR_OVERRIDE;
      password_pos = *current_dirty + strlen("override:");
      LG_log(ep_ctx, LG_DEBUG, "ep_password_clean: override [%s]", password_pos );
    }
    if (type != -1) {
      *credentials = g_list_append(*credentials, CR_credential_new(type, g_strchug(password_pos), TRUE));
    }
    else {
      g_string_append(clean_string, *current_dirty);
      g_string_append(clean_string, "\n");
    }
    current_dirty++;
  }

  a_data->exp_data->data_str = clean_string;
  a_data->credentials = g_list_concat(a_data->credentials,CR_credential_list_duplicate(*credentials));
  /* a_data->exp_data->data_str now points to the new clean_string
     so we can free the old dirty_string it used to point to */
  g_string_free(dirty_string, TRUE);
  g_strfreev(dirty);
  LG_log(ep_ctx, LG_FUNC, "<ep_password_clean: exiting");
}

/*
  Takes a piece of text (in split), turns it into a list (with one element)
  of authenticated data to be added to the calling functions return_list
  of authenticated data.
  Passwords are removed from the text and added to the new credentials list.

  split           - piece of text
  credentials     - current credentials.
  new_credentials - new credentials for the top-level.

  return - list of authenticated_data.
 */
GList* ep_grab_text(GString *split, GList *credentials, GList **new_credentials) {

  ep_authenticated_data_t *new_data;
  GList *return_list = NULL;

  LG_log(ep_ctx, LG_FUNC, ">ep_grab_text: entered");

  new_data = ep_authenticated_data_new();

  //new_data->credentials = g_list_copy(*credentials);
  /* should we dupplicate credentials? Yes, most probably */
  new_data->exp_data->type = EP_TEXT;
  new_data->exp_data->data_str = g_string_new(split->str);
  new_data->credentials = CR_credential_list_duplicate(credentials);
  ep_password_clean(new_data, new_credentials);

  return_list = g_list_append(NULL, new_data);
  LG_log(ep_ctx, LG_FUNC, "<ep_grab_text: exiting");
  return return_list;
}

/*
  ep_split_pgp - Deals with a pgp message

  Finds boundaries, processes pgp and recursively calls the text
  processor.
*/
GList *ep_split_pgp(gchar ***split, GList *credentials) {
  GString *my_split;
  gchar *current_line; 
  GList *return_list = NULL;
  GList *new_credentials = NULL;
  GList *cred = NULL;

  LG_log(ep_ctx, LG_FUNC, ">ep_split_pgp: entered");

  my_split = g_string_new(**split);
  g_string_append(my_split, "\n");

  (*split)++;
  current_line = **split;
  while (current_line != NULL) {
    g_string_append(my_split, current_line);
    g_string_append(my_split, "\n");
    if (strncmp(current_line, "-----END PGP SIGNATURE-----", 27) == 0) {
      return_list = ep_grab_pgp(my_split, credentials);
      (*split)++;
      //CHECK free of my_split
      g_string_free(my_split, TRUE);
      LG_log(ep_ctx, LG_FUNC, "<ep_split_pgp: exiting");
      return return_list;
    }
    (*split)++;
    current_line = **split;
  }
  /* This is in case of badly formed message with no END PGP SIGNATURE.
     Do not process it as valid PGP but treat it as text.
     Ignore any passwords found in this badly formed section */
  LG_log(ep_ctx, LG_DEBUG, "ep_split_pgp: badly formed pgp message, treat it as text");
  new_credentials = NULL;
  return_list = ep_grab_text(my_split, credentials, &new_credentials);

  /* free any new_credentials (passwords) found */
  cred = new_credentials;
  while (cred)
  {
    CR_credential_free( (CR_credential_t*)(cred->data) );
    cred = g_list_next(cred);
  }
  g_list_free(new_credentials);

  /* free this 'text' as it has been added to the return_list now */
  g_string_free(my_split, TRUE);
  LG_log(ep_ctx, LG_FUNC, "<ep_split_pgp: exiting");
  return return_list;
}

/*
  Adds a list of credentials to all authenticated_data elements
  belonging to the list.

  pieces      - authenticated_data list.
  credentials - Credentials to add.
*/
void ep_patch_credentials(GList *pieces, GList *credentials) {

  CR_credential_t *credential;
  GList *my_credentials = NULL;
  ep_authenticated_data_t *a_data;

  LG_log(ep_ctx, LG_FUNC, ">ep_patch_credentials: entered");

  while (pieces) {
    my_credentials = credentials;
    a_data = (ep_authenticated_data_t*) pieces->data;

    while (my_credentials) {
      credential = my_credentials->data;
      if (!CR_credential_list_has_credential(a_data->credentials,
            CR_credential_get_type(credential),
            CR_credential_get_value(credential),
            CR_credential_get_validity(credential))) {
        a_data->credentials = g_list_append(a_data->credentials, CR_credential_duplicate(credential));
      }
      my_credentials = g_list_next(my_credentials);
    }
    pieces = g_list_next(pieces);
  }
  LG_log(ep_ctx, LG_FUNC, "<ep_patch_credentials: exiting");
}

/*
  Splits text in pieces and processes it.

  The pieces are: all PGP signed content + one piece representing the
  rest of the text, example:
  text1
  pgp1
  text2
  pgp2
  text3

  is separated in 3 pieces: pgp1, pgp2 and text1+text2+text3
  text is put in front of the list
  
  We will NOT process X.509 signatures in this type of imnput.
  Any X.509 signature in this input will simply be treated as text.
*/
GList *ep_split_text(gchar **split, GList *credentials) {
  GString *my_split;
  gchar **split_iter;
  gchar *current_line;
  GList *return_list = NULL;
  GList *new_credentials = NULL;
  GList *text_part = NULL;
  gboolean first_line = TRUE;

  LG_log(ep_ctx, LG_FUNC, ">ep_split_text: entered ");

  split_iter = split;
  current_line = *split_iter;
  my_split = g_string_new("");

  while (current_line != NULL) {
    /* RFC number justifying the boundaries */
    if (strncmp(current_line, "-----BEGIN PGP SIGNED MESSAGE-----", 34) == 0) {
      LG_log(ep_ctx, LG_DEBUG, "ep_split_text: PGP SIGNED MESSAGE ");
      if (!first_line) {
        /* There was some text before this PGP signed message.
           Add this text part to the return_list and free it 
           before processing the PGP signed message. */
        text_part = ep_grab_text(my_split, credentials, &new_credentials);
        return_list = g_list_concat(return_list, text_part);
        /* we are now at the beginning again */
        first_line = TRUE;
        g_string_free(my_split, TRUE);
        my_split = g_string_new("");
      }
      return_list = g_list_concat(return_list, ep_split_pgp(&split_iter, credentials));
    } else {
      g_string_append(my_split, current_line);
      g_string_append(my_split, "\n");
      first_line = FALSE;
      split_iter++;
    }
    current_line = *split_iter;
  }
  if (!first_line) {
    text_part = ep_grab_text(my_split, credentials, &new_credentials);
    return_list = g_list_concat(return_list, text_part);
  }
  /* adds new_credentials to return_list */
  ep_patch_credentials(return_list, new_credentials);

  /* maybe at this point new_credentials can be freed as well as contents.
     but won't risk it on this pass through. */

  g_string_free(my_split, TRUE);
  LG_log(ep_ctx, LG_FUNC, "<ep_split_text: exiting");
  return return_list;
}

/*
  ep_text_driver - This is the entry point for the text driver

  This code is recursive. There is a contract regarding the
  credential list: if a function wants to use it (attach it to another
  structure then it has to copy it completely - list plus credentials).

  In practice it simply calls ep_split_text.

  Please note that the recursion comes from pgp (not mime application/signed)
  content: text can have pgp, that has text, that can have pgp, ...

  Also note that we prefer to deal with ASCII armoring of PGP here.
  
  We will NOT process X.509 signatures in this type of imnput.
  Any X.509 signature in this input will simply be treated as text.

  See split_text for more info.
*/
ep_input_structure_t *ep_text_driver(ep_authenticated_data_t *a_data) {

  GList *return_list;
  ep_input_structure_t *input;
  GString *text;
  gchar **split;

  LG_log(ep_ctx, LG_FUNC, ">ep_text_driver: entered ");

  input = ep_input_structure_new();
  text = g_string_new(a_data->exp_data->data_str->str);

  split = ut_g_strsplit_v1(text->str, "\n", -1);
  return_list = ep_split_text(split, a_data->credentials);
  g_strfreev(split);
  input->is_mail = FALSE;
  input->flattened_authenticated_data = return_list;

  g_string_free(text, TRUE);
  LG_log(ep_ctx, LG_FUNC, "<ep_text_driver: exiting ");
  return input; 
}

