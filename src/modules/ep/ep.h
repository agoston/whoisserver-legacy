#ifndef _EP_H_
#define _EP_H_

#include <stdio.h>
#include <glib.h>
#include "lg.h"
#include "mm.h"


/* return codes used by functions in EP module
   and also referenced in dbupdate 
   set to bit values, 0,1,2,4,etc */
typedef enum
{
  EP_OK      =0, /* input processed successfully */
  EP_FAIL    =1, /* input processed with error */
  EP_FATAL   =2  /* fatal error occured, stop processing */
} EP_return_codes;

/* convert the enum to a printable string */
#define EP_ret2str(r) \
  (((r) == EP_OK)        ? "EP_OK" : \
   ((r) == EP_FAIL)      ? "EP_FAIL" : \
   ((r) == EP_FATAL)     ? "EP_FATAL" : \
                            "UNKNOWN" )

typedef enum
{
  EP_PGP,
  EP_X509,
  EP_UNKNOWN
} EP_sig_type;

/* convert the enum to a printable string */
#define EP_sig_type_ret2str(r) \
  (((r) == EP_PGP)       ? "EP_PGP" : \
   ((r) == EP_X509)      ? "EP_X509" : \
                           "UNKNOWN" )

typedef struct {
  GString* from;
  GString* subject;
  GString* date;
  GString* reply_to;
  GString* cc;
  GString* message_id;
} ep_mail_info_t;

typedef struct {
  gboolean is_mail;
  ep_mail_info_t* mail_info;
  GList* flattened_authenticated_data;
  // ..
} ep_input_structure_t;

typedef enum {EP_TEXT, EP_MAIL} ep_data_type_t;

typedef struct {
  ep_data_type_t type;
  GString* data_str;
} ep_exploded_data_t;

typedef struct {
  ep_exploded_data_t* exp_data;
  GList* credentials;
} ep_authenticated_data_t;

/* codes used by the X509 verification functions */

typedef enum
{
  EP_NOT_SET       =-1,/* value not set */
  EP_GOOD_SIG      =0, /* verified with good signature */
  EP_BAD_SIG       =1, /* verified with bad signature */
  EP_NOT_SMIME     =2, /* not an s/mime message */
  EP_NO_DNAME      =4, /* no dname found */
  EP_VERIFY_FATAL  =8  /* fatal error */
} EP_verify_codes;

/* convert the enum to a printable string */
#define EP_verify_ret2str(r) \
  (((r) == EP_GOOD_SIG)       ? "EP_GOOD_SIG" : \
   ((r) == EP_BAD_SIG)        ? "EP_BAD_SIG" : \
   ((r) == EP_NOT_SMIME)      ? "EP_NOT_SMIME" : \
   ((r) == EP_NO_DNAME)       ? "EP_NO_DNAME" : \
   ((r) == EP_VERIFY_FATAL)   ? "EP_VERIFY_FATAL" : \
                              "UNKNOWN" )


typedef void (*ObjFunc) (ep_authenticated_data_t *data, gpointer user_data);

int EP_unfold(FILE* stream, gboolean is_mail, char *X509cert_file, 
                    char *X509certificate, ep_input_structure_t **input);

void EP_init(LG_context_t* ctx, RT_context_t *rtctx, gchar* tmp_dir);
void EP_end();
int EP_stream_to_string(FILE *stream, char *file_name, GString **return_string);

gchar* EP_get_candidate_keywords(ep_input_structure_t *input);
const gchar* EP_get_mail_hdr_field(ep_input_structure_t *input, gchar *field);
void EP_blobs_foreach(ep_input_structure_t *input, ObjFunc func, gpointer user_data);

const gchar* EP_get_blob(ep_authenticated_data_t *input);
GList* EP_get_credentials(ep_authenticated_data_t *input);

void EP_input_structure_free(ep_input_structure_t *input);

int ep_explode(ep_authenticated_data_t *a_data, 
                              ep_input_structure_t **input_structure);
ep_input_structure_t* ep_input_structure_new();

ep_authenticated_data_t* ep_authenticated_data_new();
void ep_authenticated_data_free(ep_authenticated_data_t *a_data);

ep_exploded_data_t* ep_exploded_data_new();
void ep_exploded_data_free(ep_exploded_data_t *exp_data);
ep_mail_info_t *ep_mail_info_new(MM_header_t *header);
void ep_mail_info_free(ep_mail_info_t *);

ep_input_structure_t* ep_text_driver(ep_authenticated_data_t* data);
ep_input_structure_t* ep_mail_driver(ep_authenticated_data_t* data);

GList* ep_split_text(gchar** split, GList* creds);

GList* ep_flatten(GList* list);

int EP_stream_to_string(FILE *stream, char *file_name, GString **return_string);

GList* ep_content_explode(MM_content_t* content, GList* credentials);

GList* ep_plain_process(gchar* content, GList* credentials);
GList* ep_mixed_process(MM_content_t* content, GList* credentials);
GList* ep_pgp_process(MM_content_t* content, GList* credentials) ;

int ep_get_key_id(const gchar *ep_FP, char *X509cert_file,
                char *X509certificate, GList **key_id_good, GList **key_id_bad);
int ep_check_X509_sig( char *X509cert_file, char *X509certificate, char *message,
                          KM_key_return_t **key_data, 
                          int type, char **cert_file );


extern LG_context_t *ep_ctx;
extern gchar *ep_temporary_directory;
extern gchar *ep_DN;
extern int ep_retval;
extern RT_context_t *rt_ctx;

CR_credential_t* x509_process(gchar* stream);

#endif
