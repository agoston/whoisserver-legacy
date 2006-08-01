/***************************************
  $Revision: 1.10 $

  Reporting module.

  Status: REVIEWED, TESTED

  Author(s):       Tiago Antao

  ******************/ /******************
  Modification History:
        tiago (10/04/2003) Created.
  ******************/ /******************
  Copyright (c) 2003               RIPE NCC
 
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
#ifndef RT_DBUPDATE_H
#define RT_DBUPDATE_H

#include "rt.h"
#include "km.h"
#include "cr.h"
#include "ep.h"
#include "cr.h"
#include "km.h"
#include "syntax_api.h"


typedef enum {
  RT_UPD_NONE,
  RT_UPD_DEBUG,
  RT_UPD_INFO,
  RT_UPD_NOTICE,
  RT_UPD_WARN,
  RT_UPD_ERROR,
  RT_UPD_CRIT,
  RT_UPD_FATAL
} RT_upd_level;

typedef enum {
  RT_UPD_ADD,
  RT_UPD_UPD,
  RT_UPD_DEL,
  RT_UPD_NOOP,
  RT_UPD_SYNTAX_ERR,
  RT_UPD_FWD_CREATE,
  RT_UPD_FWD_MODIFY,
  RT_UPD_FWD_DELETE,
  RT_UPD_FWD_POLICY
} RT_upd_op;

gchar* RT_EP_analyse(RT_context_t* ctx, ep_input_structure_t* input);
void RT_unparsable_input(RT_context_t* ctx, gchar* object_candidate);

void RT_internal_error(RT_context_t* ctx);
void RT_help_request(RT_context_t* ctx);

void RT_invalid_keyword(RT_context_t* ctx, gchar* keyword);
void RT_invalid_keyword_combination(RT_context_t* ctx, gchar* keyword_comb);

void RT_set_object(RT_context_t* ctx, rpsl_object_t* object);
void RT_unset_object(RT_context_t* ctx, RT_upd_op operation, gboolean result);

void RT_syntax_error(RT_context_t* ctx);
void RT_syntax_ok(RT_context_t* ctx);

void RT_circular_dependency(RT_context_t* ctx);

void RT_clean_object(RT_context_t* ctx, char *msg);
void RT_invalid_source(RT_context_t* ctx);
void RT_filtered_source(RT_context_t* ctx);

void RT_unknown_country(RT_context_t* ctx, gchar* country);

void RT_unknown_nic_suffix(RT_context_t* ctx);

void RT_enforcednew_exists(RT_context_t* ctx);
void RT_postproc_obj(RT_context_t* ctx, rpsl_object_t* postproc_obj_str);
void RT_no_objects_processed(RT_context_t* ctx);

void RT_kc_gen_diff(RT_context_t* ctx, GList* attributes);
void RT_keycert_auto(RT_context_t* ctx);
void RT_process_time(RT_context_t* ctx, gchar* date_stamp);

void RT_process_time(RT_context_t*, gchar*);

/*
  Changed line stuff...
 */
void RT_changed_date_missing(RT_context_t* ctx, gchar* date, gchar* value);
void RT_multiple_changed_date_missing(RT_context_t* ctx);
void RT_peering_set_syntax(RT_context_t* ctx);
void RT_filter_set_syntax(RT_context_t* ctx);
void RT_changed_date_order(RT_context_t* ctx, gchar* value, gchar* previous);
void RT_changed_line_order(RT_context_t* ctx, gchar* msg, gchar* date);
void RT_changed_date_syntax(RT_context_t* ctx, gchar* msg);

/* 
 * Prefix to range conversion on inetnums 
 */
void RT_inetnum_prefix_converted(RT_context_t* ctx, gchar* prefix, gchar* range);
void RT_inetnum_prefix_convert_failed(RT_context_t* ctx, gchar* prefix);

/*
  has_auth - if some kind of credential was presented (and was wrong)
 */
void RT_auth_failure(RT_context_t* ctx,
		     gchar* type, GList* mntner,
		     gboolean has_auth);

void RT_auth_ok(RT_context_t* ctx,
		gchar* type, gchar* mntner,
		gboolean has_override);

void RT_policy_fail(RT_context_t* ctx, char* reason);



/* delete...
 */
void RT_non_existent_object(RT_context_t* ctx);
void RT_object_exists(RT_context_t* ctx);

void RT_versions_dont_match(RT_context_t* ctx, rpsl_object_t* existing_object);
void RT_versions_match(RT_context_t* ctx);

void RT_object_still_referenced(RT_context_t* ctx);
void RT_object_not_referenced(RT_context_t* ctx);


/* maintainer of object has mail-from
   ok - succeded still
   only_mfrom - only has mfrom
*/
void RT_has_mail_from(RT_context_t* ctx, gboolean ok, gboolean only_mfrom);
void RT_no_mail_from(RT_context_t* ctx);


/* A refered object doesn't exist
 */
void RT_unknown_refered_object(RT_context_t* ctx, gchar* refered_name);
void RT_known_refered_object(RT_context_t* ctx, gchar* refered_name);


void RT_unknown_auto_key(RT_context_t* ctx, gchar* key);

/*
 PKI
 */
void RT_cert_error(RT_context_t* ctx,gchar* err_str);
void RT_report_key_info(RT_context_t* ctx, KM_key_return_t* info);
void RT_key_add_error(RT_context_t* ctx, KM_status_t status);
void RT_key_modify_error(RT_context_t* ctx, KM_status_t status);
void RT_key_remove_error(RT_context_t* ctx, KM_status_t status);
void RT_kc_gen_diff(RT_context_t* ctx, GList* warn_list) ;

/*
  To be researched
 */
void RT_RIP_update_error(RT_context_t* ctx, gchar* error);
void RT_RIP_update_ok(RT_context_t* ctx);

/*
  AS, IRT, mnt, org
 */
void RT_non_exist_mntner(RT_context_t* ctx, const gchar* mntner);
void RT_parent_not_exist(RT_context_t* ctx);
void RT_overlap_inetnums(RT_context_t* ctx, GList *overlap);
void RT_ds_not_accepted(RT_context_t* ctx);
void RT_origin_not_exist(RT_context_t* ctx);
void RT_slash_not_allowed(RT_context_t* ctx);
void RT_manual_creation(RT_context_t* ctx);
void RT_automatic_creation(RT_context_t* ctx);
void RT_irt_auth(RT_context_t* ctx, gchar* key, gchar* type, gchar* attr_checked,
                 GList* irt_ok, GList* irt_fail);
void RT_irt_auth_result(RT_context_t* ctx, gboolean result, gboolean override);

void RT_object_not_found(RT_context_t* ctx, gchar* name);
void RT_non_exist_irt(RT_context_t* ctx, gchar* irt);
void RT_non_exist_org(RT_context_t* ctx, const gchar* org);
void RT_org_auth(RT_context_t* ctx, gchar* key, gchar* type, gchar* attr_checked,
        GList* org_ok, GList* org_fail);
void RT_org_auth_result(RT_context_t* ctx, gboolean result, gboolean override,
                                 gboolean organisation_type_check_failed);
void RT_org_creation_auth_result(RT_context_t* ctx, gboolean result, gboolean override);
void RT_wrong_org_attr_optionality(RT_context_t* ctx);
void RT_organisation_creation_without_auto_id(RT_context_t* ctx);


typedef struct {
  char *from;
  char *from_email;
  char *cc;
  char *subject;
  char *date;
  char *replyto;
  char *msgid;
} mail_hdr_t;

void RT_header_to(RT_context_t* ctx, gchar* to_address);
void RT_header_from(RT_context_t* ctx, gchar* from_address);
void RT_header_subject(RT_context_t* ctx, gchar* subject);
void RT_from_mail(RT_context_t* ctx, mail_hdr_t* mail_info);
void RT_notif_origin(RT_context_t* ctx, gchar* origin);
void RT_notif_add_msg(RT_context_t* ctx, gchar* message, char *op);
void RT_update_result(RT_context_t* ctx, gchar* result);

void RT_status_check_failed(RT_context_t* ctx, gchar* reason);
void RT_name_changed(RT_context_t* ctx);

void RT_status_check_failed(RT_context_t* ctx, gchar* reason);
void RT_name_changed(RT_context_t* ctx);

void RT_credentials(RT_context_t *rt_ctx, GList *credentials);
void RT_full_input(RT_context_t *rt_ctx, char *filename);

void RT_no_frwd_create(RT_context_t *rt_ctx);

void RT_multiple_parents(RT_context_t *ctx, GList *parents);

/* auth check */
void RT_banned_operation(RT_context_t* ctx, gchar* type);
void RT_auth(RT_context_t* ctx, gchar* key, gchar* type, gchar* attr_checked,
        GList* auth_ok, GList* auth_fail, gchar *parent_type);
void RT_irt_auth(RT_context_t* ctx, gchar* key, gchar* type, gchar* attr_checked,
        GList* irt_ok, GList* irt_fail);
void RT_irt_auth_result(RT_context_t* ctx, gboolean result, gboolean override);

void RT_invalid_mnt_routes(RT_context_t* ctx, gchar *key, gchar *type, 
        gchar *parent_text, GList *failed_mntners, GList *invalid_list);
void RT_no_mntners(RT_context_t* ctx, gchar *key, gchar *type, gchar *parent_text);
void RT_disallowed_mnt(RT_context_t* ctx, gchar* name);

/* final result */
void RT_auth_result(RT_context_t* ctx, gboolean result, gboolean override);

/* rdns related */
void RT_rdns_message(RT_context_t*, gchar*,gchar*);

void RT_rdns_overlap(RT_context_t*);
void RT_rdns_trailingdotremoved(RT_context_t*,gchar*);
void RT_rdns_rir(RT_context_t*,gchar*);
void RT_rdns_useotherrir(RT_context_t*,gchar*);
void RT_rdns_cannotregister(RT_context_t*);
void RT_rdns_nonservers(RT_context_t*);
void RT_rdns_parenthasourns(RT_context_t*);
void RT_rdns_parentisnotenum(RT_context_t*);
void RT_rdns_notdelegated(RT_context_t*);
void RT_rdns_delcheckwarning(RT_context_t*,gchar*);
void RT_rdns_threshold(RT_context_t*);
void RT_rdns_invalid_range(RT_context_t*,gchar*);
void RT_rdns_auth_result(RT_context_t* ctx, gboolean result, gboolean override);

/* status check related */
void RT_status_check_failed_parentwithoutstatus(RT_context_t*,gchar*,gchar*);
void RT_status_check_failed_missingstatus(RT_context_t*);
void RT_status_check_failed_allocated(RT_context_t*);
void RT_status_check_failed_earlyregistration(RT_context_t*);
void RT_status_check_failed_notset(RT_context_t*);
void RT_status_check_failed_allocbyrir(RT_context_t*);

#endif
