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

#ifndef SYNTAX_API_H
#define SYNTAX_API_H

/***********************************************************************
Introduction

The RIPE RPSL parsing API is intended to allow applications to verify
and manipulate RPSL objects in a straightforward manner.  

What it does:

* Syntax checks on attributes and objects

* Provides functions to access and manipulate attributes and objects

What it does not do:

* I/O (for loading or storing RPSL objects)

* Database interaction

* Non-syntax related checks (e.g. existance of objects referred to)



Usage

In a typical program, you need to initialize the data structures
representing an RPSL object, check for certain errors, manipulate the
data, and finally output the resulting object.  To do this:

1. Create a string representing your RPSL object.  This should be the
   literal text of the object, as described in the RPSL documentation.
   This can be read from a file or database, from user input, or created
   dynamically by the program.

2. Call rpsl_object_init(), verifying the call worked.

3. Call rpsl_object_errors(), and check to see if you care about any of
   the errors returned, possibly reporting them to the user.

4. Call rpsl_object_attr() for the attributes that you want to process.

5. For each attribute you want to use, call rpsl_attr_errors(), and
   check to see if you care about any of the errors returned, possibly
   reporting them to the user.

6. Remove unwanted attributes by rpsl_object_attr_delete().

7. Add new attributes by first calling rpsl_attr_init() and then
   rpsl_object_attr_insert() or rpsl_object_attr_append().

8. When done, call rpsl_object_get_text() to retrieve an RPSL formatted
   version of the object.


Data Structures

These are presented here primarily for enlightenment - often a quick
glance at a data structure can clarify the intent in ways that looking
at function descriptions never can.

************************************************************************/

/* FIXME: This should be an object. All this is a horrible c-ish hack to achieve that. The drawback
 * of this is that every function call has to check its input (which includes the object/attrib structures)
 * before proceeding. Apart from being very slow right where speed would be most important, it also
 * results in a lot of code duplication/repeat, which is very bad again.
 * A possible emulation of OO would be to add a 'dontcheck' flag to each structure (obj, attrib, ...) to
 * mark that it was already validated by rpsl code, and put the checking routines in dedicated functions,
 * each routine using only those.
 *  
 * FIXME: The data structure should better accomodate the below function's needs (so that they could
 * avoid iterating through the attribute list, for example - imagine dbupdate iterating through the attributes
 * of a 3 megabyte object, resulting O(n^2) time operation, brrr), as well as locking and init should be
 * redesigned (see FIXMEs about this scattered throughout the code) - agoston, 2007-11-02  */

/* typedefs allow for forward references within structure definitions */
typedef struct rpsl_object rpsl_object_t;
typedef struct rpsl_attr rpsl_attr_t;
typedef struct rpsl_error rpsl_error_t;
typedef struct rpsl_template rpsl_template_t;

/* strictness of checking */
enum {
    RPSL_DICT_CORE,
    RPSL_DICT_FRONT_END
};

/* various RPSL error levels (similar as syslog errors) */
enum {
    RPSL_ERRLVL_NONE,
    RPSL_ERRLVL_DEBUG,
    RPSL_ERRLVL_INFO,
    RPSL_ERRLVL_NOTICE,
    RPSL_ERRLVL_WARN,
    RPSL_ERRLVL_ERROR,
    RPSL_ERRLVL_CRIT,
    RPSL_ERRLVL_FATAL
};

/* error codes */
enum {
    /* attribute-related errors */
    RPSL_ERR_BADATTR, 
    RPSL_ERR_UNKNOWNATTR, 
    RPSL_ERR_EMPTYLIST,
    RPSL_ERR_EMPTYATTR,
    RPSL_ERR_SYNERR, 
    /* object-related errors */
    RPSL_ERR_ONLYCOMMENTS,
    RPSL_ERR_BADCLASS,
    RPSL_ERR_UNKNOWNCLASS,
    RPSL_ERR_ATTRNOTALLOWED,
    RPSL_ERR_ATTRSINGLE,
    RPSL_ERR_ATTRGENERATED,
    RPSL_ERR_MISSINGATTR,
    RPSL_ERR_MISSINGKEY,
    /* modification-related errors */
    RPSL_ERR_BADOFFSET,
    RPSL_ERR_NOSUCHATTR
};

/* per-attribute errors */
struct rpsl_error {
    gint level;               /* level of the error (enum above) */
    gint code;                /* code for the error */
    gchar *descr;             /* description of the error */
    gint attr_num;            /* offset of attribute with this error, or 
                                 -1 if none */
};

/* information about an attribute */
struct rpsl_attr {
    gchar *name;             /* name, e.g. "inetnum" or "person" */
    gchar *lcase_name;       /* lower-case version of the name */
    gchar *value;            /* value of the object, e.g. "192.168.0.0/24" */
    GList *errors;           /* any errors with this attribute */
    gint num;                /* Position of attribute.  
                                For class name, num = 0. 
                                For attributes not in a class, num = -1. */
    void *attr_info;         /* attribute information (INTERNAL USE ONLY) */
};

/* information about an object */
struct rpsl_object {
    GList *attributes;       /* ordered attributes for this object */
    GHashTable *attr_lookup; /* hash table used to do by-name lookups */
    GList *errors;           /* any errors with this object */
    void *class_info;        /* class information (INTERNAL USE ONLY) */
};

/* information about a class */
struct rpsl_template {
    gchar *name;                /* "person", "descr", ... */
    gchar *code;                /* "pn", "de", ... */
    gboolean is_required;       /* mandatory/optional */
    gboolean is_generated;      /* generated/user-specified */
    gboolean is_multivalued;    /* single/multiple */
    gboolean is_lookup;         /* lookup key */
    gboolean is_inverse;        /* inverse key */
    gboolean is_primary;        /* primary key */
    gboolean is_list;           /* RPSL list (comma-separated) */
    gboolean is_ripe_list;      /* RIPE list (space-separated) */   
};

/* default column to start data on */
#define RPSL_STD_COLUMN 16

/* Allow C++ code to link */
#ifdef __cplusplus
extern "C" {
#endif

/* prototypes */
extern rpsl_attr_t *rpsl_attr_init(const gchar *s, const gchar *class);
extern rpsl_attr_t *rpsl_attr_copy(const rpsl_attr_t *attr);
extern rpsl_attr_t *rpsl_attr_clean_copy(const rpsl_attr_t *attr);
extern void rpsl_attr_delete(rpsl_attr_t *attr);
extern void rpsl_attr_delete_list(GList *attributes);
extern const gchar *rpsl_attr_get_name(const rpsl_attr_t *attr);
extern gint rpsl_attr_get_ofs(const rpsl_attr_t *attr);
extern const gchar *rpsl_attr_get_value(const rpsl_attr_t *attr);
extern gchar *rpsl_attr_get_clean_value(const rpsl_attr_t *attr);
extern gchar *rpsl_attr_get_clean_lines(const rpsl_attr_t *attr);
extern GList *rpsl_attr_get_split_list(const rpsl_attr_t *attr);
extern void rpsl_attr_split_multiple (GList **attrs);
extern void rpsl_attr_replace_value(rpsl_attr_t *attr, const gchar *value);
extern const GList *rpsl_attr_errors(const rpsl_attr_t *attr);

extern GList **rpsl_mnt_routes_clean_split (rpsl_attr_t *attr, GList **prefix_range_list, GList **invalid_list);
extern GList **rpsl_attr_uniq_list (GList **attr_list);

extern rpsl_object_t *rpsl_object_init(const gchar *s);
extern rpsl_object_t *rpsl_object_copy(const rpsl_object_t *object);
extern rpsl_object_t *rpsl_object_copy_flattened(const rpsl_object_t *object);
extern void rpsl_object_delete(rpsl_object_t *object);
extern const char *rpsl_object_get_class(const rpsl_object_t *object);
extern gchar *rpsl_object_get_text(const rpsl_object_t *object, 
                                   guint data_column);
extern gint rpsl_object_get_num_attr(const rpsl_object_t *object);
extern const GList *rpsl_object_get_all_attr(const rpsl_object_t *object);
extern GList *rpsl_object_get_attr(const rpsl_object_t *object, 
                                   const gchar *name);
extern const rpsl_attr_t *rpsl_object_get_attr_by_ofs(const rpsl_object_t *object,
                                                      gint ofs);
extern gchar *rpsl_object_get_key_value(const rpsl_object_t *object);

extern int rpsl_object_append_attr(rpsl_object_t *object, 
                                   rpsl_attr_t *attr,
                                   rpsl_error_t *error);
extern int rpsl_object_add_attr(rpsl_object_t *object, 
                                rpsl_attr_t *attr, 
                                gint ofs,
                                rpsl_error_t *error);
extern int rpsl_object_add_first_attr(rpsl_object_t *object, 
                                rpsl_attr_t *attr, 
                                rpsl_error_t *error);
extern int rpsl_object_add_attr_internal(rpsl_object_t *object, 
                                rpsl_attr_t *attr, 
                                gint ofs,
                                rpsl_error_t *error);
extern rpsl_attr_t *rpsl_object_remove_attr(rpsl_object_t *object, 
                                            gint ofs,
                                            rpsl_error_t *error);
extern rpsl_attr_t *rpsl_object_remove_first_attr(rpsl_object_t *object,
                                            rpsl_error_t *error);
extern rpsl_attr_t *rpsl_object_remove_attr_internal(rpsl_object_t *object,
                                            gint ofs,
                                            rpsl_error_t *error);
extern rpsl_attr_t *rpsl_object_remove_attr_name(rpsl_object_t *object, 
                                                 const gchar *name,
                                                 rpsl_error_t *error);
extern const GList *rpsl_object_errors(const rpsl_object_t *object);
  
extern gboolean rpsl_attr_is_required(const rpsl_object_t *object, 
                                      const gchar *attr);
extern gboolean rpsl_attr_is_multivalued(const rpsl_object_t *object,
                                         const gchar *attr);
extern gboolean rpsl_attr_is_lookup(const rpsl_object_t *object,
                                    const gchar *attr);
/* checks if attr is a primary key */
extern gboolean rpsl_attr_is_key(const rpsl_object_t *object,
                                 const gchar *attr);
extern gboolean rpsl_attr_is_generated(const rpsl_object_t *object,
                                       const gchar *attr);
extern gboolean rpsl_object_is_deleted(const rpsl_object_t *object);

extern gboolean rpsl_attr_has_error(const rpsl_attr_t *attr, int error_level);
extern gboolean rpsl_object_has_error(const rpsl_object_t *object, 
                                      int error_level);
extern void rpsl_error_free(rpsl_error_t *error);
extern void rpsl_error_all_free(rpsl_error_t *error);
extern void rpsl_error_list_free(GList *errors);

gint rpsl_get_attr_id(const gchar *attr_name);
gint rpsl_get_class_id(const gchar *class_name);

extern void rpsl_load_dictionary(int level);
extern int rpsl_read_dictionary();

extern const gchar* const *rpsl_get_classes();
extern const rpsl_template_t* const *rpsl_get_template(const gchar *class);

#ifdef __cplusplus
}
#endif  /* C++ */

#endif /* SYNTAX_API_H */
