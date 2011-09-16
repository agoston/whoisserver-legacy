#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>

#include "which_keytypes.h"
#include "lg.h"
#include "memwrap.h"
#include "lu_whois.h"
#include "ut_string.h"

/* XXX: no "-k" flag */

#undef MAX
#undef MIN
#include "sk.h"

/* the logging context is defined in lu.c */
extern LG_context_t *lu_context;

const char * const LU_ret_to_string[] = {
        "LU_ERROR",
        "LU_INVARG",
        "LU_OKAY"
};

const char *LU_ret2str(int ret) {
    if (ret < 0 || ret > 2) return NULL;
    return LU_ret_to_string[ret];
}
/*
  Cleans up a WHOIS lookup connection

  server   - WHOIS server connection

  Frees memory and closes the socket, if open.
 */
void
lu_whois_cleanup (LU_server_t *server)
{
  LU_whois_info_t *whois_info;

  whois_info = (LU_whois_info_t *)server->info;
  if (whois_info->socket >= 0)
  {
    SK_close(whois_info->socket);
    whois_info->socket = -1;
  }
  /* note that g_free sets the pointer passed to NULL */
  g_free(whois_info->hostname);
  g_free(whois_info);
  g_free(server);
}


/*
  Perform a WHOIS lookup.

  whois_info   - structure with information about the WHOIS server
  query        - text of query to send
  objects      - used to return the list of objects from the server

  return       - TRUE on success, FALSE on error

  Errors include communication problems and server error replies.
  Server, port, timeout are from the info structure.

  Entire reply read into memory, so be careful!  Malicious servers could
  cause problems, as could clients with little RAM.

  Logging perfomed via LG module.
 */
static gboolean
lu_whois_query (LU_whois_info_t *whois_info, const gchar *query,
                GList **objects)
{
  int connect_timeout;
  int sk_err;
  gchar *tmp;
  gchar *fixed_query;
  struct timeval puts_timeout;
  int puts_ret;

  struct pollfd pollfd;
  int read_timeout;
  char reply_buf[4096];
  int read_ret;
  GString *reply;

  gchar **split_reply;
  int i;

  rpsl_object_t *object;
  int tmp_error_code;
  int error_code;

  LG_log(lu_context, LG_FUNC, ">lu_whois_query: entered");

  LG_log(lu_context, LG_DEBUG, "lu_whois_query: query is \"%s\"", query);

  /* initialise return values */
  *objects = NULL;

  /* connect if not already connected */
  if (whois_info->socket < 0)
  {
    if (whois_info->timeout == 0)
    {
      connect_timeout = 86400;  /* SK_connect doesn't have "unlimited" */
    }
    else
    {
      connect_timeout = whois_info->timeout;
    }
    sk_err = SK_connect(&whois_info->socket,
                        whois_info->hostname,
                        whois_info->port,
                        connect_timeout);
    if (sk_err != SK_OK)
    {
      LG_log(lu_context, LG_WARN, "lu_whois_query: error connecting to WHOIS server");
      LG_log(lu_context, LG_FUNC, "<lu_whois_query: exiting with value [FALSE]");
      return FALSE;
    }
  }

  /* clean and send query */
  tmp = g_strdup(query);
  g_strstrip(tmp);
  fixed_query = g_strconcat(tmp, "\r\n", NULL);
  if (whois_info->timeout == 0)
  {
    puts_ret = SK_puts(whois_info->socket, fixed_query, NULL);
  }
  else
  {
    puts_timeout.tv_sec = whois_info->timeout;
    puts_timeout.tv_usec = 0;
    puts_ret = SK_puts(whois_info->socket, fixed_query, &puts_timeout);
  }
  g_free(fixed_query);
  g_free(tmp);

  /* handle error with send */
  if (puts_ret == -1)
  {
    SK_close(whois_info->socket);
    whois_info->socket = -1;
    LG_log(lu_context, LG_WARN, "lu_whois_query: error sending to WHOIS server");
    LG_log(lu_context, LG_FUNC, "<lu_whois_query: exiting with value [FALSE]");
    return FALSE;
  }

  /* read any reply back into a buffer */
  reply = g_string_sized_new(2048);
  pollfd.fd = whois_info->socket;
  pollfd.events = POLLIN;
  read_timeout = whois_info->timeout * 1000;
  if (read_timeout == 0)
  {
    read_timeout = -1;
  }
  for (;;)
  {
    if (poll(&pollfd, 1, read_timeout) == -1)
    {
      SK_close(whois_info->socket);
      whois_info->socket = -1;
      g_string_free(reply, TRUE);
      LG_log(lu_context, LG_WARN, "lu_whois_query: error polling WHOIS server");
      LG_log(lu_context, LG_FUNC, "<lu_whois_query: exiting with value [FALSE]");
      return FALSE;
    }
    if (!pollfd.revents)
    {
      SK_close(whois_info->socket);
      whois_info->socket = -1;
      g_string_free(reply, TRUE);
      LG_log(lu_context, LG_WARN, "lu_whois_query: timeout from WHOIS server");
      LG_log(lu_context, LG_FUNC, "<lu_whois_query: exiting with value [FALSE]");
      return FALSE;
    }
    read_ret = read(whois_info->socket, reply_buf, sizeof(reply_buf)-1);
    if (read_ret == -1)
    {
      SK_close(whois_info->socket);
      whois_info->socket = -1;
      g_string_free(reply, TRUE);
      LG_log(lu_context, LG_WARN, "lu_whois_query: error reading timeout from WHOIS server");
      LG_log(lu_context, LG_FUNC, "<lu_whois_query: exiting with value [FALSE]");
      return FALSE;
    }
    if (read_ret == 0)
    {
      break;
    }
    reply_buf[read_ret] = '\0';
    g_string_append(reply, reply_buf);
  }

  /* XXX: close for now (we don't believe in "-k" yet) */
  SK_close(whois_info->socket);
  whois_info->socket = -1;

  /* convert to objects result */
  LG_log(lu_context, LG_DEBUG, "lu_whois_query: got result [%s]", reply->str);
  split_reply = (gchar**)g_strsplit(reply->str, "\n\n", 0);
  g_string_free(reply, TRUE);

  error_code = 0;
  for (i=0; split_reply[i] != NULL; i++)
  {
    /* skip comments */
    if (split_reply[i][0] == '%')
    {
      /* if this comment is an error code, get the value */
      if (strncmp(split_reply[i], "%ERROR:", 7) == 0)
      {
        tmp_error_code = atoi(split_reply[i]+7);
        /* error 101 is "no objects found", which isn't really an error */
        if (tmp_error_code != 101)
        {
          LG_log(lu_context, LG_WARN, "lu_whois_query: server returned code %d",
                 tmp_error_code);
          error_code = tmp_error_code;
        }
      }
      continue;
    }

    /* clean trailing whitespace and skip if nothing left (i.e. last object) */
    g_strchomp(split_reply[i]);
    if (split_reply[i][0] == '\0')
    {
      continue;
    }

    /* anything else should be an object, so add object to list */
    object = rpsl_object_init(split_reply[i]);
    if (object == NULL)
    {
      LG_log(lu_context, LG_WARN, "lu_whois_query: error parsing object returned by WHOIS");
    }
    else
    {
      *objects = g_list_prepend(*objects, object);
    }
  }
  g_strfreev(split_reply);

  if (error_code == 0)
  {
    LG_log(lu_context, LG_FUNC, "<lu_whois_query: exiting with value [TRUE]");
    return TRUE;
  }
  else
  {
    LG_log(lu_context, LG_FUNC, "<lu_whois_query: exiting with value [FALSE]");
    return FALSE;
  }
}

/*
  Log object found

  obj      - object
 */
static void
lu_log_object (rpsl_object_t *obj)
{
  const GList *attr;
  const GList *p;

  attr = rpsl_object_get_all_attr(obj);
  for (p=attr; p != NULL; p = g_list_next(p))
  {
    LG_log(lu_context, LG_DEBUG, "%s:%s", rpsl_attr_get_name(p->data),
                                          rpsl_attr_get_value(p->data));
  }
}

/*
  Lookup a single object via WHOIS

  server   - connection information
  obj      - used to return object, if any
  type     - class of object, e.g. "person", "inetnum"
  key      - lookup key, e.g. "NC123-RIPE" or "193.0.1.0/24AS3333"
  source   - source, e.g. "RIPE"

  return       - LU_OKAY on success, LU_ERROR on error

  Errors can come from lu_whois_query(), or if there are duplicate
  key values (should be impossible).

  Logging perfomed via LG module.
 */
LU_ret_t
lu_whois_lookup (LU_server_t *server, rpsl_object_t **obj,
                 const gchar *type, const gchar *key, const gchar *source)
{
  gchar *query;
  gchar *key_copy;
  gchar *end_of_key;
  gboolean query_ret;
  GList *tmp_obj;

  GList *p;
  rpsl_object_t *object;
  gchar *object_key;

  LU_ret_t ret_val;

  int num_query_attempts;

  LG_log(lu_context, LG_FUNC, ">lu_whois_lookup: entered");

  LG_log(lu_context, LG_DEBUG, "lu_whois_lookup: type=\"%s\", key=\"%s\", source=\"%s\"",
         type, key, source);

  /* special case route & route6 , because they have a composite key */
  if (!strcasecmp(type, "route") || !strcasecmp(type, "route6"))
  {
    /* route keys look like this: "1.2.3.4/5AS6", but we cannot
       perform a lookup on this, so we turn this into "1.2.3.4/5" for
       the query and filter out any prefixes that do not have the correct
       AS number */
    key_copy = g_strdup(key);
    g_strup(key_copy);
    end_of_key = strstr(key_copy, "AS");
    if (end_of_key != NULL)
    {
      *end_of_key = '\0';
    }
    query = g_strdup_printf("-G -B -s %s -r -x -T %s %s", source, type, key_copy);
    g_free(key_copy);
  }

  /* also special case inetnum and inet6num, since we want the -x flag */
  else if (!strcasecmp(type, "inetnum") || !strcasecmp(type, "inet6num"))
  {
    query = g_strdup_printf("-G -B -s %s -r -x -T %s %s", source, type, key);
  }
  /* also special case domain, since we want the -R flag */
  else if (!strcasecmp(type, "domain"))
  {
    query = g_strdup_printf("-G -B -s %s -r -R -T %s %s", source, type, key);
  }
  else
  {
    query = g_strdup_printf("-G -B -s %s -r -T %s %s", source, type, key);
  }

  /* XXX: There is a bug in the core server that causes a WHOIS query to
          occasionally return the same object multiple times.  I think this
          is related to the server not being strict about locking access
          on reads.  This will be fixed at a later time, but for now, try
          the query again, and see if we still get two objects.
   */
  num_query_attempts = 0;
  do
  {
    /* query server for objects */
    query_ret = lu_whois_query(server->info, query, &tmp_obj);
    if (!query_ret)
    {
      LG_log(lu_context, LG_FUNC, "<lu_whois_lookup: exiting with value [%s]",
             LU_ret2str(LU_ERROR));
      g_free(query);
      return LU_ERROR;
    }
    num_query_attempts++;

    /* filter out the objects that don't have the key */
    /* we have to do this, because certain object types can return multiple
       objects even if we specify the key, for example if you want to find
       the person object with the "nic-hdl: MIKE" */
    /* see also the comment on routes, above */
    *obj = NULL;
    ret_val = LU_OKAY;
    LG_log(lu_context, LG_DEBUG, "lu_whois_lookup: lookup key is \"%s\"", key);
    for (p = tmp_obj; p != NULL; p = g_list_next(p))
    {
      object = p->data;
      object_key = rpsl_object_get_key_value(object);
      LG_log(lu_context, LG_DEBUG, "lu_whois_lookup: object key is \"%s\"", object_key);
      if (strcasecmp(object_key, key) == 0)
      {
        if (*obj != NULL)
        {
          LG_log(lu_context, LG_ERROR, "lu_whois_lookup: duplicate key \"%s\" found", key);
          rpsl_object_delete(*obj);
          ret_val = LU_ERROR;
        }
        *obj = object;
      }
      else
      {
        rpsl_object_delete(object);
      }
      UT_free(object_key);
    }
    g_list_free(tmp_obj);

    /* free resources on error */
    if (ret_val == LU_ERROR)
    {
      rpsl_object_delete(*obj);
      *obj = NULL;
    }

  }
  while ((ret_val == LU_ERROR) && (num_query_attempts < 3));

  /* free query memory */
  g_free(query);

  /* log object */
  if (*obj != NULL)
  {
    LG_log(lu_context, LG_DEBUG, "lu_whois_lookup: object returned");
    LG_log(lu_context, LG_DEBUG, "------------------------------------------");
    lu_log_object(*obj);
    LG_log(lu_context, LG_DEBUG, "------------------------------------------");
  }
  else
  {
    LG_log(lu_context, LG_DEBUG, "lu_whois_lookup: object is NULL");
  }

  /* return result */
  LG_log(lu_context, LG_FUNC, "<lu_whois_lookup: exiting with value [%s]",
         LU_ret2str(ret_val));
  return ret_val;
}

GString* glist_to_string (GList *list, GString *string) {
GList *p;

 p = list;
 if (p != NULL)
 { /* list is defined, prepare the string */
   g_string_append(string, p->data);
   for (p=g_list_next(list); p != NULL; p = g_list_next(p))
   {
     g_string_append(string, ",");
     g_string_append(string, p->data);
   }
 }
 else {
   g_string_free(string, TRUE);
   string = NULL;
 }
 return string;
}

/*
  Inverse lookup a list of objects via WHOIS

  server   - connection information
  objects  - used to return a list of objects, if any
  types    - list of objects types to be returned, e.g. "person", "inetnum", may be NULL
  key      - lookup key, e.g. "NC123-RIPE" or "193.0.1.0/24AS3333"
  inverse_keys - list of inverse keys to be looked up, NOT NULL
  source   - source, e.g. "RIPE"

  return       - LU_OKAY on success, LU_ERROR on error

  Errors can come from lu_whois_query(), or if list of inverse_keys is not specified

  Logging perfomed via LG module.
 */

LU_ret_t
lu_whois_inverse_lookup (LU_server_t *server, GList **objects,
                 GList *types, const gchar *key, GList *inverse_keys, const gchar *source)
{
GList *p;
GString *types_string;
GString *inverse_keys_string;
gchar* query;
LU_ret_t ret_val;
int num_query_attempts;
gboolean query_ret;

 LG_log(lu_context, LG_FUNC, ">lu_whois_inverse_lookup: entered");

 types_string = g_string_sized_new(MAX_QUERY_LENGTH);
 inverse_keys_string = g_string_sized_new(MAX_QUERY_LENGTH);

 types_string = glist_to_string(types,types_string);
 inverse_keys_string = glist_to_string(inverse_keys,inverse_keys_string);

 if (inverse_keys_string == NULL)
 {
   LG_log(lu_context, LG_FUNC, "lu_whois_inverse_lookup: no inverse keys specified");
   LG_log(lu_context, LG_FUNC, "<lu_whois_inverse_lookup: exiting with value [%s]", LU_ret2str(LU_ERROR));
   if (types_string != NULL)
   {
     g_string_free(types_string,TRUE);
   }
   return LU_ERROR;
 }

 /* compose the query */
 if (types_string != NULL)
 {
   LG_log(lu_context, LG_DEBUG, "lu_whois_inverse_lookup: types=\"%s\", key=\"%s\", \
                                 inverse_keys=\"%s\", source=\"%s\"",
                                 types_string->str, key, inverse_keys_string->str, source);
   query = g_strdup_printf("-G -B -s %s -r -i %s -T %s %s", source, inverse_keys_string->str, types_string->str, key);
 }
 else
 {
   LG_log(lu_context, LG_DEBUG, "lu_whois_inverse_lookup: types=all, key=\"%s\", \
                                 inverse_keys=\"%s\", source=\"%s\"",
                                 key, inverse_keys_string->str, source);
   query = g_strdup_printf("-G -B -s %s -r -i %s %s", source, inverse_keys_string->str, key);
 }

 /* free the temporary strings */
 if (types_string != NULL ) g_string_free(types_string,TRUE);
 g_string_free(inverse_keys_string,TRUE);

 /* XXX: There is a bug in the core server that causes a WHOIS query to
          occasionally return the same object multiple times.  I think this
          is related to the server not being strict about locking access
          on reads.  This will be fixed at a later time, but for now, try
          the query again, and see if we still get two objects.
 */
 num_query_attempts = 0;
 ret_val = LU_OKAY;
 *objects = NULL;

 do
  {
    /* query server for objects */
    query_ret = lu_whois_query(server->info, query, objects);
    if (!query_ret)
    {
      LG_log(lu_context, LG_FUNC, "<lu_whois_inverse_lookup: exiting with value [%s]",
             LU_ret2str(LU_ERROR));
      g_free(query);
      return LU_ERROR;
    }
    num_query_attempts++;
  }
  while ((ret_val == LU_ERROR) && (num_query_attempts < 3));

  /* free query memory */
  g_free(query);

  /* log objects */
  if (*objects != NULL)
  {
   LG_log(lu_context, LG_DEBUG, "lu_whois_inverse_lookup: object(s) returned");
   LG_log(lu_context, LG_DEBUG, "------------------------------------------");
   for (p=*objects; p != NULL; p = g_list_next(p))
    {
      lu_log_object(p->data);
      LG_log(lu_context, LG_DEBUG, "------------------------------------------");
    }
  }
  else
  {
    LG_log(lu_context, LG_DEBUG, "lu_whois_inverse_lookup: object(s) is NULL");
  }

  /* return result */
  LG_log(lu_context, LG_FUNC, "<lu_whois_inverse_lookup: exiting with value [%s]",
         LU_ret2str(ret_val));
  return ret_val;

}



/*
  Lookup a current version of an object via WHOIS

  server     - connection information
  dbase_obj  - used to return object, if any
  obj        - object to look up
  source     - source, e.g. "RIPE", or NULL to get from obj

  return       - LU_OKAY on success, LU_ERROR on error

  Errors can come from lu_whois_lookup(), or if source is NULL and
  there is no "source:" in the object.

  Logging perfomed via LG module.
 */
LU_ret_t
lu_whois_get_object (LU_server_t *server, rpsl_object_t **dbase_obj,
                     const rpsl_object_t *obj, const gchar *source)
{
  const gchar *class;
  gchar *key;
  GList *source_attr;
  gchar *lookup_source;
  LU_ret_t ret_val;

  LG_log(lu_context, LG_FUNC, ">lu_whois_get_object: entered");

  /* get information */
  if (source == NULL)
  {
    source_attr = rpsl_object_get_attr(obj, "source");
    if (g_list_length(source_attr) != 1)
    {
      /* lg(error, missing source) */
      rpsl_attr_delete_list(source_attr);
      LG_log(lu_context, LG_FUNC, "<lu_whois_get_object: exiting with value [%s]",
             LU_ret2str(LU_ERROR));
      return LU_ERROR;
    }
    lookup_source = rpsl_attr_get_clean_value(source_attr->data);
  }
  else
  {
    source_attr = NULL;
    lookup_source = UT_strdup(source);
  }
  class = rpsl_object_get_class(obj);
  key = rpsl_object_get_key_value(obj);

  /* preform lookup */
  ret_val = lu_whois_lookup(server, dbase_obj, class, key, lookup_source);

  /* cleanup */
  rpsl_attr_delete_list(source_attr);
  g_free(key);
  UT_free(lookup_source);

  /* return */
  LG_log(lu_context, LG_FUNC, "<lu_whois_get_object: exiting with value [%s]",
         LU_ret2str(ret_val));
  return ret_val;
}

/*
  determine if the class allows hierarchical naming

  class      - name of class, e.g. "person", "as-set"

  return     - TRUE or FALSE

  XXX: possibly bogus to hard-code this.  Add to syntax_api?
 */
static gboolean
has_hierarchical_name (const gchar *class)
{
  if (strcasecmp(class, "as-set") == 0) return TRUE;
  if (strcasecmp(class, "filter-set") == 0) return TRUE;
  if (strcasecmp(class, "peering-set") == 0) return TRUE;
  if (strcasecmp(class, "route-set") == 0) return TRUE;
  if (strcasecmp(class, "rtr-set") == 0) return TRUE;
  return FALSE;
}

/* this function converts the ip ranges from two RPSL objects  *
 * (assuming inetnum) to their binary representation           *
 * and checks if they are overlapping *
 * true if overlap, false otherwise *
*/
int is_overlap (const rpsl_object_t *obj1, rpsl_object_t *obj2)
{
  ip_range_t range1;
  ip_range_t range2;

  IP_rang_e2b(&range1, rpsl_object_get_key_value(obj1));
  IP_rang_e2b(&range2, rpsl_object_get_key_value(obj2));

  return (IP_rang_overlap(&range1, &range2));

}

/* this function checks if object from the list *list  *
   overlaps with inetnum  *obj,                        *
   and adds it to the *overlap list in this case.      *
*/
GList **check_and_append(GList **overlap, GList *list, const rpsl_object_t *obj)
{
  GList *p;
  for (p=list; p != NULL; p = g_list_next(p))
  {
    if (is_overlap(obj, p->data))
    {
      LG_log(lu_context, LG_DEBUG, "is overlap");
      *overlap = g_list_append (*overlap, (rpsl_object_t *) p->data);
    }
    else {
       LG_log(lu_context, LG_DEBUG, "is not overlap");
    }
  }
  return overlap;
}

/*
  Check if there are overlaps for this object (supposed for inetnum only)
  dbupdate should pass only "inetnum" object to this function

  server     - connection information
  overlap    - used to return overlaps, if any
  obj        - object to look up overlaps of
  source     - source, e.g. "RIPE", or NULL to get from obj

  return       - LU_OKAY on success, LU_ERROR on error

  Errors can come from lu_whois_query(), or if source is NULL and
  there is no "source:" in the object.

  Logging perfomed via LG module.
 */


LU_ret_t
lu_whois_check_overlap (LU_server_t *server, GList **overlap,
                      const rpsl_object_t *obj, const gchar *source)
{
  LU_ret_t ret_val;
  GList *start_parents = NULL;
  GList *end_parents = NULL;
  GList *source_attr;
  GList *p;
  gchar *lookup_source;
  gchar *query;
  const gchar *class;

  gchar *range = NULL;
  gchar **split = NULL;

  LG_log(lu_context, LG_FUNC, ">lu_whois_check_overlap: entered");

  /* default values */
  ret_val = LU_OKAY;
  query = NULL;
  *overlap = NULL;

  /* extract information */
  if (source == NULL)
  {
    source_attr = rpsl_object_get_attr(obj, "source");
    if (g_list_length(source_attr) != 1)
    {
      /* lg(error, missing source) */
      rpsl_attr_delete_list(source_attr);
      LG_log(lu_context, LG_FUNC, "<lu_whois_check_overlap: exiting with value [%s]",
             LU_ret2str(LU_ERROR));
      return LU_ERROR;
    }
    lookup_source = rpsl_attr_get_clean_value(source_attr->data);
  }
  else
  {
    source_attr = NULL;
    lookup_source = UT_strdup(source);
  }
  class = rpsl_object_get_class(obj);

  /* construct new ranges */
  range = rpsl_object_get_key_value(obj);
  split = ut_g_strsplit_v1(range, " - ", 0);

  /* query for overlap candidates */
  query = g_strdup_printf("-G -B -s %s -r -T %s -L %s", lookup_source, class, split[0]);
  if (! lu_whois_query(server->info, query, &start_parents))
  {
    ret_val = LU_ERROR;
  }
  g_free(query);

  /* if start ip != end ip, query end ip */
  if (strncasecmp(split[0], split[1],strlen(split[0])) != 0)
  {
    query = g_strdup_printf("-G -B -s %s -r -T %s -L %s", lookup_source, class, split[1]);
    if (! lu_whois_query(server->info, query, &end_parents))
    {
      ret_val = LU_ERROR;
    }
    g_free(query);
  }

  if ( ret_val == LU_OKAY)
  {
    /* clean all non-overlaps from the list */
    overlap = check_and_append(overlap, start_parents, obj);
    overlap = check_and_append(overlap, end_parents, obj);

    /* log objects */
    if (*overlap != NULL)
    {
      LG_log(lu_context, LG_DEBUG, "lu_whois_check_overlap: object(s) returned");
      LG_log(lu_context, LG_DEBUG, "------------------------------------------");
      for (p=*overlap; p != NULL; p = g_list_next(p))
      {
        lu_log_object(p->data);
        LG_log(lu_context, LG_DEBUG, "------------------------------------------");
      }
    }
    else
    {
      LG_log(lu_context, LG_DEBUG, "lu_whois_check_overlap: object(s) is NULL");
    }
  }

  LG_log(lu_context, LG_FUNC, "<lu_whois_check_overlap: exiting with value [%s]",
                       LU_ret2str(ret_val));
  g_free(range);
  g_strfreev(split);
  UT_free(lookup_source);
  return ret_val;

}

/*
 * Lookup the corresponding inetnum
 *
 * server    - connection information
 * domain    - reverse domain string
 * source    - update source
 * result    - resulting rpsl object list
 */
LU_ret_t LU_get_inetnum_from_domain(LU_server_t *server, gchar *domain, gchar *source, GList **result) {
    gint ret_val;
    ip_revd_t revd;
    gchar inetnum_str[300];
    GList *query_result;
    gchar *query;
    gboolean query_ret;

    /* find the corresponding inetnum object */
    LG_log(lu_context, LG_FUNC, ">Entering LU_get_inetnum_from_domain");
    LG_log(lu_context, LG_FUNC, "domain is %s", domain);
    LG_log(lu_context, LG_FUNC, "source is %s", source);
    if (IP_revd_t2b(&revd, domain) == IP_OK) {
        if (IP_revd_b2t_prefrang(&revd, inetnum_str, 255) == IP_OK) {
            LG_log(lu_context, LG_FUNC, "inetnum is %s", inetnum_str);
            query = g_strdup_printf("-C -G -B -Tinetnum,inet6num -s %s -r %s", source, inetnum_str);
            query_ret = lu_whois_query(server->info, query, &query_result);
            g_free(query);
            if (!query_ret) {
                ret_val = LU_ERROR;
            } else {
                *result = query_result;
                ret_val = LU_OKAY;
            }
        } else {
            LG_log(lu_context, LG_FUNC, "error converting prefix to string");
            ret_val = LU_ERROR;
        }
    } else {
        LG_log(lu_context, LG_FUNC, "error converting %s to prefix", domain);
        ret_val = LU_INVARG;
    }
    LG_log(lu_context, LG_FUNC, "<Exiting LU_get_inetnum_from_domain");
    return ret_val;
}

/*
  Lookup a parents of an object via WHOIS

  server     - connection information
  parents    - used to return parents, if any
  obj        - object to look up parents of
  source     - source, e.g. "RIPE", or NULL to get from obj

  return       - LU_OKAY on success, LU_ERROR on error

  Errors can come from lu_whois_query(), or if source is NULL and
  there is no "source:" in the object.

  Logging perfomed via LG module.
 */
LU_ret_t
lu_whois_get_parents (LU_server_t *server, GList **parents,
                      const rpsl_object_t *obj, const gchar *source)
{
  GList *source_attr;
  gchar *lookup_source;
  const gchar *class;
  gchar *key;
  gchar *query;
  LU_ret_t ret_val;
  gboolean query_ret;

  GList *p;

  LG_log(lu_context, LG_FUNC, ">lu_whois_get_parents: entered");

  /* default values */
  query = NULL;
  ret_val = LU_OKAY;
  *parents = NULL;

  /* extract information */
  if (source == NULL)
  {
    source_attr = rpsl_object_get_attr(obj, "source");
    if (g_list_length(source_attr) != 1)
    {
      /* lg(error, missing source) */
      rpsl_attr_delete_list(source_attr);
      LG_log(lu_context, LG_FUNC, "<lu_whois_get_parents: exiting with value [%s]",
             LU_ret2str(LU_ERROR));
      return LU_ERROR;
    }
    lookup_source = rpsl_attr_get_clean_value(source_attr->data);
  }
  else
  {
    source_attr = NULL;
    lookup_source = UT_strdup(source);
  }
  class = rpsl_object_get_class(obj);

  /* lookup parent based on object type */
  if (has_hierarchical_name(class))
  {
    gchar *parent_name;
    gchar *p;
    const gchar *parent_type;

    LG_log(lu_context, LG_DEBUG, "lu_whois_get_parents: processing hierarchical name");

    parent_name = rpsl_object_get_key_value(obj);
    p = strrchr(parent_name, ':');
    if (p == NULL)
    {
      query = NULL;
    }
    else
    {
      /* colon, has parent */
      *p = '\0';
      if (WK_is_aut_num((char *)parent_name))
      {
        parent_type = "aut-num";
      }
      else
      {
        parent_type = class;
      }
      /* preform lookup */
      query = g_strdup_printf("-G -B -s %s -r -T %s %s", lookup_source,
                              parent_type, parent_name);
    }
    UT_free(parent_name);
    query_ret = lu_whois_query(server->info, query, parents);
    g_free(query);
    if (!query_ret)
    {
      ret_val = LU_ERROR;
    }
  }
  else if (!strcasecmp(class, "aut-num") || !strcasecmp(class, "as-block"))
  {
    LG_log(lu_context, LG_DEBUG, "lu_whois_get_parents: processing ASN");

    key = rpsl_object_get_key_value(obj);
    query = g_strdup_printf("-G -B -s %s -r -T as-block %s", lookup_source, key);
    UT_free(key);
    query_ret = lu_whois_query(server->info, query, parents);
    g_free(query);
    if (!query_ret)
    {
      ret_val = LU_ERROR;
    }
  }
  else if (!strcasecmp(class, "inetnum") ||  !strcasecmp(class, "inet6num"))
  {
    LG_log(lu_context, LG_DEBUG, "lu_whois_get_parents: processing IP allocation");

    key = rpsl_object_get_key_value(obj);
    query = g_strdup_printf("-G -B -s %s -r -T %s -l %s", lookup_source, class, key);
    UT_free(key);
    query_ret = lu_whois_query(server->info, query, parents);
    g_free(query);
    if (!query_ret)
    {
      ret_val = LU_ERROR;
    }
  }
  else if (!strcasecmp(class, "route") || !strcasecmp(class, "route6"))
  {
    const rpsl_attr_t *attr;
    GList *net_parents = NULL;
    GList *attrs;

    LG_log(lu_context, LG_DEBUG, "lu_whois_get_parents: processing route");

    attr = rpsl_object_get_attr_by_ofs(obj, 0);
    key = rpsl_attr_get_clean_value(attr);

    /* look for exact match route first */
    query = g_strdup_printf("-G -B -T%s -s %s -r -x %s", class, lookup_source, key);
    query_ret = lu_whois_query(server->info, query, &net_parents);
    g_free(query);

    if (query_ret && ! net_parents)
    {
      /* look for less specific route */
      query = g_strdup_printf("-G -B -T%s -s %s -r -l %s", class, lookup_source, key);
      query_ret = lu_whois_query(server->info, query, &net_parents);
      g_free(query);

      if (query_ret && ! net_parents)
      {
        /* look for exact match inetnum/inet6num */
        if ( !strcasecmp(class, "route") )
        {
          query = g_strdup_printf("-G -B -Tinetnum -s %s -r -x %s", lookup_source, key);
        }
        else
        {
          query = g_strdup_printf("-G -B -Tinet6num -s %s -r -x %s", lookup_source, key);
        }
        query_ret = lu_whois_query(server->info, query, &net_parents);
        g_free(query);

        if (query_ret && ! net_parents)
        {
          /* look for less specific inetnum/inet6num */
          if( !strcasecmp(class, "route") ){
            query = g_strdup_printf("-G -B -Tinetnum -s %s -r -l %s", lookup_source, key);
          }
          else
          {
            query = g_strdup_printf("-G -B -Tinet6num -s %s -r -l %s", lookup_source, key);
          }
          query_ret = lu_whois_query(server->info, query, &net_parents);
          g_free(query);
        }
      }
    }
    UT_free(key);
    if (!query_ret)
    {
      ret_val = LU_ERROR;
    }
    else
    {
      attrs = rpsl_object_get_attr(obj, "origin");
      assert(g_list_length(attrs) == 1);
      key = rpsl_attr_get_clean_value((rpsl_attr_t *)attrs->data);
      rpsl_attr_delete_list(attrs);
      query = g_strdup_printf("-G -B -s %s -r -T aut-num %s", lookup_source, key);
      UT_free(key);
      query_ret = lu_whois_query(server->info, query, parents);
      g_free(query);
      if (!query_ret)
      {
        ret_val = LU_ERROR;
        LU_free_parents(net_parents);
      }
      else
      {
        *parents = g_list_concat(*parents, net_parents);
      }
    }
  }
  else if (!strcasecmp(class, "domain"))
  {
    gchar *parent_name;
    gchar *domain;

    LG_log(lu_context, LG_DEBUG, "lu_whois_get_parents: processing domain");

    domain = rpsl_object_get_key_value(obj);
    parent_name = strchr(domain, '.');
    while ((ret_val == LU_OKAY) && (parent_name != NULL) && (*parents == NULL))
    {
      parent_name++;  /* skip the dot, '.' */
      query = g_strdup_printf("-G -B -s %s -R -r -T domain %s", lookup_source,
                              parent_name);
      query_ret = lu_whois_query(server->info, query, parents);
      g_free(query);
      if (!query_ret)
      {
        ret_val = LU_ERROR;
      }
      parent_name = strchr(parent_name, '.');
    }
    UT_free(domain);
  }
  else
  {
    LG_log(lu_context, LG_DEBUG, "lu_whois_get_parents: processing non-hierarchical class");
  }

  if (ret_val == LU_OKAY)
  {
    for (p = *parents; p != NULL; p = g_list_next(p))
    {
      key = rpsl_object_get_key_value(p->data);
      LG_log(lu_context, LG_DEBUG, "lu_whois_get_parents: parent \"%s\" found", key);
      UT_free(key);
    }
    if (*parents == NULL)
    {
      LG_log(lu_context, LG_DEBUG, "lu_whois_get_parents: no parents found");
    }
  }

  /* clean up and return */
  rpsl_attr_delete_list(source_attr);
  UT_free(lookup_source);
  LG_log(lu_context, LG_FUNC, "<lu_whois_get_parents: exiting with value [%s]",
         LU_ret2str(ret_val));
  return ret_val;
}

/*
 * Lookup anything
 *
 * server    - connection information
 * qyery     - query string
 * source    - update source
 * result    - resulting rpsl object list
 * key       - query search key
 */
LU_ret_t lu_whois_get(LU_server_t *server, gchar *query_str,
                  gchar *source, GList **result, const gchar *key)
{
  gchar *query;
  gint ret_val;
  gboolean query_ret;

  LG_log(lu_context, LG_FUNC, ">Entering lu_whois_get");
  LG_log(lu_context, LG_FUNC, "with query string %s",query_str);
  LG_log(lu_context, LG_FUNC, "source is %s",source);
  LG_log(lu_context, LG_FUNC, "search key is %s",key);

  query = g_strdup_printf("%s -s %s %s", query_str, source, key);
  query_ret = lu_whois_query(server->info, query, result);
  g_free(query);
  if (!query_ret) {
    ret_val = LU_ERROR;
  } else  {
    ret_val = LU_OKAY;
  }

  LG_log(lu_context, LG_FUNC, "<lu_whois_get: exiting with value [%s]", LU_ret2str(ret_val));
  return ret_val;
}

/*
  Initialise a WHOIS lookup connection

  hostname   - name of server, e.g. "whois.ripe.net"
  port       - port, e.g. 43
  timeout    - timeout in seconds, or 0 of "no timeout"

  return     - pointer to new structure
 */
LU_server_t *
LU_whois_init (const gchar *hostname, int port, int timeout)
{
  LU_whois_info_t *whois_info;
  LU_server_t *ret_val;

  whois_info = g_new(LU_whois_info_t, 1);
  whois_info->hostname = g_strdup(hostname);
  whois_info->port = port;
  whois_info->timeout = timeout;
  whois_info->socket = -1;

  ret_val = g_new(LU_server_t, 1);
  ret_val->lookup = lu_whois_lookup;
  ret_val->inverse_lookup = lu_whois_inverse_lookup;
  ret_val->get_object = lu_whois_get_object;
  ret_val->get = lu_whois_get;
  ret_val->get_parents = lu_whois_get_parents;
  ret_val->check_overlap = lu_whois_check_overlap;
  ret_val->cleanup = lu_whois_cleanup;
  ret_val->info = whois_info;

  return ret_val;
}


