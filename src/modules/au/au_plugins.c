#include "au_plugins.h"

/**********************************************************************
  Each plug-in provides an initialisation and a check function.
 **********************************************************************/

/* RPSL prototypes */
gboolean au_rpsl_init();
PG_status_t au_rpsl_check(PG_transaction_t *trans, gpointer *info);

/* RIPE prototypes */
gboolean au_ripe_init();
PG_status_t au_ripe_check(PG_transaction_t *trans, gpointer *info);

/* irt object prototypes */
gboolean au_irt_init();
PG_status_t au_irt_check(PG_transaction_t *trans, gpointer *info);

/* organisation object prototypes */
gboolean au_organisation_init();
PG_status_t au_organisation_check (PG_transaction_t *trans, gpointer *info);

/* rdns related prototypes */
#ifdef RDNSYES
gboolean au_rdns_init();
PG_status_t au_rdns_check(PG_transaction_t *trans, gpointer *info);
#endif

const au_plugin_t au_plugins[] = {
#ifdef RDNSYES
  { "rdns", au_rdns_init, au_rdns_check },
#endif
  { "rpsl", au_rpsl_init, au_rpsl_check },
  { "ripe", au_ripe_init, au_ripe_check },
  { "irt", au_irt_init, au_irt_check },
  { "organisation", au_organisation_init, au_organisation_check },
  { NULL }   /* end of list */
};

/**********************************************************************/

static const au_check_by_type_t *find_type(const gchar *object_type, 
                                           const au_check_by_type_t *type_table)
{
  int i;

  /* find appropriate callbacks */
  for (i=0; type_table[i].object_type != NULL; i++) 
  {
    if (strcmp(type_table[i].object_type, "*") == 0) 
    {
        return type_table + i;
    }
    if (strcmp(type_table[i].object_type, object_type) == 0) 
    {
        return type_table + i;
    }
  }
  return NULL;
}

AU_ret_t AU_check_by_type (const au_check_by_type_t *type_table,
                           const au_plugin_callback_info_t *info)
{
    const au_check_by_type_t *type_info;
    AU_ret_t ret_val;

    LG_log(au_context, LG_FUNC, ">AU_check_by_type: entering");

    type_info = find_type(rpsl_object_get_class(info->obj), type_table);
    if (type_info == NULL) 
    {
      ret_val = AU_AUTHORISED;
    }
    else 
    {
      switch (info->type) 
      {
        case AU_CREATE:
          ret_val = (type_info->check_create)(info);
          break;
        case AU_MODIFY:
          ret_val = (type_info->check_modify)(info);
          break;
        case AU_DELETE:
          ret_val = (type_info->check_delete)(info);
          break;
        default:
          LG_log(au_context, LG_ERROR, "AU_check_by_type: unrecognised type %d",
            info->type);
          /* XXX: RT */
          ret_val = AU_ERROR;
      }
    }

    LG_log(au_context, LG_FUNC, "<AU_check_by_type: exiting with value [%s]",
           AU_ret2str(ret_val));
    return ret_val;
}


