#ifndef READ_DEFS
#define READ_DEFS

/***************************************
  $Revision: 1.1 $

  Objects module (ob) - this _should_ eventually get merged in with the
  config module.

  Status: NOT REVUED, NOT TESTED

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
#include <glib.h>
#include "bitmask.h"

#include "DF_class_enum.def"
#include "DF_class_mask.def"

#include "DF_attribute_enum.def"
#include "DF_attribute_inv_attr_mask.def"

typedef enum _R_Type_t {
  R_SQL=0,
  R_RADIX,
  R_END
} R_Type_t;

typedef enum _Q_Type_t {
  Q_LOOKUP=0,
  Q_INVERSE
} Q_Type_t;

#include "which_keytypes.h"
#include "iproutines.h"
#include "rxroutines.h"

typedef struct {
  R_Type_t refer;
  Q_Type_t querytype;
  WK_Type keytype;
  A_Type_t attribute;
  C_Type_t class;
  char * const query;
  char * const descr;
} Query_t;

#ifdef DEFS_IMPL
#include "QI_queries.def"
#else
extern Query_t Query[];
#endif


typedef struct {
  A_Type_t attr;
  rx_fam_t family;
  char *   ipv4_load;
  char *   ipv6_load;
} DF_Load_t;
#ifdef DEFS_IMPL
#include "DF_radix_load.def"
#else
extern  DF_Load_t DF_radix_load[];
#endif



typedef enum UD_qtype_t {
  UD_AUX__,
  UD_AX_PR,
  UD_AX_MT,
  UD_AX_MO,
  UD_AX_MR,
  UD_AX_MU,
  UD_AX_IT,
  UD_AX_OA,
  UD_AX_M6,
  UD_AX_FR,
  UD_AX_PF,
  UD_LEAF_,
  UD_LF_IF,
  UD_LF_RF,
  UD_LF_AT,
  UD_LF_OT,
  UD_LF_AY,
  UD_LF_IE,
  UD_MAIN_,
  UD_MA_RT,
  UD_MA_OR,
  UD_MA_PR,
  UD_MA_U2,
  UD_MA_IN,
  UD_MA_I6,
  UD_MA_AK,
  UD_MA_R6,
  UD_NULL_
} UD_qtype;

typedef struct UD_query_t {
  UD_qtype qtype;
  char * const qry;
} UD_query;

/* take nothing, return array of strings */
char * const *DF_get_filter_names(void);
char * const *DF_get_class_names(void);
char * const *DF_get_class_aliases(void);
char * const *DF_get_attribute_aliases(void);
char * const *DF_get_attribute_names(void);


/* take class type enum, return the corresponding string  */
char * const DF_get_class_sql_table(C_Type_t index);
const char *DF_get_class_template(C_Type_t index);
const char *DF_get_class_template_v(C_Type_t index);
const char *DF_class_type2name(C_Type_t index);
char * const DF_get_class_code(C_Type_t index);

/* take index to class alias array */
int DF_get_class_index(int alias_index);

/* index to attribute alias array */
int DF_get_attribute_index(int alias_index);

/* take attribute type enum, return the corresponding string  */
const char *DF_get_attribute_name(A_Type_t index);
const char *DF_get_attribute_code(A_Type_t index);

/* take attribute type enum, return UD related data: */
	/* return the corresponding UD query string */
const char *DF_get_update_query(A_Type_t index);
const char *DF_get_select_query(A_Type_t index);
const char *DF_get_insert_query(A_Type_t index);
const char *DF_get_dummy_query(A_Type_t index);
	/* return the corresponding UD query type  */
UD_qtype DF_get_update_query_type(A_Type_t index);
UD_qtype DF_get_insert_query_type(A_Type_t index);
UD_qtype DF_get_select_query_type(A_Type_t index);
UD_qtype DF_get_dummy_query_type(A_Type_t index);

/* take attribute type enum, return radix related data: */
	/* return boolean if attribute has a radix tree */
int DF_attrcode_has_radix_lookup(A_Type_t attr);
	/* return radix family */
rx_fam_t DF_attrcode_radix_family(A_Type_t attr);
	/* return query string to load the v4/v6 radix tree*/
char * DF_attrcode_radix_load_v4(A_Type_t attr);
char * DF_attrcode_radix_load_v6(A_Type_t attr);

/* take string, return corresponding enum code */
A_Type_t DF_attribute_code2type(const gchar *token);
A_Type_t DF_attribute_name2type (const gchar *token);
C_Type_t DF_class_name2type(const gchar *token);

#endif /* READ_DEFS */

