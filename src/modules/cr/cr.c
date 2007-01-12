#include "cr.h"
#include <stdlib.h>
#include <string.h>

LG_context_t* cr_ctx;

void CR_init(LG_context_t* ctx ) {
  cr_ctx = ctx;
}

/*
  Creates a new credential.

  type  - The credential type (PGP, X509, password, override)
  value - the value

  return - a credential
 */
CR_credential_t* CR_credential_new(CR_type type, const char* value,
  gboolean valid) {
  cr_credential_t* credential;

  credential = g_malloc(sizeof(cr_credential_t));
  credential->type = type;
  credential->value = g_strchomp(strdup(value));
  credential->valid = valid;
  credential->deprecated = FALSE;

  return (CR_credential_t*) credential;
}

/*
  Frees a credential.
 */
void CR_credential_free(CR_credential_t* credential) {
  cr_credential_t* cred;

  cred = (cr_credential_t*)credential;
  g_free(cred->value);
  g_free(cred);
}
 
/*
  Returns the type of a credential.

  credential - the credential

  return - the type
*/
CR_type CR_credential_get_type(CR_credential_t* credential) {
  cr_credential_t* cred;

  cred = (cr_credential_t*)credential;
  return cred->type;
}

/*
  Returns the value of a credential.

  credential - the credential

  return - the type
*/
const char* CR_credential_get_value(CR_credential_t* credential) {
  cr_credential_t* cred;

  cred = (cr_credential_t*)credential;
  return cred->value;
}

/*
  Returns the validity of a credential.

  credential - the credential

  return - the type
*/
gboolean CR_credential_get_validity(CR_credential_t* credential) {

  cr_credential_t* cred;
  gboolean validity;

  LG_log(cr_ctx, LG_FUNC, ">CR_credential_get_validity entered");

  cred = (cr_credential_t*)credential;
  validity = cred->valid;
  
  LG_log(cr_ctx, LG_FUNC, "<CR_credential_get_validity exiting with %s",
                               validity ? "valid" : "invalid" );
  return validity;
}

#include "../au/crypt-md5.c" //An hack for now.
#include "../au/misc.c" //An hack for now.
extern char *crypt(const char *key, const char *salt);

gboolean cr_check_crypted(const gchar * plain, const gchar * crypted)
{
	gchar *my_crypted;

	LG_log(cr_ctx, LG_FUNC, ">cr_check_crypted entered plain [%s] crypted [%s]", plain, crypted);

	if (crypted[0] == '$') {
		my_crypted = crypt_md5(plain, crypted);
		if (strcmp(my_crypted, (char *)crypted) == 0) {
			LG_log(cr_ctx, LG_FUNC, "<cr_check_crypted exiting with TRUE");
			return TRUE;
		}
	} else {
		my_crypted = crypt(plain, crypted);
		if (strcmp(my_crypted, crypted) == 0) {
			LG_log(cr_ctx, LG_FUNC, "<cr_check_crypted exiting with TRUE");
			return TRUE;
		}
	}

	LG_log(cr_ctx, LG_FUNC, "<cr_check_crypted exiting with FALSE");
	return FALSE;
}

/*
  Checks if a credential list has a specific credential, the value is
  the crypted version of the credential.

  list            - credential list to be checked
  type            - credential type
  value           - credential value
  include_invalid - includes invalid credentials

  return - TRUE if the credential is contained AND valid, FALSE if not
 */
gboolean CR_credential_list_check(GList * list, CR_type type, const char *value, gboolean include_invalid)
{

	cr_credential_t *credential;
	gboolean retval;

	LG_log(cr_ctx, LG_FUNC, ">CR_credential_list_check entered with type [%s] value [%s]", CR_type2str(type), value);

	if (type != CR_PASSWORD && type != CR_OVERRIDE) {
		retval = CR_credential_list_has_credential(list, type, value, include_invalid);
		LG_log(cr_ctx, LG_FUNC, "<CR_credential_list_check exiting with %s", retval ? "TRUE" : "FALSE");
		return retval;
	}
	while (list) {
		credential = (cr_credential_t *) list->data;
		if (credential->type == type && cr_check_crypted(credential->value, value)) {
			if (CR_credential_get_validity(credential) || include_invalid) {
				/* for Crypt-PW deprecation */
				if (type != CR_OVERRIDE && value[0] != '$') {	/* if it's crypt */
					credential->deprecated = TRUE;
				} 
				
				LG_log(cr_ctx, LG_FUNC, "<CR_credential_list_check exiting with TRUE");
				return TRUE;
			}
		}
		list = list->next;
	}

	LG_log(cr_ctx, LG_FUNC, "<CR_credential_list_check exiting with FALSE");
	return FALSE;
}

/*
  Checks if a credential list has a specific credential, the value is
  the plain version of the credential.

  list            - credential list to be checked
  type            - credential type
  value           - credential value
  include_invalid - includes invalid credentials

  return - TRUE if the credential is contained AND valid, FALSE if not
 */
gboolean CR_credential_list_has_credential(GList* list,
  CR_type type, const char *value, gboolean include_invalid) {
  cr_credential_t* credential;

  LG_log(cr_ctx, LG_DEBUG, "CR_credential_list_has_credential entered");

  while(list) {
    credential = (cr_credential_t*) list->data;
    if (credential->type == type && strcmp(credential->value, value) == 0 ) {
      if (CR_credential_get_validity(credential) || include_invalid) {
        return TRUE;
      }
    }
    list = list->next;
  }

  return FALSE;
}

/*
  Duplicates a list of credentials

  Note: duplicates everything - list and credentials
*/
GList* CR_credential_list_duplicate(GList* credentials) {

  GList* new_credentials = NULL;
  CR_credential_t* cred;

  while (credentials) {
    cred = (CR_credential_t*) credentials->data;
    new_credentials =
      g_list_append(new_credentials,
                    CR_credential_new(
                      CR_credential_get_type(cred),
                      CR_credential_get_value(cred),
                      CR_credential_get_validity(cred)));
    credentials = g_list_next(credentials);
  }

  return new_credentials;
}


/*
  Frees a list of credentials

  Note: Frees everyting - credentials and list
*/
void CR_credential_list_free(GList* creds) {
  GList* del;

  del = creds;

  while(del) {
    CR_credential_free((CR_credential_t*)del->data);
    del = del->next;
  }

  g_list_free(creds);
}

CR_credential_t* CR_credential_duplicate(CR_credential_t* credential) {
  return CR_credential_new(
    CR_credential_get_type(credential),
    CR_credential_get_value(credential),
    CR_credential_get_validity(credential));
}
