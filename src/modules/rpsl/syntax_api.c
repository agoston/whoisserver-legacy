/******************
  Copyright (c) 2002                              RIPE NCC

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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <glib.h>
#include <pthread.h>
#include "syntax_api.h"
#include "syntax.h"
#include "attribute.h"
#include "class.h"
#include "iproutines.h"

#include <assert.h>

#define RUNTIME_CHECK 0

#if RUNTIME_CHECK
#include <stdio.h>
#define chk_obj(o)  rpsl_object_check((o),__FILE__,__LINE__)
#define chk_attr(a) rpsl_attr_check((a),__FILE__,__LINE__)
#define chk_err(e)  rpsl_error_check((e),__FILE__,__LINE__)

void rpsl_error_check(const GList *errors, const char *file, int line);
void rpsl_attr_check(const rpsl_attr_t *attr, const char *file, int line);
void rpsl_object_check(const rpsl_object_t *obj, const char *file, int line);
#else
#define chk_obj(o) 
#define chk_attr(a) 
#define chk_err(e) 
#endif /* RUNTIME_CHECK */

/* type of check currently in place */
static int rpsl_level = RPSL_DICT_FRONT_END;

/* return true if the character is an RPSL line-continuation */
#define is_rpsl_line_cont(c) (((c)==' ') || ((c) == '\t')  || ((c) == '+'))

/* needed by hash table */
static guint
my_g_str_hash (gconstpointer v)
{
    gchar *s;
    guint hash;

    s = g_strdup(v);
    g_strdown(s);
    hash = g_str_hash(s);
    g_free(s);
    return hash;
}

static gint
my_g_strcasecmp (gconstpointer a, gconstpointer b)
{
    return (g_strcasecmp(a, b) == 0);
}


/* remove comments, join lines, and compress whitespace */
static gchar *
attribute_clean (const gchar *val)
{
    gchar **lines;
    int i;
    gchar *p;
    gchar *q;
    gchar *ret_val;

    /* split our value up into lines */
    lines = ut_g_strsplit_v1(val, "\n", 0);

    for (i=0; lines[i] != NULL; i++) {
        /* remove comments */
        p = strchr(lines[i], '#');
        if (p != NULL) {
            *p = '\0';
        }

        /* convert line continuation characters to whitespace */
        if (i > 0) {
            /* XXX: error in attribute */
            assert(is_rpsl_line_cont(lines[i][0]));
            lines[i][0] = ' ';
        }
    }

    /* join the lines into a single string */
    ret_val = g_strjoinv("", lines);
    g_strfreev(lines);

    /* we read from p, and write to q */
    p = ret_val;
    q = ret_val;

    /* skip leading whitespace */
    while (isspace(0xFF & (int)*p)) {
        p++;
    }

    /* convert all series of whitespace into a single space */
    /*  (this will happily convert '\n' into ' ') */
    while (*p != '\0') {
        if (isspace(0xFF & (int)*p)) {
            *q = ' ';
            q++;
            do {
                p++;
            } while (isspace(0xFF & (int)*p));
        } else {
            *q = *p;
            q++;
            p++;
        }
    }

    /* remove any trailing whitespace (there will be at most one space,
       because of the whitespace compression already performed above) */
    if ((q > ret_val) && isspace(0xFF & (int)*(q-1))) {
        q--;
    }

    /* NUL-terminate our string */
    *q = '\0';

    /* return our new line */
    return (gchar *)ret_val;
}

/* remove comments, clean lines, and compress whitespace */
static gchar *
attribute_clean_lines (const gchar *val)
{
    gchar **lines;
    int i;
    gchar *p;
    gchar *q;
    gchar *ret_val;

    /* split our value up into lines */
    lines = ut_g_strsplit_v1(val, "\n", 0);

    /* clean each line separately */
    for (i=0; lines[i] != NULL; i++) {
        /* remove comments */
        p = strchr(lines[i], '#');
        if (p != NULL) {
            *p = '\0';
        }

        /* convert line continuation characters to whitespace */
        if (i > 0) {
            /* XXX: error in attribute */
            assert(is_rpsl_line_cont(lines[i][0]));
            lines[i][0] = ' ';
        }

        /* we read from p, and write to q, for whitespace compression */
        p = lines[i];
        q = lines[i];

        /* skip leading whitespace */
        while (isspace(0xFF & (int)*p)) {
            p++;
        }

        /* convert all series of whitespace into a single space */
        /*  (this will happily convert '\n' into ' ') */
        while (*p != '\0') {
            if (isspace(0xFF & (int)*p)) {
                *q = ' ';
                q++;
                do {
                    p++;
                } while (isspace(0xFF & (int)*p));
            } else {
                *q = *p;
                q++;
                p++;
            }
        }

        /* remove any trailing whitespace (there will be at most one space,
           because of the whitespace compression already performed above) */
        if ((q > lines[i]) && isspace(0xFF & (int)*(q-1))) {
            q--;
        }

        /* NUL-terminate our line */
        *q = '\0';
    }

    /* join the lines into a single string */
    ret_val = g_strjoinv("\n", lines);
    g_strfreev(lines);

    /* return our new line */
    return (gchar *)ret_val;
}

/* see if the given string ends with the other string */
static gboolean
str_ends_with (const char *s1, const char *s2)
{
    int s1_len;
    int s2_len;

    s1_len = strlen(s1);
    s2_len = strlen(s2);
    if (s1_len < s2_len) {
        return FALSE;
    }
    if (strcmp(s1 + s1_len - s2_len, s2) == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/* return each element in the list as a seperate gchar* */
/* be sure to call g_strfreev() when done with this result!!! */
static gchar **
generic_list_split (const char *val, const char *separator_char)
{
    gchar *tmp_str;
    gchar **ret_val;
    gboolean has_empty_last_element;
    int i;
    int list_size;

    /* clean up to remove comments and newlines */
    tmp_str = attribute_clean(val);

    /* check for empty last element, which g_strsplit() will silently drop */
    has_empty_last_element = str_ends_with(tmp_str, separator_char);

    /* split based on separator character */
    ret_val = ut_g_strsplit_v1(tmp_str, separator_char, 0);

    /* free our temporary variable */
    g_free(tmp_str);

    /* clean whitespace from each element */
    list_size = 0;
    for (i=0; ret_val[i] != NULL; i++) {
        g_strstrip(ret_val[i]);
        list_size++;
    }

    /* if we have an empty last element, add it back in */
    if (has_empty_last_element) {
        ret_val = g_renew(gchar*, ret_val, list_size+2);
        ret_val[list_size] = g_strdup("");
        ret_val[list_size+1] = NULL;
    }

    /* return our array */
    return ret_val;
}


static gchar **
attribute_list_split (const char *val)
{
    return generic_list_split(val, ",");
}

/* this function cleans the mnt-routes attribute value, leaves only maintainer name 
   and separates optional list to a GList of ip_prefix_range_t*.
   if prefix range is invalid, it is added t the list of invalid prefix ranges (Glist of gchars).
   prefix_range_list is NULL if there is no list in the value
   if the list contains only "ANY", it is set to NULL
*/

GList **rpsl_mnt_routes_clean_split (rpsl_attr_t *attr, GList **prefix_range_list, GList **invalid_list)
{
  gchar *attr_value;
  gchar **tokens;
  gchar **rest;
  static gchar **prefix_list;
  int i;

  *prefix_range_list = NULL;
  *invalid_list = NULL;

  attr_value = rpsl_attr_get_clean_value(attr);

  /* is there any prefix range list in the attribute? */
  if (strstr (attr_value, "{") == NULL )
  {
    /* no list, do nothing and return */
    g_free(attr_value);  
    return (prefix_range_list);
  }
 
  tokens = g_strsplit(attr_value, " ", 2);

  /* save mntner, modify the attribute */
  rpsl_attr_replace_value ((rpsl_attr_t *)attr, tokens[0]);

  /* get the prefix range list as a string, skipping '{' */
  rest = g_strsplit(tokens[1] + 1, "}", 2);

  /* convert string to a list ** */
  prefix_list = attribute_list_split(rest[0]);

  /* clean up */
  g_free(attr_value);  

  /* convert list of gchar* to GList of ip_prefix_range_t */
  for (i=0; prefix_list[i] != NULL; i++)
  {
    if (strcasecmp (prefix_list[i], "ANY") != 0)
    {
      /* this must be a prefix. do convert and append */
      ip_prefix_range_t *prefix_range;
      prefix_range = malloc(sizeof(ip_prefix_range_t));
      if (IP_pref_rang_t2b (prefix_range, prefix_list[i], IP_PLAIN) == IP_OK)
      {
        *prefix_range_list = g_list_append (*prefix_range_list, prefix_range);
      }
      else 
      {
        *invalid_list = g_list_append (*invalid_list, strdup(prefix_list[i]));
      }
    }
    else 
    { 
      /* if list contains ANY (should be only 1 element), empty the list */
      g_list_free (*prefix_range_list);
      g_strfreev (prefix_list);
      return (prefix_range_list);
    }
  }
  
  return (prefix_range_list);
}

static gchar **
ripe_list_split (const char *val)
{
    /* 
       We can call generic_list_split() because it calls 
       attribute_clean() before using g_strsplit() to divide the string,
       and attribute_clean() converts any runs of whitespace into a 
       single space. 
     */
    return generic_list_split(val, " ");
}

static void
rpsl_error_assign (rpsl_error_t *error, 
                   gint level, 
                   gint code, 
                   gchar *descr_fmt, 
                   ...)
{
    va_list args;

    if (error != NULL) {
        error->level = level;
        error->code = code;
        va_start(args, descr_fmt);
        error->descr = g_strdup_vprintf(descr_fmt, args);
        va_end(args);
        error->attr_num = -1;
    }
}

static void
rpsl_error_add (GList **errors, gint level, gint code, gint attr_num, 
                gchar *descr_fmt, ...)
{
    rpsl_error_t *error;
    va_list args;

    error = g_new(rpsl_error_t, 1);
    error->level = level;
    error->code = code;
    va_start(args, descr_fmt);
    error->descr = g_strdup_vprintf(descr_fmt, args);
    va_end(args);
    error->attr_num = attr_num;
    *errors = g_list_append(*errors, error);
}

/* returns TRUE if okay, else FALSE */
static gboolean 
rpsl_attr_syntax_check (const attribute_t *attr_info,
                        const gchar *value, 
                        GList **errors)
{
    int level;
    gchar **split_val;
    int i;
    int error_cnt;
    GPtrArray *parse_errors;
    gchar *parse_descr;

    /* set up for exit */
    split_val = NULL;

    /* set our syntax checking level */
    if (rpsl_level == RPSL_DICT_CORE) {
        level = SYNTAX_CHECK_CORE;
    } else {
        level = SYNTAX_CHECK_FRONT_END;
    }

    error_cnt = 0;

    /* check the syntax */
    if ((attr_info->is_list) || (attr_info->is_ripe_list)) {
        if (attr_info->is_list) {
            split_val = attribute_list_split(value);
        } else {
            split_val = ripe_list_split(value);
        }
        if (split_val[0] == NULL) {
            rpsl_error_add(errors,
                           RPSL_ERRLVL_ERROR,
                           RPSL_ERR_EMPTYLIST,
                           -1, 
                           "Attribute \"%s\" has an empty list", 
                           attr_info->name);
            goto exit_rpsl_syntax;
        }
    } else {
        split_val = g_new(gchar*, 2);
        split_val[0] = attribute_clean(value);
        split_val[1] = NULL;
    }

    for (i=0; split_val[i] != NULL; i++) {
        parse_errors = syntax_check_by_offset(attr_info->syntax_offset,
                                              level,
                                              split_val[i]);
        error_cnt += parse_errors->len;
        while (parse_errors->len > 0) {
            parse_descr = g_ptr_array_remove_index(parse_errors, 0);
            rpsl_error_add(errors,
                           RPSL_ERRLVL_ERROR,
                           RPSL_ERR_SYNERR,
                           -1,
                           "%s",
                           parse_descr);
            g_free(parse_descr);
        }
        g_ptr_array_free(parse_errors, TRUE);
    }

exit_rpsl_syntax:
    if (split_val != NULL) {
        g_strfreev(split_val);
    }
    if (error_cnt == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/* 
   returns NULL on *coding errors*
      non-existant class specified
      attribute does not exist for class
      attribute without class in ambiguous
   returns a structure otherwise
      on *syntax errors* errors are in the rpsl_attr_t structure
 */

/* XXX: there should be a way to preserve the original text, so 
        that garbage attributes still retain meaning
 */
rpsl_attr_t *
rpsl_attr_init (const gchar *s, const gchar *class)
{
    rpsl_attr_t *retval;
    gchar **attr_val;
    const class_t *class_info;
    const class_attr_t *class_attr_info;
    const attribute_t *attr_info;
    gboolean is_ambiguous;

    /* return NULL if string is empty */
    if ((s == NULL) || (s[0] == '\0')) {
        return NULL;
    }

    /* get class info as early as possible */
    if (class != NULL) {
        class_info = class_lookup(class);
        if (class_info == NULL) {
            return NULL;
        }
    } else {
        class_info = NULL;
    }

    /* initialize the structure */
    retval = g_new(rpsl_attr_t, 1);
    retval->name = NULL;
    retval->lcase_name = NULL;
    retval->value = NULL;
    retval->errors = NULL;
    retval->num = -1;
    retval->attr_info = NULL;

    /* prepare for early return */
    attr_val = NULL;

    /* split into attribute and value */
    /* g_strsplit() puts max # of tokens + the rest of the string */
    /* so in this case we will have 1 token (attr name maybe) and the rest */
    if (strchr(s, ':') == NULL) {
        /* this is a critical error - very bad if it is a class attribute */
        rpsl_error_add(&retval->errors,
                       RPSL_ERRLVL_CRIT, 
                       RPSL_ERR_BADATTR, 
                       -1,
                       "Attribute missing colon, ':'");
        retval->name = g_strdup("");
        retval->lcase_name = g_strdup("");
        retval->value = g_strdup("");
        goto exit_rpsl_attr_init;
        
    }
    attr_val = g_strsplit(s, ":", 2);
    assert(attr_val[0] != NULL);

    /* assign our name and value */
    retval->name = g_strdup(attr_val[0]);
    retval->lcase_name = g_strdup(attr_val[0]);
    g_strdown(retval->lcase_name);
    if (attr_val[1] == NULL) {
        /* possible if nothing after the ':' */
        retval->value = g_strdup("");
    } else {
        /* the usual case, copy our data */
        retval->value = g_strdup(attr_val[1]);
        assert(attr_val[2] == NULL);
    }

    /* get our attribute information */
    if (class_info != NULL) {
        class_attr_info = class_attr_lookup(class_info, retval->name);
    } else {
        class_attr_info = NULL;
    }
    if ((class_info != NULL) && (class_attr_info != NULL)) {
        attr_info = attribute_lookup_by_offset(class_attr_info->offset);
        assert(attr_info != NULL);
    } else {
        attr_info = attribute_lookup(retval->name, &is_ambiguous);
        if (is_ambiguous) {
            rpsl_attr_delete(retval);
            retval = NULL;
            goto exit_rpsl_attr_init;
        }
        if (attr_info == NULL) {
            /* this is a critical error - bad if it is a class attribute */
            rpsl_error_add(&retval->errors,
                           RPSL_ERRLVL_CRIT,
                           RPSL_ERR_UNKNOWNATTR,
                           -1,
                           "\"%s\" is not a known RPSL attribute",
                           retval->name);
            goto exit_rpsl_attr_init;
        }
    }
    /* dangerous, but we promise not to make any changes to this value! */
    retval->attr_info = (void *)attr_info;

    /* check for errors (adds to the error list) */
    rpsl_attr_syntax_check(attr_info, retval->value, &retval->errors);

    /* clean up and leave */
exit_rpsl_attr_init:
    if (attr_val != NULL) {
        g_strfreev(attr_val);
    }

    chk_attr(retval);

    return retval;
}

static rpsl_error_t *
rpsl_error_copy (const rpsl_error_t *err)
{
    rpsl_error_t *retval;

    retval = g_new(rpsl_error_t, 1);
    retval->level = err->level;
    retval->code = err->code;
    retval->descr = g_strdup(err->descr);
    retval->attr_num = err->attr_num;
    return retval;
}

rpsl_attr_t *
rpsl_attr_copy (const rpsl_attr_t *attr)
{
    rpsl_attr_t *retval;
    GList *ptr;

    chk_attr(attr);

    retval = g_new(rpsl_attr_t, 1);
    retval->name = g_strdup(attr->name);
    retval->lcase_name = g_strdup(attr->lcase_name);
    retval->value = g_strdup(attr->value);
    retval->errors = NULL;
    for (ptr=attr->errors; ptr != NULL; ptr = g_list_next(ptr)) {
        retval->errors = g_list_append(retval->errors, 
                                       rpsl_error_copy(ptr->data));
    }
    retval->num = attr->num;
    retval->attr_info = attr->attr_info;

    chk_attr(retval);

    return retval;
}

rpsl_attr_t *
rpsl_attr_clean_copy (const rpsl_attr_t *attr)
{
    rpsl_attr_t *retval;

    chk_attr(attr);

    retval = g_new(rpsl_attr_t, 1);
    retval->name = g_strdup(attr->name);
    retval->lcase_name = g_strdup(attr->lcase_name);
    retval->value = attribute_clean(attr->value);
    retval->errors = NULL;
    retval->num = attr->num;
    retval->attr_info = attr->attr_info;

    chk_attr(retval);

    return retval;
}


void
rpsl_attr_delete (rpsl_attr_t *attr)
{
    GList *ptr;
    rpsl_error_t *err;

    chk_attr(attr);

    g_free(attr->name);
    attr->name = NULL;
    g_free(attr->lcase_name);
    attr->lcase_name = NULL;
    g_free(attr->value);
    attr->value = NULL;
    for (ptr=attr->errors; ptr != NULL; ptr = g_list_next(ptr)) {
        err = ptr->data;
        g_free(err->descr);
        g_free(err);
    }
    g_list_free(attr->errors);
    attr->errors = NULL;
    attr->num = -1;
    attr->attr_info = NULL;
    g_free(attr);
}

void 
rpsl_attr_delete_list (GList *attributes)
{
    GList *ptr;

    for (ptr=attributes; ptr != NULL; ptr = g_list_next(ptr)) {
        rpsl_attr_delete(ptr->data);
    }
    g_list_free(attributes);
}

const gchar *
rpsl_attr_get_name (const rpsl_attr_t *attr)
{
    chk_attr(attr);

    /* XXX: there should be a way to get the original name */
    /*return attr->name;*/
    return attr->lcase_name;
}

gint
rpsl_attr_get_ofs (const rpsl_attr_t *attr)
{
    chk_attr(attr);

    return attr->num;
}

const gchar *
rpsl_attr_get_value (const rpsl_attr_t *attr)
{
    chk_attr(attr);

    return attr->value;
}

gchar *
rpsl_attr_get_clean_value (const rpsl_attr_t *attr)
{
    gchar *tmp;
    gchar *retval;

    chk_attr(attr);

    /* don't just return the value from attribute_clean(), since we
       need to return memory that can be freed via free(), and the
       gchar* returned by attribute_clean needs to be freed via g_free() */
    tmp = attribute_clean(attr->value);
    retval = strdup(tmp);
    g_free(tmp);
    return retval;
}

gchar *
rpsl_attr_get_clean_lines (const rpsl_attr_t *attr)
{
    gchar *tmp;
    gchar *retval;

    chk_attr(attr);

    /* don't just return the value from attribute_clean_lines(), since we
       need to return memory that can be freed via free(), and the
       gchar* returned by attribute_clean needs to be freed via g_free() */
    tmp = attribute_clean_lines(attr->value);
    retval = strdup(tmp);
    g_free(tmp);
    return retval;
}

GList *
rpsl_attr_get_split_list (const rpsl_attr_t *attr)
{
    const attribute_t *attr_info;
    GList *retval;
    gchar **split;
    int i;
    rpsl_attr_t *newattr;

    chk_attr(attr);

    attr_info = attr->attr_info;
    if ((attr_info!=NULL) && (attr_info->is_list || attr_info->is_ripe_list)) {
        if (attr_info->is_list) {
            split = attribute_list_split(attr->value);
        } else {
            split = ripe_list_split(attr->value);
        }
        retval = NULL;
        for (i=0; split[i] != NULL; i++) {
            /* XXX: perpaps consolidate this with rpsl_attr_init()? */
            newattr = g_new(rpsl_attr_t, 1);
            assert(newattr != NULL);
            newattr->name = g_strdup(attr->name);
            newattr->lcase_name = g_strdup(attr->lcase_name);
            newattr->value = g_strdup(split[i]);
            newattr->errors = NULL;
            newattr->num = attr->num;
            newattr->attr_info = attr->attr_info;
            retval = g_list_append(retval, newattr);
        }
        g_strfreev(split);
        return retval;
    } else {
        return g_list_append(NULL, rpsl_attr_clean_copy(attr));
    }
}

void 
rpsl_attr_split_multiple (GList **attrs)
{
    GList *new;
    GList *old;
    rpsl_attr_t *oldattr;
    GList *newattrs;

    new = NULL;
    for (old=*attrs; old != NULL; old = g_list_next(old)) {
        oldattr = (rpsl_attr_t *)old->data;
        newattrs = rpsl_attr_get_split_list(oldattr);
        new = g_list_concat(new, newattrs);
    }
    rpsl_attr_delete_list(*attrs);
    *attrs = new;
}

void
rpsl_attr_replace_value (rpsl_attr_t *attr, const gchar *value)
{
    chk_attr(attr);

    /* perform check to add any new errors */
    if (attr->attr_info != NULL) {
        rpsl_attr_syntax_check(attr->attr_info, value, &attr->errors);
    }

    /* copy the value */
    g_free(attr->value);
    attr->value = g_strdup(value);

    chk_attr(attr);
}

const GList *
rpsl_attr_errors (const rpsl_attr_t *attr)
{
    chk_attr(attr);

    return attr->errors;
}

static gboolean 
object_is_comment (const gchar *s)
{
    const gchar *p, *q;

    /* skip blank lines */
    p = s;
    for (;;) {
        while ((*p == ' ') || (*p == '\t')) {
            p++;
        }
        /* if object is only blank lines, then we are *not* a comment */
        if (*p == '\0') {
            return FALSE;
        }
        if (*p != '\n') {
            break;
        }
        p++;
    }
    /* skip comment lines */
    for (;;) {
        if ((*p != '%') && (*p != '#')) {
            break;
        }
        q = strchr(p, '\n');
        /* if we end on a comment without newline, we *are* a comment */
        if (q == NULL) {
            return TRUE;
        }
        p = q + 1;
    }
    /* skip trailing blank lines */
    for (;;) {
        while ((*p == ' ') || (*p == '\t')) {
            p++;
        }
        if (*p != '\n') {
            break;
        }
        p++;
    }
    /* see if we skipped the whole object */
    if (*p == '\0') {
        return TRUE;
    } else {
        return FALSE;
    }
}

/* we don't want to check whether an attribute belongs in the template 
   if it is a bad attribute, or and unknown attribute */
static gboolean
template_check_needed (const rpsl_attr_t *attr)
{
    const GList *p;
    const rpsl_error_t *error;

    p = rpsl_attr_errors(attr);
    while (p != NULL) {
        error = p->data;
        if (error->code == RPSL_ERR_BADATTR) {
            return FALSE;
        }
        if (error->code == RPSL_ERR_UNKNOWNATTR) {
            return FALSE;
        }
        p = g_list_next(p);
    }
    return TRUE;
}

static void
renumber_attr (rpsl_attr_t *attr, int num)
{
    attr->num = num;
}

static rpsl_attr_t *
rpsl_empty_attr ()
{
    rpsl_attr_t *retval;

    retval = g_new(rpsl_attr_t, 1);
    retval->name = g_strdup("");
    retval->lcase_name = g_strdup("");
    retval->value = g_strdup("");
    retval->errors = NULL;
    retval->num = -1;
    retval->attr_info = NULL;
    return retval;
}

rpsl_object_t *
rpsl_object_init (const gchar *s)
{
    rpsl_object_t *retval;
    GPtrArray *lines;
    const gchar *p, *q;
    gchar *line;
    rpsl_attr_t *attr;
    const class_t *class_info;
    GList *attr_list;
    const class_attr_t *class_attr_info;
    const attribute_t *attr_info;
    const gchar *attr_name;
    const gchar *class_name;
    int i;
    gboolean removed_trailing_empty_lines;

    /* initialize the structure */
    retval = g_new(rpsl_object_t, 1);
    retval->attributes = NULL;
    retval->attr_lookup = g_hash_table_new(my_g_str_hash, my_g_strcasecmp);
    retval->errors = NULL;
    retval->class_info = NULL;

    /* make some lines */
    lines = g_ptr_array_new();

    /* see if entire string is comments */
    if (object_is_comment(s)) {
        rpsl_error_add(&retval->errors,
                       RPSL_ERRLVL_WARN,
                       RPSL_ERR_ONLYCOMMENTS,
                       -1,
                       "Object contains only comments");
        goto exit_rpsl_object_init;
    }

    /* p is the beginning of the current attribute, q searches for the end */
    p = s;
    for (;;) {
        /* done with string, finish */
        if (*p == '\0') {
            break;
        }

        /* search for end of attribute */
        q = strchr(p, '\n');
        while ((q != NULL) && is_rpsl_line_cont(q[1])) {
            q = strchr(q+1, '\n');
        }

        if (q == NULL) {
            /* add the final attribute */
            g_ptr_array_add(lines, g_strdup(p));
            /* and exit */
            break;
        } else {
            /* add this attribute */
            g_ptr_array_add(lines, g_strndup(p, q-p));
            /* and proceed to the next one */
            p = q+1;
        }
    }

    /* be nice and strip empty lines at the end */
    removed_trailing_empty_lines = FALSE;
    while (lines->len > 0) {
        line = g_ptr_array_index(lines, lines->len - 1);
        if (line[0] != '\0') {
            break;
        }
        g_ptr_array_remove_index_fast(lines, lines->len - 1);
        g_free(line);
        removed_trailing_empty_lines = TRUE;
    }
    if (removed_trailing_empty_lines) {
        rpsl_error_add(&retval->errors,
                       RPSL_ERRLVL_INFO,
                       RPSL_ERR_EMPTYATTR,
                       -1,
                       "Trailing blank lines ignored");
    }

    /* verify we have at least one line */
    if (lines->len <= 0) {
        rpsl_error_add(&retval->errors,
                       RPSL_ERRLVL_CRIT, 
                       RPSL_ERR_BADCLASS,
                       -1,
                       "Empty object");
        goto exit_rpsl_object_init;
    }

    /* create the magic first attribute, which is the class */
    attr = rpsl_attr_init(g_ptr_array_index(lines, 0), NULL);
    if (attr == NULL) {
        rpsl_error_add(&retval->errors,
                       RPSL_ERRLVL_CRIT, 
                       RPSL_ERR_BADCLASS,
                       -1,
                       "Error with class attribute, class invalid");
        goto exit_rpsl_object_init;
    }
    renumber_attr(attr, 0);

    /* check for errors with the class attribute */
    /* only critical errors matter - let innocent syntax errors pass through */
    if (rpsl_attr_has_error(attr, RPSL_ERRLVL_CRIT)) {
        rpsl_error_add(&retval->errors,
                       RPSL_ERRLVL_CRIT,
                       RPSL_ERR_BADCLASS,
                       -1,
                       "Error with class attribute, class invalid");
        rpsl_attr_delete(attr);
        goto exit_rpsl_object_init;
    }
    

    /* get the class information */
    class_name = rpsl_attr_get_name(attr);
    class_info = class_lookup(class_name);
    if (class_info == NULL) {
        rpsl_error_add(&retval->errors,
                       RPSL_ERRLVL_CRIT,
                       RPSL_ERR_UNKNOWNCLASS,
                       -1,
                       "First attribute, \"%s\", is not a known RPSL class", 
                       class_name);
        rpsl_attr_delete(attr);
        goto exit_rpsl_object_init;
    }

    /* check for syntax errors with the class attribute */
    if (rpsl_attr_errors(attr) != NULL) {
                rpsl_error_add(&retval->errors,
                               RPSL_ERRLVL_ERROR,
                               RPSL_ERR_BADATTR,
                               0,
                               "Error with attribute \"%s\"", 
                               class_name);
    }

    /* possibly dangerous, but we promise only to read this value! */
    retval->class_info = (void *)class_info;

    /* add class attribute */
    retval->attributes = g_list_append(NULL, attr);
    attr_list = g_list_append(NULL, attr);
    g_hash_table_insert(retval->attr_lookup, 
                        (void *)rpsl_attr_get_name(attr), 
                        attr_list);

    /* valid class, process each attribute */
    for (i=1; i < lines->len; i++) {
        attr = rpsl_attr_init(g_ptr_array_index(lines, i), class_name);
        if (attr == NULL) {
            /* XXX: we should preserve the original information somehow */
            attr = rpsl_empty_attr();
            rpsl_error_add(&(attr->errors),
                           RPSL_ERRLVL_ERROR,
                           RPSL_ERR_BADATTR,
                           -1, 
                           "Attribute not valid in this class");
        }
        assert(attr != NULL);
        renumber_attr(attr, i);

        /* add the attribute to the list of attributes for this object */
        retval->attributes = g_list_append(retval->attributes, attr);

        /* check for errors at attribute level */
        if (rpsl_attr_errors(attr) != NULL) {
            attr_name = rpsl_attr_get_name(attr);
            if (attr_name != NULL) {
                rpsl_error_add(&retval->errors,
                               RPSL_ERRLVL_ERROR,
                               RPSL_ERR_BADATTR,
                               i,
                               "Error with attribute \"%s\"", 
                               attr_name);
            } else {
                rpsl_error_add(&retval->errors,
                               RPSL_ERRLVL_ERROR,
                               RPSL_ERR_BADATTR,
                               i,
                               "Error with attribute");
                /* no name - no sense to process this attr further */
                continue;
            }
        }


        /* get list of existing attributes of this name, if any */
        attr_list = g_hash_table_lookup(retval->attr_lookup, 
                                        rpsl_attr_get_name(attr));


        /* perform template checking if attribute is known type */
        if (template_check_needed(attr)) {

            /* verify this attribute can go in this class */
            class_attr_info = class_attr_lookup(class_info, 
                                                rpsl_attr_get_name(attr));
            if (class_attr_info == NULL) {
                rpsl_error_add(&retval->errors,
                               RPSL_ERRLVL_ERROR,
                               RPSL_ERR_ATTRNOTALLOWED,
                               i,
                               "Attribute \"%s\" is not allowed in this class",
                               rpsl_attr_get_name(attr));
            } else {
                /* if we have added a "single" attribute more than once */
                if ((class_attr_info->number == ATTR_SINGLE) && 
                    (attr_list != NULL)) 
                {
                    rpsl_error_add(&retval->errors,
                                   RPSL_ERRLVL_ERROR,
                                   RPSL_ERR_ATTRSINGLE,
                                   i,
                                   "Attribute \"%s\" appears more than once",
                                   rpsl_attr_get_name(attr));
                }
            }

        } /* template_check_required(attr)) */

        /* add the attribute to the hash table for the class */
        attr_list = g_list_append(attr_list, attr);
        g_hash_table_insert(retval->attr_lookup, 
                            (void *)rpsl_attr_get_name(attr),
                            attr_list);         /* replaces any old value */
    }

    /* check for missing required attributes */
    for (i=0; i<class_info->num_attr; i++) {
        class_attr_info = &class_info->attr[i];
        if (class_attr_info->choice == ATTR_MANDATORY) {
            attr_info = attribute_lookup_by_offset(class_attr_info->offset);
            attr_list = g_hash_table_lookup(retval->attr_lookup,
                                            attr_info->name);
            if (attr_list == NULL) {
                rpsl_error_add(&retval->errors,
                               RPSL_ERRLVL_ERROR,
                               RPSL_ERR_MISSINGATTR,
                               -1,
                               "Required attribute \"%s\" is missing",
                               attr_info->name);
                if (attr_info->is_primary) {
                    rpsl_error_add(&retval->errors,
                                   RPSL_ERRLVL_ERROR,
                                   RPSL_ERR_MISSINGKEY,
                                   -1,
                                   "Primary key \"%s\" is missing",
                                   attr_info->name);
                }
            }
        }
    }

    /* done - enjoy your new object */

exit_rpsl_object_init:
    /* free memory used by split lines */
    for (i=0; i<lines->len; i++) {
        g_free(g_ptr_array_index(lines, i));
    }
    g_ptr_array_free(lines, TRUE);
    lines = NULL;

    chk_obj(retval);

    /* return our object */
    return retval;
}

rpsl_object_t *
rpsl_object_copy (const rpsl_object_t *object)
{
    rpsl_object_t *retval;
    GList *p;
    rpsl_attr_t *attr;
    GList *attr_list;

    chk_obj(object);

    retval = g_new(rpsl_object_t, 1);
    retval->attributes = NULL;
    retval->attr_lookup = g_hash_table_new(my_g_str_hash, my_g_strcasecmp);
    retval->errors = NULL;
    retval->class_info = object->class_info;

    /* copy attributes */
    for (p=object->attributes; p != NULL; p = g_list_next(p)) {
        /* insert copy of attribute into list */
        attr = rpsl_attr_copy(p->data);
        retval->attributes = g_list_append(retval->attributes, attr);

        /* insert copy of attribute into hash table */
        attr_list = g_hash_table_lookup(retval->attr_lookup,
                                        rpsl_attr_get_name(attr));
        attr_list = g_list_append(attr_list, attr);  /* works for NULL too */
        g_hash_table_insert(retval->attr_lookup, 
                            (void *)rpsl_attr_get_name(attr),
                            attr_list);          /* replaces any old value */
    }

    /* copy errors */
    for (p=object->errors; p != NULL; p = g_list_next(p)) {
        retval->errors = g_list_append(retval->errors, 
                                       rpsl_error_copy(p->data));
    }

    chk_obj(retval);

    /* return the copy */
    return retval;
}

rpsl_object_t *
rpsl_object_copy_flattened (const rpsl_object_t *object)
{
    rpsl_object_t *retval;
    GList *p1, *p2;
    GList *split_attr;
    rpsl_attr_t *attr;
    GList *attr_list;
    int num_attr;

    chk_obj(object);

    retval = g_new(rpsl_object_t, 1);
    retval->attributes = NULL;
    retval->attr_lookup = g_hash_table_new(my_g_str_hash, my_g_strcasecmp);
    retval->errors = NULL;
    retval->class_info = object->class_info;

    /* copy attributes */
    num_attr = 0;
    for (p1=object->attributes; p1 != NULL; p1 = g_list_next(p1)) {
        /* split the attribute into separate values (may be 1) */
        split_attr = rpsl_attr_get_split_list(p1->data);

        /* add each resulting attribute */
        for (p2=split_attr; p2 != NULL; p2 = g_list_next(p2)) {
            attr = p2->data;

            /* renumber attribute */
            renumber_attr(attr, num_attr);
            num_attr++;

            /* insert split attribute into list */
            retval->attributes = g_list_append(retval->attributes, attr);

            /* insert split attribute into hash table */
            attr_list = g_hash_table_lookup(retval->attr_lookup,
                                            rpsl_attr_get_name(attr));
            attr_list = g_list_append(attr_list, attr); /* works for NULL too */
            g_hash_table_insert(retval->attr_lookup, 
                                (void *)rpsl_attr_get_name(attr),
                                attr_list);         /* replaces any old value */
        }

        /* free the list */
        g_list_free(split_attr);
    }

    chk_obj(retval);

    /* return the copy */
    return retval;
}

static void 
rpsl_object_delete_helper (gpointer attr_name, 
                           gpointer attr_list, 
                           gpointer null)
{
    g_list_free((GList *)attr_list);
}

void 
rpsl_object_delete (rpsl_object_t *object)
{
    GList *p;
    rpsl_error_t *err;

    chk_obj(object);
  
    /* free the attributes */
    for (p=object->attributes; p != NULL; p = g_list_next(p)) {
        rpsl_attr_delete(p->data);
    }
    g_list_free(object->attributes);
    object->attributes = NULL;

    /* remove the lists from the hash table */
    g_hash_table_foreach(object->attr_lookup, rpsl_object_delete_helper, NULL);
    g_hash_table_destroy(object->attr_lookup);
    object->attr_lookup = NULL;

    /* free the errors */
    for (p=object->errors; p != NULL; p = g_list_next(p)) {
        err = p->data;
        g_free(err->descr);
        g_free(err);
    }
    g_list_free(object->errors);
    object->errors = NULL;

    /* free the object itself */
    g_free(object);
}

const char *
rpsl_object_get_class (const rpsl_object_t *object)
{
    rpsl_attr_t *attr;

    chk_obj(object);

    if (object->attributes != NULL) {
        attr = (rpsl_attr_t *)object->attributes->data;
        return attr->lcase_name;
    } else {
        return NULL;
    }
}

/* default number of spaces per tab character */
#define TABSTOP 8

/* returns the position of the next tab stop */
static guint
next_tabstop (guint col)
{
    guint tab;

    tab = (col / TABSTOP) + 1;
    return tab * TABSTOP;
}

/* gets the leading whitespace from the given string */
static void
separate_leading_whitespace (const gchar *str, GString **ws, GString **non_ws)
{
    int n;

    n = 0; 
    while ((str[n] == ' ') || (str[n] == '\t')) {
        n++;
    }

    *ws = g_string_new(str);
    g_string_truncate(*ws, n);
    *non_ws = g_string_new(str + n);
}

/* gets the length of a string of whitespace */
static int 
whitespace_length (const gchar *str, int start_col)
{
    int len;

    len = start_col;
    for (;;) {
        if (*str == ' ') {
            len++;
        } else if (*str == '\t') {
            len = next_tabstop(len);
        } else {
            break;
        }
        str++;
    }
    return len;
}

/* removes the number of columns specified from the string, from the right */
static void
remove_columns (GString *s, int col, int start_col)
{
    int old_len;
    int new_len;
    int col_removed;

    col_removed = 0;

    /* first, remove characters until we've removed enough */
    while ((s->len > 0) && (col_removed < col)) {
        old_len = whitespace_length(s->str, start_col);
        g_string_truncate(s, s->len-1);
        new_len = whitespace_length(s->str, start_col);
        col_removed += old_len - new_len;
    }

    /* if we've removed too many, add some spaces back */
    while (col_removed > col) {
        g_string_append_c(s, ' ');
        col_removed--;
    }
}

/* align the text of the attribute to the specific column */
static void
add_aligned_val (GString *s, const rpsl_attr_t *attr, int col)
{
    const gchar *name;
    const gchar *val;
    int start_col;
    const gchar *p, *q;
    int col_to_add;
    int col_to_sub;
    gchar **lines;
    int i, j;
    GString *ws;
    GString *non_ws;

    /* get the information from the attribute */
    name = rpsl_attr_get_name(attr);
    val = rpsl_attr_get_value(attr);

    /* calculate the column we're at after the attribute name */
    start_col = strlen(name) + 1;

    /* if the desired column is too small based on the name of the 
       attribute, set to the smallest allowed column */
    if (col < (start_col + 1)) {
        col = start_col + 1;
    }


    /* find out what column the attribute value currently starts at */
    p = val;
    for (;;) {
        if (*p == ' ') {
            start_col++;
        } else if (*p == '\t') {
            start_col = next_tabstop(start_col);
        } else {
            break;
        } 
        p++;
    }

    /* special case:
       if there are *only* whitespace on the first line, or if it only 
       contains a comment, then use "as-is" */
    if ((*p == '\0') || (*p == '\n') || (*p == '#')) {
          g_string_append(s, val);
          g_string_append_c(s, '\n');
          /* EARLY RETURN */
          return;
    }

    /* next, see how many columns we need to add or subtract */
    col_to_add = col - start_col;

    /* adding is (relatively) easy */
    if (col_to_add > 0) { 
        lines = ut_g_strsplit_v1(val, "\n", 0);
        /* for the first line, append the spaces and the line itself */
        q = lines[0];
        while ((*q == ' ') || (*q == '\t')) {
            g_string_append_c(s, *q);
            q++;
        }
        for (j=0; j<col_to_add; j++) {
            g_string_append_c(s, ' ');
        }
        g_string_append(s, q);
        g_string_append_c(s, '\n');
        for (i=1; lines[i] != NULL; i++) {
            /* for subsequent lines... */
            /* append the first (line continuation) character */
            g_string_append_c(s, lines[i][0]);
            /* append any leading whitespace */
            q = lines[i]+1;
            while ((*q == ' ') || (*q == '\t')) {
                g_string_append_c(s, *q);
                q++;
            }
            /* now append the spaces and the remainder of the line */
            for (j=0; j<col_to_add; j++) {
                g_string_append_c(s, ' ');
            }
            g_string_append(s, q);
            g_string_append_c(s, '\n');
        }
        g_strfreev(lines);
    }
    /* subtracting is a bit more tricky, due to tabs (AKA "minions of evil") */
    else if (col_to_add < 0) {
        col_to_sub = -col_to_add;

        lines = ut_g_strsplit_v1(val, "\n", 0);

        /* add first line after subtracting columns */
        separate_leading_whitespace(lines[0], &ws, &non_ws);
        remove_columns(ws, col_to_sub, strlen(name)+1);
        g_string_append(s, ws->str);
        g_string_append(s, non_ws->str);
        g_string_append_c(s, '\n');
        g_string_free(ws, TRUE);
        g_string_free(non_ws, TRUE);

        for (i=1; lines[i] != NULL; i++) {
            separate_leading_whitespace(lines[i]+1, &ws, &non_ws);
             /* if the line continuation character is a tab and
                we don't have enough columns, convert it to spaces */
            if (lines[i][0] == '\t') {
                if (whitespace_length(ws->str, 0) < col_to_sub) { 
                    lines[i][0] = ' ';
                    for (j=1; j<TABSTOP; j++) {
                        g_string_prepend_c(ws, ' ');
                    }
                }
            }
            remove_columns(ws, col_to_sub, 0);
            g_string_append_c(s, lines[i][0]);
            g_string_append(s, ws->str);
            g_string_append(s, non_ws->str);
            g_string_append_c(s, '\n');
            g_string_free(ws, TRUE);
            g_string_free(non_ws, TRUE);
        }
        g_strfreev(lines);
    }
    /* and if no adjustment is necessary, it's trivial */
    else {
        g_string_append(s, val);
        g_string_append_c(s, '\n');
    }
}

gchar *
rpsl_object_get_text (const rpsl_object_t *object, guint data_column)
{
    GString *s;
    GList *p;
    rpsl_attr_t *attr;
    gchar *retval;
    const gchar *name;

    chk_obj(object);

    /* return NULL on empty object, as promised */
    if (object->attributes == NULL) {
        return NULL;
    }

    /* concatinate attribute names and values together */
    s = g_string_new("");
    for (p=object->attributes; p != NULL; p = g_list_next(p)) {
        attr = p->data;
        name = rpsl_attr_get_name(attr);
        if (name != NULL) {
            g_string_append(s, name);
            g_string_append_c(s, ':');
            if (data_column > 0) {
                add_aligned_val(s, attr, data_column);
            } else {
                g_string_append(s, rpsl_attr_get_value(attr));
                g_string_append_c(s, '\n');
            }
        }
    }

    /* copy value to return */
    retval = (gchar *)malloc(s->len + 1);
    if (retval != NULL) {
        strcpy(retval, s->str);
    }

    /* free string */
    g_string_free(s, TRUE);

    /* return result (returns NULL if memory allocation failed) */
    return retval;
}

gint
rpsl_object_get_num_attr (const rpsl_object_t *object)
{
    chk_obj(object);

    return g_list_length(object->attributes);
}

const GList *
rpsl_object_get_all_attr (const rpsl_object_t *object)
{
    chk_obj(object);

    return object->attributes;
}

static gint
attr_list_order_by_ofs (gconstpointer a, gconstpointer b)
{
    gint ofs_a = rpsl_attr_get_ofs(a);
    gint ofs_b = rpsl_attr_get_ofs(b);

    return ofs_a - ofs_b;
}

GList *
rpsl_object_get_attr (const rpsl_object_t *object, const gchar *name)
{
    GList *attr_list;
    GList *retval;

    chk_obj(object);

    retval = NULL;
    attr_list = g_hash_table_lookup(object->attr_lookup, name);
    while (attr_list != NULL) {
        retval = g_list_append(retval, rpsl_attr_copy(attr_list->data));
        attr_list = g_list_next(attr_list);
    }
    return g_list_sort(retval, attr_list_order_by_ofs);
}

/* makes the GList of rpsl attrs unique */

GList **
rpsl_attr_uniq_list (GList **attr_list)
{
    GList *retval;
    GList *p;

    retval = NULL;
    while (*attr_list != NULL) {
      p = retval;
      while ((p != NULL) && \
             (strcasecmp (rpsl_attr_get_clean_value((*attr_list)->data), \
                          rpsl_attr_get_clean_value(p->data)) != 0)) 
      {
        p = g_list_next(p);
      }
      if (p == NULL) /* reached end of list or list empty */
      {
        retval = g_list_append(retval, rpsl_attr_copy((*attr_list)->data));
      }
      *attr_list = g_list_next(*attr_list);
    }
    rpsl_attr_delete_list(*attr_list);
    *attr_list = g_list_sort(retval, attr_list_order_by_ofs);
    return attr_list;
}

const rpsl_attr_t *
rpsl_object_get_attr_by_ofs (const rpsl_object_t *object, gint ofs)
{
    rpsl_attr_t *attr;

    chk_obj(object);
    attr = g_list_nth_data(object->attributes, ofs);
    chk_attr(attr);

    return attr;
}

gchar *
rpsl_object_get_key_value (const rpsl_object_t *object)
{
    GString *key;
    gchar *tmp;
    const GList *p;
    const rpsl_attr_t *attr;
    const attribute_t *attr_info;
    gchar *ret_val;

    chk_obj(object);

    key = g_string_new("");
    for (p = object->attributes; p != NULL; p = g_list_next(p)) {
        attr = (const rpsl_attr_t *)p->data;
        chk_attr(attr);
        attr_info = (const attribute_t *)attr->attr_info;
        if ((attr_info != NULL) && (attr_info->is_primary)) {
            tmp = rpsl_attr_get_clean_value(attr);
            g_string_append(key, tmp);
            free(tmp);
        }
    }

    ret_val = strdup(key->str);
    g_string_free(key, TRUE);
    return ret_val;
}

/* using -1 for offset (ofs) to append to the end of the object */
static int 
add_attr_to_object(rpsl_object_t *object,
                rpsl_attr_t *attr,
                gint ofs,
                rpsl_error_t *error)
{
    const gchar *attr_name;
    class_t *class_info;
    const class_attr_t *class_attr_info;
    GList *attr_list;
    gint num_attr;
    gint i;
    GList *p;
    rpsl_attr_t *tmp;
    GList *err_list;
    rpsl_error_t *err;

    chk_obj(object);
    chk_attr(attr);

    /* empty object - bogus, reject, abort, error */
    if (object->attributes == NULL) {
        rpsl_error_assign(error,
                          RPSL_ERRLVL_ERROR,
                          RPSL_ERR_BADCLASS,
                          "Empty class");
        chk_obj(object);
        chk_attr(attr);
        return 0;
    }

    /* check our offset number */
    num_attr = rpsl_object_get_num_attr(object);
    /* if ((ofs == 0) || (ofs > num_attr)) { */
    /* ofs == 0 is already checked by upper-level API function */
    if (ofs > num_attr) {
        rpsl_error_assign(error,
                          RPSL_ERRLVL_ERROR,
                          RPSL_ERR_BADOFFSET,
                          "Offset %d not between 1 and %d", ofs, num_attr);
        chk_obj(object);
        chk_attr(attr);
        return 0;
    }

    /* get attributes with this name (may be NULL, which is okay) */
    attr_name = rpsl_attr_get_name(attr);
    attr_list = g_hash_table_lookup(object->attr_lookup, attr_name);

    /* get class info */
    class_info = object->class_info;
    if (class_info != NULL) {    /* we can only check for valid classes... */

        /* verify this attribute can go in this class */
        class_attr_info = class_attr_lookup(class_info, attr_name);
        if (class_attr_info == NULL) {
            rpsl_error_assign(error,
                              RPSL_ERRLVL_ERROR,
                              RPSL_ERR_ATTRNOTALLOWED,
                              "Attribute \"%s\" is not allowed in this class",
                              attr_name);
            chk_obj(object);
            chk_attr(attr);
            return 0;
        }

        /* check to see if it is "single" and already found */
        if ((class_attr_info->number == ATTR_SINGLE) && (attr_list != NULL)) {
            rpsl_error_assign(error,
                              RPSL_ERRLVL_ERROR,
                              RPSL_ERR_ATTRSINGLE,
                              "Attribute \"%s\" already appears in this class",
                              attr_name);
            chk_obj(object);
            chk_attr(attr);
            return 0;
        }

        /* otherwise we can safely add this attribute */
    }

    /* update any attribute offsets in the error list */
    err_list = object->errors;
    while (err_list != NULL) {
        err = err_list->data;
        if (err->attr_num >= ofs) { 
            /* increment errors from later attributes */
            err->attr_num++;
        }
        err_list = g_list_next(err_list);
    }

    /* add attribute to attribute list */
    if ((ofs < 0) || (ofs >= num_attr)) {
        renumber_attr(attr, num_attr);
        object->attributes = g_list_append(object->attributes, attr);
    } else {
        /* insert the entry at the appriate offset */
        renumber_attr(attr, ofs);
        object->attributes = g_list_insert(object->attributes, attr, ofs);
        num_attr++;

        /* renumber entries moved down */
        p = g_list_nth(object->attributes, ofs+1);
        for (i=ofs+1; p != NULL; i++, p = g_list_next(p)) {
            tmp = p->data;
            renumber_attr(tmp, i);
        }
    }

    /* add attribute to hash table */
    attr_list = g_list_append(attr_list, attr);
    g_hash_table_insert(object->attr_lookup, (void *)attr_name, attr_list);

    chk_obj(object);
    chk_attr(attr);

    return 1;
}

int 
rpsl_object_append_attr (rpsl_object_t *object,
                         rpsl_attr_t *attr,
                         rpsl_error_t *error)
{        
    return add_attr_to_object(object, attr, -1, error);
}

int rpsl_object_add_attr (rpsl_object_t *object,
                      rpsl_attr_t *attr,
                      gint ofs,
                      rpsl_error_t *error) 
{
  if (ofs <= 0) {
        rpsl_error_assign(error,
                          RPSL_ERRLVL_ERROR,
                          RPSL_ERR_BADOFFSET,
                          "Offset %d is less than 1", ofs);
        return 0;
    } else {
        return rpsl_object_add_attr_internal(object, attr, ofs, error);
    }

}

int rpsl_object_add_first_attr (rpsl_object_t *object,
                      rpsl_attr_t *attr,
                      rpsl_error_t *error)
{
  return rpsl_object_add_attr_internal(object, attr, 0, error);
}

int 
rpsl_object_add_attr_internal (rpsl_object_t *object,
                      rpsl_attr_t *attr,
                      gint ofs,
                      rpsl_error_t *error)
{
  return add_attr_to_object(object, attr, ofs, error);
}

rpsl_attr_t *
rpsl_object_remove_attr (rpsl_object_t *object, gint ofs, rpsl_error_t *error)
{
  gint num_attr;
  chk_obj(object);
  
  num_attr = rpsl_object_get_num_attr(object);

  if ((ofs <= 0) || (ofs >= num_attr)) {
        rpsl_error_assign(error,
                          RPSL_ERRLVL_ERROR,
                          RPSL_ERR_BADOFFSET,
                          "Offset %d not between 1 and %d", ofs, num_attr);
        chk_obj(object);
        return NULL;
  }
  return rpsl_object_remove_attr_internal(object, ofs, error);
}

rpsl_attr_t *
rpsl_object_remove_first_attr (rpsl_object_t *object, rpsl_error_t *error)
{
  return rpsl_object_remove_attr_internal(object, 0, error); 

}

rpsl_attr_t *
rpsl_object_remove_attr_internal (rpsl_object_t *object, gint ofs, rpsl_error_t *error)
{
    rpsl_attr_t *attr;
    const gchar *attr_name;
    const gchar *new_attr_name;
    class_t *class_info;
    const class_attr_t *class_attr_info;
    GList *attr_list;
    GList *p;
    gint i;
    rpsl_attr_t *tmp;
    GList *err_list, *tmp_err_list;
    rpsl_error_t *err;

    chk_obj(object);

    attr = g_list_nth_data(object->attributes, ofs);
    attr_name = rpsl_attr_get_name(attr);

    /* get class info */
    class_info = object->class_info;
    if (class_info != NULL) {    /* we must check valid classes... */

        /* verify this attribute can be removed */
        class_attr_info = class_attr_lookup(class_info, attr_name);
        if ((class_attr_info != NULL) && 
            (class_attr_info->choice == ATTR_MANDATORY)) 
        {
            rpsl_error_assign(error,
                              RPSL_ERRLVL_ERROR,
                              RPSL_ERR_ATTRNOTALLOWED,
                              "Attribute \"%s\" is required in this class",
                              attr_name);
        }
    }

    /* remove from list and renumber */
    object->attributes = g_list_remove(object->attributes, attr);
    for (i=0, p=object->attributes; p != NULL; i++, p = g_list_next(p)) {
        tmp = p->data;
        renumber_attr(tmp, i);
    }

    /* remove from hash table */
    attr_list = g_hash_table_lookup(object->attr_lookup, attr_name);
    assert(attr_list != NULL);
    g_hash_table_remove(object->attr_lookup, attr_name);
    attr_list = g_list_remove(attr_list, attr);
    if (attr_list != NULL) {
        new_attr_name = rpsl_attr_get_name((rpsl_attr_t *)attr_list->data);
        g_hash_table_insert(object->attr_lookup, 
                            (void *)new_attr_name, 
                            attr_list);
    }

    /* fix any attribute offsets in the error list */
    err_list = object->errors;
    while (err_list != NULL) {
        err = err_list->data;
        if (err->attr_num == ofs) { 
            /* remove errors from this attribute */
            /* XXX: is this safe? should I just scan from the beginning? */
            tmp_err_list = g_list_next(err_list);
            object->errors = g_list_remove_link(object->errors, err_list);
            g_free(err->descr);
            g_free(err);
            g_list_free(err_list);
            err_list = tmp_err_list;
        } else if (err->attr_num > ofs) {
            /* decrement errors from later attributes */
            err->attr_num--;
            err_list = g_list_next(err_list);
        } else {
            /* ignore earlier attributes */
            err_list = g_list_next(err_list);
        }
    }

    chk_obj(object);
    chk_attr(attr);

    return attr;
}

rpsl_attr_t *
rpsl_object_remove_attr_name (rpsl_object_t *object,
                              const gchar *name,
                              rpsl_error_t *error)
{
    GList *attr_list;
    rpsl_attr_t *attr;
    rpsl_attr_t *retval;

    chk_obj(object);

    attr_list = g_hash_table_lookup(object->attr_lookup, name);
    if (attr_list == NULL) {
        rpsl_error_assign(error,
                          RPSL_ERRLVL_ERROR,
                          RPSL_ERR_NOSUCHATTR,
                          "Attribute \"%s\" not in this object",
                          name);
        return NULL;
    }
    attr = attr_list->data;

    retval = rpsl_object_remove_attr(object, attr->num, error);

    chk_obj(object);
    if (retval != NULL) {
        chk_attr(retval);
    }

    return retval;
}

const GList *
rpsl_object_errors (const rpsl_object_t *object)
{
    chk_obj(object);

    return object->errors;
}

gboolean 
rpsl_attr_is_required (const rpsl_object_t *object, const gchar *attr)
{
    const class_attr_t *class_attr_info;

    chk_obj(object);

    class_attr_info = class_attr_lookup(object->class_info, attr);
    return (class_attr_info != NULL) && 
           (class_attr_info->choice == ATTR_MANDATORY);
}

gboolean 
rpsl_attr_is_generated (const rpsl_object_t *object, const gchar *attr)
{
    const class_attr_t *class_attr_info;

    chk_obj(object);

    class_attr_info = class_attr_lookup(object->class_info, attr);
    return (class_attr_info != NULL) && 
           (class_attr_info->choice == ATTR_GENERATED);
}

gboolean 
rpsl_attr_is_multivalued (const rpsl_object_t *object, const gchar *attr)
{
    const class_attr_t *class_attr_info;

    chk_obj(object);

    class_attr_info = class_attr_lookup(object->class_info, attr);
    return (class_attr_info == NULL) ||
           (class_attr_info->number == ATTR_MULTIPLE);
}

gboolean 
rpsl_attr_is_lookup (const rpsl_object_t *object, const gchar *attr)
{
    const class_attr_t *class_attr_info;
    const attribute_t *attr_info;

    chk_obj(object);

    class_attr_info = class_attr_lookup(object->class_info, attr);
    if (class_attr_info == NULL) {
        return FALSE;
    } else {
        attr_info = attribute_lookup_by_offset(class_attr_info->offset);
        assert(attr_info != NULL);
        return attr_info->is_lookup || attr_info->is_inverse;
    }
}

gboolean 
rpsl_attr_is_key (const rpsl_object_t *object, const gchar *attr)
{
    const class_attr_t *class_attr_info;
    const attribute_t *attr_info;

    chk_obj(object);

    class_attr_info = class_attr_lookup(object->class_info, attr);
    if (class_attr_info == NULL) {
        return FALSE;
    } else {
        attr_info = attribute_lookup_by_offset(class_attr_info->offset);
        assert(attr_info != NULL);
        return attr_info->is_primary;
    }
}

gboolean 
rpsl_object_is_deleted (const rpsl_object_t *object)
{
    GList *attr_list;

    chk_obj(object);

    attr_list = g_hash_table_lookup(object->attr_lookup, "delete");
    if (attr_list != NULL) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static gboolean
search_errors (const GList *errors, int error_level)
{
    rpsl_error_t *e;

    while (errors != NULL) {
        e = errors->data;
        if (e->level >= error_level) {
            return TRUE;
        }
        errors = g_list_next(errors);
    }
    return FALSE;
}


gboolean 
rpsl_attr_has_error (const rpsl_attr_t *attr, int error_level)
{
    chk_attr(attr);

    return search_errors(attr->errors, error_level);
}

gboolean 
rpsl_object_has_error (const rpsl_object_t *object, int error_level)
{
    chk_obj(object);

    return search_errors(object->errors, error_level);
}

gint 
rpsl_get_attr_id (const gchar *attr_name)
{
    const attribute_t *attr_info;
    gboolean is_ambiguous;
    
    attr_info = attribute_lookup(attr_name, &is_ambiguous);
    if (attr_info == NULL) {
        return -1;
    } else {
        return attr_info->id;
    }
}

gint 
rpsl_get_class_id (const gchar *class_name)
{
    const class_t *class_info;

    if (class_name == NULL) {
        return -1;
    }

    class_info = class_lookup(class_name);
    if (class_info == NULL) {
        return -1;
    } else {
        return class_info->id;
    }
}

void 
rpsl_load_dictionary (int level)
{
    rpsl_level = level;
}

int 
rpsl_read_dictionary ()
{
    return rpsl_level;
}


const gchar* const *
rpsl_get_classes ()
{
    return get_class_names();
}


/* mapping from class name to class template */
static GHashTable *class_name_to_template = NULL;
static pthread_mutex_t class_name_to_template_mutex = PTHREAD_MUTEX_INITIALIZER;

const rpsl_template_t* const *
rpsl_get_template (const gchar *class)
{
    const class_t *class_info;
    rpsl_template_t **ret_val;
    int i;
    const class_attr_t *class_attr;
    const attribute_t *attr;
    rpsl_template_t *t;

    pthread_mutex_lock(&class_name_to_template_mutex);

    if (class_name_to_template == NULL) {
        class_name_to_template = g_hash_table_new(my_g_str_hash, 
                                                  my_g_strcasecmp);
    }

    ret_val = g_hash_table_lookup(class_name_to_template, class);
    if (ret_val == NULL) {
        class_info = class_lookup(class);
        if (class_info != NULL) {
            ret_val = g_new(rpsl_template_t*, class_info->num_attr+1);
            for (i=0; i<class_info->num_attr; i++) {
                class_attr = &class_info->attr[i];
                attr = attribute_lookup_by_offset(class_attr->offset);
                t = g_new(rpsl_template_t, 1);
                t->name = attr->name;
                t->code = attr->code;
                t->is_required = (class_attr->choice == ATTR_MANDATORY);
                t->is_generated = (class_attr->choice == ATTR_GENERATED);
                t->is_multivalued = (class_attr->number == ATTR_MULTIPLE);
                t->is_lookup = attr->is_lookup;
                t->is_inverse = attr->is_inverse;
                t->is_primary = attr->is_primary;
                t->is_list = attr->is_list;
                t->is_ripe_list = attr->is_ripe_list;
                ret_val[i] = t;
            }
            ret_val[i] = NULL;
            g_hash_table_insert(class_name_to_template, (void *)class, ret_val);
        }
    }


    pthread_mutex_unlock(&class_name_to_template_mutex);

    return (const rpsl_template_t **)ret_val;
}


#if RUNTIME_CHECK
static void
rpsl_error_check (const GList *errors, const char *file, int line)
{
    const rpsl_error_t *err;

    while (errors != NULL) {
        err = errors->data;
        switch (err->level) {
            case RPSL_ERRLVL_NONE:
            case RPSL_ERRLVL_DEBUG:
            case RPSL_ERRLVL_INFO:
            case RPSL_ERRLVL_NOTICE:
            case RPSL_ERRLVL_WARN:
            case RPSL_ERRLVL_ERROR:
            case RPSL_ERRLVL_CRIT:
            case RPSL_ERRLVL_FATAL:
                break;
            default:
                fprintf(stderr, "rpsl_error_check: bad level %d at %s:%d\n",
                        err->level, file, line);
                exit(1);
        }
        /* XXX: could check attr-codes ONLY appear in attr, and so on */
        switch (err->code) {
            case RPSL_ERR_BADATTR:
            case RPSL_ERR_UNKNOWNATTR:
            case RPSL_ERR_EMPTYLIST:
            case RPSL_ERR_EMPTYATTR:
            case RPSL_ERR_SYNERR:
            case RPSL_ERR_ONLYCOMMENTS:
            case RPSL_ERR_BADCLASS:
            case RPSL_ERR_UNKNOWNCLASS:
            case RPSL_ERR_ATTRNOTALLOWED:
            case RPSL_ERR_ATTRSINGLE:
            case RPSL_ERR_ATTRGENERATED:
            case RPSL_ERR_MISSINGATTR:
            case RPSL_ERR_MISSINGKEY:
            case RPSL_ERR_BADOFFSET:
            case RPSL_ERR_NOSUCHATTR:
                break;
            default:
                fprintf(stderr, "rpsl_error_check: bad code %d at %s:%d\n",
                        err->code, file, line);
                exit(1);
        }
        if (err->descr == NULL) {
            fprintf(stderr, "rpsl_error_check: NULL descr at %s:%d\n",
                    file, line);
            exit(1);
        }
        /* XXX: should check attr_num is within object */
        if (err->attr_num < -1) {
            fprintf(stderr, "rpsl_error_check: bad attr_num %d at %s:%d\n",
                    err->attr_num, file, line);
            exit(1);
        }
        errors = g_list_next(errors);
    }
}

static void
rpsl_attr_check (const rpsl_attr_t *attr, const char *file, int line)
{
    const GList *errors;
    const rpsl_error_t *err;

    if (attr == NULL) {
        fprintf(stderr, "rpsl_attr_check: NULL attr at %s:%d\n",
                file, line);
        exit(1);
    }
    if (attr->name == NULL) {
        fprintf(stderr, "rpsl_attr_check: NULL name at %s:%d\n",
                file, line);
        exit(1);
    }
    if (attr->lcase_name == NULL) {
        fprintf(stderr, "rpsl_attr_check: NULL name at %s:%d\n",
                file, line);
        exit(1);
    }
    if (attr->value == NULL) {
        fprintf(stderr, "rpsl_attr_check: NULL value at %s:%d\n",
                file, line);
        exit(1);
    }
    rpsl_error_check(attr->errors, file, line);
    if (attr->num < -1) {
        fprintf(stderr, "rpsl_attr_check: bad num %d at %s:%d\n",
                attr->num, file, line);
        exit(1);
    }
    for (errors=attr->errors; errors != NULL; errors=g_list_next(errors)) {
        err = errors->data;
        if (err->attr_num != -1) {
            fprintf(stderr, 
                    "rpsl_attr_check: attr_num (%d) != -1 at %s:%d\n",
                    err->attr_num, file, line);
            exit(1);
        }
    }
    /* XXX: think of a check for attr_info.... */
}

/* XXX: could also verify keys - but that's a bit extreme */
static void 
count_attr_in_list (gpointer key, gpointer value, gpointer user_data)
{
    GList *l;
    int sum;
    int *cnt;

    sum = 0;
    for (l=value; l != NULL; l = g_list_next(l)) {
        sum++;
    }
    cnt = (int *)user_data;
    *cnt += sum;
}

static void
rpsl_object_check (const rpsl_object_t *obj, const char *file, int line)
{
    const GList *l;
    int i;
    const rpsl_attr_t *attr;
    const GList *errors;
    const rpsl_error_t *err;
    int num_attr;
    gboolean attr_in_list;
    int cnt;

    if (obj == NULL) {
        fprintf(stderr, "rpsl_object_check: NULL object at %s:%d\n",
                file, line);
        exit(1);
    }
    if (obj->attributes == NULL) {
        fprintf(stderr, "rpsl_object_check: NULL attributes at %s:%d\n",
                file, line);
        exit(1);
    }
    if (obj->attr_lookup == NULL) {
        fprintf(stderr, "rpsl_object_check: NULL attr_lookup at %s:%d\n",
                file, line);
        exit(1);
    }
    /* make sure each attribute in the hash is in the list */
    num_attr = g_list_length(obj->attributes);
    cnt = 0;
    g_hash_table_foreach(obj->attr_lookup, count_attr_in_list, &cnt);
    if (num_attr != cnt) {
        fprintf(stderr, 
            "rpsl_object_check: list count (%d) != hash count (%d) at %s:%d\n",
            num_attr, cnt,
            file, line);
        exit(1);
    }
    for (l=obj->attributes, i=0; l != NULL; l=g_list_next(l), i++) {
        attr = l->data;
        rpsl_attr_check(attr, file, line);
        /* make sure each attribute is in the hash table */
        l = g_hash_table_lookup(obj->attr_lookup, rpsl_attr_get_name(attr));
        attr_in_list = FALSE;
        while ((l != NULL) && !attr_in_list) {
            if (l->data == attr) {
                attr_in_list = TRUE;
            }
            l = g_list_next(l);
        }
        if (!attr_in_list) {
            fprintf(stderr, 
                    "rpsl_object_check: attr #%d not in hash for %p %s:%d\n",
                    i, obj, file, line);
            exit(1);
        }
        if (attr->num != i) {
            fprintf(stderr, 
                "rpsl_object_check: attr #%d does not match offset %d %s:%d\n",
                    attr->num, i, file, line);
            exit(1);
        }
    }
    rpsl_error_check(obj->errors, file, line);
    for (errors=attr->errors; errors != NULL; errors=g_list_next(errors)) {
        err = errors->data;
        if (err->attr_num >= num_attr) {
            fprintf(stderr, 
                "rpsl_object_check: attr_num (%d) >= num_attr (%d) at %s:%d\n",
                err->attr_num, num_attr, file, line);
            exit(1);
        }
    }
    /* XXX: think of a check for class_info... */
}
#endif /* RUNTIME_CHECK */

#ifdef TEST

#include <stdio.h>

/* for a test, check to make sure our we convert the following values into
   the expected results */
struct {
    gchar *input;
    gchar *expected_result;
} test_attr[] = {
    /* all tests on a single-line attributes */
    { "unmodified", "unmodified" },
    { "also unmodified", "also unmodified" },
    { "   leading whitespace", "leading whitespace" },
    { "trailing whitespace ", "trailing whitespace" },
    { "compressed    \t whitespace", "compressed whitespace" },
    { "value   # some comment", "value" },
    { " lots of      stuff# here too  ", "lots of stuff" },
    { "", "" },
    /* basic tests on multi-line attributes */
    { "multiple\n"
      " lines",
      "multiple lines" },
    { "multiple\n"
      "\ttablines",
      "multiple tablines" },
    { "multiple\n"
      "+pluslines",
      "multiple pluslines" },
    { "\n"
      " \n"
      "\t\n"
      "+\n",
      "" },
    /* multi-line whitespace tests */
    { " leading\n"
      " multiline whitespace",
      "leading multiline whitespace" },
    { "\tleading\n"
      "\ttabs multiline",
      "leading tabs multiline" },
    { "\t \tleading\n"
      "++ multiline",
      "leading + multiline" },
    { "trailing\n"
      " multiline   \t",
      "trailing multiline" },
    { "trailing\n"
      "\ttabful multiline     ",
      "trailing tabful multiline" },
    { "trailing\n" 
      "+plus multiline\t",
      "trailing plus multiline" },
    { "multiline   \n"
      "    whitespace   \n"
      "+compression",
      "multiline whitespace compression" },
    { "    more \t\tmultiline  \t\n"
      "+  whitespace \t \t\n"
      "+compression   \t",
      "more multiline whitespace compression" },
    /* multi-line comment tests */
    { "There # once was a man from Nantucket,\n"
      "\tWhose nic-hdl # fell in the bitbucket.\n"
      "\t\tHe grabbed his # nic-handle,\n"
      "\t\tAnd made the mail queue # full.\n"
      "\tBut # the mail bounced (we just chucked it).",
      "There Whose nic-hdl He grabbed his And made the mail queue But" },
    { " # this is an evil,\n"
      " # but legal,\n"
      " # thing to do",
      "" },
    { "this # is also \n"
      "+    # legal, but less evil I suppose\n",
      "this" },
    
};

#define NUM_TEST_ATTR (sizeof(test_attr) / sizeof(test_attr[0]))

int 
main()
{
    int i;
    gchar *actual_result;
    int num_error;

    num_error = 0;

    /* test the attribute_clean() function */
    for (i=0; i<NUM_TEST_ATTR; i++) {
        actual_result = attribute_clean(test_attr[i].input);
        if (strcmp(actual_result, test_attr[i].expected_result) != 0) {
            puts("ERROR: test failed");
            puts("--------[ input ]--------");
            puts(test_attr[i].input);
            puts("---[ expected result ]---");
            puts(test_attr[i].expected_result);
            puts("----[ actual result ]----");
            puts(actual_result);
            puts("-------------------------");
            num_error++;
        }
    }
    if (num_error == 0) {
        printf("SUCCESS: all tests passed\n");
        return 0;
    } else {
        return 1;
    }
}

#endif
