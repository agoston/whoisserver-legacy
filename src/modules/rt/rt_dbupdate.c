/***************************************
  $Revision: 1.14 $

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
#include "rip.h"
#include "rt_dbupdate.h"
#include "rt_internal.h"
#include <libxml/tree.h>

/* declared in km_pgp.c */
extern char *KM_PGP_GNUPG_ERROR_OUTPUT;

void rt_prepare_node(RT_context_t* ctx, xmlNodePtr node);
xmlNodePtr rt_generate_list(xmlNodePtr parent_node, gchar* tag, GList* list);

rpsl_object_t* rt_get_object(RT_context_t* ctx) {
  rpsl_object_t* obj;

  obj = (rpsl_object_t*)ctx->specific_data;
  return obj;
}

xmlNodePtr rt_get_object_id(RT_context_t* ctx) {
  xmlNodePtr node;
  rpsl_object_t* obj;

  obj = rt_get_object(ctx);
  node = xmlNewNode(NULL, (xmlChar*)"object_id");
  //this is wrong for Person/Roles/Routes
  rt_xml_node_add_content(node, (xmlChar*)strtok((rpsl_object_get_attr_by_ofs(obj, 0)->value), " "));

  return node;
}

typedef struct {
  GList* bad_creds;
  int    nblobs;
} analiser_struct;

void rt_blob(ep_authenticated_data_t* data, gpointer user_data) {
  GList* credentials;
  CR_credential_t* credential;
  analiser_struct* as;

  as = (analiser_struct*)user_data;

  as->nblobs += 1;
  credentials = EP_get_credentials(data);
  while (credentials) {
    credential = (CR_credential_t*) credentials->data;
    if (CR_credential_get_validity(credential) == FALSE) {
      /* we have a bad credential */
      as->bad_creds =
        g_list_append(as->bad_creds, CR_credential_duplicate(credential));
    }
    credentials = credentials->next;
  }
}

/*+
  RT_EP_analyse - Analizes the EP_input_structure

  RT_context_t* ctx - Context.

  EP_input_structure_t* input - The input structure.
  +*/
gchar* RT_EP_analyse(RT_context_t* ctx, ep_input_structure_t* input) {
  xmlNodePtr node = NULL;
  xmlNodePtr child_node = NULL;
  xmlNodePtr cred_node_pgp = NULL;
  xmlNodePtr cred_node_x509 = NULL;
  xmlNodePtr gen_node;
  analiser_struct as;
  GList* creds;
  GList* list;
  int pgp_found = 0;
  int x509_found = 0;

  node = xmlNewNode(NULL, (xmlChar*)"input_analyse");
  rt_prepare_node(ctx, node);
  as.nblobs    = 0;
  as.bad_creds = NULL;
  EP_blobs_foreach(input, rt_blob, &as);
  //PGP bad keys?
  if (as.nblobs == 0) {
    child_node = xmlNewNode(NULL, (xmlChar*)"no_valid_content");
    xmlAddChild(node, child_node);
    return "NO CONTENT";
  }
  else {
    creds = as.bad_creds;
    if (creds) {
      list = creds;
      while (list) {
        gen_node = xmlNewNode(NULL, (xmlChar*)"list");
        if ( CR_credential_get_type(list->data) == CR_PGP )
        {
          cred_node_pgp = xmlNewNode(NULL, (xmlChar*)"bad_pgp_credentials");
          pgp_found = 1;
          xmlNodeSetContent(gen_node, (xmlChar*)CR_credential_get_value(list->data));
          xmlAddChild(cred_node_pgp, gen_node);
        }
        else if ( CR_credential_get_type(list->data) == CR_X509 )
        {
          cred_node_x509 = xmlNewNode(NULL, (xmlChar*)"bad_x509_credentials");
          x509_found = 1;
          xmlNodeSetContent(gen_node, (xmlChar*)CR_credential_get_value(list->data));
          xmlAddChild(cred_node_x509, gen_node);
        }
        list = g_list_next(list);
      }
      if ( pgp_found )
      {
        xmlAddChild(node, cred_node_pgp);
      }
      if ( x509_found )
      {
        xmlAddChild(node, cred_node_x509);
      }

      /* if GnuPG has any error output while checking the PGP keys earlier, include in the message
       * This is a horrible hack, but for some reason, reporting from inside km_pgp.c did create the
       * node in the rt_ctx, but it was lost during the XSLT transformation, for reasons unknown
       * agoston, 2011-09-07 */
      if (KM_PGP_GNUPG_ERROR_OUTPUT) {
          xmlNodePtr invsignode = xmlNewNode(NULL, (xmlChar*)"invalid_signature");
          rt_add_text_node(invsignode, "msg", KM_PGP_GNUPG_ERROR_OUTPUT);
          xmlAddChild(node, invsignode);
      }

      CR_credential_list_free(creds);
      return "BAD CREDENTIALS";
    }
  }

  return "OK";
}

/*+
  RT_unparsable_input - Reports that a part of the input is not an object.

  RT_context_t* ctx - Context.

  gchar* object_candidate - Unparsable input.
  +*/
void RT_unparsable_input(RT_context_t* ctx, gchar* object_candidate) { //
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"unparsable_input");
  rt_xml_node_add_content(node, (xmlChar*)object_candidate);
  rt_prepare_node(ctx, node);
}

/*+
  RT_internal_error - Reports an internal error.

  RT_context_t* ctx - Context.

  +*/
void RT_internal_error(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"internal_error");
  rt_prepare_node(ctx, node);
}

/*+
  RT_help_request - help requested.

  RT_context_t* ctx - Context.

  gchar* keyword - The keyword.
  +*/
void RT_help_request(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"help_request");
  rt_prepare_node(ctx, node);
}


/*+
  RT_process_time - Reports process time of dbupdate execution.

  RT_context_t* ctx - Context.

  gchar* - date_stamp.
  +*/
void RT_process_time(RT_context_t* ctx, gchar* date_stamp) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"process_time");
  rt_add_text_node(node, "p_time", date_stamp);
  rt_prepare_node(ctx, node);
}


/*+
  RT_invalid_keyword - informs of invalid keyword.

  RT_context_t* ctx - Context.

  gchar* keyword - The keyword.
  +*/
void RT_invalid_keyword(RT_context_t* ctx, gchar* keyword) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"invalid_keyword");
  rt_xml_node_add_content(node, (xmlChar*)keyword);
  rt_prepare_node(ctx, node);
}

/*+
  RT_invalid_keyword_combination - informs of invalid keyword combo.

  RT_context_t* ctx - Context.

  gchar* keyword_comb - The keywords.
  +*/
void RT_invalid_keyword_combination(RT_context_t* ctx, gchar* keyword_comb) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"invalid_keyword_combination");
  rt_xml_node_add_content(node, (xmlChar*)keyword_comb);
  rt_prepare_node(ctx, node);
}

xmlNodePtr rt_process_attr(const rpsl_attr_t* attr) {
  xmlNodePtr node;
  const GList* errors;

  node = xmlNewNode(NULL, (xmlChar*)"attr");
  rt_xml_node_add_content(node, (xmlChar*)rpsl_attr_get_name(attr));
  rt_add_text_node(node, "value", rpsl_attr_get_value(attr));
  errors = rpsl_attr_errors(attr);
  while (errors) {
    if (((rpsl_error_t *)errors->data)->level >= RPSL_ERRLVL_ERROR) {
      rt_add_text_node(node, "syntax_error", ((rpsl_error_t*)errors->data)->descr);
    }
    else {
      rt_add_text_node(node, "syntax_warn", ((rpsl_error_t*)errors->data)->descr);
    }
    errors = errors->next;
  }

  return node;
}


/*+
  RT_parent_removed - Removes any parent: attributes.

  RT_context_t* ctx - Context.
  +*/
void RT_clean_object(RT_context_t* ctx, char *msg) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"clean_object");
  rt_add_text_node(node, "msg", msg);
  rt_prepare_node(ctx, node);
}


/*+
  RT_set_object - informs the system of the object being processed.

  RT_context_t* ctx - Context.

  rpsl_object_t* object - The object.
  +*/
void RT_set_object(RT_context_t* ctx, rpsl_object_t* object) {
  xmlNodePtr node;
  const GList* attrs;
  const GList* errs;
  const rpsl_attr_t* attr;
  const rpsl_error_t* err;
  xmlChar* id;


  id = (xmlChar*)rpsl_object_get_key_value(object);
  if (id == NULL) {
    id = (xmlChar*)rpsl_object_get_attr_by_ofs(object, 0)->value;
  }
  node = xmlNewNode(NULL, (xmlChar*)"obj");
  rt_xml_set_prop(node, (xmlChar*)"id", id);
  ctx->specific_data = object;
  RT_request_level(ctx, node);

  attrs = rpsl_object_get_all_attr(object);
  while (attrs) {
    attr = (rpsl_attr_t*)attrs->data;
    node = rt_process_attr(attr);
    rt_prepare_node(ctx, node);
    attrs = g_list_next(attrs);
  };

  errs = rpsl_object_errors(object);
  while (errs) {
    err = (rpsl_error_t*)errs->data;
    if (err->code > RPSL_ERR_SYNERR) {
      if ( err->level > RPSL_ERRLVL_WARN ) {
        node = xmlNewNode(NULL, (xmlChar*)"object_syntax_error");
        rt_xml_node_add_content(node, (xmlChar*)(err->descr));
        rt_prepare_node(ctx, node);
      }
      else {
        node = xmlNewNode(NULL, (xmlChar*)"object_syntax_warn");
        rt_xml_node_add_content(node, (xmlChar*)(err->descr));
        rt_prepare_node(ctx, node);
      }
    }
    errs = g_list_next(errs);
  }
}

void RT_unset_object(RT_context_t* ctx, RT_upd_op operation, gboolean result) {
  gchar op[15];
  gchar res[15];
  xmlNodePtr node;

//  if (!result) {
    //  xmlNewProp(ctx->current_aggregate_node, (xmlChar*)"operation", (xmlChar*)op);
    node = xmlNewNode(NULL,
		      (xmlChar*)"class");

    xmlNewProp(node, (xmlChar*)"class",
	       (xmlChar*)rpsl_object_get_class((rpsl_object_t*)ctx->specific_data));
    rt_prepare_node(ctx, node);

    node = xmlNewNode(NULL,
		      (xmlChar*)"pr_name");

    xmlNewProp(node, (xmlChar*)"pr_name",
     (xmlChar*)rpsl_object_get_attr_by_ofs((rpsl_object_t*)ctx->specific_data, 0)->value);
    rt_prepare_node(ctx, node);
//  }


  switch (operation) {
  case RT_UPD_ADD:
    sprintf(op, "Create");
    break;
  case RT_UPD_UPD:
    sprintf(op, "Modify");
    break;
  case RT_UPD_DEL:
    sprintf(op, "Delete");
    break;
  case RT_UPD_NOOP:
    sprintf(op, "No Operation");
    break;
  case RT_UPD_SYNTAX_ERR:
    sprintf(op, "Syntax Error");
    break;
  case RT_UPD_FWD_CREATE:
    sprintf(op, "Forward Create");
    break;
  case RT_UPD_FWD_MODIFY:
    sprintf(op, "Forward Modify");
    break;
  case RT_UPD_FWD_DELETE:
    sprintf(op, "Forward Delete");
    break;
  case RT_UPD_FWD_POLICY:
    sprintf(op, "Forward Policy");
    break;
  default:
    sprintf(op, "Unknown");
  }

  switch (result) {
  case TRUE:
    sprintf(res, "OK");
    break;
  case FALSE:
    sprintf(res, "ERR");
  }

  xmlNewProp(ctx->current_aggregate_node, (xmlChar*)"operation", (xmlChar*)op);
  xmlNewProp(ctx->current_aggregate_node, (xmlChar*)"result", (xmlChar*)res);
  RT_release_level(ctx);
}

/*+
  RT_syntax_error - Reports a syntax error.

  Note that the library might not even had been able to parse anything,
  so, in the worst case we need the verbatim copy of the object.

  One, and only one of object and object_candidate should be NULL.

  RT_context_t* ctx - Context.

  +*/
void RT_syntax_error(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"syntax_error");
  rt_prepare_node(ctx, node);

}


/*+
  RT_syntax_ok - Reports syntax OK.

  Dual of RT_syntax_error.

  RT_context_t* ctx - Context.
  +*/
void RT_syntax_ok(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"syntax_ok");
  rt_prepare_node(ctx, node);
}

/*+
  RT_circular_dependency - Reports acircular dependency with AUTO- references.

  RT_context_t* ctx - Context.

  +*/
void RT_circular_dependency(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"circular_dependency");
  rt_prepare_node(ctx, node);
}

/*+
  RT_invalid_source - Reports invalid source.

  RT_context_t* ctx - Context.
  +*/
void RT_invalid_source(RT_context_t* ctx) {
  xmlNodePtr node;
  rpsl_object_t* obj;
  GList* attr_list;
  gchar* source;

  obj = rt_get_object(ctx);
  attr_list = rpsl_object_get_attr(obj, "source");
  source = g_strdup(rpsl_attr_get_clean_value((rpsl_attr_t *)attr_list->data));
  rpsl_attr_delete_list(attr_list);


  node = xmlNewNode(NULL, (xmlChar*)"invalid_source");
  rt_add_text_node(node, "source", source);
  rt_prepare_node(ctx, node);
  g_free(source);
}

/*+
  RT_filtered_source - Reports filtered source.

  RT_context_t* ctx - Context.
  +*/
void RT_filtered_source(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"filtered_source");
  rt_prepare_node(ctx, node);
}


/*+
  RT_status_check_failed - Reports that status check as failed

  RT_context_t* ctx - Context.

  gchar* reason - reason.
  +*/

void RT_status_check_failed(RT_context_t* ctx, gchar* reason) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"status_check_failed");
  rt_add_text_node(node, "reason", reason);
  rt_prepare_node(ctx, node);
}


/*+
  RT_name_changed - Reports that person/role name has changed

  RT_context_t* ctx - Context.

  +*/

void RT_name_changed(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"name_changed");
  rt_prepare_node(ctx, node);
}


/*+
  RT_unknown_country - Reports unknown courntry.

  RT_context_t* ctx - Context.

  gchar* - country.
  +*/
void RT_unknown_country(RT_context_t* ctx, gchar* country) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"unknown_country");
  rt_add_text_node(node, "country", country);
  rt_prepare_node(ctx, node);
}

/*+
  RT_unknown_nic_suffix - Reports unknown NIC suffix.

  RT_context_t* ctx - Context.
  +*/
void RT_unknown_nic_suffix(RT_context_t* ctx) { //
  xmlNodePtr node;
  rpsl_object_t* obj;
  GList* attr_list;
  gchar* hdl;
  gchar* suffix;

  obj = rt_get_object(ctx);
  attr_list = rpsl_object_get_attr(obj, "nic-hdl");
  hdl = g_strdup(rpsl_attr_get_value((rpsl_attr_t*)attr_list->data));
  for (suffix=hdl+strlen(hdl);suffix>hdl;suffix--) {
    if (*suffix == '-') {
      suffix++;
      break;
    }
  }
  rpsl_attr_delete_list(attr_list);

  node = xmlNewNode(NULL, (xmlChar*)"unknown_nic_suffix");
  rt_add_text_node(node, "suffix", suffix);
  rt_prepare_node(ctx, node);
  g_free(hdl);
}


/*+
  RT_nichdl_not_available - Reports nic-hdl has already been used and is not available.

  RT_context_t* ctx - Context.

  gchar* - nichdl.
  +*/
void RT_nichdl_not_available(RT_context_t *ctx, const gchar *nichdl) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"nichdl_not_available");
  rt_xml_node_add_content(node, (xmlChar*)nichdl);
  rt_prepare_node(ctx, node);
}


/*+
  RT_nichdl_not_valid - Reports nic-hdl syntax not valid.

  RT_context_t* ctx - Context.

  gchar* - nichdl.
  +*/
void RT_nichdl_not_valid(RT_context_t *ctx, const gchar *nichdl) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"nichdl_not_valid");
  rt_xml_node_add_content(node, (xmlChar*)nichdl);
  rt_prepare_node(ctx, node);
}


/*+
  RT_auth_failure - Reports an authorization failure.

  RT_context_t* ctx - Context.

  gchar* type - Type of failure, e.g. "MNT-BY", "MNT-ROUTES", "MNT-LOWER"...

  GList* mntner - List of maintainers that can authorize.

  gboolean has_auth - Some form of authentication was provided.

  has_auth should be replaced by credentials.

  +*/
void RT_auth_failure(RT_context_t* ctx,
		     gchar* type, GList* mntner,
		     gboolean has_auth) {
  xmlNodePtr node;
  xmlNodePtr mnt_node;

  node = xmlNewNode(NULL, (xmlChar*)"auth_failure");

  mnt_node = xmlNewNode(NULL, (xmlChar*)"possible_maintainers");
  rt_generate_list(mnt_node, "list", mntner);
  xmlAddChild(node, mnt_node);

  rt_prepare_node(ctx, node);
}


/*+
  RT_auth_ok - Reports an authorization OK.

  RT_context_t* ctx - Context.

  gchar* type - Type of failure, e.g. "MNT-BY", "MNT-ROUTES", "MNT-LOWER"...

  Gchar* mntner - Maintainer associated with authorization (NULL OK).

  gboolean has_override - Authorization with override.
  +*/
void RT_auth_ok(RT_context_t* ctx,
		gchar* type, gchar* mntner,
		gboolean has_override) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"auth_ok");

  /*
  if (has_override) {
    xmlAddChild(node, xmlNewNode(NULL, (xmlChar*)"override"));
  }
  */

  rt_add_text_node(node, "mntner", mntner);
  rt_prepare_node(ctx, node);
}

/*+
  RT_enforcednew_exists - Its an update, but NEW was specified

  RT_context_t* ctx - Context.
 +*/
void RT_enforcednew_exists(RT_context_t* ctx) { //
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"enforced_new_exists");
  rt_prepare_node(ctx, node);
}

/*+
  RT_postproc_obj - Final state of the object after postprocessing.

  RT_context_t* ctx - Context.

  gchar* object - the postprocessed object.

  INFO
 +*/
void RT_postproc_obj(RT_context_t* ctx, rpsl_object_t* object) { //
  xmlChar* id;

  /* XXX rpsl bug */
  id = (xmlChar*)rpsl_object_get_key_value(object);
  if (id == NULL) {
    id = (xmlChar*)rpsl_object_get_attr_by_ofs(object, 0)->value;
  }
  rt_xml_set_prop(ctx->current_aggregate_node,
    (xmlChar*)"id", id);

}

void RT_no_objects_processed(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"no_objects_processed");
  rt_prepare_node(ctx, node);
}

void RT_kc_gen_diff(RT_context_t* ctx, GList* attributes) {
  xmlNodePtr node;

  while (attributes) {
    node = xmlNewNode(NULL, (xmlChar*)"incorrect_generated");
    rt_xml_node_add_content(node, (xmlChar*)attributes->data);
    rt_prepare_node(ctx, node);
    attributes = attributes->next;
  }
}

/*+
  RT_unmaintained_person - This person/role object is not maintained
  RT_context_t* ctx - Context.
  gchar* key - nic-hdl
  gchar* type - person or role
 +*/
void RT_unmaintained_person(RT_context_t* ctx, gchar* key, const char *type) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"unmaintained_person");
  rt_add_text_node(node, "key", key);
  rt_add_text_node(node, "type", type);
  rt_prepare_node(ctx, node);
}

/*+
  RT_unmaintained_person_in_mntner - This person/role object referenced in the mntner
                                     is not maintained
  RT_context_t* ctx - Context.
  gchar* key - nic-hdl
  gchar* type - person or role
  gchar* mntner - mntner name
 +*/
void RT_unmaintained_person_in_mntner(RT_context_t* ctx, gchar* key, const char *type, gchar* mntner) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"unmaintained_person_in_mntner");
  rt_add_text_node(node, "key", key);
  rt_add_text_node(node, "type", type);
  rt_add_text_node(node, "mntner", mntner);
  rt_prepare_node(ctx, node);
}

/*+
  RT_startup - Person creation references a non existant mntner
  RT_context_t* ctx - Context.
 +*/
void RT_startup(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"startup");
  rt_prepare_node(ctx, node);
}

/*+
  RT_inetnum_prefix_converted - The prefix on an inetnum range was converted to range (WARNING)
  RT_context_t* ctx - Context.

  gchar* prefix - The prefix

  gchar* range - Converted range
 +*/
void RT_inetnum_prefix_converted(RT_context_t* ctx, gchar* prefix, gchar* range) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"inetnum_prefix_converted");
  rt_add_text_node(node, "prefix", prefix);
  rt_add_text_node(node, "range", range);
  rt_prepare_node(ctx, node);
}

/*+
  RT_inetnum_prefix_convert_failed - The prefix on an inetnum range could not be converted into a range (WARNING)
  RT_context_t* ctx - Context.

  gchar* prefix - The prefix

 +*/
void RT_inetnum_prefix_convert_failed(RT_context_t* ctx, gchar* prefix) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"inetnum_prefix_convert_failed");
  rt_add_text_node(node, "prefix", prefix);
  rt_prepare_node(ctx, node);
}


/*+
  RT_changed_date_missing - The changed date is missing (WARNING)

  RT_context_t* ctx - Context.

  gchar* date - Current date.

  gchar* value - Current value.
 +*/
void RT_changed_date_missing(RT_context_t* ctx, gchar* date, gchar* value) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"changed_date_missing");
  rt_add_text_node(node, "date", date);
  rt_add_text_node(node, "value", value);
  rt_prepare_node(ctx, node);
}

/*+
  RT_peering_set_syntax - The peering-set object does not contain at least one
    mp-peering or peering attribute.

  RT_context_t* ctx - Context.
 +*/
void RT_peering_set_syntax(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"peering_set_syntax");
  rt_prepare_node(ctx, node);
}


/*+
  RT_filter_set_syntax - The filter-set object contains both mp-filter and filter
    attributes.

  RT_context_t* ctx - Context.
 +*/
void RT_filter_set_syntax(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"filter_set_syntax");
  rt_prepare_node(ctx, node);
}


/*+
  RT_invalid_asblock_range - The as-block range is invalid: ASx - ASy where y<x

  RT_context_t* ctx - Context.
 +*/
void RT_invalid_asblock_range(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"as_block_range");
  rt_prepare_node(ctx, node);
}


/*+
  RT_multiple_changed_date_missing - Multiple changed without date.

  RT_context_t* ctx - Context.
 +*/
void RT_multiple_changed_date_missing(RT_context_t* ctx) { //
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"multiple_changed_date_missing");
  rt_prepare_node(ctx, node);
}

/*+
  RT_changed_date_order - Changed date order is not correct.

  RT_context_t* ctx - Context.

  gchar* value - 2nd value.

  gchar* previous - previous value.
 +*/
void RT_changed_date_order(RT_context_t* ctx, gchar* value, gchar* previous) {
//
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"changed_date_order");
  rt_add_text_node(node, "value", value);
  rt_add_text_node(node, "previous", previous);
  rt_prepare_node(ctx, node);
}

/*+
  RT_changed_date_syntax - General syntax problem.

  RT_context_t* ctx - Context.

  gchar* msg - Message.

  gchar* date - Date.
 +*/
void RT_changed_date_syntax(RT_context_t* ctx, gchar* msg) { //
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"changed_date_syntax");
  rt_add_text_node(node, "message", msg);
  rt_prepare_node(ctx, node);
}

/*+
  RT_policy_fail - Reports that an object fails a policy check.

  RT_context_t* ctx - Context.

  char* reason - The reason why it failed.
  +*/
void RT_policy_fail(RT_context_t* ctx, gchar* reason) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"policy_fail");

  rt_add_text_node(node, "reason", reason);

  rt_prepare_node(ctx, node);
}

/*+
  RT_non_existent_object - Reports that an object doesn't exist.

  To be used when there is a deletion attempt on a non existent object.

  RT_context_t* ctx - Context.
  +*/
void RT_non_existent_object(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"non_existent_object");
  rt_prepare_node(ctx, node);
}

/*+
  RT_object_exists - Dual of RT_non_existent_object

  RT_context_t* ctx - Context.
  +*/
void RT_object_exists(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"object_exists");

  rt_prepare_node(ctx, node);
}

/*+
  RT_versions_dont_match - Reports that an object doesnt match another.

  To be used when an existent object doesn't match the supplied object
  for deletion.

  RT_context_t* ctx - Context.

  rpsl_object_t* existing_object - The current object on the database.
  +*/
void RT_versions_dont_match(RT_context_t* ctx,rpsl_object_t* existing_object) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"versions_dont_match");

  rt_add_text_node(node,
		   "object", rpsl_object_get_text(existing_object, 0));

  rt_prepare_node(ctx, node);
}

/*+
  RT_versions_match - Dual of RT_versions_dont_match
  RT_context_t* ctx - Context.
  +*/
void RT_versions_match(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"versions_match");

  rt_prepare_node(ctx, node);
}

/*+
  RT_object_still_referenced - Reports that an object is still referenced.

  A referenced object cannot be deleted, this functions allow the
  reporting that an object is still referenced.

  RT_context_t* ctx - Context.
  +*/
void RT_object_still_referenced(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"object_still_referenced");

  rt_prepare_node(ctx, node);
}

/*+
  RT_keycert_auto - Reports invalid name for creating X.509 key-cert object.

  RT_context_t* ctx - Context.
  +*/
void RT_keycert_auto(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"keycert_auto");
  rt_prepare_node(ctx, node);
}

/*+
  RT_object_not_referenced - Dual of RT_object_still_referenced.

  RT_context_t* ctx - Context.
  +*/
void RT_object_not_referenced(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"object_not_referenced");

  rt_prepare_node(ctx, node);
}

/*+
  RT_has_mail_from - reports MAIL-FROM references.

  To be used when that are MAIL-FROMs in maintainers.
  This might disapear and RT_message usage recommended.

  RT_context_t* ctx - Context.

  gboolean ok - This was not cause of failure (e.g. there are other auths).

  gboolean only_mfrom - mntner only has MAIL-FROM.
  +*/
void RT_has_mail_from(RT_context_t* ctx, gboolean ok, gboolean only_mfrom) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"mail_from");
  if (only_mfrom) {
    xmlAddChild(node, xmlNewNode(NULL, (xmlChar*)"only_mfrom"));
  }
  else if (ok){
    xmlAddChild(node, xmlNewNode(NULL, (xmlChar*)"ok"));
  }

  rt_prepare_node(ctx, node);
}

/*+
  RT_no_mail_from - Dual of RT_has_mail_from.

  RT_context_t* ctx - Context.
  +*/
void RT_no_mail_from(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"no_mail_from");

  rt_prepare_node(ctx, node);
}

/*+
  RT_unknown_refered_object - Object points to unknown objects.

  To be used when an object has reference to non existing objects

  RT_context_t* ctx - Context.

  gchar* refered_name - The referenced name.
  +*/
void RT_unknown_refered_object(RT_context_t* ctx,
			       gchar* refered_name) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"unknown_refered_object");
  rt_add_text_node(node, "refered_object", refered_name);

  rt_prepare_node(ctx, node);
}

/*+
  RT_known_refered_object - Dual of RT_unknown_refered_object.

  RT_context_t* ctx - Context.
   +*/
void RT_known_refered_object(RT_context_t* ctx, gchar* refered_name){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"known_refered_object");
  rt_add_text_node(node, "refered_object", refered_name);

  rt_prepare_node(ctx, node);
}


/*+
  RT_unknown_auto_key - An AUTO- key is unknown.

  RT_context_t* ctx - Context.

  gchar* key - The unknown key
  +*/
void RT_unknown_auto_key(RT_context_t* ctx,
				gchar* key) {
  xmlNodePtr     node;

  node = xmlNewNode(NULL, (xmlChar*)"unknown_auto_key");
  rt_add_text_node(node, "key", key);

  rt_prepare_node(ctx, node);
}

void RT_cert_error(RT_context_t* ctx,
				gchar* err_str) {
  xmlNodePtr     node;

  node = xmlNewNode(NULL, (xmlChar*)"cert_error");
  rt_add_text_node(node, "err_str", err_str);

  rt_prepare_node(ctx, node);
}

void RT_report_key_info(RT_context_t* ctx, KM_key_return_t* info) {
  KM_status_t    status;
  xmlNodePtr     node;
  xmlNodePtr     child;
  int            x509_err = 0;

  LG_log(lg_ctx, LG_FUNC,">RT_report_key_info: entered\n");
  status = KM_key_return_get_status(info);
  LG_log(lg_ctx, LG_DEBUG,"RT_report_key_info: km status [%d]\n", (int)status);
  node = xmlNewNode(NULL, (xmlChar*)"key_info");

  /* check X.509 errors first as there can be more than one */
  if ( status & KM_NO_FP )
  {
    LG_log(lg_ctx, LG_DEBUG,"RT_report_key_info: status KM_NO_FP\n");
    child = xmlNewNode(NULL, (xmlChar*)"no_fingerpr");
    xmlAddChild(node, child);
    x509_err = 1;
  }
  if ( status & KM_NO_SJ )
  {
    LG_log(lg_ctx, LG_DEBUG,"RT_report_key_info: status KM_NO_SJ\n");
    child = xmlNewNode(NULL, (xmlChar*)"no_subject");
    xmlAddChild(node, child);
    x509_err = 1;
  }

  if ( ! x509_err )
  {
    /* check for pgp errors */
    LG_log(lg_ctx, LG_DEBUG,"RT_report_key_info: check for pgp errors\n");
    switch (status) {
    case KM_OK:
      child = xmlNewNode(NULL, (xmlChar*)"ok");
      break;
    case KM_MULTIPLE_KEYS:
      child = xmlNewNode(NULL, (xmlChar*)"multiple_keys");
      break;
    case KM_NO_KEY:
      child = xmlNewNode(NULL, (xmlChar*)"no_key");
      break;
    case KM_KEY_EXISTS:
      child = xmlNewNode(NULL, (xmlChar*)"keys_exists");
      break;
    case KM_SECRET_KEY:
      child = xmlNewNode(NULL, (xmlChar*)"secret_key");
      break;
    case KM_IDS_DONT_MATCH:
      child = xmlNewNode(NULL, (xmlChar*)"ids_dont_match");
      break;
    case KM_INTERNAL:
      child = xmlNewNode(NULL, (xmlChar*)"internal");
      break;
    case KM_UNEXISTENT_KEY:
      child = xmlNewNode(NULL, (xmlChar*)"unexistent_key");
      break;
    case KM_NOT_REMOVED:
      child = xmlNewNode(NULL, (xmlChar*)"not_removed");
      break;
    default:
      child = xmlNewNode(NULL, (xmlChar*)"unknown_error");
    }
    xmlAddChild(node, child);
  }

  rt_prepare_node(ctx, node);
  LG_log(lg_ctx, LG_FUNC,"<RT_report_key_info: exiting\n");
}

/*+
  RT_invalid_signature - Validating pgp signature failed.

  RT_context_t* ctx - Context.
  +*/
void RT_invalid_signature(RT_context_t* ctx, gchar *msg) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"invalid_signature");
  rt_add_text_node(node, "msg", msg);
  rt_prepare_node(ctx, node);
}

/*+
  RT_key_add_error - Reports an add key error.

  RT_context_t* ctx - Context.

  KM_status_t - status.
  +*/
void RT_key_add_error(RT_context_t* ctx, KM_status_t status) {
  xmlNodePtr node;
  gchar*     error_string;

  node = xmlNewNode(NULL, (xmlChar*)"key_add_error");
  error_string = KM_return_string(status);
  rt_add_text_node(node, "error", error_string);

  rt_prepare_node(ctx, node);
}

/*+
  RT_key_modify_error - Reports a modify key error.

  RT_context_t* ctx - Context.

  KM_status_t - status.

  +*/
void RT_key_modify_error(RT_context_t* ctx, KM_status_t status) { //
  xmlNodePtr node;
  gchar*     error_string;

  node = xmlNewNode(NULL, (xmlChar*)"key_modify_error");
  error_string = KM_return_description(status);
  rt_add_text_node(node, "error", error_string);

  rt_prepare_node(ctx, node);
}

/*+
  RT_key_remove_error - Reports a remove key error.

  RT_context_t* ctx - Context.

  KM_status_t - status.

  +*/
void RT_key_remove_error(RT_context_t* ctx, KM_status_t status) { //
  xmlNodePtr node;
  gchar* error_string;

  node = xmlNewNode(NULL, (xmlChar*)"key_remove_error");
  error_string = KM_return_string(status);
  rt_add_text_node(node, "error", error_string);

  rt_prepare_node(ctx, node);
}

/*+
  RT_RIP_update_error - Reports an error from RIPUpdate

  RIPUpdate error, still to be thinked.

  RT_context_t* ctx - Context.
  +*/
void RT_RIP_update_error(RT_context_t* ctx, gchar* error) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"rip_update_error");
  rt_add_text_node(node, "error", error);

  rt_prepare_node(ctx, node);
}

/*+
  RT_RIP_update_ok - Dual of RT_RIP_update_ok.

  RT_context_t* ctx - Context.
  +*/
void RT_RIP_update_ok(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"rip_update_ok");

  rt_prepare_node(ctx, node);
}


/*+
  RT_non_exist_mntner - Reports a non existent maintainer
  +*/
void RT_non_exist_mntner(RT_context_t* ctx, const gchar* mntner) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"non_existent_maintainer");
  rt_xml_node_add_content(node, (xmlChar*)mntner);
  rt_prepare_node(ctx, node);
}

/*+
  RT_non_exist_irt - Reports a non existent irt
  +*/
void RT_non_exist_irt(RT_context_t* ctx, gchar* irt) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"non_existent_irt");
  rt_xml_node_add_content(node, (xmlChar*)irt);
  rt_prepare_node(ctx, node);
}


/*+
  RT_non_exist_org - Reports a non existent org
  +*/
void RT_non_exist_org(RT_context_t* ctx, const gchar* org) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"non_existent_org");
  rt_xml_node_add_content(node, (xmlChar*)org);
  rt_prepare_node(ctx, node);
}


/*+
  RT_parent_not_exist - Report that object has no parent in the DB

  RT_context_t* ctx - Context.
  +*/
void RT_parent_not_exist(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"non_existent_parent");

  rt_prepare_node(ctx, node);
}


/*+
  RT_no_address_space - Report that rDNS object has no corresponding inet(6)num in the DB

  RT_context_t* ctx - Context.
  +*/
void RT_no_address_space(RT_context_t *ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"RDNSno_address_space");

  rt_prepare_node(ctx, node);
}


/*+
  RT_rdns_ds_not_accepted - Report that we don't accept DS records for this zone

  RT_context_t* ctx - Context.
  +*/
void RT_rdns_ds_not_accepted(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"RDNSds_not_accepted");

  rt_prepare_node(ctx, node);
}


/*+
  RT_rdns_size_not_accepted - Report that we don't accept this delegation size

  RT_context_t* ctx - Context.
  +*/
void RT_rdns_size_not_accepted(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"RDNSsize_not_accepted");

  rt_prepare_node(ctx, node);
}

/*+
  RT_rdns_hierarchy - A more or less specific domain object found

  RT_context_t *ctx - Context.

  gchar *specifics - 'more' or 'less'.

  gchar *name - name of domain object found.
 +*/
void RT_rdns_hierarchy(RT_context_t *ctx, gchar *specifics, gchar *name) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"rdns_hierarchy");
  rt_add_text_node(node, "specifics", specifics);
  rt_add_text_node(node, "name", name);
  rt_prepare_node(ctx, node);
}


/*+
  RT_origin_not_exist - Report that route object has no origin AS in the DB

  RT_context_t* ctx - Context.
  +*/
void RT_origin_not_exist(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"non_existent_origin");

  rt_prepare_node(ctx, node);
}


/*+
  RT_slash_not_allowed - Slash notation is only allowed for create.

  RT_context_t* ctx - Context.
  +*/
void RT_slash_not_allowed(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"slash_not_allowed");

  rt_prepare_node(ctx, node);
}


/*+
  RT_manual_creation - Report that object requires manual creation.

  This is normally irt, mntner and AS.

  RT_context_t* ctx - Context.
  +*/
void RT_manual_creation(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"manual_creation");

  rt_prepare_node(ctx, node);
}

/*+
  RT_automatic_creation - Dual of RT_is_manual_creation.

  RT_context_t* ctx - Context.
  +*/
void RT_automatic_creation(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"automatic_creation");

  rt_prepare_node(ctx, node);
}


/*+
  RT_object_not_found - A certain object was not found.

  RT_context_t* ctx - Context.

  gchar* name - pkey.

  WARNING
  +*/
void RT_object_not_found(RT_context_t* ctx, gchar* name) { //
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"object_not_found");
  rt_add_text_node(node, "object", name);

  rt_prepare_node(ctx, node);
}

/*+
  RT_header - Report output headers.

  RT_context_t* ctx   - Context.

  gchar* to_address   - To address.

  gchar* from_address - From address.

  FATAL  -> not fatal but has always to go
  +*/
void RT_header_to(RT_context_t* ctx, gchar* to_address) {
//
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"notification_header_to");
  rt_add_text_node(node, "to", to_address);

  rt_prepare_node(ctx, node);
}

void RT_header_from(RT_context_t* ctx, gchar* from_address) {
//
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"notification_header_from");
  rt_add_text_node(node, "from", from_address);

  rt_prepare_node(ctx, node);
}

void RT_header_subject(RT_context_t* ctx, gchar* subject) {
//
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"notification_header_subject");
  rt_add_text_node(node, "subject", subject);

  rt_prepare_node(ctx, node);
}

/*+
  RT_from_mail - Report mail originator.

  RT_context_t* ctx - Context.

  mail_hdr_t* mail_info - Mail info.

  INFO
  +*/
void RT_from_mail(RT_context_t* ctx, mail_hdr_t* mail_info) { //
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"mail_origin_of_request");
  rt_add_text_node(node, "from", mail_info->from);
  rt_add_text_node(node, "from_email", mail_info->from_email);
  //rt_add_text_node(node, "cc", mail_info->cc);
  rt_add_text_node(node, "subject", mail_info->subject);
  rt_add_text_node(node, "date", mail_info->date);
  rt_add_text_node(node, "reply_to", mail_info->replyto);
  rt_add_text_node(node, "msgid", mail_info->msgid);

  rt_prepare_node(ctx, node);
}

/*+
  RT_notif_origin - Report originating IP address of a request.

  RT_context_t* ctx - Context.

  gchar* origin - IP.

  INFO
  +*/
void RT_notif_origin(RT_context_t* ctx, gchar* origin) { //
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"ip_origin_of_request");
  rt_add_text_node(node, "ip", origin);

  rt_prepare_node(ctx, node);
}

void RT_notif_add_msg(RT_context_t* ctx, gchar* message, char *op) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"notif_message");
  rt_add_text_node(node, "op", op);
  rt_add_text_node(node, "message", message);

  rt_prepare_node(ctx, node);
}

void RT_credentials(RT_context_t *ctx, GList *credentials) {
  xmlNodePtr node;
  CR_credential_t* cr;
  char *cred_value = NULL;

  node = xmlNewNode(NULL, (xmlChar*)"credentials");
  while (credentials) {
    cr = (CR_credential_t*)(credentials->data);
    if (CR_credential_get_validity(cr)) {
      /* This string copy is an emergency repair.
         CR_credential_get_value returns a const char*
         which points into the list of credentials returned
         from the EP input processing module.
         But the contents are changed in rt_add_text_node.
         This string is NOT freed as I do not know at what
         point in the XML it is no longer needed.
      */
      cred_value = strdup( CR_credential_get_value(cr) );
      rt_add_text_node(node, "list", cred_value);
    }
    credentials = credentials->next;
  }
  rt_prepare_node(ctx, node);
}


void RT_full_input(RT_context_t* ctx, char *filename) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"full_input");
  rt_add_text_node(node, "message", filename);

  rt_prepare_node(ctx, node);
}


void RT_no_frwd_create(RT_context_t* ctx) {
}

void RT_operation(RT_context_t* ctx, gint operation, gboolean ok) {
  xmlNodePtr     node;
  char           op[10];

  if (ok) {
    node = xmlNewNode(NULL, (xmlChar*)"operation_ok");
  }
  else {
    node = xmlNewNode(NULL, (xmlChar*)"operation_failed");
  }

  switch (operation) {
  case RT_UPD_ADD:
    sprintf((char*)op, "ADD");
    break;
  case RT_UPD_UPD:
    sprintf((char*)op, "UPD");
    break;
  case RT_UPD_DEL:
    sprintf((char*)op, "DEL");
    break;
  }

  rt_add_text_node(node, "operation", op);

  if (ok) {
    rt_prepare_node(ctx, node);
    //Not an error, but with equal importance
  }
  else {
    rt_prepare_node(ctx, node);
  }
}

xmlChar* rt_object_class(rpsl_object_t* object) {
  return (xmlChar*)rpsl_object_get_key_value(object);
}

void RT_multiple_parents(RT_context_t *ctx, GList *parents) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"multiple_parents");
  rt_generate_list_map(node, "list_line", parents, (rt_map_function)rt_object_class);
  rt_prepare_node(ctx, node);
}

/*+
  RT_overlap_inetnums - Report that object has overlapping objects in the DB

  RT_context_t* ctx - Context.
  GList *overlap - list of overlaps
  +*/
void RT_overlap_inetnums(RT_context_t* ctx, GList *overlap) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"overlap_inetnums");
  rt_generate_list_map(node, "list_line", overlap, (rt_map_function)rt_object_class);

  rt_prepare_node(ctx, node);
}

/*+
  RT_no_mntners - Report that there are no mntners to be checked

  RT_context_t* ctx - Context.
  gchar *key  - key value of the parent object
  gchar *type - type of the parent object
  gchar *parent_text - exact match/parent
  +*/
void RT_no_mntners(RT_context_t *ctx, gchar *key, gchar *type, gchar *parent_text)
{
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"no_mntners");
  rt_add_text_node(node, "parent_text", parent_text);
  rt_add_text_node(node, "type", type);
  rt_add_text_node(node, "key", key);

  rt_prepare_node(ctx, node);
}
/*+
  RT_invalid_mnt_routes - Report that parent object has invalid mnt-routes attributes

  RT_context_t* ctx - Context.
  gchar *key  - key value of the parent object
  gchar *type - type of the parent object
  gchar *parent_text - exact match/parent
  GList* failed_mntners - list of mntners
  GList* invalid_list - list of invalid prefixes
  +*/

void RT_invalid_mnt_routes(RT_context_t *ctx, gchar *key, gchar *type, gchar *parent_text, GList *failed_mntners, GList *invalid_list) {
  xmlNodePtr node;
  xmlNodePtr failed_mntners_node;
  xmlNodePtr invalid_list_node;
  xmlNodePtr gen_node;
  GList *p;

  node = xmlNewNode(NULL, (xmlChar*)"invalid_mnt_routes");
  rt_add_text_node(node, "parent_text", parent_text);
  rt_add_text_node(node, "type", type);
  rt_add_text_node(node, "key", key);

  p = invalid_list;
  invalid_list_node = xmlNewNode(NULL, (xmlChar*)"invalid_list");

  while (p) {
    gen_node = xmlNewNode(NULL, (xmlChar*)"list");
    xmlNodeSetContent(gen_node, (xmlChar*)(p->data));
    xmlAddChild(invalid_list_node, gen_node);
    p = g_list_next(p);
  }
  xmlAddChild(node, invalid_list_node);

  p = failed_mntners;
  failed_mntners_node = xmlNewNode(NULL, (xmlChar*)"failed_mntners");

  while (p) {
    gen_node = xmlNewNode(NULL, (xmlChar*)"list");
    xmlNodeSetContent(gen_node, (xmlChar*)rpsl_attr_get_clean_value(p->data));
    xmlAddChild(failed_mntners_node, gen_node);
    p = g_list_next(p);
  }
  xmlAddChild(node, failed_mntners_node);

  rt_prepare_node(ctx, node);
}

/*+
  RT_disallowed_mnt - Report that a disallowed mntner has been used

  RT_context_t* ctx - Context.
  GList *overlap - list of overlaps
  +*/
void RT_disallowed_mnt(RT_context_t* ctx, gchar* name) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"disallow_mntner");
  rt_add_text_node(node, "mntner", name);

  rt_prepare_node(ctx, node);
}

/* auth check */
void RT_irt_auth(RT_context_t* ctx, gchar* key, gchar* type, gchar* attr_checked,
        GList* irt_ok, GList* irt_fail) {
  xmlNodePtr node;
  xmlNodePtr child;

  node = xmlNewNode(NULL, (xmlChar*)"irt_auth_info");
  rt_add_text_node(node, "key", key);
  rt_add_text_node(node, "type", type);
//  rt_add_text_node(node, "type", rpsl_object_get_class(rt_get_object(ctx)));
  if (!attr_checked) {
    child = xmlNewNode(NULL, (xmlChar*)"no_irt");
    xmlAddChild(node, child);
  }
  else {
    rt_add_text_node(node, "attribute_checked", attr_checked);
    if (irt_ok) {
      child = xmlNewNode(NULL, (xmlChar*)"irt_auth_ok");
      xmlAddChild(node, child);
      rt_generate_list_map(child, "list", irt_ok, (rt_map_function)rt_object_class);
    }
    if (irt_fail) {
      child = xmlNewNode(NULL, (xmlChar*)"irt_auth_fail");
      xmlAddChild(node, child);
      rt_generate_list_map(child, "list", irt_fail, (rt_map_function)rt_object_class);
    }
  }

  rt_prepare_node(ctx, node);
}

/* external check: optionality of "org:" attribute */
void RT_wrong_org_attr_optionality(RT_context_t *ctx){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"wrong_org_attr_optionality");

  rt_prepare_node(ctx, node);
}

/* external check: missing required "assignment-size:" attribute */
void RT_missing_ass_size_attr(RT_context_t *ctx){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"missing_ass_size_attr");

  rt_prepare_node(ctx, node);
}

/* external check: missing required "assignment-size:" attribute */
void RT_ass_size_attr_found(RT_context_t *ctx){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"ass_size_attr_found");

  rt_prepare_node(ctx, node);
}

/* external check: size of "assignment-size:" */
void RT_wrong_ass_size(RT_context_t *ctx){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"wrong_ass_size");

  rt_prepare_node(ctx, node);
}

void RT_ass_size_changed(RT_context_t *ctx, gchar *old_ass_size){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"ass_size_changed");
  rt_add_text_node(node, "old_ass_size", old_ass_size);

  rt_prepare_node(ctx, node);
}

void RT_ass_size_out_of_bounds(RT_context_t *ctx){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"ass_size_out_of_bounds");

  rt_prepare_node(ctx, node);
}

void RT_invalid_more_specific_prefix_size(RT_context_t *ctx, gchar *parent_ass_size){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"invalid_more_specific_prefix_size");
  rt_add_text_node(node, "parent_ass_size", parent_ass_size);

  rt_prepare_node(ctx, node);
}

/* external check: for organisation object creations, the orgID must be AUTO-id */
void RT_organisation_creation_without_auto_id(RT_context_t *ctx){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"organisation_creation_without_auto_id");

  rt_prepare_node(ctx, node);
}

void RT_addr_format_error(RT_context_t *ctx, gchar *ping_add){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"addr_format_error");
  rt_add_text_node(node, "ping_add", ping_add);

  rt_prepare_node(ctx, node);
}

void RT_ping_addr_space_mismatch(RT_context_t *ctx, gchar *ping_add){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"ping_addr_space_mismatch");
  rt_add_text_node(node, "ping_add", ping_add);

  rt_prepare_node(ctx, node);
}

void RT_ping_addr_outside_prefix(RT_context_t *ctx, gchar *ping_add){
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"ping_addr_outside_prefix");
  rt_add_text_node(node, "ping_add", ping_add);

  rt_prepare_node(ctx, node);
}


/* final result */
void RT_irt_auth_result(RT_context_t* ctx, gboolean result, gboolean override) {
  xmlNodePtr node;
  xmlNodePtr child;

  node = xmlNewNode(NULL, (xmlChar*)"irt_auth_result");
  if (override) {
    rt_xml_set_prop(node, (xmlChar*)"override", (xmlChar*)"yes");
  }
  if (result) {
    child = xmlNewNode(NULL, (xmlChar*)"ok");
  }
  else {
    child = xmlNewNode(NULL, (xmlChar*)"fail");
  }
  xmlAddChild(node, child);
  rt_prepare_node(ctx, node);
}

/* auth check for org */
void RT_org_auth(RT_context_t* ctx, gchar* key, gchar* type, gchar* attr_checked,
        GList* org_ok, GList* org_fail) {
  xmlNodePtr node;
  xmlNodePtr child;

  node = xmlNewNode(NULL, (xmlChar*)"org_auth_info");
  rt_add_text_node(node, "key", key);
  rt_add_text_node(node, "type", type);
  if (!attr_checked) {
    child = xmlNewNode(NULL, (xmlChar*)"no_org");
    xmlAddChild(node, child);
  }
  else {
    rt_add_text_node(node, "attribute_checked", attr_checked);
    if (org_ok) {
      child = xmlNewNode(NULL, (xmlChar*)"org_auth_ok");
      xmlAddChild(node, child);
      rt_generate_list_map(child, "list", org_ok, (rt_map_function)rt_object_class);
    }
    if (org_fail) {
      child = xmlNewNode(NULL, (xmlChar*)"org_auth_fail");
      xmlAddChild(node, child);
      rt_generate_list_map(child, "list", org_fail, (rt_map_function)rt_object_class);
    }
  }

  rt_prepare_node(ctx, node);
}

/* final result of "org:" authorisation check */
void RT_org_auth_result(RT_context_t* ctx, gboolean result, gboolean override,
                                gboolean organisation_type_check_failed) {
  xmlNodePtr node;
  xmlNodePtr child;

  node = xmlNewNode(NULL, (xmlChar*)"org_auth_result");
  if (override) {
    rt_xml_set_prop(node, (xmlChar*)"override", (xmlChar*)"yes");
  }
  if (result) {
    child = xmlNewNode(NULL, (xmlChar*)"ok");
  }
  else {
    child = xmlNewNode(NULL, (xmlChar*)"fail");
  }
  xmlAddChild(node, child);

  if (organisation_type_check_failed) {
    child = xmlNewNode(NULL, (xmlChar*)"organisation_type_check_failed");
    xmlAddChild(node, child);
  }

  rt_prepare_node(ctx, node);
}

/* organisation object creation auth result */
void RT_org_creation_auth_result(RT_context_t* ctx, gboolean result, gboolean override) {
  xmlNodePtr node;
  xmlNodePtr child;

  node = xmlNewNode(NULL, (xmlChar*)"org_creation_auth_result");
  if (override) {
    rt_xml_set_prop(node, (xmlChar*)"override", (xmlChar*)"yes");
  }
  if (result) {
    child = xmlNewNode(NULL, (xmlChar*)"ok");
  }
  else {
    child = xmlNewNode(NULL, (xmlChar*)"fail");
  }
  xmlAddChild(node, child);


  rt_prepare_node(ctx, node);
}


/* auth check */
void RT_auth(RT_context_t* ctx, gchar* key, gchar* type, gchar* attr_checked,
        GList* auth_ok, GList* auth_fail, gchar *parent_text) {
  xmlNodePtr node;
  xmlNodePtr child;

  node = xmlNewNode(NULL, (xmlChar*)"auth_info");
  rt_add_text_node(node, "key", key);
  rt_add_text_node(node, "type", type);
//  rt_add_text_node(node, "type", rpsl_object_get_class(rt_get_object(ctx)));
  rt_add_text_node(node, "parent_text", parent_text);
  if (!attr_checked) {
    child = xmlNewNode(NULL, (xmlChar*)"no_maintainer");
    xmlAddChild(node, child);
  }
  else {
    rt_add_text_node(node, "attribute_checked", attr_checked);
    if (auth_ok) {
      child = xmlNewNode(NULL, (xmlChar*)"mnt_auth_ok");
      xmlAddChild(node, child);
      rt_generate_list_map(child, "list", auth_ok, (rt_map_function)rt_object_class);
    }
    if (auth_fail) {
      child = xmlNewNode(NULL, (xmlChar*)"mnt_auth_fail");
      xmlAddChild(node, child);
      rt_generate_list_map(child, "list", auth_fail, (rt_map_function)rt_object_class);
    }
  }

  rt_prepare_node(ctx, node);
}


/* auth check */
void RT_banned_operation(RT_context_t* ctx, gchar* type) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"banned_operation");
  rt_add_text_node(node, "type", type);

  rt_prepare_node(ctx, node);
}

/* final result */
void RT_auth_result(RT_context_t* ctx, gboolean result, gboolean override) {
  xmlNodePtr node;
  xmlNodePtr child;

  node = xmlNewNode(NULL, (xmlChar*)"auth_result");
  if (override) {
    rt_xml_set_prop(node, (xmlChar*)"override", (xmlChar*)"yes");
  }
  if (result) {
    child = xmlNewNode(NULL, (xmlChar*)"ok");
  }
  else {
    child = xmlNewNode(NULL, (xmlChar*)"fail");
  }
  xmlAddChild(node, child);
  rt_prepare_node(ctx, node);
}

/* final result */
void RT_update_result(RT_context_t* ctx, gchar* result) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"upd_result");
  rt_add_text_node(node, "result", result);

  rt_prepare_node(ctx, node);
}

/*
 * rdns messages
 */

void RT_rdns_message(RT_context_t* ctx, gchar *severity, gchar *message) {
  xmlNodePtr node;
  node = xmlNewNode(NULL, (xmlChar*)severity);
  rt_add_text_node(node, "", message);
  rt_prepare_node(ctx, node);
}

void RT_rdns_auth_result(RT_context_t* ctx, gboolean result, gboolean override) {
  xmlNodePtr node;
  xmlNodePtr child;
  xmlNodePtr childov;

  node = xmlNewNode(NULL, (xmlChar*)"rdns_auth_result");
  if (override) {
    rt_xml_set_prop(node, (xmlChar*)"override", (xmlChar*)"yes");
  }
  if (result) {
    child = xmlNewNode(NULL, (xmlChar*)"ok");
  }
  else {
    child = xmlNewNode(NULL, (xmlChar*)"fail");
  }
  xmlAddChild(node, child);
  if (override) {
    childov = xmlNewNode(NULL, (xmlChar*)"override");
    xmlAddChild(node, childov);
  }
  rt_prepare_node(ctx, node);
}

void RT_rdns_overlap(RT_context_t *ctx) {
  RT_rdns_message(ctx,"RDNSoverlap","");
}

void RT_rdns_trailingdotremoved(RT_context_t *ctx,gchar *domain) {
  RT_rdns_message(ctx,"RDNSremovedtrailingdot",domain);
}

void RT_rdns_rir(RT_context_t *ctx,gchar *message) {
  RT_rdns_message(ctx,"RDNSrir",message);
}

void RT_rdns_useotherrir(RT_context_t *ctx,gchar *message) {
  RT_rdns_message(ctx,"RDNSuseotherrir",message);
}

void RT_rdns_cannotregister(RT_context_t *ctx) {
  RT_rdns_message(ctx,"RDNScannotregister","");
}

void RT_rdns_nonservers(RT_context_t *ctx) {
  RT_rdns_message(ctx,"RDNSnonservers","");
}

void RT_rdns_parenthasourns(RT_context_t *ctx) {
  RT_rdns_message(ctx,"RDNSparenthasourns","");
}

void RT_rdns_parentisnotenum(RT_context_t *ctx) {
  RT_rdns_message(ctx,"RDNSparentisnotenum","");
}

void RT_rdns_notdelegated(RT_context_t *ctx) {
  RT_rdns_message(ctx,"RDNSnotdelegated","");
}

void RT_rdns_delcheckwarning(RT_context_t *ctx,gchar *message) {
  RT_rdns_message(ctx,"RDNSdelcheckwarning",message);
}
void RT_rdns_threshold(RT_context_t *ctx) {
  RT_rdns_message(ctx,"RDNSthreshold","");
}

void RT_rdns_invalid_range(RT_context_t *ctx,gchar *object_str) {

  xmlNodePtr node;
  node = xmlNewNode(NULL, (xmlChar*)"RDNSinvalidrange");
  rt_xml_node_add_content(node, (xmlChar*)object_str);
  rt_prepare_node(ctx, node);
}

void RT_status_check_failed_parentwithoutstatus(
    RT_context_t *ctx, gchar *parent, gchar *status) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"status_check_failed_parentwithoutstatus");
  rt_add_text_node(node, "parent", parent);
  rt_add_text_node(node, "status", status);
  rt_prepare_node(ctx, node);
}

void RT_invalid_grandparent_status(RT_context_t* ctx) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)"invalid_grandparent_status");

  rt_prepare_node(ctx, node);
}

static void RT_status_check_failed_message(RT_context_t *ctx,gchar *t) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*)t);
  rt_prepare_node(ctx, node);
}

void RT_status_check_failed_missingstatus(RT_context_t *ctx) {
  RT_status_check_failed_message(ctx,"status_check_failed_missingstatus");
}

void RT_status_check_failed_earlyregistration(RT_context_t *ctx) {
  RT_status_check_failed_message(ctx,"status_check_failed_earlyregistration");
}

void RT_status_check_failed_notset(RT_context_t *ctx) {
  RT_status_check_failed_message(ctx,"status_check_failed_notset");
}

void RT_status_check_failed_allocbyrir(RT_context_t *ctx) {
  RT_status_check_failed_message(ctx,"status_check_failed_allocbyrir");
}

void RT_status_check_failed_allocated(RT_context_t *ctx) {
  RT_status_check_failed_message(ctx,"status_check_failed_allocated");
}

void RT_status_check_failed_modify(RT_context_t *ctx, char* status) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*) "status_check_failed_modify");
  rt_add_text_node(node, "status", status);
  rt_prepare_node(ctx, node);
}

void RT_status_check_failed_anycast_rir(RT_context_t *ctx) {
  RT_status_check_failed_message(ctx,"status_check_failed_anycast_rir");
}

void RT_status_check_failed_rir(RT_context_t *ctx, char* status) {
  xmlNodePtr node;

  node = xmlNewNode(NULL, (xmlChar*) "status_check_failed_rir");
  rt_add_text_node(node, "status", status);
  rt_prepare_node(ctx, node);
}
