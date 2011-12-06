/***************************************
 $Revision: 1.18.2.1 $

 Query instructions (qi).  This is where the queries are executed.

 Status: NOT REVUED, TESTED

 ******************//******************
 Filename            : query_instructions.c
 Authors             : ottrey@ripe.net - framework and draft implementation
 marek@ripe.net - cleaned and extended, added referral,
 accounting support and watchdog cancellation.
 OSs Tested          : Solaris
 ******************//******************
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
#include "rip.h"

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <glib.h>

/* maximum number of names to query for */
/*   MySQL only allows 31 tables in a single query, so this limits us */
/*   As of 2002-03-04 the most names in a person object are 14 */
#define MAX_NAMES 30

/* logging contexts */
static LG_context_t *qi_context;
static LG_context_t *sql_context;

/*
--- snipped from http://www.tcx.se/Manual/manual.html ---

5.3 Functionality missing from MySQL

The following functionality is missing in the current version of MySQL. For a prioritized list indicating when new extensions may be added to MySQL, you should consult the
online MySQL TODO list. That is the latest version of the TODO list in this manual. See section F List of things we want to add to MySQL in the future (The TODO).

5.3.1 Sub-selects

The following will not yet work in MySQL:

SELECT * FROM table1 WHERE id IN (SELECT id FROM table2);
SELECT * FROM table1 WHERE id NOT IN (SELECT id FROM table2);

However, in many cases you can rewrite the query without a sub select:

SELECT table1.* FROM table1,table2 WHERE table1.id=table2.id;
SELECT table1.* FROM table1 LEFT JOIN table2 ON table1.id=table2.id where table2.id IS NULL

For more complicated sub queries you can create temporary tables to hold the sub query.

MySQL only supports INSERT ... SELECT ... and REPLACE ... SELECT ... Independent sub-selects will be probably be available in 3.24.0. You can now use the function IN() in other
contexts, however.

--- end snip ---

Ie. Try using a LEFT JOIN to do the "NOT IN"/ "MINUS" equivalent.
*/

/*
  mysql optimizer is sometimes sub-optimal,
  therefore we force the join order with STRAIGHT_JOIN (MB, 2000/05/02)
*/

/* RIPE 6 */
/*#define Q_OBJECTS     "SELECT last.object_id, last.sequence_id, last.object ,last.object_type FROM  %s IDS STRAIGHT_JOIN last,object_order WHERE last.object_id=IDS.id AND last.object_type != 100 AND last.object_type = object_order.object_type ORDER BY recursive, order_code"
 */

const char *Q_OBJECTS = "SELECT STRAIGHT_JOIN last.object_id, last.sequence_id, last.object, last.object_type, last.pkey, recursive, gid "
                        "FROM %s IDS, last, last glast, object_order, object_order gorder "
                        "WHERE (IDS.gid=glast.object_id AND glast.object_type=gorder.object_type) AND "
                        "(IDS.id=last.object_id AND last.object_type=object_order.object_type) AND last.thread_id = 0 AND glast.thread_id = 0 "
                        "ORDER BY %s recursive, object_order.order_code";

/* rewritten for JOIN syntax by agoston, 2011-03-30

SELECT last.object_id, last.sequence_id, last.object, last.object_type, last.pkey, recursive, gid
FROM %s IDS
JOIN last glast ON IDS.gid = glast.object_id
JOIN object_order gorder ON glast.object_type = gorder.object_type
JOIN last ON IDS.id = last.object_id
JOIN object_order ON last.object_type = object_order.object_type
ORDER BY %s recursive, object_order.order_code

*/

const char *Q_ALTER_TMP = "ALTER TABLE %s ADD COLUMN gid INT NOT NULL DEFAULT 0";
const char *Q_ALTER_TMP_GROUPED = "ALTER TABLE %s ADD COLUMN gid INT NOT NULL DEFAULT 0, DROP PRIMARY KEY, ADD PRIMARY KEY (id, gid)";
const char *Q_UPD_TMP = "UPDATE %s SET gid=id";

const char *LIST_HAS_ATTR = "SELECT DISTINCT gid FROM %s IDS,%s REF WHERE IDS.id=REF.object_id";

/* Query for finding person/role objects recursively (when -r  isn't specified) */
#define Q_REC "INSERT IGNORE INTO %s SELECT pe_ro_id,1,object_id FROM %s IDS, %s WHERE object_id = IDS.id"
/* Query for finding organisation objects recursively (when -r isn't specified) */
#define Q_REC_ORG "INSERT IGNORE INTO %s SELECT org_id,1,object_id FROM %s IDS, %s WHERE object_id = IDS.id"
/* Query for finding irt objects recursively (when -c (or -b) is specified) */
#define Q_REC_IRT "INSERT IGNORE INTO %s SELECT irt_id,1,%s FROM %s WHERE object_id = %d"

Ref_queries ref_queries[] = { { Q_REC, "author" }, { Q_REC, "admin_c" }, { Q_REC, "tech_c" }, { Q_REC, "zone_c" }, { Q_REC_ORG, "org" }, { "", "" } };

/*++++++++++++++++++++++++++++++++++++++
 Function invoked on query cancellation by the watchdog,
 used from the sql_execute_watched() function.

 It aborts the running query (the abort function in sq kills and
 reestablished the connection).

 void *qi_kill_body         result of sq_execute_query, int cast to (void*)

 void *arg                  pointer to sql connection

 Author:
 marek.

 ++++++++++++++++++++++++++++++++++++++*/
static
void *qi_kill_body(void *arg) {
    SQ_connection_t *sql_connection = arg;
    LG_log(qi_context, LG_DEBUG, "rtc: killing SQL connection %d", (sql_connection)->thread_id);
    /* abort the running query */
    SQ_abort_query(sql_connection);

    return NULL;
}

/*++++++++++++++++++++++++++++++++++++++
 wrapper around sq_execute_query: starts a query
 in a separate thread and starts the socket watchdog to cancel the query
 if the socket is closed. If the socket has problems already (its
 reason-to-close flag is set) no query is attempted.

 The execution of the query or watchdog is not guaranteed at all!

 int sql_execute_watched        Returns the return code of SQ_execute_query,
 Returns 0 for cancelled queries.

 sk_conn_st *condat             connection to watch

 SQ_connection_t **sql_connection  sql connection

 const char *query                 sql query to execute

 SQ_result_set_t **result_ptr      storage for the query result structure
 (passed to SQ_execute_query). Must either
 be NULL, or the pointer it points to must
 be NULL - in that case the result struct.
 will be allocated in SQ.

 Author:
 marek.
 ++++++++++++++++++++++++++++++++++++++*/
int sql_execute_watched(sk_conn_st *condat, SQ_connection_t **sql_connection, const char *query, SQ_result_set_t **result_ptr) {
    int retval = 0; /* return value of sq_execute_query */
    SQ_connection_t *tempcon;

    /* assert that, if defined, result_ptr is initialised to NULL
     prior to calling this function */
    if (result_ptr != NULL) {
        dieif( *result_ptr != NULL );
    }

    /* don't even try to perform the query/fire up watchdog
     if rtc is already set. Do this only if not set yet. */
    if (condat->rtc == 0) {

        /* make clean */
        SK_watch_setclear(condat);

        /* set watchdog to execute the abort function */
        SK_watch_setexec(condat, qi_kill_body, *sql_connection);

        /* start the watchdog */
        SK_watchstart(condat);

        /* start query. An error may be returned if the query is aborted */
        retval = SQ_execute_query(*sql_connection, query, result_ptr);

        /* but short queries will complete before the watchdog kills the
         connection */

        SK_watchstop(condat);

        /* if the watchdog triggered, then it is guaranteed that
         the kill_body function was invoked and therefore the sql-connection
         is now unusable...
         Close and reopen it for cleanup, use temporary connection
         to keep the login details */
        if (condat->rtc != 0) {
            /* can't rely on the error code from mysql!
             */

            /* one thing: this code must be entered ONLY if the kill_body
             thing was invoked by the watchdog.
             */

            /* if result is defined, free it here before destroying the
             associated connection */
            if (retval == 0 && result_ptr && *result_ptr) {
                SQ_free_result(*result_ptr);
                *result_ptr = NULL;
            }

            tempcon = SQ_duplicate_connection(*sql_connection);

            LG_log(qi_context, LG_DEBUG, "rtc: closing SQL thread %d", (*sql_connection)->thread_id);
            SQ_close_connection(*sql_connection);

            *sql_connection = tempcon;
            LG_log(qi_context, LG_DEBUG, "rtc: reopened as thread %d", (*sql_connection)->thread_id);

            /* make it look as if there was no error and
             the result is empty */
            retval = 0;
        } /* if watchdog set rtc */

    } /* if rtc not set before */

    return retval;
}

/*++++++++++++++++++++++++++++++++++++++
 Create an sql query for the names table.
 ++++++++++++++++++++++++++++++++++++++*/
static void qi_create_name_query(GString *query_str, const char *sql_query, const char *keys, const char *table) {
    int i;
    GString *from_clause = g_string_sized_new(STR_L);
    GString *where_clause = g_string_sized_new(STR_L);
    gchar **words = ut_g_strsplit_v1((char*) keys, " ", 0);

    /* double quotes " are used in queries to allow querying for names like O'Hara */

    /* We should use SQ_escape() here as well, but we don't yet have an sql connection here yet.
     * Luckily, qc->keys is already checked to consist only of ALLOWED_QUERY_CHARS
     * AND we also use "" as a string marker, so it is not necessary to escape here
     * Also, if ' is used in a query, the ONLY matching keytype is NAME, for which the SQL query
     * is generated here - agoston, 2011-10-26 */

    if (words[0] != NULL) {
        g_string_sprintfa(from_clause, "%s N%.2d", table, 0);
        g_string_sprintfa(where_clause, "N%.2d.name=\"%s\"", 0, words[0]);

        for (i = 1; (i < MAX_NAMES) && (words[i] != NULL); i++) {
            g_string_sprintfa(from_clause, ", %s N%.2d", table, i);
            g_string_sprintfa(where_clause, " AND N%.2d.name=\"%s\" AND N00.object_id = N%.2d.object_id", i, words[i], i);
        }
    }

    g_string_sprintf(query_str, sql_query, from_clause->str, where_clause->str);

    g_strfreev(words);
    g_string_free(where_clause, TRUE);
    g_string_free(from_clause, TRUE);

}

/*++++++++++++++++++++++++++++++++++++++
 construct a range query for the as_block table
 (a query for an AS block object) given a string like:
 AS1
 AS1 - AS10
 AS1-AS10

 int create_asblock_query    Returns 0 on success, -1 on failure
 (search term not an AS# nor range)

 char *query_str             buffer for the final query (must be big enough)

 const char *sql_query       rest of the sql query (with %d %d formats for
 AS numbers)

 const char *keys            user-supplied search term.

 Author:
 marek
 ++++++++++++++++++++++++++++++++++++++*/
static int create_asblock_query(GString *query_str, const char *sql_query, Query_command *qc) {
    gchar **tok;
    unsigned long asn[2];
    int i;

    /* tokenize & count how many tokens we have */
    tok = g_strsplit(qc->keys, "-", -1);
    for (i = 0; tok[i]; i++) {
        if (i >= 2)
            goto error_return;

        if (convert_as(tok[i], &asn[i]))
            goto error_return;
    }

    /* now construct the query */
    /* if only beginning was supplied (single token), use it also as end */
    if (i == 1) {
        asn[1] = asn[0];
    } else {
        if (asn[1] < asn[0]) {
            qc->parse_messages = g_list_append(qc->parse_messages, ca_get_qi_badrange);
            goto error_return;
            /* error */
        }
    }
    g_string_sprintf(query_str, sql_query, asn[0], asn[1]);
    g_strfreev(tok);
    return 0;

    error_return: g_strfreev(tok);
    return -1;
}

/* construct a query to limit the objects returned from the last
   table to predefined types.

   query_str           buffer for the final query, containing the initial
                       part of the query (must be big enough)

   qc                  query command structure with the bitmap of
                       object types to be included.
*/
static void add_filter(GString *query_str, const Query_command *qc) {
    unsigned i;

    /* add filters only if any bits are 0 (the number of 1's is < MAX_MAX */
    if (MA_bitcount(qc->object_type_bitmap) != MASK_MAX) {
        g_string_append(query_str, " AND (");
        for (i = 0; i < C_END; i++) {
            if (MA_isset(qc->object_type_bitmap, i)) {
                g_string_append_printf(query_str, "i.object_type = %d OR ", DF_get_class_dbase_code(i));
            }
        }

        /* this code can only be reached if there is at least one object here, meaning this
           must end with the string "OR ", which we then remove */
        dieif(query_str->len < 3);
        g_string_truncate(query_str, query_str->len - 3);
        g_string_append_c(query_str, ')');
    }
}

/* create_query() */
/*++++++++++++++++++++++++++++++++++++++
 Create an sql query from the query_command and the matching keytype and the
 selected inverse attributes.
 Note this clears the first inv_attribute it sees, so is called sequentially
 until there are no inv_attributes left.

 WK_Type keytype The matching keytype.

 Query_command *qc The query command.
 (This is no longer const to allow error messages further down the line)

 mask_t *inv_attrs_bitmap The selected inverse attributes.

 More:
 +html+ <PRE>
 Authors:
 ottrey
 +html+ </PRE>

 ++++++++++++++++++++++++++++++++++++++*/
static char *create_query(const Query_t q, Query_command *qc) {
    GString *result_buff;
    char *result;
    Q_Type_t querytype;
    int addquery = 0; /* controls if the query should be added to the list */

    result_buff = g_string_sized_new(STR_XL);

    if (MA_bitcount(qc->inv_attrs_bitmap) > 0) {
        querytype = Q_INVERSE;
    } else {
        querytype = Q_LOOKUP;
    }

    if ((q.query != NULL) && (q.querytype == querytype)) {

        if (q.keytype == WK_NAME) {
            /* Name queries require special treatment. */
            if (q.class == C_OA) {
                qi_create_name_query(result_buff, q.query, qc->keys, "org_name");
            } else {
                qi_create_name_query(result_buff, q.query, qc->keys, "names");
            }

            addquery = 1;
        } else if (q.keytype == WK_IPADDRESS) { /* ifaddr sql lookups */
            ip_range_t myrang;
            unsigned begin, end;
            int err;

            /* The only inverse query for IPADDRESS is nserver. */
            /* We need to insure that we don't try to use the numeric values for this
             * query, because the address of the server is stored as a string, and
             * the SQL query is formatted appropriately. */
            if (NOERR(err = IP_smart_range(qc->keys, &myrang, IP_EXPN, NULL))) {
                if (IP_rang_b2_space(&myrang) == IP_V4) {
                    IP_rang_b2v4(&myrang, &begin, &end);
                    if (querytype == Q_INVERSE) {
                        /* for inverse queries, convert number to dotted-quad */
                        char buf[64];
                        const char *inet_ntop_ret;
                        inet_ntop_ret = inet_ntop(AF_INET, &begin, buf, sizeof(buf));
                        dieif(inet_ntop_ret == NULL);
                        g_string_sprintf(result_buff, q.query, buf);
                    } else {
                        /* otherwise, execute appropriate query on numeric values */
                        g_string_sprintf(result_buff, q.query, begin, end);
                    }
                    addquery = 1;
                } else {
                    die;
                }
            }
        } else if (q.keytype == WK_IP6PREFIX) { /* "interface:" sql lookup */
            ip_prefix_t ip_prefix;
            char *ipv6_prefix;
            ip_v6word_t high;
            ip_v6word_t low;
            int ret;
            unsigned prefix_length;

            /* first make sure the address has a prefix length part, because IP_pref_a2v6 requires it */
            if (index(qc->keys, '/') == NULL) { /* if the query does not have a prefix length */
                ipv6_prefix = g_strdup_printf("%s/128", qc->keys);
            } else {
                ipv6_prefix = g_strdup_printf("%s", qc->keys);
            }

            ret = IP_pref_a2v6(ipv6_prefix, &ip_prefix, &high, &low, &prefix_length);
            UT_free(ipv6_prefix);

            g_string_sprintf(result_buff, q.query, high, low);

            addquery = 1;
        } else if (q.keytype == WK_ASRANGE) { /* as_block range composition */
            if (create_asblock_query(result_buff, q.query, qc) != 0) {
                addquery = 0; /* ... unless it's not correct */
            } else {
                addquery = 1;
            }
        } else {
            g_string_sprintf(result_buff, q.query, qc->keys);
            addquery = 1;
        }

        if (q.class == C_ANY && addquery == 1) {
            /* It is class type ANY so add the object filtering */
            add_filter(result_buff, qc);
        }
    }

    if (addquery == 1) {
        result = UT_strdup(result_buff->str);
    } else {
        result = NULL;
    }
    g_string_free(result_buff, TRUE);

    return result;
}

/* QI_fast_output() */
/*++++++++++++++++++++++++++++++++++++++
 This is for the '-F' flag.
 It assumes lines starting with ' ', '\t' or '+' belong to the prior attribute.
 Fast isn't fast anymore - it's just there for compatibility reasons.

 const char *str        The object to be "fast output'ed".

 More:
 +html+ <PRE>
 Authors:
 ottrey,
 marek - glib strings + small changes
 +html+ </PRE>
 ++++++++++++++++++++++++++++++++++++++*/
char *QI_fast_output(const char *str) {
    int i, j;
    char *result;
    GString *result_buff = g_string_sized_new(STR_XL);
    gchar **lines = g_strsplit((char*) str, "\n", 0);
    unsigned char *value, *colon;
    char *attr;

    g_string_assign(result_buff, "");

    for (j = 0; lines[j] != NULL; j++) {

        switch (lines[j][0]) {
        /* line continuation */
        case ' ':
        case '\t':
        case '+':
            value = (unsigned char *) lines[j] + 1;
            while (*value != '\0' && isspace(*value)) {
                value++;
            }
            g_string_append(result_buff, "\n+ ");
            g_string_append(result_buff, (char *) value);
            break;

        case 0:
            // this is the ending empty newline
            g_string_append_c(result_buff, '\n');
            break;

        default:
            /* a line of the form "attribute: value" */
            /* first: close the last line (if there was any, i.e. j>0) */
            if (j > 0) {
                g_string_append_c(result_buff, '\n');
            }

            /* get attribute name */
            attr = lines[j];
            colon = (unsigned char *) strchr(lines[j], ':');
            /* if there's no colon for whatever reason, dump the object
             and report the condition */
            if (colon == NULL) {
                LG_log(qi_context, LG_ERROR, "Object [%s], line %d has no : on the line", lines[0], j);
                goto fast_output_cleanup;
            }
            *colon = '\0';
            for (value = colon + 1; *value != '\0' && isspace(*value); value++);

            if (((i = DF_attribute_name2type(attr)) == -1)) {
                /* warning! error in the object format */
                LG_log(qi_context, LG_ERROR, "No attribute code found for attribute [%s] in object [%s], line %d", lines[j], lines[0], j);
                goto fast_output_cleanup;

            } else {
                /* This is the juicy bit that converts the likes of; "source: RIPE" to "*so: RIPE" */
                g_string_append_c(result_buff, '*');
                g_string_append(result_buff, DF_get_attribute_code(i));
                g_string_append(result_buff, ": ");
                g_string_append(result_buff, (char *) value);
            }

            *colon = ':';   // set it back to original value (e.g. for first line that we use for logging to identify the object
        }
    }

    fast_output_cleanup:

    g_strfreev(lines);
    result = UT_strdup(result_buff->str);
    dieif(result == NULL);

    g_string_free(result_buff, TRUE);

    return result;
}

/* brief_filter() */
/*++++++++++++++++++++++++++++++++++++++

 This is for -b query flag. Assumes objects are grouped (to have recursive results by relevance).
 No object separators (in the group).
 The code is basically inverted contact_attr_filter().
 Merging those two functions makes them unreadable, hence a separate function.

 const char *str  - object to be filtered

 */
char *brief_filter(const char *str) {

    char *result;
    char *attr_list[3] = { "inetnum", "inet6num", "abuse-mailbox" };
    int i, j;
    gboolean filtering_an_attribute = FALSE;
    gboolean filtered = FALSE;
    GString *result_buff = g_string_sized_new(STR_XL);
    gchar **lines = ut_g_strsplit_v1((char*) str, "\n", 0);

    g_string_assign(result_buff, "");

    for (j = 0; lines[j] != NULL; j++) {
        for (i = 0; i < sizeof(attr_list) / sizeof(char *); i++) {
            int attr_name_len = strlen(attr_list[i]);
            if ((strncmp(attr_list[i], lines[j], attr_name_len) == 0) && (lines[j][attr_name_len] == ':')) {
                filtering_an_attribute = TRUE;
                filtered = TRUE;
                g_string_sprintfa(result_buff, "%s\n", lines[j]);
                break;
            }
        }
        if (filtered == TRUE) {
            filtered = FALSE;
            continue; /* go to next line */
        }
        if (filtering_an_attribute == TRUE) {
            switch (lines[j][0]) {
            case ' ':
            case '\t':
            case '+':
                g_string_sprintfa(result_buff, "%s\n", lines[j]);
                break;

            default:
                filtering_an_attribute = FALSE;
            }
        } else {
            filtering_an_attribute = FALSE;
        }
    }

    g_strfreev(lines);
    result = UT_strdup(result_buff->str);
    g_string_free(result_buff, TRUE);

    return result;
}

/* contact_attr_filter() */
/*++++++++++++++++++++++++++++++++++++++

 This is a default behaviour and can be switched off with '-B' flag.
 If there is 'abuse-mailbox:' attribute in the object,
 'e-mail:', 'notify:' and 'changed:' attributes will be removed from the object.
 If there is no 'abuse-mailbox:' attribute, 'notify:' and 'changed:' will be removed
 from the object.

 const char *str  - object to be filtered
 unsigned abuse_attr_exists - is there abuse-mailbox attr (in the group or in the list)

 */
char *contact_attr_filter(const char *str, unsigned abuse_attr_exists) {

    char *result;
    char *attr_list[7] = { "e-mail", "notify", "changed", "ref-nfy", "mnt-nfy", "upd-to", "source" /* special case, please keep it last element */
    };
    int i, j, start = 1;
    gboolean filtering_an_attribute = FALSE;
    gboolean filtering_source_attribute = FALSE;
    gboolean filtered = FALSE;
    GString *result_buff = g_string_sized_new(STR_XL);
    GString *source_attr = g_string_sized_new(STR_XL);
    gchar **lines = ut_g_strsplit_v1((char*) str, "\n", 0);

    g_string_assign(result_buff, "");
    g_string_assign(source_attr, "");

    if (abuse_attr_exists == TRUE) {
        start = 0;
    }

    for (j = 0; lines[j] != NULL; j++) {
        for (i = start; i < sizeof(attr_list) / sizeof(char *); i++) {
            int attr_name_len = strlen(attr_list[i]);
            if ((strncmp(attr_list[i], lines[j], attr_name_len) == 0) && (lines[j][attr_name_len] == ':')) {
                if (i == 6) /* this is source attribute */{
                    filtering_source_attribute = TRUE;
                    g_string_sprintfa(source_attr, "%s", lines[j]);
                }
                filtering_an_attribute = TRUE;
                filtered = TRUE;
                break;
            }
        }
        if (filtered == TRUE) {
            filtered = FALSE;
            continue; /* go to next line */
        }
        if (filtering_an_attribute == TRUE) {
            switch (lines[j][0]) {
            case ' ':
            case '\t':
            case '+':
                /* do nothing for all except source */
                if (filtering_source_attribute == TRUE) {
                    g_string_sprintfa(source_attr, "\n%s", lines[j]);
                }
                break;

            default:
                if (filtering_source_attribute == TRUE) {
                    g_string_sprintfa(result_buff, "%s # Filtered\n", source_attr->str);
                    filtering_source_attribute = FALSE;
                }
                g_string_sprintfa(result_buff, "%s\n", lines[j]);
                filtering_an_attribute = FALSE;
            }
        } else {
            g_string_sprintfa(result_buff, "%s\n", lines[j]);
            filtering_an_attribute = FALSE;
        }
    }

    /* this is needed if source was the last attribute */
    if (filtering_source_attribute == TRUE) {
        g_string_sprintfa(result_buff, "%s # Filtered\n", source_attr->str);
    }

    g_strfreev(lines);
    result = UT_strdup(result_buff->str);
    g_string_free(result_buff, TRUE);
    g_string_free(source_attr, TRUE);
    return result;

}

/* filter() */
/*++++++++++++++++++++++++++++++++++++++
 Basically it's for the '-K' flag for non-set (and non-radix) objects.
 It assumes lines starting with ' ', '\t' or '+' belong to the prior attribute.

 This could be speed up if there were breaks out of the loops, once it matched something.

 const char *string          The string to be filtered.

 More:
 +html+ <PRE>
 Authors:
 ottrey
 +html+ </PRE>

 ++++++++++++++++++++++++++++++++++++++*/
char *filter(const char *str) {
    int i, j, passed = 0;
    char *result;
    GString *result_buff = g_string_sized_new(STR_XL);
    gchar **lines = ut_g_strsplit_v1((char*) str, "\n", 0);
    char * const *filter_names;
    gboolean filtering_an_attribute = FALSE;

    filter_names = DF_get_filter_names();

    g_string_assign(result_buff, "");

    for (i = 0; filter_names[i] != NULL; i++) {
        for (j = 0; lines[j] != NULL; j++) {
            /* match lines that start with "key-field:" */
            int filter_name_len = strlen(filter_names[i]);
            if ((strncmp(filter_names[i], lines[j], filter_name_len) == 0) && (lines[j][filter_name_len] == ':')) {

                g_string_sprintfa(result_buff, "%s\n", lines[j]);
                passed++;

                /* CONSTCOND */
                filtering_an_attribute = TRUE;
            }
            /* CONSTCOND */
            else if (filtering_an_attribute == TRUE) {
                switch (lines[j][0]) {
                case ' ':
                case '\t':
                case '+':

                    g_string_sprintfa(result_buff, "%s\n", lines[j]);

                    break;

                default:
                    filtering_an_attribute = FALSE;
                }
            }
        }
    }

    g_strfreev(lines);

    if (passed) {
        g_string_append(result_buff, "\n");
    }
    result = UT_strdup(result_buff->str);
    g_string_free(result_buff,/* CONSTCOND */TRUE);

    return result;
} /* filter() */

/*++++++++++++++++++++++++++++++++++++++
 Write the results to the client socket.

 SQ_result_set_t *result     The result set returned from the sql query.
 Query_instructions *qis     query instructions.
 ++++++++++++++++++++++++++++++++++++++*/
static int write_results(SQ_result_set_t *result, Query_instructions *qis, GHashTable *groups) {
    sk_conn_st *condat = &(qis->qe->condat);
    SQ_row_t *row;
    char *str;
    char *id;
    char *filtrate;
    char *fasted;
    char *cont_filter;
    int retrieved_objects = 0;
    char *objt;
    char *pkey = NULL;
    char *rec;
    int recursive;
    int type;
    long gid;
    int abuse_attr_exists = 0;
    unsigned int filtered = qis->qc->filtered; /* if the objects should go through a filter(-K) */
    unsigned int fast = qis->qc->fast; /* fast output */
    unsigned int grouped = qis->qc->G; /* grouped output */
    unsigned int original = qis->qc->B || qis->source->isGRS || qis->qc->filtered; /* original output */
    unsigned int brief = qis->qc->b; /* brief output */
    gboolean isPrivate = FALSE;

    /* Get all the results - one at a time */
    if (result != NULL) {

        /* here we are making use of the mysql_store_result capability
         of interrupting the cycle of reading rows. mysql_use_result
         would not allow that, would have to be read until end */

        while (condat->rtc == 0 && AC_credit_isdenied(qis->qe->acc_credit) == 0 && (row = SQ_row_next(result)) != NULL) {

            if ((original == 0) && (retrieved_objects == 0)) {
                /* this is DEFAULT */
                char *banner = ca_get_qi_filter_banner;
                SK_cd_puts(condat, banner);
                SK_cd_puts(condat, "\n");
                free(banner);
            }

            dieif ( (id = SQ_get_column_string(result, row, 0)) == NULL )
            dieif ( (objt = SQ_get_column_string(result, row, 3)) == NULL );
            if (grouped == 1) {
                UT_free(pkey);
                pkey = SQ_get_column_string(result, row, 4);
                dieif ( pkey == NULL );
                dieif ( (rec = SQ_get_column_string(result, row, 5)) == NULL );
                recursive = atoi(rec);
                UT_free (rec);
                dieif ( (rec = SQ_get_column_string(result, row, 6)) == NULL );
                gid = atol(rec);
                UT_free (rec);
            } /* if grouped == 1 */

            /* get + add object type */
            type = atoi(objt);

            /* ASP_QI_LAST_DET */
            //LG_log(qi_context, LG_DEBUG, "Retrieved serial id = %d , type = %s", atoi(id), objt);

            /* decide to account for private of public object */
            if (qis->source->isGRS || qis->qc->filtered) {
                isPrivate = FALSE;
            } else {
                /* FIXME: if -B is not used, we should not account for private objects, but have a proper filter that truly removes all private information
                 * agoston, 2011-03-04 */
                isPrivate = (type == C_PN || type == C_RO || type == C_MT || type == C_OA);
            }

            /* decrement credit for accounting purposes */
            AC_count_object(qis->qe->acc_credit, qis->qe->acl, isPrivate);

            /* person, role, mntner and organisation objects are counted as private objects */
            /* We don't count IRT objects as private as we want users to see them always    */
            /* break the loop if the credit has just been exceeded and */
            /* further results denied */
            if (AC_credit_isdenied(qis->qe->acc_credit)) {
                continue;
            }

            if ((str = SQ_get_column_string(result, row, 2)) == NULL) {
                die;
            } else {
                UT_free(id);
                UT_free(objt);

                /* brief output - for inetnum, inet6num, irt, person, role */
                if (brief == 1) {
                    cont_filter = brief_filter(str);
                    UT_free(str);
                    str = cont_filter;
                }

                /* filter out contact info - DEFAULT */
                if (original == 0) {
                    if (groups != NULL) {
                        if (((grouped == 1) && (g_hash_table_lookup(groups, (long*) gid) != NULL)) || ((grouped == 0) && (g_hash_table_size(groups) > 0))) {
                            abuse_attr_exists = 1;
                        }
                    }
                    cont_filter = contact_attr_filter(str, abuse_attr_exists);
                    UT_free(str);
                    str = cont_filter;
                }

                /* The fast output stage */
                if (fast == 1) {
                    fasted = QI_fast_output(str);
                    UT_free(str);
                    str = fasted;
                } /* if fast == 1 */

                /* The filtering stage */
                if (filtered == 0) {
                    if ((grouped == 1) && (recursive == 0)) {
                        char banner[STR_XL];
                        char *fmt_banner = ca_get_qi_fmt_group_banner;
                        sprintf (banner, fmt_banner, pkey);
                        if (pkey != NULL) {
                            UT_free(pkey);
                        }
                        pkey = NULL;
                        free(fmt_banner);
                        SK_cd_puts(condat, banner);
                        SK_cd_puts(condat, "\n");
                    }
                    if ((brief == 1) && (recursive == 0) && (retrieved_objects > 0)) {
                        SK_cd_puts(condat, "\n");
                    }
                    SK_cd_puts(condat, str);
                    if (brief == 0) {
                        SK_cd_puts(condat, "\n");
                    }
                } else {

                    /* XXX accounting should be done AFTER filtering, not to count
                     objects filtered out */

                    filtrate = filter(str);
                    SK_cd_puts(condat, filtrate);
                    UT_free(filtrate);
                }
                retrieved_objects++;
            }
            UT_free(str);
        } /* while */

        if (filtered == 0 && brief == 1 && retrieved_objects > 0) {
            SK_cd_puts(condat, "\n"); /* ending \n for brief output */
        }
    } /* if (result != NULL) */

    if (pkey != NULL) {
        UT_free(pkey);
    }
    return retrieved_objects;
} /* write_results() */


/* use a macro so we can get our file and line number */
#define report_sql_error(condat,sql_connection,sql_command) \
  __report_sql_error((condat), (sql_connection), (sql_command), \
                     __FILE__,__LINE__,pthread_self())

/* report_sql_error() */
/*++++++++++++++++++++++++++++++++++++++

 sk_conn_st *condat                 connection with user
 SQ_connection_t *sql_connection    connection to MySQL
 const char *sql_command            SQL command sent to MySQL

 ++++++++++++++++++++++++++++++++++++++*/

void __report_sql_error(sk_conn_st *condat, SQ_connection_t *sql_connection, const char *sql_command, const char *file, int line, pthread_t tid) {
    /* first, let user know what has happened */
    char *sql_error_text = ca_get_qi_internal_error;
    SK_cd_puts(condat, sql_error_text);
    free(sql_error_text);

    /* next, log this error */
    LG_log(sql_context, LG_ERROR, "at %s:%d, tid:%lu [%d] %s sql='%s'", file, line, (unsigned long) tid, SQ_errno(sql_connection), SQ_error(sql_connection),
            sql_command);
}

/* list_has_attr() */
/*++++++++++++++++++++++++++++++++++++++
 See if any object from idtable has an attribute.
 This looks into MySQL directly so it is suitable only for
 lookup/inverse keys (which have separate tables).

 condat           connection data for the client
 sql_connection   connection to the database
 id_table         table to search
 ref_table        table to search for references
 groups           a hash table containing group ids which have abuse_mailbox attr in the group;
 NULL for non-grouped
 ++++++++++++++++++++++++++++++++++++++*/
gboolean list_has_attr(sk_conn_st *condat, SQ_connection_t *sql_connection, char *id_table, const char *ref_table, GHashTable **groups) {
    GString *sql_command;
    SQ_result_set_t *result_ptr;
    int sql_error;
    SQ_row_t *row;
    long gid;
    *groups = NULL;

    /* query database for references from this object to an attribute */
    sql_command = g_string_new("");
    g_string_sprintf(sql_command, LIST_HAS_ATTR, id_table, ref_table);
    if (SQ_execute_query(sql_connection, sql_command->str, &result_ptr) == -1) {
        sql_error = SQ_errno(sql_connection);
        report_sql_error(condat, sql_connection, sql_command->str);
        g_string_free(sql_command, TRUE);
        return FALSE;
    }
    g_string_free(sql_command, TRUE);

    *groups = g_hash_table_new(NULL, NULL);
    /* get the rows */

    while ((row = SQ_row_next(result_ptr)) != NULL) {
        if (SQ_get_column_int(result_ptr, row, 0, &gid) != 0) {
            LG_log(sql_context, LG_ERROR, "bad column at %s:%d", __FILE__, __LINE__);
        } else { /* save the gid */
            g_hash_table_insert(*groups, (void *) gid, (int *) 1);
        }
    }

    SQ_free_result(result_ptr);
    if (g_hash_table_size(*groups) > 0) {
        return TRUE;
    } else {
        return FALSE;
    }
} /* list_has_attr() */

/* object_has_attr() */
/*++++++++++++++++++++++++++++++++++++++
 See if the object has an attr attribute (defined by table name)

 condat           connection data for the client
 sql_connection   connection to the database
 object_id        object_id
 table            table where to search
 ++++++++++++++++++++++++++++++++++++++*/

gboolean object_has_attr(sk_conn_st *condat, SQ_connection_t *sql_connection, int object_id, const char *table) {
    GString *sql_command;
    SQ_result_set_t *result_ptr;
    int sql_error;
    SQ_row_t *row;
    long num_attr_ref;

    /* query database "table" for the "object_id" */
    sql_command = g_string_new("");
    g_string_sprintf(sql_command, "SELECT COUNT(*) FROM %s WHERE object_id=%d", table, object_id);
    if (SQ_execute_query(sql_connection, sql_command->str, &result_ptr) == -1) {
        sql_error = SQ_errno(sql_connection);
        report_sql_error(condat, sql_connection, sql_command->str);
        g_string_free(sql_command, TRUE);
        return FALSE;
    }
    g_string_free(sql_command, TRUE);

    /* get number of references */
    row = SQ_row_next(result_ptr);
    if (row == NULL) {
        /* if the object is deleted after we get a reference to it,
         the lookup will fail - in such a case treat it the same as
         an object without any "attr" attributes */
        num_attr_ref = 0;
    } else {
        if (SQ_get_column_int(result_ptr, row, 0, &num_attr_ref) != 0) {
            LG_log(sql_context, LG_ERROR, "bad column at %s:%d", __FILE__, __LINE__);
            num_attr_ref = 0;
        }
    }
    SQ_free_result(result_ptr);

    /* return appropriate result */
    if (num_attr_ref > 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*++++++++++++++++++++++++++++++++++++++

 SQ_connection_t *sql_connection The connection to the database.

 char *id_table The id of the temporary table (This is a result of the hacky
 way we've tried to get MySQL to do sub-selects.)

 Query_instructions *qis         query instructions.

 ++++++++++++++++++++++++++++++++++++++*/
static int qi_write_objects(SQ_connection_t **sql_connection, char *id_table, Query_instructions *qis) {
    sk_conn_st *condat = &qis->qe->condat;
    SQ_result_set_t *result = NULL;
    int retrieved_objects = 0;
    char sql_command[STR_XL];
    GHashTable *groups = NULL;
    unsigned int grouped = qis->qc->G;
    unsigned int original = qis->qc->B || qis->source->isGRS || qis->qc->filtered;

    if (grouped == 1) {
        sprintf(sql_command, Q_OBJECTS, id_table, "gorder.order_code, gid, ");
    } else {
        sprintf(sql_command, Q_OBJECTS, id_table, "");
    }

    if (sql_execute_watched(condat, sql_connection, sql_command, &result) == -1) {
        report_sql_error(condat, *sql_connection, sql_command);
        return SQ_errno(*sql_connection);
    }

    /* Problem: if the query was aborted, the result structure does not
     refer to any existing connection anymore. So we check rtc here.
     */

    if (condat->rtc == 0) {

        if (original == 0) {
            /* check the abuse_mailbox attributes */
            list_has_attr(condat, *sql_connection, id_table, "abuse_mailbox", &groups);
        }

        retrieved_objects = write_results(result, qis, groups);

        if (groups != NULL) {
            g_hash_table_destroy(groups);
        }

        SQ_free_result(result);
    }
    return 0;
} /* qi_write_objects */

/* mnt_irt_filter() */
/*++++++++++++++++++++++++++++++++++++++
 Find the most specific entry in the list of network nodes passed
 and remove all entries but that one from the list.

 condat           connection data for the client
 sql_connection   connection to the database
 datlist          list of data from the radix tree (nodes of *rx_datcpy_t)
 irt_inet_id      returned object_id of inet(6)num referencing irt object
 irt_gid          returned object_id of the queried inet(6)num object

 Note:  The "-c" query flag specifies that the irt referenced by the
 most specific inetnum or inet6num with an "mnt-irt:" attribute
 should be returned.

 To do this, we get a list of encompassing networks by sending the
 same options to the RP/RX module that the '-L' query does.  Then
 we call this function, mnt_irt_filter(), and search the list for
 the first entry that has an "mnt-irt:" attribute, by looking in
 the "mnt_irt" table in MySQL for a reference (see the
 object_has_attr() for details).

 If a reference is found, the object_id of that inet(6)num is returned.

 The list is replaced with a list containing the last inet(6)num entry,
 which is the inet(6)num in the query and any route objects found.
 ++++++++++++++++++++++++++++++++++++++*/
void mnt_irt_filter(sk_conn_st *condat, SQ_connection_t *sql_connection, GList **datlist, int *irt_inet_id, int *irt_gid) {
    GList *p;
    GList *new_datlist;
    rx_datcpy_t *rx_data;
    int object_id;
    char *pkey;
    int inet_found = 0;
    int irt_found = 0;

    /* empty datlist */
    new_datlist = NULL;
    *irt_inet_id = 0;
    *irt_gid = 0;

    /* search for node with "mnt-irt:" attribute */
    for (p = g_list_last(*datlist); p; p = g_list_previous(p)) {
        /* grab the data for this node */
        rx_data = (rx_datcpy_t *) p->data;
        object_id = rx_data->leafcpy.data_key;
        pkey = rx_data->leafcpy.data_ptr;

        /* for domain, we don't do anything - they don't have irt whatsoever, so keep them in the list */
        if (!strncmp(pkey, "domain", 6)) {
            new_datlist = g_list_append(new_datlist, rx_data);
            p->data = NULL;
            continue;
        }

        /* see if this is the node we are looking for */
        if (!irt_found && object_has_attr(condat, sql_connection, object_id, "mnt_irt")) {
            /* save this object_id */
            *irt_inet_id = object_id;
            irt_found = 1;
        }

        /* save the first inet(6)num found from the end of the list */
        if (!inet_found && (!strncmp(pkey, "inetnum", 7) || !strncmp(pkey, "inet6num", 8))) {
            /* This is the object the irt object has to be grouped with
             when the objects are displayed in the output */
            *irt_gid = object_id;
            /* move this entry to the new list */
            new_datlist = g_list_append(new_datlist, rx_data);
            p->data = NULL;
            inet_found = 1;
        } else if ((!strncmp(pkey, "route", 5) || !strncmp(pkey, "route6", 6))) { /* save any route(6) found in the list */
            /* move this entry to the new list */
            new_datlist = g_list_append(new_datlist, rx_data);
            p->data = NULL;
        }
    }

    /* free our old datlist */
    for (p = *datlist; p != NULL; p = g_list_next(p)) {
        rx_data = (rx_datcpy_t *) p->data;
        if (rx_data) UT_free(rx_data->leafcpy.data_ptr);
    }
    wr_clear_list(datlist);

    /* use our new datlist */
    *datlist = new_datlist;
}

/*
 Insert the radix serial numbers into a temporary table in the database.

 condat             Client connection descriptor

 sql_connection     The connection to the database.

 id_table           The id of the temporary table (This is a result of the hacky way we've tried to get MySQL to do sub-selects.)

 datlist            The list of data from the radix tree.
*/
static int insert_radix_serials(sk_conn_st *condat, SQ_connection_t *sql_connection, char *id_table, GList *datlist) {
    GList *qitem;
    GString *sql_command = g_string_sized_new(STR_S);
    int object_id;
    int sql_error;

    sql_error = 0;
    for (qitem = g_list_first(datlist); qitem; qitem = g_list_next(qitem)) {
        rx_datcpy_t *datcpy = qitem->data;

        object_id = datcpy->leafcpy.data_key;

        /* don't bother to insert values into our temporary table */
        /* if we've lost the client connection */
        if ((condat->rtc == 0) && !sql_error) {
            g_string_sprintf(sql_command, "INSERT INTO %s values (%d, 0)", id_table, object_id);
            if (SQ_execute_query(sql_connection, sql_command->str, NULL) == -1) {
                /* it seems to be a design decision to gracefully fail here - for performance and sanity reasons,
                 * there should never be two query instructions returning the same object_id twice
                 * Note: this is weird, as SQL lookups simply use INSERT IGNORE; maybe this was simply overlooked? - agoston, 2010-07-29 */
                sql_error = SQ_errno(sql_connection);
                report_sql_error(condat, sql_connection, sql_command->str);
            }
        }
        UT_free(datcpy->leafcpy.data_ptr);
    }

    g_string_free(sql_command, TRUE);

    wr_clear_list(&datlist);

    return sql_error;
}


/*
 Display the immediate data carried with the objects returned by the
 radix tree.

 GList *datlist      The linked list of dataleaf copies

 sk_conn_st *condat  Connection data for the client

 acc_st  *acc_credit Accounting struct

 Also free the list of answers. */
static void write_radix_immediate(GList *datlist, sk_conn_st *condat, acc_st *acc_credit, acl_st *acl) {
    GList *qitem;

    for (qitem = g_list_first(datlist); qitem != NULL; qitem = g_list_next(qitem)) {
        rx_datcpy_t *datcpy = qitem->data;

        SK_cd_puts(condat, datcpy->leafcpy.data_ptr);
        SK_cd_puts(condat, "\n");

        UT_free(datcpy->leafcpy.data_ptr);

        /* account it as public object (private=0) */
        AC_count_object(acc_credit, acl, 0);

        if (condat->rtc) {
            break;
        }
    }

    wr_clear_list(&datlist);
}


/*++++++++++++++++++++++++++++++++++++++
 The mapping between a query_command and a radix query.

 Query_instruction *qi The Query Instruction to be created from the mapping
 of the query command.

 const Query_command *qc The query command to be mapped.

 More:
 +html+ <PRE>
 Authors:
 ottrey,
 marek - simplified the logic, added stealth -S option
 +html+ </PRE>

 ++++++++++++++++++++++++++++++++++++++*/
static int map_qc2rx(Query_instruction *qi, const Query_command *qc) {
    int result = 1;
    int allflags = 0;

    if (qc->L) allflags++;
    if (qc->M) allflags++;
    if (qc->l) allflags++;
    if (qc->m) allflags++;
    if (qc->x) allflags++;
    if (qc->c) allflags++;

    qi->rx_keys = qc->keys;

    /* only one option can be active at a time */
    if (allflags > 1) {
        /* user error  (this should have been checked before) */

        LG_log(qi_context, LG_ERROR, "ERROR in qc2rx mapping: bad combination of flags");
        result = 0;
    }
    if (allflags == 0) {
        /* no options active - default search */
        qi->rx_srch_mode = RX_SRCH_EXLESS;
        qi->rx_par_a = 0;
    } else if (qc->c) {
        /* for grabbing all the less specific objects up the radix tree hierarchy
         * will be filtered later during query processing in mnt_irt_filter(), so end result is exactly like RX_SRCH_EXLESS */
        qi->rx_srch_mode = RX_SRCH_LESS;
        qi->rx_par_a = RX_ALL_DEPTHS;
    } else if (qc->L == 1) {
        qi->rx_srch_mode = RX_SRCH_LESS;
        qi->rx_par_a = RX_ALL_DEPTHS;
    } else if (qc->M == 1) {
        qi->rx_srch_mode = RX_SRCH_MORE;
        qi->rx_par_a = RX_ALL_DEPTHS;
    } else if (qc->l == 1) {
        qi->rx_srch_mode = RX_SRCH_LESS;
        qi->rx_par_a = 1;
    } else if (qc->m == 1) {
        qi->rx_srch_mode = RX_SRCH_MORE;
        qi->rx_par_a = 1;
    } else if (qc->x == 1) {
        qi->rx_srch_mode = RX_SRCH_EXACT;
        qi->rx_par_a = 0;
    }

    return result;

} /* map_qc2rx() */

/* run_referral() */
/*++++++++++++++++++++++++++++++++++++++

 invoked when no such domain found. Goes through the domain table
 and searches for shorter domains, then if it finds one with referral
 it performs it, otherwise it just returns nothing.

 to perform referral, it actually composes the referral query
 for a given host/port/type and calls the whois query function.

 Well, it returns nothing anyway (void). It just prints to the socket.

 char *ref_host           referral server host name

 unsigned ref_port_int    referral server port number

 char *qry                query to be run

 Author:
 marek
 ++++++++++++++++++++++++++++++++++++++*/
void run_referral(Query_environ *qe, char *ref_host, unsigned ref_port_int, char *qry) {
    int err;
    int sk_err;
    char *rep;

    /* WH_sock(sock, host, port, query, maxlines, timeout)) */
    err = WH_cd_sock(&(qe->condat), ref_host, ref_port_int, qry, ca_get_referralmaxlines, ca_get_referraltimeout, &sk_err);

    if (err != WH_OK) {
        if (err == WH_MAXLINES) {
            /* Referral reply line limit exceeded */
            rep = ca_get_qi_ref_overmaxlin;
            SK_cd_puts(&(qe->condat), rep);
            UT_free(rep);
        } else if (err == WH_SK_ERR) {
            switch (sk_err) {
            case SK_TIMEOUT:
                /* Referral timeout */
                rep = ca_get_qi_ref_tmout;
                SK_cd_puts(&(qe->condat), rep);
                UT_free(rep);
                break;

            case SK_BADHOST:
                /* Referral host not found */
                rep = ca_get_qi_ref_badhost;
                SK_cd_puts(&(qe->condat), rep);
                UT_free(rep);
                break;

            case SK_CONNECT:
                /* Referral host not responding */
                rep = ca_get_qi_ref_hostnottresp;
                SK_cd_puts(&(qe->condat), rep);
                UT_free(rep);
                break;

            case SK_BIND:
            case SK_SOCKET:
                /* XXX internal server problem...  */
                die
                ;

            default:
                /* XXX unexpected socket error...  */
                die
                ;
            }
        } else {
            /* XXX unexpected WH error...  */
            die;
        }
    }

}/*run_referral*/

/*++++++++++++++++++++++++++++++++++++++

 prepare and run the referral, displaying the results directly to the
 client's connection.

 XXX still missing protection against a referral loop
 XXX handling inverse flag not needed, to be removed

 char *domain               domain being looked up

 Query_instructions *qis    original query instructions structure

 Query_instruction *qi      specific query instruction triggered

 SQ_result_set_t *result    result of the lookup containing referral details

 SQ_row_t *row              first row (should be only 1) of the result
                            should contain columns: type, port, host

 Author:
 marek
 ++++++++++++++++++++++++++++++++++++++*/
static void qi_prep_run_refer(char *domain, Query_instructions *qis, Query_instruction *qi, SQ_result_set_t *result, SQ_row_t *row) {
    int err;
    long ref_type;
    long ref_port;
    char *ref_host;
    GString *querystr;

    /* get values from SQL query */
    err = SQ_get_column_int(result, row, 0, &ref_type);
    dieif(err);
    err = SQ_get_column_int(result, row, 1, &ref_port);
    dieif(err);
    ref_host = SQ_get_column_string(result, row, 2);
    dieif(ref_host == NULL);

    querystr = g_string_sized_new(STR_L);

    /* put -r if the reftype is RIPE and -r or -i were used */
    if ((ref_type == RF_RIPE) && (Query[qi->queryindex].querytype == Q_INVERSE || qis->qc->recursive > 0)) {
        g_string_append(querystr, "-r ");
    }

    /* prepend with -Vversion,IP for type CLIENTADDRESS */
    if( ref_type == RF_CLIENTADDRESS ) {
        g_string_sprintf(querystr, "-V%s,%s ", VERSION, qis->qe->condat.rIPs);
    }

    /* now set the search term - set to the stripped down version
     for inverse query, full-length otherwise */
    if (Query[qi->queryindex].querytype == Q_INVERSE) {
        g_string_append(querystr, domain);
    } else {
        g_string_append(querystr, qis->qc->keys);
    }

    {
        /* the object is not from %s,
         it comes from %s %d, use -R to see %s */
        char *rep = ca_get_qi_fmt_refheader;
        SK_cd_printf(&qis->qe->condat, rep, qis->source->name, ref_host, ref_port, qis->source->name);
        UT_free(rep);
    }

    /* do the referral */
    LG_log(qi_context, LG_DEBUG, "referral host is %s", ref_host);

    run_referral(qis->qe, ref_host, ref_port, querystr->str);

    { /* End of referred query result */
        char *rep = ca_get_qi_reftrailer;
        SK_cd_puts(&(qis->qe->condat), rep);
        UT_free(rep);
    }
    SK_cd_puts(&(qis->qe->condat), "\n");

    g_string_free(querystr, TRUE);
    UT_free(ref_host);
}

/*++++++++++++++++++++++++++++++++++++++
 specific case of the object ID collection: the domains.
 Checks to see if the domain exists, and runs the referral if it is defined
 and the domain is missing.

 Arguments:

 SQ_connection_t *sql_connection      sql connection dedicated to this thread
 char *id_table                       name of the temporary table to be used
 char *sub_table                      name of the temporary subtable
 Query_instructions *qis              original query instructions structure
 Query_instruction *qi                specific query instruction triggered
 ++++++++++++++++++++++++++++++++++++++*/
static int qi_collect_domain(SQ_connection_t *sql_connection, char *id_table, char *sub_table, Query_instructions *qis, Query_instruction *qi, int *sql_error) {
    char *domain = qis->qc->keys;
    char *dot = domain;
    int subcount = 0;
    int foundcount = 0;
    GString *sql_command;
    char *esc_dot;

    /* disconnection from the server */
    if (qis->qe->condat.rtc != 0) return 0;

    /* create a string for our queries */
    sql_command = g_string_sized_new(STR_XL);

    /* while nothing found and still some pieces of the name left */
    while (dot != NULL && subcount == 0) {
        int refcount = 0;
        SQ_row_t *row;
        SQ_result_set_t *result_referrals = NULL;

        LG_log(qi_context, LG_DEBUG, "run_referral: checking %s", dot);

        /* domain lookup -- query into the _S table */
        esc_dot = SQ_escape_string(sql_connection, dot);
        g_string_sprintf(sql_command, "INSERT IGNORE INTO %s SELECT object_id "
            "FROM domain "
            "WHERE domain = '%s'", sub_table, esc_dot);
        free(esc_dot);
        if (SQ_execute_query(sql_connection, sql_command->str, NULL) != 0) {
            *sql_error = SQ_errno(sql_connection);
            report_sql_error(&qis->qe->condat, sql_connection, sql_command->str);
            foundcount = 0;
            goto exit_qi_collect_domain;
        }
        subcount = SQ_get_affected_rows(sql_connection);

        if (subcount != 0) { /* domain exists in the database */

            /* referral check. Always done except for -R, GRS and INVERSE queries */
            if (qis->qc->R == 0 && !qis->source->isGRS && Query[qi->queryindex].querytype != Q_INVERSE) {
                g_string_sprintf(sql_command, "SELECT type, port, host "
                    "FROM %s ID, refer "
                    "WHERE ID.id = refer.object_id", sub_table);

                if (SQ_execute_query(sql_connection, sql_command->str, &result_referrals) == -1) {
                    *sql_error = SQ_errno(sql_connection);
                    report_sql_error(&qis->qe->condat, sql_connection, sql_command->str);
                    foundcount = 0;
                    goto exit_qi_collect_domain;
                }
                refcount = SQ_num_rows(result_referrals);
            }

            /* if referral allowed and defined, even if domain was found but
             contained referral - refer the query */
            if (refcount) {
                /* get the referral parameters from the first row
                 and perform it
                 */

                row = SQ_row_next(result_referrals);
                /* now: query for the original domain */
                qi_prep_run_refer(domain, qis, qi, result_referrals, row);

                if (qis->qe->acc_credit) qis->qe->acc_credit->referrals -= 1;
            } else {
                /* domain found
                 and (referral undefined  or  disabled by -R or inverse)
                 two possible outcomes depending on whether 'dot' is:
                 * the original search term -> pass what's in _S and quit
                 * a 'stripped' domain name -> return no result and quit
                 */
                if (dot == domain) {
                    g_string_sprintf(sql_command, "INSERT IGNORE INTO %s SELECT id,0 FROM %s", id_table, sub_table);
                    if (SQ_execute_query(sql_connection, sql_command->str, NULL) == -1) {
                        *sql_error = SQ_errno(sql_connection);
                        report_sql_error(&qis->qe->condat, sql_connection, sql_command->str);
                        foundcount = 0;
                        goto exit_qi_collect_domain;
                    }
                    foundcount = SQ_get_affected_rows(sql_connection);
                }
            }
            dot = NULL; /* don't make another round */
        } /* a domain was found */

        if (result_referrals != NULL) {
            SQ_free_result(result_referrals);
            result_referrals = NULL;
        }

        if (dot != NULL && (dot = index(dot, '.')) != NULL) {
            dot++;
        }
    }

    /* unified success/failure exit point to perform cleanup */
    exit_qi_collect_domain:

    /* free the string we used for queries */
    g_string_free(sql_command, TRUE);

    return foundcount;
} /* qi_collect_domain */

///* add_ref_name */
///*++++++++++++++++++++++++++++++++++++++
//
//  Creates a SQL query for a reference-by-name lookup. Uses standard name
//  lookup query generator (qi_create_name_query), so the order of the names
//  doesn't matter.
//
//  SQ_connection_t *sql_connection   sql connection dedicated to this thread
//
//  char *rectable       table in which to look up
//
//  char *allnames       all name words to be looked up, space delimited.
//
//++++++++++++++++++++++++++++++++++++++*/
//static
//int
//add_ref_name(SQ_connection_t *sql_connection,
//	     char *rectable,
//	     char *allnames,
//	     sk_conn_st *condat
//	     )
//{
//  int error;
//
//  error = 0;
//
//  /* construct the query, allow zero-length list */
//  if( strlen(allnames) > 0 ) {
//    GString *final_query;
//    GString *select_query;
//
//    final_query = g_string_sized_new(STR_XL);
//    select_query = g_string_sized_new(STR_XL);
//
//    qi_create_name_query(select_query, "SELECT N00.object_id FROM %s WHERE %s "
//		      "AND N00.object_type != 100 AND N00.thread_id = 0",
//		      allnames);
//
//    g_string_sprintf(final_query, "INSERT IGNORE INTO %s %s",
//	             rectable, select_query->str);
//
//    allnames[0]=0;
//
//    if (SQ_execute_query(sql_connection, final_query->str, NULL) == -1 ) {
//        report_sql_error(condat, sql_connection, final_query->str);
//        error = SQ_errno(sql_connection);
//    }
//
//    g_string_free(select_query, TRUE);
//    g_string_free(final_query, TRUE);
//  }
//
//  return error;
//}/* add_ref_name */

/* qi_collect_ids */
/*++++++++++++++++++++++++++++++++++++++

 collects object ID's from all queries defined in the Query_instructions
 array. The results from RADIX trees are maintained in a linked list, the
 results from SQL lookups are kept in a temporary table. For domains,
 a specific function is invoked that may run the referral.
 Any sql lookup will be limited to the maximum number of objects allowed
 for the client (acl and credit are checked for this).
 The routine uses its own temporary _S table, destroyed at exit.

 SQ_connection_t **sql_connection  sql connection dedicated to this thread
 (replaced on cancel)

 Query_instructions *qis           original query instructions structure

 char *id_table                    the table to store the ID's found

 GList **datlist                   the list  to store the Radix leaves found
 ++++++++++++++++++++++++++++++++++++++*/
static int qi_collect_ids(SQ_connection_t **sql_connection, Query_instructions *qis, char *id_table, GList **datlist) {
    sk_conn_st *condat = &qis->qe->condat;
    Query_instruction **ins = NULL;
    int i;
    int count, errors = 0;
    GString *sql_command;
    int err;
    char sub_table[64];
    int limit;
    /* a limit on the max number of objects to be returned
     from a single search. For some queries the object types
     are not known at this stage, so the limit must be
     the higher number of the two: private / public,
     or unlimited if any of them is 'unlimited'.
     */
    char limit_str[32];
    int sql_error;

    /* use a nice resizing GString for our command */
    sql_command = g_string_sized_new(STR_XL);

    if ((limit = AC_get_higher_limit(qis->qe->acc_credit, qis->qe->acl)) == -1) {
        strcpy(limit_str,"");
    } else {
        sprintf(limit_str," LIMIT %d", limit+1); /* make sure we collect more so that the client hits the limit */
    }

    sprintf(sub_table, "%s_S ", id_table);

    /* see if there was a leftover table from a crashed session
     * (assume the ID cannot be currently in use)
     *
     * update: this can't ever happen with TEMPORARY tables, but we're going to
     *         check for it anyway  - shane
     */
    g_string_sprintf(sql_command, "DROP TABLE IF EXISTS %s", sub_table);
    if (SQ_execute_query(*sql_connection, sql_command->str, NULL) == -1) {
        report_sql_error(condat, *sql_connection, sql_command->str);
        return SQ_errno(*sql_connection);
    }

    /* create a table for special subqueries (domain only for now) */
    g_string_sprintf(sql_command, "CREATE TEMPORARY TABLE %s ( id int ) TYPE=HEAP", sub_table);
    if (SQ_execute_query(*sql_connection, sql_command->str, NULL) == -1) {
        report_sql_error(condat, *sql_connection, sql_command->str);
        return SQ_errno(*sql_connection);
    }

    /* Iterate through query instructions */
    ins = qis->instruction;
    sql_error = 0;
    for (i = 0; ins[i] != NULL && errors == 0; i++) {
        Query_instruction *qi = ins[i];

        /* check if the client is still there */
        if (qis->qe->condat.rtc) {
            break;
        }

        switch (qi->search_type) {
        case R_SQL:
            count = 0;
            if (qi->query_str != NULL) {

                /* handle special cases first */
                /* forward domain - run referral to foreign registry */
                if (Query[qi->queryindex].class == C_DN && Query[qi->queryindex].querytype == Q_LOOKUP) {

                    /* if any more cases than just domain appear, we will be cleaning the _S table from the previous query here
                      (a.k.a "DELETE FROM %s_S") */
                    count = qi_collect_domain(*sql_connection, id_table, sub_table, qis, qi, &sql_error);

                } else {
                    /* any other class of query */

                    g_string_sprintf(sql_command, "INSERT IGNORE INTO %s %s %s", id_table, qi->query_str, limit_str);

                    if (sql_execute_watched(condat, sql_connection, sql_command->str, NULL) == -1) {
                        errors++;
                        sql_error = SQ_errno(*sql_connection);
                        report_sql_error(condat, *sql_connection, sql_command->str);
                    }
                    count = SQ_get_affected_rows(*sql_connection);
                } /* not DN */
            } /* if SQL query not NULL */

//            LG_log(qi_context, LG_DEBUG, "%d entries added in %s query for %s", count, Query[qi->queryindex].descr, qis->qc->keys);
            break;

        case R_RADIX:

#ifdef DEBUG_QUERY
            fprintf(stderr, "RP_asc_search:  query %s : mode %d (%s) (par %d) for %s\n", DF_get_attribute_name(Query[qi->queryindex].attribute), qi->rx_srch_mode,
                    RX_text_srch_mode(qi->rx_srch_mode), qi->rx_par_a, qi->rx_keys);
#endif

            err = RP_asc_search(qi->rx_srch_mode, qi->rx_par_a, 0, qi->rx_keys, qis->source, Query[qi->queryindex].attribute, datlist, limit);

            if (NOERR(err)) {
#ifdef DEBUG_QUERY
                fprintf(stderr, "%d entries after %s (mode %d par %d reg %s) query for %s:\n", g_list_length(*datlist), Query[qi->queryindex].descr,
                        qi->rx_srch_mode, qi->rx_par_a, qis->source->name, qi->rx_keys);
                GList *pp = *datlist;
                for (; pp; pp = pp->next) {
                    fprintf(stderr, "%s", (char *)(((rx_datcpy_t *)(pp->data))->leafcpy.data_ptr));
                }
#endif
            } else {
                LG_log(qi_context, LG_INFO, "RP_asc_search returned error code %x after %s (mode %d par %d reg %s) query for %s", err,
                        Query[qi->queryindex].descr, qi->rx_srch_mode, qi->rx_par_a, qis->source->name, qi->rx_keys);
            }
            break;

        default:
            die;
        }
    }

    /* Now drop the _S table */
    g_string_sprintf(sql_command, "DROP TABLE IF EXISTS %s", sub_table);
    if (SQ_execute_query(*sql_connection, sql_command->str, NULL) == -1) {
        report_sql_error(condat, *sql_connection, sql_command->str);
        sql_error = SQ_errno(*sql_connection);
    }

    /* free our command buffer */
    g_string_free(sql_command, TRUE);

    /* return success */
    return sql_error;
}

/* qi_find_refs */
/*++++++++++++++++++++++++++++++++++++++

 either queries the search_table for references to a single obj_id and writes
 the results to the id_table, or queries the search_table for references to
 all obj_ids from id_table and writes the results to the temporary table
 rec_table
 Runs queries in watched mode, to be able to cancel them.
 The 'recursive' column will be used to specify if a row is a result of the main query,
 or a result of the recursive query.  This is important when we print out the
 objects to the client (the main results must come first, then the results of
 recursive query).


 SQ_connection_t **sql_connection  sql connection dedicated to this thread
 (replaced on cancel)

 Query_environ *qe                 original query environment structure

 char *query                       format string containing query command

 char *search_table                string containing sql table to query

 char *rec_table                   the table to write the query results to
 (maybe null string)

 char *id_table                    the table with the ID's found

 int  obj_id                       object_id to search for in search_table

 int  gid                          group id of object being searched for

 ++++++++++++++++++++++++++++++++++++++*/
static
int qi_find_refs(SQ_connection_t **sql_connection, Query_environ *qe, char *query, char *search_table, char *rec_table, char *id_table, int obj_id, int gid) {
    char sql_command[STR_XL];
    char gid_str[STR_M];
    int sql_error;

    if (obj_id == 0) {
        sprintf(sql_command, query, rec_table, id_table, search_table);
    } else {
        /* an object_id has been specified, so only search for this one object */
        if (gid) {
            /* a group id has been specified for this object */
            sprintf(gid_str, "%d", gid);
            sprintf(sql_command, query, id_table, gid_str, search_table, obj_id);
        } else {
            sprintf(sql_command, query, id_table, "object_id", search_table, obj_id);
        }
    }
    if (sql_execute_watched(&(qe->condat), sql_connection, sql_command, NULL) == -1) {
        sql_error = SQ_errno(*sql_connection);
        report_sql_error(&qe->condat, *sql_connection, sql_command);
        return -1;
    }
    return 0;
}

/*
 given the list of object ID's collects the references from these objects
 to person, role and organisation objects. Uses its own temporary SQL table (_R)
 and upon completion transfers the results from it to the main
 temporary table. Runs queries in watched mode, to be able to cancel them.
 The 'recursive' column will be used to specify if a row is a result of the main query,
 or a result of the recursive query.  This is important when we print out the
 objects to the client (the main results must come first, then the results of
 recursive query).


 SQ_connection_t **sql_connection  sql connection dedicated to this thread
 (replaced on cancel)

 Query_environ *qe                 original query environment structure

 char *id_table                    the table with the ID's found
*/
static int qi_fetch_references(SQ_connection_t **sql_connection, Query_environ *qe, char *id_table) {
    char rec_table[64];
    char sql_command[STR_XL];
    int sql_error;
    int ref_cnt;

    /* use sql_error to flag errors */
    sql_error = 0;

    sprintf(rec_table, "%s_R", id_table);

    /* see if there was a leftover table from a crashed session
     * (assume the ID cannot be currently in use)
     */
    sprintf(sql_command, "DROP TABLE IF EXISTS %s ", rec_table);
    if (SQ_execute_query(*sql_connection, sql_command, NULL) == -1) {
        report_sql_error(&qe->condat, *sql_connection, sql_command);
        return SQ_errno(*sql_connection);
    }

    /* a temporary table for recursive data must be created, because
     a query using the same table as a source and target is illegal
     ( like: INSERT into ID_123 SELECT * FROM ID_123,admin_c WHERE ... )
     */
    sprintf(sql_command, "CREATE TEMPORARY TABLE %s ( id int NOT NULL, recursive BOOL DEFAULT 1, gid INT NOT NULL DEFAULT 0, PRIMARY KEY (id, gid)) TYPE=HEAP", rec_table);
    if (SQ_execute_query(*sql_connection, sql_command, NULL) == -1) {
        report_sql_error(&qe->condat, *sql_connection, sql_command);
        return SQ_errno(*sql_connection);
    }

    /* from this point on, we can't just return on error, because
     we need to insure the table we just created gets dropped */

    /* find the contacts (ah,ac,tc,zc,org) */
    ref_cnt = 0;
    while (strcmp(ref_queries[ref_cnt].query, "")) {
        if ((sql_error = qi_find_refs(sql_connection, qe, ref_queries[ref_cnt].query, ref_queries[ref_cnt].search_table, rec_table, id_table, 0, 0)) == -1) {
            break;
        }
        ref_cnt++;
    }

    /* if we've lost connection, don't bother with this extra work */
    if (!sql_error && (qe->condat.rtc == 0)) {

        /* now copy things back to the main temporary table   */
        sprintf(sql_command, "INSERT IGNORE INTO %s SELECT * FROM %s",
                id_table, rec_table);
        if (SQ_execute_query(*sql_connection, sql_command, NULL) == -1) {
            sql_error = SQ_errno(*sql_connection);
            report_sql_error(&qe->condat, *sql_connection, sql_command);
        }

    }

    /* Now drop the IDS recursive table (try to do this even if
     we had an SQL error, to avoid leaving extra tables lying around) */
    sprintf(sql_command, "DROP TABLE IF EXISTS %s", rec_table);
    if (SQ_execute_query(*sql_connection, sql_command, NULL) == -1) {
        sql_error = SQ_errno(*sql_connection);
        report_sql_error(&qe->condat, *sql_connection, sql_command);
    }

    /* return error, if any */
    return sql_error;
}


/*++++++++++++++++++++++++++++++++++++++
 Execute the query instructions.  This is called for each source.
 This is linked into MySQL by the fact that MySQL doesn't have sub selects
 (yet).  The queries are done in two stages.  Make some temporary tables and
 insert into them.  Then use them in the next select.

 Query_instructions *qis         query instructions.
 ++++++++++++++++++++++++++++++++++++++*/
int QI_execute(Query_instructions *qis) {
    sk_conn_st *condat = &qis->qe->condat;
    /* those things must be freed after use! */
    char id_table[64];
    char sql_command[STR_XL];
    GList *datlist = NULL;
    SQ_connection_t *sql_connection = NULL;
    int sql_error = 0;
    int irt_inet_id = 0;
    int irt_gid = 0;

    sql_connection = SQ_get_connection_by_source_hdl(qis->source);

    sprintf(id_table, "ID_%lu_%u", mysql_thread_id(sql_connection), (unsigned int)pthread_self());

    /* see if there was a leftover table from a crashed session
     * (assume the ID cannot be currently in use)
     */
    sprintf(sql_command, "DROP TABLE IF EXISTS %s ", id_table);
    if (SQ_execute_query(sql_connection, sql_command, NULL) == -1) {
        sql_error = SQ_errno(sql_connection);
        report_sql_error(condat, sql_connection, sql_command);
    }

    /* create a table for id's of all objects found NOT NULL , UNIQUE(id) */
    if (!sql_error) {
        sprintf(sql_command, "CREATE TEMPORARY TABLE %s ( id INT PRIMARY KEY NOT NULL, recursive BOOL DEFAULT 0 ) TYPE=HEAP", id_table);
        if (SQ_execute_query(sql_connection, sql_command, NULL) == -1) {
            sql_error = SQ_errno(sql_connection);
            report_sql_error(condat, sql_connection, sql_command);
        }
    }

    if (!sql_error) {
        /* here we collect the object_ids into the temporary id_table by running all the identified query instructions */
        sql_error = qi_collect_ids(&sql_connection, qis, id_table, &datlist);
    }

    /* post-processing */
    if (!sql_error && !qis->qc->filtered) {
        /* add radix results (only if -K is not active and still connected) */
        if (!condat->rtc) {
            /* if -c selected, find the referencing inet(6)num and remove
             all the less specific inet(6)num object_ids from the list */
            if (qis->qc->c) {
                mnt_irt_filter(condat, sql_connection, &datlist, &irt_inet_id, &irt_gid);
            }

#ifdef DEBUG_QUERY
      {
          fprintf(stderr, "After mnt_irt_filter, before insert_radix_serials:\n");
          GList *pp = datlist;
          for (; pp; pp = pp->next) {
              fprintf(stderr, "%s", (char *)(((rx_datcpy_t *)(pp->data))->leafcpy.data_ptr));
          }
      }
#endif

            /* add radix results to the table and destroy the datlist */
            sql_error = insert_radix_serials(condat, sql_connection, id_table, datlist);
        }
    }

    /* change the idtable */
    if (!sql_error) {
        if (qis->qc->G) {
            sprintf(sql_command, Q_ALTER_TMP_GROUPED, id_table);
        } else {
            sprintf(sql_command, Q_ALTER_TMP, id_table);
        }

        if (SQ_execute_query(sql_connection, sql_command, NULL) == -1) {
            sql_error = SQ_errno(sql_connection);
            report_sql_error(condat, sql_connection, sql_command);
        }
        if (!sql_error) {
            sprintf(sql_command, Q_UPD_TMP, id_table);
            if (SQ_execute_query(sql_connection, sql_command, NULL) == -1) {
                sql_error = SQ_errno(sql_connection);
                report_sql_error(condat, sql_connection, sql_command);
            }
        }
    }

#ifdef DEBUG_QUERY
    {
        SQ_result_set_t *res;

        sprintf(sql_command, "SELECT * from %s", id_table);
        if (SQ_execute_query(sql_connection, sql_command, &res)) {
            fprintf(stderr, "ERROR: %s", SQ_error(sql_connection));
        } else {
            char *temp = SQ_result_to_string(res);
            fprintf(stderr, "Before recursive fetch (table %s):\n%s\n", id_table, temp);
            free(temp);
        }

        if (res) {
            SQ_free_result(res);
        }
    }
#endif

    /* fetch recursive objects (ac,tc,zc,ah,org,irt(if -c)) */
    if (!sql_error && qis->qc->recursive && (condat->rtc == 0)) {
        sql_error = qi_fetch_references(&sql_connection, qis->qe, id_table);
    }

    /* find the irt objects (for -c) */
    if (!sql_error && qis->qc->c && irt_inet_id) {
        sql_error = qi_find_refs(&sql_connection, qis->qe, Q_REC_IRT, "mnt_irt", "", id_table, irt_inet_id, irt_gid);
    }

    /* display */
    /* -K filtering:
     * right now only filtering, no expanding sets like write_set_objects() */

    /* display the immediate data from the radix tree */
    if (!sql_error && qis->qc->filtered) {
        write_radix_immediate(datlist, condat, qis->qe->acc_credit, qis->qe->acl);
    }

    /* display objects from the IDs table */
    if (!sql_error) {
        sql_error = qi_write_objects(&sql_connection, id_table, qis);
    }

    /* drop the table */
    /* try to do this, even if there is an SQL error */
    sprintf(sql_command, "DROP TABLE IF EXISTS %s ", id_table);
    if (SQ_execute_query(sql_connection, sql_command, NULL) == -1) {
        sql_error = SQ_errno(sql_connection);
        report_sql_error(condat, sql_connection, sql_command);
    }

    /* Now disconnect (temporary tables get dropped automatically) */
    SQ_close_connection(sql_connection);

    /* return appropriate value */
    if (sql_error) {
        return QI_SQLERR;
    } else {
        return QI_OK;
    }
}


/* instruction_free() */
/*++++++++++++++++++++++++++++++++++++++
 Free the instruction.

 Query_instruction *qi query_instruction to be freed.

 More:
 +html+ <PRE>
 Authors:
 ottrey
 +html+ </PRE>
 ++++++++++++++++++++++++++++++++++++++*/
static void instruction_free(Query_instruction *qi) {
    if (qi != NULL) {
        if (qi->query_str != NULL) {
            UT_free(qi->query_str);
        }
        UT_free(qi);
    }
} /* instruction_free() */

/* QI_free() */
/*++++++++++++++++++++++++++++++++++++++
 Free the query_instructions.

 Query_instructions *qis Query_instructions to be freed.

 More:
 +html+ <PRE>
 Authors:
 ottrey, marek
 +html+ </PRE>
 ++++++++++++++++++++++++++++++++++++++*/
void QI_free(Query_instructions *qis) {
    int i;

    for (i = 0; qis->instruction[i] != NULL; i++) {
        instruction_free(qis->instruction[i]);
    }

    if (qis != NULL) {
        UT_free(qis);
    }

} /* QI_free() */

/*++++++++++++++++++++++++++++++++++++++
 Determine if this query should be conducted or not.

 If it was an inverse query - if the attribute appears in the query command's bitmap.
 If it was a lookup query - if the attribute appears in the object type bitmap or
 disregard if there is no object_type bitmap (Ie object filter).

 mask_t bitmap The bitmap of attribute to be converted.

 const Query_command *qc  The query_command that the instructions are created
 from.

 const Query_t q          The query being considered.
 +html+ <PRE>
 Authors:
 ottrey,
 marek.
 +html+ </PRE>
 ++++++++++++++++++++++++++++++++++++++*/
static int valid_query(const Query_command *qc, const Query_t q) {
    int result = 0;

    if (MA_isset(qc->keytypes_bitmap, q.keytype) == 1) {
        if (q.query != NULL) {
            switch (q.querytype) {
            case Q_INVERSE:
                if (MA_isset(qc->inv_attrs_bitmap, q.attribute) && (q.class == C_ANY || MA_isset(qc->object_type_bitmap, (unsigned) q.class))) {
                    result = 1;
                }
                break;

            case Q_LOOKUP:
                if (q.class == C_ANY || MA_isset(qc->object_type_bitmap, (unsigned) q.class)) {
                    result = 1;
                }
                break;

            default:
                /* XXX */
                fprintf(stderr, "qi:valid_query() -> Bad querytype\n");
            }
        }
    }

    return result;
} /* valid_query() */


/*++++++++++++++++++++++++++++++++++++++
 Create a new set of query_instructions. Returns an allocated structure which
 must be freed after use with QI_free().

 Query_command *qc The query_command that the instructions are created from.
 (This is no longer const to allow error messages further down the line)

 Query_environ *qe The environmental variables that they query is being
 performed under.

 ++++++++++++++++++++++++++++++++++++++*/
Query_instructions *QI_new(Query_command *qc, Query_environ *qe) {
    Query_instructions *qis = NULL;
    Query_instruction *qi = NULL;
    Query_instruction tmp_qi;
    int i_no = 0;
    int i;
    char *query_str;

    qis = (Query_instructions *) UT_calloc(1, sizeof(Query_instructions));

    qis->qc = qc;
    qis->qe = qe;

    for (i = 0; Query[i].query != NULL; i++) {

        /* If a valid query. */
        if (valid_query(qc, Query[i]) == 1) {

            memset(&tmp_qi, 0, sizeof(Query_instruction));
            tmp_qi.queryindex = i;

            /* SQL Query */
            if (Query[i].refer == R_SQL) {
                tmp_qi.search_type = R_SQL;
                query_str = create_query(Query[i], qc);

                if (query_str != NULL) {
                    tmp_qi.query_str = query_str;
                    qi = (Query_instruction *) UT_calloc (1, sizeof(Query_instruction));
                    *qi = tmp_qi;
                    qis->instruction[i_no++] = qi;
                }
            }
            /* Radix Query */
            else if (Query[i].refer == R_RADIX) {

                /* no inverse queries should use the RADIX tree */
                if (MA_bitcount(qc->inv_attrs_bitmap) > 0) {
                    continue;
                }

                tmp_qi.search_type = R_RADIX;

                if (map_qc2rx(&tmp_qi, qc) == 1) {
                    int j;
                    int found = 0;

                    /* check that there is no such query yet, for example if
                     more than one keytype (wk) matched */
                    for (j = 0; j < i_no; j++) {
                        Query_instruction *qij = qis->instruction[j];

                        if (qij->search_type == R_RADIX && Query[qij->queryindex].attribute == Query[tmp_qi.queryindex].attribute) {
                            found = 1;
                            break;
                        }
                    }

                    if (!found) {
                        /* Add the query_instruction to the array */
                        qi = (Query_instruction *) UT_calloc(1, sizeof(Query_instruction));
                        *qi = tmp_qi;
                        qis->instruction[i_no++] = qi;
                    }
                }
            } else {
                /* ERROR: bad search_type */
                die;
            }
        }
    }
    qis->instruction[i_no++] = NULL;

#ifdef DEBUG_QUERY
    { /* tracing */
        char *descrstr = QI_queries_to_string(qis);
        fprintf(stderr, "Query Instructions to execute: %s\n", descrstr);
        UT_free( descrstr );
    }
#endif

    return qis;

}

/*++++++++++++++++++++++++++++++++++++++

 char *QI_queries_to_string    returns a list of descriptions for queries
 that will be performed (debugging only).
 Allocated text, must be freed after use.

 Query_instructions *qis       query instructions structure

 Author:
 marek.
 ++++++++++++++++++++++++++++++++++++++*/

char *QI_queries_to_string(Query_instructions *qis) {
    Query_instruction *qi;
    int i;
    char *resstr;

    resstr = (char *) UT_malloc(2);
    strcpy(resstr, "{");

    for (i = 0; (qi = qis->instruction[i]) != NULL; i++) {
        char *descr = Query[qi->queryindex].descr;
        int oldres = strlen(resstr);

        resstr = (char *) UT_realloc (resstr, oldres+strlen(descr)+2);
        strcat(resstr, descr);
        strcat(resstr, ",");
    }
    if (i > 0) {
        /* cancel the last comma */
        resstr[strlen(resstr) - 1] = 0;
    }

    resstr = (char *) UT_realloc(resstr, strlen(resstr) + 2);
    strcat(resstr, "}");

    return resstr;
}

void QI_init(LG_context_t *qi_ctx, LG_context_t *sql_ctx) {
    qi_context = qi_ctx;
    sql_context = sql_ctx;
}

