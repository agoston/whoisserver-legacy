#include <rip.h>

GList* svrs;
GList* srcs;

void KM_lu_init(GList* servers, GList* sources)
{
  svrs = servers;
  srcs = sources;
}

/* Lookup key-cert object from any of the multiple sources/servers.
   If found then get the certif data from this key-cert object.
   Receives key  key id
   Returns  certif data from this key-cert object
            or NULL if no object found
            or LU_ERROR for lookup error.
*/

gchar* KM_lu_get(gchar* key)
{
  /* key  - including prefix  */
  LU_ret_t lu_ret;
  rpsl_object_t* obj = NULL;
  GString* str;
  GList* attr_list;
  GList* loop;
  gchar* line;
  GList* sources;
  GList* servers;

  sources = srcs;
  servers = svrs;
  /* Do a lookup on all sources/servers */
  while (sources && servers)
  {
    lu_ret = LU_lookup ((LU_server_t*)servers->data, &obj, "key-cert",
                           key, (gchar*)sources->data);
    if (lu_ret == LU_ERROR)
    {
      /* should return LU_ERROR, but calling function cannot handle that */
      return NULL;   
    }
    if (obj != NULL)
    {
      break;
    }
    servers = servers->next;
    sources = sources->next;
  }

  if (obj == NULL)
  {
    return NULL;
  }

  /* get the certif data */
  attr_list = rpsl_object_get_attr(obj, "certif");
  loop  = attr_list;
  str = g_string_new("");

  while(loop)
  {
    line = rpsl_attr_get_clean_lines((const rpsl_attr_t*)loop->data);
    g_string_append(str, line);
    g_string_append_c(str, '\n');
    loop = loop->next;
  }
  line = str->str;
  g_string_free(str, FALSE);
  rpsl_attr_delete_list(attr_list);
  return line;
}

/* Lookup key-cert object from any of the multiple sources/servers
   using an inverse lookup.
   Receives key            key id
            key_cert_list  location of pointer to list of objects
   Returns  LU_OKAY 
            LU_ERROR
*/

LU_ret_t KM_lu_inverse_lookup(const gchar *key, GList **key_cert_list)
{
  LU_ret_t lu_ret;
  GList *types = NULL;
  GList *inv_keys = NULL;
  GList* sources;
  GList* servers;
  GList *key_cert_objects = NULL;

  *key_cert_list = NULL;
  sources = srcs;
  servers = svrs;
  types = g_list_append(types, "key-cert");
  inv_keys = g_list_append(inv_keys, "fingerpr");

  /* Do an inverse lookup on all sources/servers */
  while (sources && servers)
  {
    lu_ret = LU_inverse_lookup((LU_server_t*)servers->data, &key_cert_objects, 
                                types, key, inv_keys, (gchar*)sources->data);
    if (lu_ret == LU_ERROR)
    {
      return lu_ret;   
    }
    *key_cert_list = g_list_concat(*key_cert_list, key_cert_objects);
    servers = servers->next;
    sources = sources->next;
  }

  return LU_OKAY;
}
