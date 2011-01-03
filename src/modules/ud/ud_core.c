/***************************************

 $Revision: 1.7 $

 Core functions for update lower layer

 Status: NOT REVUED, NOT TESTED

 Author(s):       Chris Ottrey, Andrei Robachevsky

 ******************//******************
 Modification History:
 andrei (17/01/2000) Created.
 ******************//******************
 Copyright (c) 2000                              RIPE NCC

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
#include "lg.h"

#include <sys/types.h>
#include <signal.h>
#include <time.h>

/* return codes from create_attr() function */
#define ATTR_CREATE_OK       0
#define ATTR_CREATE_DONE     1
#define ATTR_CREATE_ERR      2
#define ATTR_CREATE_NO       3

/* logging (defined in ud_main.c) */
extern LG_context_t *ud_context;

static int perform_update(Transaction_t *tr);

static int perform_create(Transaction_t *tr);

static void each_attribute_process(void *element_data, void *tr_ptr);

static void update_attr(const rpsl_attr_t *attr, Transaction_t *tr);

static int create_dummy(const rpsl_attr_t *attr, Transaction_t *tr);

static int auth_member_of(const rpsl_attr_t *attr, Transaction_t *tr);

/**********************************************************
 * Attribute expansion/conversion functions                *
 ***********************************************************/
/* Convert ifaddr attribute into a number */
int convert_if(const char *avalue, unsigned int *pif_address) {
	char *delim;
	char *ifaddr;
	ip_addr_t ip_addr;
	int ret;

	ifaddr= g_strdup(avalue);
	delim = ifaddr;

	while ((*delim) && (!isspace((int)*delim))) {
		delim++;
	}
	*delim='\0';

	ret=IP_addr_a2v4(ifaddr, &ip_addr, pif_address);

	UT_free(ifaddr);
	return (ret);
}

/* Expand interface attribute. */
int convert_ie(const char *avalue, unsigned int *pif_address, ip_v6word_t *high, ip_v6word_t *low) {
	char *ipv6_prefix;
	char *attribute_value, *interface_address;
	gchar **words;
	int ret;
	ip_addr_t ip_addr;
	ip_prefix_t ip_prefix;
	unsigned prefix_length;

	/* An example for "interface:" attribute value could be */
	/* '144.122.45.5 masklen 24' */

	interface_address = NULL;

	attribute_value = g_strdup(avalue);
	/* Remove white space in the beginning */
	g_strchug(attribute_value);

	/* split the string into words */
	words = g_strsplit(attribute_value, " ", 100);

	/* Get the interface address */
	interface_address = g_strdup(words[0]);

	/* Fill in address variables */
	if (index(interface_address, ':') != NULL) {/* Does a colon ':' char exist in the string? */

		/* Then this is an IPv6 address */
		/* First, add a dummy prefix length to address, so that we can use IP_pref_a2v6 */
		/* (there is no IP_addr_a2v6 function in IP module) */
		ipv6_prefix = g_strdup_printf("%s/128", interface_address);
		*pif_address = 0;
		ret = IP_pref_a2v6(ipv6_prefix, &ip_prefix, high, low, &prefix_length);
		UT_free(ipv6_prefix);

	} else {

		/* Then this is an IPv4 address */
		*high = 0;
		*low = 0;
		ret=IP_addr_a2v4(interface_address, &ip_addr, pif_address);

	}

	/* Free the vector */
	g_strfreev(words);

	UT_free(interface_address);
	UT_free(attribute_value);
	return ret;

}

/* Convert refer attribute. Free host after use ! */
char *convert_rf(const char *avalue, int *type, int *port) {
	char *delim, *token;
	char *refer;
	char *host;

	/* An example for "refer:" attribute could be */
	/* 'SIMPLE whois.some-domain.net 43' */
	host=NULL;
	refer = g_strdup(avalue);
	g_strchug(refer);
	delim=refer;
	while ((*delim) && (!isspace((int)*delim))) {
		delim++;
	}
	if (*delim)
		*delim='\0';
	else
		return (NULL);
	delim++;

	/* convert the type      */
	if (g_strcasecmp(refer, S_RIPE)==0)
		*type=RF_RIPE;
	else if (g_strcasecmp(refer, S_INTERNIC)==0)
		*type=RF_INTERNIC;
	else if (g_strcasecmp(refer, S_SIMPLE)==0)
		*type=RF_SIMPLE;
	else if (g_strcasecmp(refer, S_CLIENTADDERSS)==0)
		*type=RF_CLIENTADDRESS;

	token=delim;
	g_strchug(token);
	delim=token;
	while ((*delim) && (!isspace((int)*delim))) {
		delim++;
	}

	if (*delim) {
		*delim='\0';
		delim++;
	}
	/* convert the hostname      */
	host = g_strdup(token);

	/* convert port number      */
	if (*delim) {
		token=delim;
		*port = atoi(token);
		if (*port==0)
			*port=RF_DEF_PORT; /* default port number*/
	} else
		*port=RF_DEF_PORT;

	UT_free(refer);
	return (host);
}

/* Convert AS# into integer
 *
 * returns: 0 if OK
 *          1 if error (*asnum left untouched)
 *
 * */
int convert_as(const char *as, unsigned long *asnum) {
	char *ptr, *endptr, *finptr;
	unsigned long res;

	/* discard the letters and leading whitespace); we do a cast here, but we indeed won't change the contents of char *as */
	for (ptr = (char *)as; *ptr && !isdigit(*ptr); ptr++);
	/* walk through the digits */
	for (endptr = ptr; *endptr && isdigit(*endptr); endptr++);
	/* check if the end only contains whitespace */
	for (finptr = endptr; *finptr && isspace(*finptr); finptr++);
	/* return error if there is any garbage at the end */
	if (*finptr) return 1;

	/* chomp trailing whitespace */
	*endptr = 0;

	res = strtoul(ptr, &endptr, 10);
	if (*endptr) {
	    return 1;
	} else {
        *asnum = res;
        return 0;
	}
}

/* Convert AS range (AS4321 - AS5672) into numbers
 *
 * returns: 0 if OK
 *          1 on error (*begin and *end are left untouched)
 * */
int convert_as_range(const char *as_range, unsigned long *begin, unsigned long *end) {
	gchar **tok;
	unsigned long ret[2];
	int i;

	tok = g_strsplit(as_range, "-", -1);
	for (i=0; tok[i] ; i++) {
	    if (i >= 2) goto error_return;

	    if (convert_as(tok[i], &ret[i])) goto error_return;
	}
	/* if no - (which would indicate an autnum instead of range), throw an error */
	if (i < 2) goto error_return;

	*begin = ret[0];
	*end = ret[1];
	g_strfreev(tok);
	return 0;

	error_return:
    g_strfreev(tok);
    return 1;
}

/* Convert time in ASCII format (19991224) into time_t unix time */
time_t convert_time(const char *asc_time) {
	struct tm tm;
	char buf[5];
	char *ptr;

	bzero(&tm, sizeof(tm));

	strncpy(buf, asc_time, 4);
	ptr=buf+4;
	*ptr='\0';
	tm.tm_year = atoi(buf) - 1900;

	strncpy(buf, (asc_time+4), 2);
	ptr=buf+2;
	*ptr='\0';
	tm.tm_mon = atoi(buf) - 1;

	strncpy(buf, (asc_time+6), 2);
	ptr=buf+2;
	*ptr='\0';
	tm.tm_mday = atoi(buf);

	return (mktime(&tm));

}

/************************************************************
 *  char *get_set_name()                                     *
 *                                                           *
 * Returns set name for the specified object class           *
 *                                                           *
 * **********************************************************/
static char *get_set_name(C_Type_t class_type) {
	switch (class_type) {
		case C_RT:
			return ("route_set");
		case C_R6:
			return ("route_set");
		case C_AN:
			return ("as_set");
		case C_IR:
			return ("rtr_set");
		default:
			return (NULL);
	}
}

/************************************************************
 * long get_object_id()                                      *
 * Queries the database for an object.                       *
 * For constructing a query uses each_primary_key_select()   *
 *                                                           *
 * Returns:                                                  *
 * >0 - object exists, returns object_id                     *
 * 0  - object does not exist                                *
 * -1 - error (f.e. more than one object with the same PK)   *
 * -2 - synatx error in the primary key                      *
 * Error code is stored in tr->error                         *
 *                                                           *
 * **********************************************************/
long get_object_id(Transaction_t *tr) {
	rpsl_object_t *object;
	SQ_result_set_t *sql_result;
	SQ_row_t *sql_row;
	char *sql_str;
	long object_id=0;
	int sql_err;
	const GList *attr_list;

	object=tr->object;

	/* compose query */
	g_string_sprintf(tr->query, "SELECT object_id FROM %s WHERE",
	        DF_get_class_sql_table(rpsl_get_class_id(rpsl_object_get_class(object))));

	/* add all primary keys */
	attr_list = rpsl_object_get_all_attr(object);
	g_list_foreach((GList *)attr_list, ud_each_primary_key_select, tr);

	/* check if error has been detected */
	/* the caller should check tr->succeeded */
	if (tr->succeeded == 0)
		return (-2);

	/* truncate the last ' AND '*/
	g_string_truncate(tr->query, (tr->query->len) - 4);

	/* execute query */
	LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, tr->query->str);
	sql_err=SQ_execute_query(tr->sql_connection, tr->query->str, &sql_result);

	/* in case of an error copy error code and return */
	if (sql_err) {
		LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
		tr->succeeded=0;
		/*XXX need to return (-2) in case wrong PK was specified */
		tr->error |= ERROR_U_DBS;
		die;
	}

	/* Fetch the row */
	if ((sql_row = SQ_row_next(sql_result)) != NULL) {
		/* Object exists */
#define OBJECT_ID 0
		sql_str = SQ_get_column_string(sql_result, sql_row, OBJECT_ID);
		if (sql_str != NULL) {
			object_id = atol(sql_str);
			UT_free(sql_str);
		}

		/* We must process all the rows of the result */
		/* otherwise we'll have them as part of the next qry */
		while ( (sql_row = SQ_row_next(sql_result)) != NULL)
			object_id=-1;
	} else
		object_id=0; /* object does not exist*/

	SQ_free_result(sql_result);
	return (object_id);
}

/************************************************************
 * get_qresult_str()                                         *
 *                                                           *
 * Returns string containing query result                    *
 *                                                           *
 *                                                           *
 * Returns:                                                  *
 *  String containing the result.Needs to be freed after use *
 *  NULL in case of an error                                 *
 *  - SQL error                                              *
 *  - if query returns more than one string (row)            *
 *                                                           *
 *************************************************************/
char *get_qresult_str(SQ_connection_t *sql_connection, char *query) {
	SQ_result_set_t *sql_result;
	SQ_row_t *sql_row;
	char *sql_str;
	int sql_err;

	LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, query);
	sql_err=SQ_execute_query(sql_connection, query, &sql_result);

	if (sql_err) {
		LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(sql_connection), query);
		die;
	}

	if ((sql_row = SQ_row_next(sql_result)) != NULL) {
		sql_str = SQ_get_column_string(sql_result, sql_row, 0);

		/* We must process all the rows of the result,*/
		/* otherwise we'll have them as part of the next qry */
		while ( (sql_row = SQ_row_next(sql_result)) != NULL) {
			LG_log(ud_context, LG_WARN, "duplicate PK [%s]\n", query);
			if (sql_str)
				UT_free(sql_str);
			sql_str=NULL;
		}
	} else
		sql_str=NULL;

	SQ_free_result(sql_result);
	return (sql_str);
}

/************************************************************
 * get_field_str()                                           *
 *                                                           *
 * Returns string containing the field.                      *
 *  field - field name to be retrieved                       *
 *  ref_tbl_name - name of the table containing the field    *
 *  ref_name - reference name                                *
 *  attr_value - reference value                             *
 *  condition - additional condition ( f.e. 'AND dummy=0'    *
 *                                                           *
 * Returns:                                                  *
 *  String containing the field. Needs to be freed after use *
 *  NULL in case of an error                                 *
 *                                                           *
 *************************************************************/
char *get_field_str(SQ_connection_t *sql_connection, const char *field, const char *ref_tbl_name, const char *ref_name,
        const char * attr_value, const char *condition) {
	GString *query;
	char *result;

	query = g_string_sized_new(STR_L);

	g_string_sprintf(query, "SELECT %s FROM %s "
		"WHERE %s='%s' ", field, ref_tbl_name, ref_name, attr_value);
	if (condition)
		g_string_append(query, condition);

	result = get_qresult_str(sql_connection, query->str);
	g_string_free(query, TRUE);
	return (result );

}

/************************************************************
 * Executes a query and returns the first row as an integer array
 * caller must preallocate buffer
 * dies on sql errors
 *
 * Returns:
 * SQ_OK    - no error
 * SQ_NORES - no results returned by query
 ************************************************************/
int get_fields_int_noalloc(SQ_connection_t *sql_connection, const char *sql_query, long *sql_res) {
    int retval = SQ_OK;
	SQ_result_set_t *sql_result;
	SQ_row_t *sql_row;
	int sql_err;
	int ret_size, i;

	sql_err = SQ_execute_query(sql_connection, sql_query, &sql_result);

	if (sql_err) {
		LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(sql_connection), sql_query);
		die;
	}

	if ((sql_row = SQ_row_next(sql_result)) != NULL) {
		ret_size = SQ_get_column_count(sql_result);
		for (i=0; i < ret_size; i++) {
			if (SQ_get_column_int(sql_result, sql_row, i, &sql_res[i])) {
				LG_log(ud_context, LG_SEVERE, "Error during SQ_get_column_int(%d) [%s]", i, sql_query);
				die;
			}
		}
	} else {
        retval = SQ_NORES;
    }

	if (sql_result) {
		SQ_free_result(sql_result);
		sql_result=NULL;
	}
    return retval;
}

/************************************************************
 * Executes a query and returns the first row as a long array
 * dies on sql errors
 *
 * Returns:
 * long* pointing to the integer array, to be freed by caller
 *
 *************************************************************/
long *get_fields_int(SQ_connection_t *sql_connection, const char *sql_query) {
	SQ_result_set_t *sql_result;
	SQ_row_t *sql_row;
	int sql_err;
	int ret_size, i;
	long *ret = NULL;

	sql_err = SQ_execute_query(sql_connection, sql_query, &sql_result);

	if (sql_err) {
		LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(sql_connection), sql_query);
		die;
	}

	if ((sql_row = SQ_row_next(sql_result)) != NULL) {
		ret_size = SQ_get_column_count(sql_result);
		if (ret_size) {
			ret = malloc(sizeof(long)*ret_size);
			for (i=0; i < ret_size; i++) {
				if (SQ_get_column_int(sql_result, sql_row, i, &ret[i])) {
					LG_log(ud_context, LG_SEVERE, "Error during SQ_get_column_int(%d) [%s]", i, sql_query);
					die;
				}
			}
		}
	}

	if (sql_result) {
		SQ_free_result(sql_result);
		sql_result=NULL;
	}

	return ret;
}

/************************************************************
 * long get_sequence_id(Transaction_t *tr)
 * >0 - success
 * -1 - sql error
 *
 * **********************************************************/

long get_sequence_id(Transaction_t *tr) {
	char *sql_str;
	char str_id[STR_M];
	long sequence_id=-1;

	sprintf(str_id, "%ld", tr->object_id);
	sql_str= get_field_str(tr->sql_connection, "sequence_id", "last", "object_id", str_id, NULL);
	if (sql_str) {
		sequence_id = atol(sql_str);
		UT_free(sql_str);
	}

	return (sequence_id);

}

/************************************************************
 * long get_ref_id(char *ref_tbl_name, char *ref_name, char * attr_value)
 * >0 - success
 * -1 - sql error
 *
 * **********************************************************/

static long get_ref_id(Transaction_t *tr, const char *ref_tbl_name, const char *ref_name, const char * attr_value,
        const char *condition) {
	char *sql_str;
	long ref_id=-1;

	sql_str= get_field_str(tr->sql_connection, "object_id", ref_tbl_name, ref_name, attr_value, condition);
	if (sql_str) {
		ref_id = atol(sql_str);
		UT_free(sql_str);
	}
	return (ref_id);
}

/************************************************************
 * int isdummy()
 *
 * Returns 1 if the object in question is a dummy,
 * otherwise returns 0.
 *
 * In case of error:
 * -1 - sql error or object does not exist
 *
 ***********************************************************/
int isdummy(Transaction_t *tr)
{
    char *sql_str;
    char str_id[STR_M];
    int object_type = -1;

    sprintf(str_id, "%ld", tr->object_id);
    sql_str = get_field_str(tr->sql_connection, "object_type", "last", "object_id", str_id, NULL);
    if (sql_str)
    {
        object_type = atoi(sql_str);
        UT_free(sql_str);
    }

    if (object_type == -1)
    {
        LG_log(ud_context, LG_SEVERE, "cannot get object type for object_id %s", str_id);
        die;
    }
    if (object_type == DUMMY_TYPE)
        return (1);
    else
        return (0);
}

/************************************************************
 * process_reverse_domain()                                  *
 *                                                           *
 * Tries to insert additional data for reverse domains       *
 * This data includes prefix and perfix length for reverse   *
 * delegation block. It is stored in inaddr_arpa table for   *
 * IPv4 and ip6int table for IPv6 address spaces             *
 *                                                           *
 * Returns:                                                  *
 * 0  success                                                *
 * -1 sql error                                              *
 *                                                           *
 *************************************************************/

static int process_reverse_domain(Transaction_t *tr, ip_prefix_t *prefptr, int op) {
	unsigned prefix, prefix_length; /* ipv4 */
	ip_v6word_t msb, lsb; /* ipv6 */
	char query[512];
	int num;
	int sql_err;

	if (IP_pref_b2_space(prefptr) == IP_V4) { /* ipv4 */
		if (op==0) { /* insert record */
			IP_revd_b2v4(prefptr, &prefix, &prefix_length);
			sprintf(query, "INSERT INTO inaddr_arpa SET thread_id=%d, object_id=%ld, prefix=%u, prefix_length=%d ",
			        tr->thread_ins, tr->object_id, prefix, prefix_length);
		} else {
			/* update record */
			sprintf(query, "UPDATE inaddr_arpa SET thread_id=%d WHERE object_id=%ld ", tr->thread_upd, tr->object_id);
		}
	} else { /* ipv6 */
		if (op==0) { /* insert record */
			IP_revd_b2v6(prefptr, &msb, &lsb, &prefix_length);
			sprintf(query,
			        "INSERT INTO ip6int SET thread_id=%d, object_id=%ld, msb='%llu', lsb='%llu', prefix_length=%d ",
			        tr->thread_ins, tr->object_id, (long long unsigned int)msb, (long long unsigned int)lsb, prefix_length);
		} else {
			/* update record */
			sprintf(query, "UPDATE ip6int SET thread_id=%d WHERE object_id=%ld ", tr->thread_upd, tr->object_id);
		}
	}

	LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, query);
	sql_err = SQ_execute_query(tr->sql_connection, query, (SQ_result_set_t **)NULL);
	num = SQ_get_affected_rows(tr->sql_connection);

	/* Check for errors */
	if (sql_err) {
		LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), query);
		die;
	}
	/* If nothing was affected then WHERE clause returned nothing - DB error */
	if (num == 0) {
		LG_log(ud_context, LG_ERROR, "insert inaddr had no effect [%s]\n", query);
		die;
	}
	return (0);
}

#define insert_reverse_domain(tr, pr) process_reverse_domain(tr, pr, 0)
#define update_reverse_domain(tr, pr) process_reverse_domain(tr, pr, 1)

/************************************************************
 * auth_member_of()                                          *
 *                                                           *
 * Function that checks the authorization for membership     *
 * (i.e. if the object is authorized to be a memeber by      *
 * mbrs-by-ref attribute of the set is refers by member-of   *
 * attribute).                                               *
 * First checks if 'mbrs-by-ref: ANY'                        *
 * If not then checks that maintner referenced by            *
 * mbrs-by-ref attribute of the set is the one in mnt-by.    *
 *                                                           *
 * Returns:                                                  *
 * 0  success                                                *
 * 1  not allowed                                            *
 * -1 SQL error                                              *
 *                                                           *
 *************************************************************/
static int auth_member_of(const rpsl_attr_t *attribute, Transaction_t *tr) {
	GString *query;
	char *set_name;
	char *qresult;

	int attribute_type;
	const gchar *attribute_value;

	attribute_type = rpsl_get_attr_id(rpsl_attr_get_name(attribute));
	attribute_value = rpsl_attr_get_value(attribute);

	/* Check if set has mbrs_by_ref==ANY
	 In such case mbrs_by_ref.mnt_id==0
	 */

	query = g_string_sized_new(STR_XL);

	set_name = get_set_name(tr->class_type);

	/* Check if the set protects itself with mbrs-by-ref attribute */
	g_string_sprintf(query, "SELECT COUNT(*) FROM mbrs_by_ref, %s "
		"WHERE mbrs_by_ref.object_id=%s.object_id "
		"AND %s.%s='%s' ", set_name, set_name, set_name, set_name, attribute_value);

	qresult = get_qresult_str(tr->sql_connection, query->str);
	/* should be '0' if there is no mbrs-by-ref attribute */
	if (strcmp(qresult, "0")==0) {
		/* there is no mbrs-by-ref attribute - so we cannot go ahead */
		LG_log(tr->src_ctx, LG_DEBUG, "[%ld] membership by reference is not allowed (no mbrs-by-ref) [%d:%s]",
		        tr->transaction_id, attribute_type, attribute_value);
		UT_free(qresult);
		g_string_free(query, TRUE);
		return (1);
	} else
		UT_free(qresult);

	/* Check if membership is protected by the keyword "ANY" */
	/* There is a dummy record in the mntner table with object_id==0, check src/SQL/main.data.sql */
	g_string_sprintf(query, "SELECT %s.object_id FROM mbrs_by_ref, %s "
		"WHERE mbrs_by_ref.object_id=%s.object_id "
		"AND %s.%s='%s' AND mbrs_by_ref.mnt_id=0 ", set_name, set_name, set_name, set_name, set_name, attribute_value);

	qresult = get_qresult_str(tr->sql_connection, query->str);
	/* if such record exists - go ahead */
	if (qresult) {
		UT_free(qresult);
		g_string_free(query, TRUE);
		return (0);
	}

	/* Now check if our mnt_by belongs to mbrs_by_ref list of the set */
	/* we search only mnt_by.thread_id!=0 to check against new/updated mnt-by attribute */
	/* We use MIN() as there are possibly multiple maintainers that are present in both the X-set and X, thus resulting in
	 * multiple records, that get_qresult_str() chokes on */
	g_string_sprintf(query, "SELECT MIN(mbrs_by_ref.object_id) FROM %s "
	        "JOIN mbrs_by_ref ON %s.object_id=mbrs_by_ref.object_id "
	        "JOIN mnt_by ON mnt_by.object_id = %ld AND mbrs_by_ref.mnt_id = mnt_by.mnt_id "
	        "WHERE %s.%s='%s' AND ( mnt_by.thread_id=%d OR mnt_by.thread_id=%d ) ",
	        set_name,
	        set_name,
	        tr->object_id,
	        set_name, set_name, attribute_value,
	        tr->thread_upd, tr->thread_ins);

	qresult = get_qresult_str(tr->sql_connection, query->str);
	/* If our mntner is listed (non-empty result)  membership is authorized */
	if (qresult) {
		UT_free(qresult);
		g_string_free(query, TRUE);
		return (0);
	} else {
		LG_log(tr->src_ctx, LG_DEBUG, "[%ld] membership by reference is not authorised [%d:%s]", tr->transaction_id,
		        attribute_type, attribute_value);
		g_string_free(query, TRUE);
		return (1);
	}
}/* auth_member_of()  */

/************************************************************
 * create_dummy()                                            *
 *                                                           *
 * Function that creates a dummy object (that is one that    *
 * is referenced from an object but does not                 *
 * exist in the database).                                   *
 * Dummy object exists only in relevant main and 'last'      *
 * tables. Its creation is controlled by tr->dummy_allowed.  *
 * Queries for the dummies are defined in Dummy[] array.     *
 *                                                           *
 * Returns:                                                  *
 * 0  success                                                *
 * 1  no rf integrity and dummy not allowed                  *
 * -1 SQL error                                              *
 *                                                           *
 *************************************************************/
static int create_dummy(const rpsl_attr_t *attribute, Transaction_t *tr) {
	const char *query_fmt;
	long dummy_id;
	GString *query;
	int result=0;
	char *set_name;
	char *p_name;
	int query_type;
	long timestamp;
	char str_id[16];
	int sql_err;
	char *token=NULL;

	int commit_now;
	nic_handle_t *nh_ptr;
	char *a_value;

	int attribute_type;
	const gchar *attribute_value;

	/* Determine the attribute type */
	attribute_type = rpsl_get_attr_id(rpsl_attr_get_name(attribute));
	/* Get attribute value .It is already clean since we process the flattened copy of the object */
	attribute_value = rpsl_attr_get_value(attribute);

	query_fmt = DF_get_dummy_query(attribute_type);
	if (strcmp(query_fmt, "") == 0) {
		LG_log(ud_context, LG_SEVERE, "empty query string\n");
		die;
	}
	/* for loader we do not perform commits/rollbacks */
	if (IS_STANDALONE(tr->mode))
		commit_now = 1;
	else
		commit_now = 0;

	/* do not allow dummy objects to be created when the mode doesnt permit */
	if (!IS_DUMMY_ALLOWED(tr->mode))
		return (1);

	/* Allocate resourses. We cannot use tr->query because it is in use */
	query = g_string_sized_new(STR_XL);

	/* Insert dummy in the last table */
	/* Calculate the object_id - should be max+1 */
	dummy_id = SQ_get_max_id(tr->sql_connection, "object_id", "last") +1;
	/* Record dummy's object_id, it'll be needed in commit/rollback */
	tr->dummy_id[tr->ndummy]=dummy_id;
	tr->ndummy++;

	/* Update the TR for crash recovery */
	/* If we crash before actually creating an entry in last */
	/* there should be no harm - later in rollback we will just try to delete nonexistent object */
	TR_update_dummy(tr);

	sprintf(str_id, "%ld", tr->object_id);
	timestamp=time(NULL);
	g_string_sprintf(query,
	        "INSERT INTO last SET thread_id=%d, object_id=%ld, timestamp=%ld, object_type=%d, object='DUMMY for %s'",
	        tr->thread_ins, dummy_id, timestamp, DUMMY_TYPE, str_id);

	LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, query->str);
	sql_err = SQ_execute_query(tr->sql_connection, query->str, (SQ_result_set_t **)NULL);

	/* Check for errors */
	if (sql_err) {
		LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), query->str);
		die;
	}

	/* compose the query */
	query_type=DF_get_dummy_query_type(attribute_type);
	switch (query_type) {

		/* person_role */
		case UD_AX_PR:
			/* irt      */
		case UD_AX_IT:
			/* maintner */
		case UD_AX_MT:
		case UD_AX_FR:
		case UD_AX_OA:
			g_string_sprintf(query, query_fmt, tr->thread_ins, dummy_id, attribute_value, DUMMY_TYPE);
			break;

			/* as_set, route_set */
		case UD_AX_MO:
			set_name = get_set_name(tr->class_type);
			g_string_sprintf(query, query_fmt, set_name, tr->thread_ins, dummy_id, set_name, attribute_value);
			break;

		default:
			LG_log(ud_context, LG_SEVERE, "query not defined for this type of attribute[%d]\n", attribute_type);
			die;
			break;
	}

	LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, query->str);
	sql_err = SQ_execute_query(tr->sql_connection, query->str, (SQ_result_set_t **)NULL);
	if (sql_err) {
		LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), query->str);
		die;
	}

	/* for legacy person/role reference try to allocate a nic handle in NHR, */
	/* if failied - create records in names table */
	if (query_type == UD_AX_PR) {
		if (ACT_UPD_NHR(tr->action) && (NH_parse(attribute_value, &nh_ptr)>0)) {
			int nhres;
			/* go ahead and register */
			if (NH_check(nh_ptr, tr->sql_connection)>0) {
				nhres = NH_register(nh_ptr, tr->sql_connection, commit_now);
				if (nhres == -1) {
					tr->succeeded=0;
					tr->error |= ERROR_U_DBS;
					LG_log(ud_context, LG_SEVERE, "cannot allocate nic handle %s", attribute_value);
					die;
				} else if (nhres == 0) {
					tr->succeeded=0;
					tr->error |= ERROR_U_OBJ;
					LG_log(tr->src_ctx, LG_INFO, "[%ld] nic handle [%s]: wrong or already in use", tr->transaction_id,
					        attribute_value);
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:nic-handle wrong or already in use\n",
					        ERROR_U_OBJ, attribute_type, attribute_value);
					result=-1;
				}
			} else {
				tr->succeeded=0;
				tr->error |= ERROR_U_OBJ;
				LG_log(tr->src_ctx, LG_INFO, "[%ld] nic handle [%s]: wrong or already in use", tr->transaction_id,
				        attribute_value);
				g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:nic-handle wrong or already in use\n", ERROR_U_OBJ,
				        attribute_type, attribute_value);
				result=-1;
			}
			free_nh(nh_ptr);
		} else {
			/* create record in the names table */
			query_fmt = DF_get_insert_query(A_PN);

			/* we need to copy string as strsep is destructive */
			a_value = g_strdup(attribute_value);
			token = a_value;

			while ((p_name=strsep(&token, " \t"))) {
				if (*p_name != '\0') {
					g_string_sprintf(query, query_fmt, tr->thread_ins, dummy_id, DUMMY_TYPE, p_name);
					LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, query->str);
					sql_err = SQ_execute_query(tr->sql_connection, query->str, (SQ_result_set_t **)NULL);
					if (sql_err && (SQ_errno(tr->sql_connection) != ER_DUP_ENTRY)) {
						LG_log(ud_context, LG_ERROR, "insert dummy names:%s[%s]\n", SQ_error(tr->sql_connection),
						        query->str);
						result=-1;
					}
				}
			}
			UT_free(a_value);
		}
	}

	g_string_free(query, TRUE);
	return (result);
}

/************************************************************
 * update_attr()                                             *
 *                                                           *
 * Function that updates an attribute if it already exists.  *
 * Called from each_attribute_proces() function if it        *
 * cannot insert the row.                                    *
 * Queries for the attributes are defined in Update[] array. *
 *                                                           *
 * Returns: Nothing. Error code is stored in tr->error.      *
 *                                                           *
 *************************************************************/
static void update_attr(const rpsl_attr_t *attribute, Transaction_t *tr) {
	int num;
	const char *query_fmt;
	char *set_name;
	unsigned int if_address;
	char * rf_host;
	int rf_port, rf_type;
	char *a_value;
	int sq_info[3];
	char * condition;
	char *sq_error;
	int sql_err;
	char *token;
	char *mu_mntner;
	ip_prefix_t dn_pref;
	ip_v6word_t high_ipv6, low_ipv6;

	int attribute_type;
	const char *attribute_value;

	/* Determine the attribute type */
	attribute_type = rpsl_get_attr_id(rpsl_attr_get_name(attribute));

	/* For 2-pass loader it may be needed to update second attribute */
	/* stored in the main table, like inetnum, filter-set, etc. */
	if ((IS_STANDALONE(tr->mode))&&(DF_get_update_query_type(attribute_type)!=UD_MA_U2))
		return;

	/* get the value of the attribue. It is already clean since we process the flattened copy of the object */
	attribute_value = rpsl_attr_get_value(attribute);

	/* Do some additional processing for reverse domains */
	/*We need to tag the record in inaddr_arpa or int6 tables dor the commit/rollback */
	/* Otherwise it will be deleted */
	/* XXX Later we will implement this under UD_MA_DN case */
	if ((attribute_type == A_DN) && (IP_revd_a2b(&dn_pref, attribute_value)==IP_OK)) {
		if (update_reverse_domain(tr, &dn_pref) !=0) {
			tr->error|=ERROR_U_DBS;
			tr->succeeded=0;
			g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:%s\n", ERROR_U_DBS, attribute_type, attribute_value,
			        SQ_error(tr->sql_connection));
		}
	}

	/* get query format string */
	query_fmt = DF_get_update_query(attribute_type);

	if (strcmp(query_fmt, "") == 0)
		return;

	switch (DF_get_update_query_type(attribute_type)) {
		case UD_MAIN_:
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id);
			break;
		case UD_MA_PR:
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->class_type, tr->object_id);
			break;
		case UD_MA_U2: /* save the new value of the attribute for commit*/
			/* this is necessary for filter(filter-set), netname (inet?num), */
			/* local-as(inet-rtr) attributes, as they are another field in the record */
			if (IS_STANDALONE(tr->mode)) {
				/* for 2-pass loader we need to update the field as we have no commit */
				g_string_sprintf(tr->query, query_fmt, DF_get_class_sql_table(tr->class_type), 0, attribute_value,
				        tr->object_id);
			} else {
				/* free memory in case U2 attribute appears more than once in the object */
				/* this should be spotted by dbupdate, otherwise just write over */
				if (tr->save)
					UT_free(tr->save);
				tr->save=g_strdup(attribute_value);
				/* update TR for crash recovery */
				TR_update_save(tr);
				return;
			}
			break;
		case UD_AX_PR:
			if (!IS_DUMMY_ALLOWED(tr->mode))
				condition="AND dummy=0 ";
			else
				condition=NULL;
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, get_ref_id(tr, "person_role",
			        "nic_hdl", attribute_value, condition));
			break;
		case UD_AX_MT:
			if (!IS_DUMMY_ALLOWED(tr->mode))
				condition="AND dummy=0 ";
			else
				condition=NULL;
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, get_ref_id(tr, "mntner", "mntner",
			        attribute_value, condition));
			break;
		case UD_AX_FR:
			if (!IS_DUMMY_ALLOWED(tr->mode))
				condition="AND dummy=0 ";
			else
				condition=NULL;
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, get_ref_id(tr, "poetic_form",
			        "poetic_form", attribute_value, condition));
			break;
		case UD_AX_OA:
			if (!IS_DUMMY_ALLOWED(tr->mode))
				condition="AND dummy=0 ";
			else
				condition=NULL;
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, get_ref_id(tr, "organisation",
			        "organisation", attribute_value, condition));
			break;
		case UD_AX_IT:
			if (!IS_DUMMY_ALLOWED(tr->mode))
				condition="AND dummy=0 ";
			else
				condition=NULL;
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, get_ref_id(tr, "irt", "irt",
			        attribute_value, condition));
			break;
		case UD_AX_MU: /* for mnt_routes & mnt_routes6 tables */
			a_value=g_strdup(attribute_value);
			token = a_value;
			/* we only store the mntner name, don't care about prefixes */
			mu_mntner=strsep(&token, " \t");
			if (!IS_DUMMY_ALLOWED(tr->mode))
				condition="AND dummy=0 ";
			else
				condition=NULL;
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, get_ref_id(tr, "mntner", "mntner",
			        mu_mntner, condition));
			UT_free(a_value);
			break;
		case UD_AX_MO:
			set_name = get_set_name(tr->class_type);
			if (!IS_DUMMY_ALLOWED(tr->mode))
				condition="AND dummy=0 ";
			else
				condition=NULL;
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, get_ref_id(tr, set_name, set_name,
			        attribute_value, condition));
			break;
		case UD_AX_MR:
			if ((g_strcasecmp(attribute_value, "ANY")==0))
				g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, get_ref_id(tr, "mntner",
				        "mntner", "ANY", NULL));
			else {
				if (!IS_DUMMY_ALLOWED(tr->mode))
					condition="AND dummy=0 ";
				else
					condition=NULL;
				g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, get_ref_id(tr, "mntner",
				        "mntner", attribute_value, condition));
			}
			break;
		case UD_LEAF_:
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, attribute_value);
			break;
		case UD_LF_IF:
			/* Convert ascii ip -> numeric one */
			convert_if(attribute_value, &if_address);
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, if_address);
			break;
		case UD_LF_IE: /* for interface table */
			/* get the components of the attribute value */
			/*  IPv4 address: numeric value of IPv4 interface address, when applicable         */
			/*  IPv6 address high half: most significant half of IPv6 address, when applicable  */
			/*  IPv6 address low half: least significant half of IPv6 address, when applicable  */
			/* when not applicable, high_ipv6, low_ipv6, if_address must be set to 0            */
			convert_ie(attribute_value, &if_address, &high_ipv6, &low_ipv6);
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, high_ipv6, low_ipv6, if_address);
			break;
		case UD_LF_RF:
			rf_host=convert_rf(attribute_value, &rf_type, &rf_port);
			if (rf_host == NULL) {
				tr->error|=ERROR_U_OBJ;
				tr->succeeded=0;
				g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:incorrect refer attribute\n", ERROR_U_OBJ,
				        attribute_type, attribute_value);
				return;
			} else {
				g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, rf_type, rf_host, rf_port);
				UT_free(rf_host);
			}
			break;
		case UD_LF_AY:
			g_string_sprintf(tr->query, query_fmt, tr->thread_upd, tr->object_id, convert_time(attribute_value));
			break;
		default:
			tr->error|=ERROR_U_BUG;
			tr->succeeded=0;
			g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:no update qry\n", ERROR_U_BUG, attribute_type,
			        attribute_value);
			LG_log(ud_context, LG_SEVERE, "query not defined for this type of attribute:[%d:%s]\n", attribute_type,
			        attribute_value);
			die;
			break;
	}
	/* Execute the query */
	LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, tr->query);
	sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
	if (sql_err) { /* an error occured*/
		/* Error - copy the error condition and return */
		sq_error=SQ_error(tr->sql_connection);
		LG_log(ud_context, LG_ERROR, "%s[%s]\n", sq_error, tr->query->str);
		tr->error|=ERROR_U_DBS;
		tr->succeeded=0;
		g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:%s\n", ERROR_U_DBS, attribute_type, attribute_value, sq_error);
		die;
	} else {
		/* Query OK */
		num = SQ_get_affected_rows(tr->sql_connection);
		if (num == 0) { /* check for duplicates*/
			SQ_get_info(tr->sql_connection, sq_info); /* UPDATE ... SET*/
			if ((sq_info[SQL_DUPLICATES]==0) && (sq_info[SQL_MATCHES]==0)) {
				/* Condition with zero duplicates and matches may occur when the object is a dummy */
				/* and we are running in protected mode ( dummies are not allowed, tr->dummy==0). */
				/* In such case we will append "AND dummy=0" to the query, which won't */
				/* return a match if the object in question is a dummy */
				LG_log(tr->src_ctx, LG_INFO, "[%ld] dummy prevents update: [%s]", tr->transaction_id, tr->query->str);
				tr->error|=ERROR_U_OBJ;
				tr->succeeded=0;
				g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:ref integrity: reference cannot be resolved\n",
				        ERROR_U_OBJ, attribute_type, attribute_value);
			} /* else duplicate entry - silently drop it  */
		}
		/* For member_of attribute we need to check membership claim in protected mode */
		if ((attribute_type == A_MO) && (!IS_DUMMY_ALLOWED(tr->mode))) {
			if (auth_member_of(attribute, tr)!=0) {
				tr->error|=ERROR_U_AUT;
				tr->succeeded=0;
				LG_log(tr->src_ctx, LG_INFO, "[%ld] membership by reference is not allowed [%d:%s]",
				        tr->transaction_id, attribute_type, attribute_value);
				g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:membership not allowed\n", ERROR_U_AUT,
				        attribute_type, attribute_value);
			}
		}
	}
	return;
}/*  update_attr()  */

/************************************************************
 * create_attr()                                             *
 *                                                           *
 * A function that tries to create an attribute in a relevant*
 * sql table. If unsuccessful we check if it's an update     *
 * For primary keys no creations are attempted               *
 *                                                           *
 * Returns:                                                  *
 *  ATTR_CREATE_OK = ok                                      *
 *  ATTR_CREATE_NO = this is an update, go to update_attr()  *
 *  ATTR_CREATE_DONE = no further action is needed, return   *
 *  ATTR_CREATE_ERR = an sql error occured                   *
 *                                                           *
 *************************************************************/
static int create_attr(const rpsl_attr_t *attribute, Transaction_t *tr) {
	const char *query_fmt;
	int query_type;
	int do_query;
	unsigned int prefix, prefix_length, if_address;
	unsigned int begin_in, end_in;
	ip_v6word_t high, low;
	ip_v6word_t high_ipv6, low_ipv6;

	unsigned long begin_as, end_as;
	char * set_name;
	char * rf_host; /* needs to be freed after use*/
	int rf_type, rf_port;
	char *a_value;
	char *mu_mntner;
	int sql_err;
	int res;
	char *token;
	int result;

	int attribute_type;
	const gchar *attribute_value;

	/* To switch off querying for some types of attributes */
	do_query=1;
	/* default result */
	result = ATTR_CREATE_NO;

	/* Determine the query type */
	attribute_type = rpsl_get_attr_id(rpsl_attr_get_name(attribute));
	/* attribute value is already clean since we have a "flattened" object */
	attribute_value = rpsl_attr_get_value(attribute);

	query_type=DF_get_insert_query_type(attribute_type);
	query_fmt = DF_get_insert_query(attribute_type);

	/* compose the query depending on the attribute */
	switch (query_type) {
		case UD_MAIN_: /* for MAIN tables */
			if (ACT_UPDATE(tr->action))
				do_query=0;
			else
				g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, attribute_value);

			/* for organisation attributes, we need to update NHR (organisation_id table) */
			if ((attribute_type == A_OA) && ACT_UPD_NHR(tr->action)) {
				/* Check if we can allocate it */
				res = NH_check_org(tr->nh, tr->sql_connection);
				if (res == -1) { /* we cannot allocate this NIC handle (DB error) */
					tr->succeeded=0;
					tr->error |= ERROR_U_DBS;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:cannot allocate nic-handle\n", ERROR_U_DBS,
					        attribute_type, attribute_value);
					LG_log(ud_context, LG_SEVERE, "cannot allocate nic hdl[%s]\n", attribute_value);
					die;
				} else if (res == 0) { /* we cannot allocate this NIC handle (full space or ID in use) */
					tr->succeeded=0;
					tr->error |= ERROR_U_OBJ;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:nic-handle wrong or already in use\n",
					        ERROR_U_OBJ, attribute_type, attribute_value);
					result = ATTR_CREATE_DONE;
					do_query = 0;
				}
			}
			break;

		case UD_MA_OR: /* for the origin attribute */
			if (ACT_UPDATE(tr->action))
				do_query=0;
			else {
				/* Here, the two possible table names ('route' and 'route6') are of the
				 same name as the two possible object names.
				 So we can use rpsl_object_get_class(tr->object) */
				g_string_sprintf(tr->query, query_fmt, rpsl_object_get_class(tr->object), tr->thread_ins,
				        attribute_value, tr->object_id);
				tr->action |= TA_UPD_RX;
				RP_pack_set_orig(attribute_type, tr->packptr, attribute_value);
			}
			break;

		case UD_MA_PR: /* for person_role table*/
			if (ACT_UPDATE(tr->action))
				do_query=0;
			else
				g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->class_type, tr->object_id, attribute_value);

			/* check if we need to update NHR */
			if (ACT_UPD_NHR(tr->action)) {
				/* Check if we can allocate it */
				res = NH_check(tr->nh, tr->sql_connection);
				if (res == -1) { /* we cannot allocate this NIC handle (DB error) */
					tr->succeeded=0;
					tr->error |= ERROR_U_DBS;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:cannot allocate nic-handle\n", ERROR_U_DBS,
					        attribute_type, attribute_value);
					LG_log(ud_context, LG_SEVERE, "cannot allocate nic hdl[%s]\n", attribute_value);
					die;
				} else if (res == 0) { /* we cannot allocate this NIC handle (full space or ID in use) */
					tr->succeeded=0;
					tr->error |= ERROR_U_OBJ;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:nic-handle wrong or already in use\n",
					        ERROR_U_OBJ, attribute_type, attribute_value);
					result = ATTR_CREATE_DONE;
					do_query = 0;
				}
			}
			break;
		case UD_MA_RT: /* for route table*/
			if (ACT_UPDATE(tr->action))
				do_query=0;
			else {
				tr->action |= TA_UPD_RX;
				/* be strict here. bit inconsistencies should result in an error */
				if (RP_pack_set_pref4(attribute_type, attribute_value, tr->packptr, &prefix, &prefix_length)==IP_OK) {
					g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, prefix, prefix_length);
				} else {
					tr->succeeded=0;
					tr->error |= ERROR_U_OBJ;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:wrong prefix specified\n", ERROR_U_OBJ,
					        attribute_type, attribute_value);
					result = ATTR_CREATE_DONE;
					do_query = 0;
				}
			}
			break;
		case UD_MA_R6: /* for route6 table*/
			if (ACT_UPDATE(tr->action))
				do_query=0;
			else {
				/* be strict here. bit inconsistencies should result in an error */
				if (RP_pack_set_pref6(attribute_type, attribute_value, tr->packptr, &high, &low, &prefix_length)==IP_OK) {
					g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, high, low, prefix_length);
					tr->action |= TA_UPD_RX;
				} else {
					tr->succeeded=0;
					tr->error |= ERROR_U_OBJ;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:wrong prefix specified\n", ERROR_U_OBJ,
					        attribute_type, attribute_value);
					result = ATTR_CREATE_DONE;
					do_query = 0;
				}
			}
			break;
		case UD_MA_IN: /* for inetnum table*/
			if (ACT_UPDATE(tr->action))
				do_query=0;
			else {
				/* be strict here. bit inconsistencies should result in an error */
				if (RP_pack_set_rang(attribute_type, attribute_value, tr->packptr, &begin_in, &end_in)==IP_OK) {
					tr->action |= TA_UPD_RX;
					g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, begin_in, end_in);
				} else {
					tr->succeeded=0;
					tr->error |= ERROR_U_OBJ;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:wrong range specified\n", ERROR_U_OBJ,
					        attribute_type, attribute_value);
					result = ATTR_CREATE_DONE;
					do_query = 0;
				}
			}
			break;
		case UD_MA_I6: /* for inet6num table*/
			if (ACT_UPDATE(tr->action))
				do_query=0;
			else {
				/* be strict here. bit inconsistencies should result in an error */
				if (RP_pack_set_pref6(attribute_type, attribute_value, tr->packptr, &high, &low, &prefix_length)==IP_OK) {
					g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, high, low, prefix_length);
					tr->action |= TA_UPD_RX;
				} else {
					tr->succeeded=0;
					tr->error |= ERROR_U_OBJ;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:wrong prefix specified\n", ERROR_U_OBJ,
					        attribute_type, attribute_value);
					result = ATTR_CREATE_DONE;
					do_query = 0;
				}
			}
			break;
		case UD_MA_U2: /* This is actually an update - go to update_attr - this is more logical */
			do_query=0;
			break;
		case UD_MA_AK: /* for as_block table*/
			if (ACT_UPDATE(tr->action))
				do_query=0;
			else {
				convert_as_range(attribute_value, &begin_as, &end_as);
				g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, begin_as, end_as);
			}
			break;
		case UD_AUX__: /* for AUX tables*/
			g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, tr->class_type, attribute_value);
			/* A_OG is the code for "org:" attr */
			if (!IS_DUMMY_ALLOWED(tr->mode) && attribute_type!=A_OG)
				g_string_sprintfa(tr->query, " AND dummy=0 ");
			break;
		case UD_AX_MO: /* for member_of table*/
			set_name = get_set_name(tr->class_type);
			g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, set_name, tr->class_type, set_name,
			        set_name, set_name, attribute_value);
			break;
		case UD_AX_MR: /* for mbrs_by_ref table*/
			if ((g_strcasecmp(attribute_value, "ANY")==0))
				g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, tr->class_type, "ANY");
			else
				g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, tr->class_type, attribute_value);
			break;
		case UD_AX_MU: /* for mnt_routes and mnt_routes6 tables */
			a_value = g_strdup(attribute_value);
			token = a_value;
			/* we only store the mntner name, don't care about prefixes */
			mu_mntner=strsep(&token, " \t");
			g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, tr->class_type, mu_mntner);
			UT_free(a_value);
			if (!IS_DUMMY_ALLOWED(tr->mode))
				g_string_sprintfa(tr->query, " AND dummy=0 ");
			break;
		case UD_LEAF_: /* for LEAF tables*/
			g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, attribute_value);
			break;
		case UD_LF_OT: /* for LEAF tables containing object_type field*/
			g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, tr->class_type, attribute_value);
			break;
		case UD_LF_AT: /* check PGPKEY. If yes - check the existence of key-cert.*/
			if (!IS_DUMMY_ALLOWED(tr->mode)) {
				if (strncmp("PGPKEY", attribute_value, 6)==0) {
					if (get_ref_id(tr, "key_cert", "key_cert", attribute_value, NULL)<=0) {
						LG_log(tr->src_ctx, LG_INFO, "[%ld] no key-cert object[%s]", tr->transaction_id,
						        attribute_value);
						tr->error|=ERROR_U_OBJ;
						tr->succeeded=0;
						g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:no key-cert object\n", ERROR_U_OBJ,
						        attribute_type, attribute_value);
						result = ATTR_CREATE_DONE;
						do_query = 0;
						break;
					}
				}
			}
			g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, tr->class_type, attribute_value);
			break;
		case UD_LF_IF: /* for ifaddr tables*/
			/* Convert ascii ip -> numeric one*/
			convert_if(attribute_value, &if_address);
			g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, if_address);
			break;
		case UD_LF_IE: /* for interface table */
			/* get the components of the attribute value */
			/*  IPv4 address: numeric value of IPv4 interface address, when applicable         */
			/*  IPv6 address high half: most significant half of IPv6 address, when applicable  */
			/*  IPv6 address low half: least significant half of IPv6 address, when applicable  */
			/* when not applicable, high_ipv6, low_ipv6, if_address must be set to 0            */
			convert_ie(attribute_value, &if_address, &high_ipv6, &low_ipv6);
			g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, high_ipv6, low_ipv6, if_address);
			break;
		case UD_LF_RF: /* for refer table*/
			rf_host=convert_rf(attribute_value, &rf_type, &rf_port);
			/*XXX Maybe this is redundant */
			if (rf_host == NULL) {
				tr->error|=ERROR_U_OBJ;
				tr->succeeded=0;
				g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:incorrect refer attribute\n", ERROR_U_OBJ,
				        attribute_type, attribute_value);
				result = ATTR_CREATE_DONE;
				do_query = 0;
			} else {
				g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, rf_type, rf_host, rf_port);
				UT_free(rf_host);
			}
			break;
		case UD_LF_AY: /* for auth_override table*/
			g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, convert_time(attribute_value));
			break;
		default:
			tr->succeeded=0;
			tr->error |= ERROR_U_BUG;
			g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:query not defined for the attribute\n", ERROR_U_BUG,
			        attribute_type, attribute_value);
			LG_log(ud_context, LG_SEVERE, "query not defined for this type of attribute:[%d:%s]\n", attribute_type,
			        attribute_value);
			die;
			break;
		case UD_AX_PF: /* for Poetic Form tables*/
			g_string_sprintf(tr->query, query_fmt, tr->thread_ins, tr->object_id, attribute_value);
			break;
	}

	/* Make the query. For primary keys go straight to updates if we are updating the object */
	if (do_query) {
		LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, tr->query->str);
		sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
		if (sql_err)
			result = ATTR_CREATE_ERR;
		else
			result = ATTR_CREATE_OK;
	}

	return (result);
}/* create_attr() */

/************************************************************
 * each_attribute_proces()                                   *
 *                                                           *
 * Main function that processes object attributes one by one.*
 * Called from g_slist_foreach() function.                   *
 * First it tries to insert an attribute.                    *
 * If an error it assumes that attribute is already in       *
 * a table and calls update_attr() to update it.             *
 * Queries for the attributes are defined in Insert[] array. *
 *                                                           *
 * Returns: Nothing. Error code is stored in tr->error.      *
 *                                                           *
 *************************************************************/
static void each_attribute_process(void *element_data, void *tr_ptr) {
	int num;
	const char *query_fmt;
	int query_type;
	int do_query;
	const rpsl_attr_t *attribute = (const rpsl_attr_t *)element_data;
	Transaction_t *tr = (Transaction_t *)tr_ptr;

	int sq_info[3];
	int dummy_err;
	char *sq_error;
	ip_prefix_t dn_pref;
	int sql_err;

	int attribute_type;
	const gchar *attribute_value;

	int result;

	/* we still want to continue to collect all possible errors*/
	/* To switch off querying for some types of attributes */
	do_query=1;

	/* Determine the query type */
	attribute_type = rpsl_get_attr_id(rpsl_attr_get_name(attribute));
	query_type=DF_get_insert_query_type(attribute_type);

	/* For loading pass #1 we need to process only main tables */
	if (tr->load_pass==1) {
		switch (query_type) {
			case UD_MAIN_:
			case UD_MA_U2:
			case UD_MA_PR:
			case UD_MA_RT:
			case UD_MA_R6:
			case UD_MA_IN:
			case UD_MA_I6:
			case UD_MA_OR:
			case UD_MA_AK:
				break;
			default:
				return; /* return for other than MAIN tables*/
		}
	}

	query_fmt = DF_get_insert_query(attribute_type);

	/* return if no query is defined for this attribute */
	if (strcmp(query_fmt, "") == 0)
		return;

	/* Try to create an attribute in SQL database*/
	result = create_attr(attribute, tr);

	/* The value is clean since the object is "flattened" */
	attribute_value = rpsl_attr_get_value(attribute);

	switch (result) {

		case ATTR_CREATE_DONE:
			/* no more processing required */
			break;

		case ATTR_CREATE_NO:
			/* for object updates for primary keys go straight to updates */
			/* most of the primary keys are just skipped */
			/* secod keys are updated, also dummy flag is reset for sets, pn/ro, mt */
			update_attr(attribute, tr);
			break;

		case ATTR_CREATE_ERR:
			if (SQ_errno(tr->sql_connection) == ER_DUP_ENTRY) { /* Only error "Duplicate entry" may be considered*/
				if (ACT_UPDATE(tr->action)) { /* In update mode this is common (so actually not an error)*/
					update_attr(attribute, tr);
				}
				/* Otherwise this is a duplicate attribute, just ignore it */
				/* In the future if we are more stringent, checks may be added here */
			} else { /* Other errors reveal a database/server problem*/
				sq_error=SQ_error(tr->sql_connection);
				tr->error|=ERROR_U_DBS;
				tr->succeeded=0;
				LG_log(ud_context, LG_ERROR, "%s[%s]\n", sq_error, tr->query->str);
				g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:%s\n", ERROR_U_DBS, attribute_type, attribute_value,
				        sq_error);
				die;
			}
			break;

		case ATTR_CREATE_OK:
			/* If the query was successful */
			num = SQ_get_affected_rows(tr->sql_connection);
			if (num>0) { /* this is OK*/
				/* Do some additional processing for member_of attribute  */
				if ((attribute_type == A_MO) && (!IS_DUMMY_ALLOWED(tr->mode))) {
					if (auth_member_of(attribute, tr)!=0) {
						tr->error|=ERROR_U_AUT;
						tr->succeeded=0;
						LG_log(tr->src_ctx, LG_INFO, "[%ld] membership not allowed [%d:%s]", tr->transaction_id,
						        attribute_type, attribute_value);
						g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:membership not allowed\n", ERROR_U_AUT,
						        attribute_type, attribute_value);
					}
				} else {
					/* Do some additional processing for reverse zones domains */
					if ((attribute_type == A_DN) && IP_revd_a2b(&dn_pref, attribute_value)==IP_OK) {

						if (insert_reverse_domain(tr, &dn_pref) != 0) {
							tr->error|=ERROR_U_DBS;
							tr->succeeded=0;
							LG_log(ud_context, LG_SEVERE, "cannot insert inverse domain:[%d:%s]\n", attribute_type,
							        attribute_value);
							die;
						} else {
							/* save data for the radix tree update */
							tr->action |= TA_UPD_RX;
							RP_pack_set_revd(attribute_type, attribute_value, tr->packptr);
						}
					}
				}
			} else { /* if num == 0 */
				/* this could be an empty update or a null select */
				SQ_get_info(tr->sql_connection, sq_info);
				if (sq_info[SQL_DUPLICATES]>0) {
					/* INSERT ... SELECT ... affected 0 rows, but there is 1 duplicate */
					/* which means that we already have such record in the table */
					/* this indicates that this is actually an update - update this attribute */
					if (sq_info[SQL_DUPLICATES]>1) {
						tr->error|=ERROR_U_DBS;
						tr->succeeded=0;
						LG_log(ud_context, LG_SEVERE, "too many duplicates:[%d:%s]\n", attribute_type, attribute_value);
						die;
					}
					update_attr(attribute, tr);
				} else {
					/* this is an emty SELECT because there is no referred object */
					/* try to create dummy and repeat the original query*/
					dummy_err = create_dummy(attribute, tr);
					if (dummy_err == 0) {
						/* Dummy was created */
						g_string_sprintfa(tr->error_script, "W[%d][%d:%s]:dummy created\n", 0, attribute_type,
						        attribute_value);
						LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, tr->query->str);
						sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
						num = SQ_get_affected_rows(tr->sql_connection);
						if (sql_err) {
							sq_error=SQ_error(tr->sql_connection);
							LG_log(ud_context, LG_ERROR, "%s[%s]\n", sq_error, tr->query->str);
							tr->error|=ERROR_U_DBS;
							tr->succeeded=0;
							g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:%s\n", ERROR_U_DBS, attribute_type,
							        attribute_value, sq_error);
							die;
						}
						if (num==0) {
							LG_log(ud_context, LG_ERROR, "0 rows affected [%s]\n", tr->query->str);
							tr->error|=ERROR_U_DBS;
							tr->succeeded=0;
							g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:re-insert qry\n", ERROR_U_DBS,
							        attribute_type, attribute_value);
							die;
						}
					} else {
						if (dummy_err == 1) {
							/* dummy not allowed */
							tr->error |= ERROR_U_OBJ;
							tr->succeeded=0;
							g_string_sprintfa(tr->error_script,
							        "E[%d][%d:%s]:ref integrity: reference cannot be resolved\n", ERROR_U_OBJ,
							        attribute_type, attribute_value);
							LG_log(tr->src_ctx, LG_INFO, "[%ld] dummy not allowed [%d:%s]", tr->transaction_id,
							        attribute_type, attribute_value);
						} else {
							/* SQL problem */
							tr->error|=ERROR_U_DBS;
							tr->succeeded=0;
							LG_log(ud_context, LG_ERROR, "dummy cannot be created [%d:%s]", attribute_type,
							        attribute_value);
							g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:dummy cannot be created\n", ERROR_U_DBS,
							        attribute_type, attribute_value);
							die;
						}
					}
				} /* RI*/
			}/* if num == 0*/
			break;

		default:
			LG_log(ud_context, LG_SEVERE, "unknown return code %d from create_attr()", result);
			die;
			break;
	} /* switch result from create_attr() */

	return;
} /* each_attribute_process() */

/************************************************************
 * ud_each_primary_key_select()                              *
 *                                                           *
 * Function that forms a query for an object (w/ primary keys)*
 * Called from g_slist_foreach() function.                   *
 * Primary keys are defined in Select[] array.               *
 *                                                           *
 * Returns: Nothing.                                         *
 *                                                           *
 *************************************************************/
void ud_each_primary_key_select(void *element_data, void *result_ptr) {
	const rpsl_attr_t *attribute = (const rpsl_attr_t *)element_data;
	Transaction_t *tr = (Transaction_t *)result_ptr;
	const char *query_fmt;
	unsigned int prefix, prefix_length;
	unsigned int begin_in, end_in;
	unsigned long begin_as, end_as;
	ip_prefix_t prefstr;
	ip_range_t rangstr;
	ip_v6word_t i6_msb, i6_lsb;

	int attribute_type;
	const gchar * attribute_value;
	int do_query;

	/* get type the attribute */
	attribute_type = rpsl_get_attr_id(rpsl_attr_get_name(attribute));

	query_fmt = DF_get_select_query(attribute_type);

	/* For a 1/2 pass fill tr->K only (used in loader 1 pass) */
	if (tr->load_pass == 1)
		do_query = 0;
	else
		do_query=1;

	if (strcmp(query_fmt, "") != 0) {
		/* get value of the attribute. It is already clean since the object is "flattened" */
		attribute_value = rpsl_attr_get_value(attribute);

		switch (DF_get_select_query_type(attribute_type)) {
			case UD_MAIN_:
				if (do_query)
					g_string_sprintfa(tr->query, query_fmt, attribute_value);
				g_string_sprintfa(tr->K, attribute_value);
				break;
			case UD_MA_RT:
				/* be strict here. bit inconsistencies should result in an error */
				if (IP_pref_a2v4(attribute_value, &prefstr, &prefix, &prefix_length)==IP_OK) {
					if (do_query)
						g_string_sprintfa(tr->query, query_fmt, prefix, prefix_length);
					g_string_sprintfa(tr->K, attribute_value);
				} else {
					tr->succeeded = 0;
					tr->error |= ERROR_U_OBJ;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:wrong prefix specified\n", ERROR_U_OBJ,
					        attribute_type, attribute_value);
				}
				break;
			case UD_MA_IN:
				if (IP_rang_a2v4(attribute_value, &rangstr, &begin_in, &end_in)==IP_OK) {
					if (do_query)
						g_string_sprintfa(tr->query, query_fmt, begin_in, end_in);
					g_string_sprintfa(tr->K, attribute_value);
				} else {
					tr->succeeded = 0;
					tr->error |= ERROR_U_OBJ;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:wrong range specified\n", ERROR_U_OBJ,
					        attribute_type, attribute_value);
				}
				break;
			case UD_MA_R6:
				/* be strict here. bit inconsistencies should result in an error */
				if (IP_pref_a2v6(attribute_value, &prefstr, &i6_msb, &i6_lsb, &prefix_length)==IP_OK) {
					if (do_query)
						g_string_sprintfa(tr->query, query_fmt, i6_msb, i6_lsb, prefix_length);
					g_string_sprintfa(tr->K, attribute_value);
				} else {
					tr->succeeded = 0;
					tr->error |= ERROR_U_OBJ;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:wrong prefix specified\n", ERROR_U_OBJ,
					        attribute_type, attribute_value);
				}
				break;
			case UD_MA_I6:
				/* be strict here. bit inconsistencies should result in an error */
				if (IP_pref_a2v6(attribute_value, &prefstr, &i6_msb, &i6_lsb, &prefix_length)==IP_OK) {
					if (do_query)
						g_string_sprintfa(tr->query, query_fmt, i6_msb, i6_lsb, prefix_length);
					g_string_sprintfa(tr->K, attribute_value);
				} else {
					tr->succeeded = 0;
					tr->error |= ERROR_U_OBJ;
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:wrong prefix specified\n", ERROR_U_OBJ,
					        attribute_type, attribute_value);
				}
				break;
			case UD_MA_AK:
				convert_as_range(attribute_value, &begin_as, &end_as);
				if (do_query)
					g_string_sprintfa(tr->query, query_fmt, begin_as, end_as);
				g_string_sprintfa(tr->K, attribute_value);
				break;
			default:
				LG_log(ud_context, LG_SEVERE, "query not defined for this type of attribute:[%d:%s]\n", attribute_type,
				        attribute_value);
				die;
				break;
		}
	}
}

/************************************************************
 * perform_create(const Object_t *obj, Transaction_t *tr)    *
 *                                                           *
 * Procedure for creating a new object.                      *
 * First inserts object into 'last' table and gets object_id.*
 * Then processes all attributes.                            *
 *                                                           *
 * Returns: tr->succeeded: >0 success, 0 - error             *
 * Error code is stored in tr->error.                        *
 *                                                           *
 *************************************************************/
static int perform_create(Transaction_t *tr) {
	long timestamp;
	int sql_err;

	timestamp=time(NULL);
	tr->sequence_id=1; /* we start with 1*/
	/* Calculate the object_id - should be max+1 */
	/* XXX we cannot use autoincrement with MyISAM tables */
	/* XXX because they keep the max inserted id even if  */
	/* XXX it was deleted later, thus causing gaps we don't want */

	tr->object_id = SQ_get_max_id(tr->sql_connection, "object_id", "last") +1;
	TR_update_id(tr);

	g_string_sprintf(tr->query, "INSERT INTO last SET thread_id=%d, object_id=%ld, "
		"timestamp=%ld, sequence_id=1, object_type=%d, object='%s', pkey='%s' ", tr->thread_ins, tr->object_id,
	        timestamp, tr->class_type, tr->object_txt, tr->K->str);

	LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, tr->query->str);
	sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);

	/* Check for affected rows. One row should be affected . */
	if (sql_err) {
		tr->error|=ERROR_U_DBS;
		tr->succeeded=0;
		LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
		die;
	} else {
		g_list_foreach((GList *)rpsl_object_get_all_attr(tr->object), each_attribute_process, tr);
	}

	return (tr->succeeded);
} /* perform_create() */

/************************************************************
 * perform_update(Transaction_t *tr)                         *
 *                                                           *
 * Procedure for updating (existing) object.                 *
 * First processes all attributes.                           *
 * Then saves previous object in 'history' and updates       *
 * 'last' table.                                             *
 *                                                           *
 * Returns: tr->succeeded: >0 success, 0 - error             *
 * Error code is stored in tr->error.                        *
 *                                                           *
 *************************************************************/
static int perform_update(Transaction_t *tr) {
	int num;
	long sequence_id;
	long timestamp;
	char *sq_error;
	int sql_err;

	/* get sequence number */
	sequence_id = get_sequence_id(tr);
	if (sequence_id==-1) {
		tr->error|=ERROR_U_DBS;
		tr->succeeded=0;
		LG_log(ud_context, LG_SEVERE, "cannot get sequence_id");
		die;
	} else
		tr->sequence_id=sequence_id; /* save it for rollback*/
	/* Update TR record */
	TR_update_id(tr);

	/* GET a timestamp */
	timestamp=time(NULL);

	/* process each attribute one by one */
	g_list_foreach((GList *)rpsl_object_get_all_attr(tr->object), each_attribute_process, tr);

	/* If we've already failed or this is 2-pass load - just return */
	if ((tr->succeeded == 0) || (tr->load_pass == 1) || (tr->load_pass == 2))
		return (tr->succeeded);

	/*XXX */
	if (IS_STANDALONE(tr->mode)) {
		g_string_sprintf(tr->query, "REPLACE last "
			"SET thread_id=%d, object_id=%ld, sequence_id=%ld, timestamp=%ld, object_type=%d, object='%s', pkey='%s' ",
		        tr->thread_ins, tr->object_id, tr->sequence_id, timestamp, tr->class_type, tr->object_txt, tr->K->str);
	} else {
		/* No return: thread_id=0 */
		/* Do it only if previous transactions finished well */
		/* copy object to the history table */
		g_string_sprintf(tr->query, "INSERT history "
			"SELECT %d, object_id, sequence_id, timestamp, object_type, object, pkey, serial, prev_serial "
			"FROM last "
			"WHERE object_id=%ld ", tr->thread_ins, tr->object_id);
	}

	LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, tr->query->str);
	sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);

	/* Check for affected rows. One row should be affected . */
	num = SQ_get_affected_rows(tr->sql_connection);
	if (num < 1) {
		tr->error|=ERROR_U_DBS;
		tr->succeeded=0;
		if (sql_err) {
			sq_error=SQ_error(tr->sql_connection);
			LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
			die;
		} else {
			LG_log(ud_context, LG_ERROR, "0 rows affected [%s]\n", tr->query->str);
			/* This is to check that this really could happen */
			die;
		}
		return (tr->succeeded);
	}

	if (IS_STANDALONE(tr->mode)) {
		/* for pn, mt, rs, as update dummy field */

		if ((tr->class_type==C_PN) || (tr->class_type==C_RO)) {
			/* this table has object_type field which is set to 100 for dummies - need to update too */
			g_string_sprintf(tr->query, "UPDATE %s SET thread_id=0, object_type=%d, dummy=0 WHERE object_id=%ld ",
			        DF_get_class_sql_table(tr->class_type), tr->class_type, tr->object_id);
		} else if ((tr->class_type==C_AS) || (tr->class_type==C_RS) || (tr->class_type==C_MT) || (tr->class_type==C_IT)) {
			g_string_sprintf(tr->query, "UPDATE %s SET thread_id=0, dummy=0 WHERE object_id=%ld ",
			        DF_get_class_sql_table(tr->class_type), tr->object_id);
		}
		sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);
		if (sql_err) {
			sq_error=SQ_error(tr->sql_connection);
			LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
			die;
		}
	} else {
		/* Insert new version into the last */

		/* update last for commit/rollback */

		g_string_sprintf(tr->query, "INSERT last "
			"SET thread_id=%d, object_id=%ld, sequence_id=%ld, timestamp=%ld, object_type=%d, object='%s', pkey='%s' ",
		        tr->thread_ins, tr->object_id, tr->sequence_id+1, timestamp, tr->class_type, tr->object_txt, tr->K->str);

		LG_log(ud_context, LG_DEBUG, "%s [%s]", UD_TAG, tr->query->str);
		sql_err = SQ_execute_query(tr->sql_connection, tr->query->str, (SQ_result_set_t **)NULL);

		/* Check for affected rows. One row should be affected */
		num = SQ_get_affected_rows(tr->sql_connection);
		if (num < 1) {
			tr->error|=ERROR_U_DBS;
			tr->succeeded=0;
			if (sql_err) {
				g_string_sprintfa(tr->error_script, "E[%d][:]:UPDATE last failed:%s\n", ERROR_U_DBS,
				        SQ_error(tr->sql_connection));
				LG_log(ud_context, LG_ERROR, "%s[%s]\n", SQ_error(tr->sql_connection), tr->query->str);
				die;
			} else {
				LG_log(ud_context, LG_ERROR, "0 rows affected [%s]\n", tr->query->str);
				/* This is to check that this is really could happen */
				die;
			}
		}
	}

	return (tr->succeeded);
} /* perform_update() */

/************************************************************
 * int object_process(Transaction_t *tr)                     *
 *                                                           *
 * This is the interface between core and upper layer        *
 * All it gets is Transaction *tr, which contains all        *
 * necessary information, including the object in its        *
 * internal representation.                                  *
 *                                                           *
 * Returns: tr->succeeded: >0 success, 0 - error             *
 * Error code is stored in tr->error.                        *
 *                                                           *
 *************************************************************/
int object_process(Transaction_t *tr) {
	int res;
	char *nic;
	int commit_now;

	/* for loader we do not perform commits/rollbacks */
	if (IS_STANDALONE(tr->mode))
		commit_now = 1;
	else
		commit_now = 0;

	/* create and initialize TR record for crash recovery */
	TR_create_record(tr);

	if (ACT_DELETE(tr->action)) {
		LG_log(tr->src_ctx, LG_DEBUG, "%s object: delete", UD_TAG);
		/* check referential integrity of deletion */
		UD_check_ref(tr);
		/* if everything is Ok we are ready to commit */
		if (tr->succeeded) {
			/* update object_id and sequence_id fields */
			tr->sequence_id = get_sequence_id(tr);
			if (tr->sequence_id==-1) {
				tr->error|=ERROR_U_DBS;
				tr->succeeded=0;
				LG_log(ud_context, LG_SEVERE, "cannot get sequence_id");
				die;
			}

			TR_update_id(tr);

			/* checkpoint the TR  - we are going to commit*/
			CP_COMMIT(tr->action);
			TR_update_escript(tr);
			TR_update_status(tr);

			/* delete the object and checkpoint it*/
			UD_delete(tr);
			UD_update_rx(tr, RX_OPER_DEL);

			/* we need to update sequence_id because it was changed during update */
			CP_DELETE_PASSED(tr->action);
			TR_update_id(tr);
			TR_update_status(tr);
		}
		/* send an ack */
		UD_ack(tr);
		return (tr->succeeded); /*commit is not needed*/
	} else if (ACT_UPDATE(tr->action)) {
		LG_log(tr->src_ctx, LG_DEBUG, "%s object: update\n", UD_TAG);
		perform_update(tr);

	} else if (ACT_CREATE(tr->action)) {
		LG_log(tr->src_ctx, LG_DEBUG, "%s object: create", UD_TAG);
		perform_create(tr);

		/* Commit nic-handle allocation (if any) to the repository */
		if (ACT_UPD_NHR(tr->action) && tr->succeeded && ((tr->class_type==C_PN) || (tr->class_type==C_RO)
		        || (tr->class_type==C_OA)) && tr->nh) {
			/* convert nh to DB nIC handle before registration */
			if (tr->class_type==C_OA) {
				nic = NH_convert_org(tr->nh);
			} else {
				nic = NH_convert(tr->nh);
			}

			if (nic==NULL) {
				res=0;
				nic="N/A";
			} else {
				if (tr->class_type==C_OA) {
					res = NH_register_org(tr->nh, tr->sql_connection, commit_now);
				} else {
					res = NH_register(tr->nh, tr->sql_connection, commit_now);
				}
			}

			if (res == -1) {
				tr->succeeded=0;
				tr->error |= ERROR_U_DBS;
				LG_log(ud_context, LG_SEVERE, "cannot allocate nic handle");
				die;
			} else if (res == 0) {
				tr->succeeded=0;
				tr->error |= ERROR_U_OBJ;
				LG_log(tr->src_ctx, LG_INFO, "[%ld] nic handle [%s] wrong or already in use", tr->transaction_id, nic);
				if (tr->class_type==C_OA) {
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:nic-handle wrong or already in use\n",
					        ERROR_U_OBJ, A_OA, nic);
				} else {
					g_string_sprintfa(tr->error_script, "E[%d][%d:%s]:nic-handle wrong or already in use\n",
					        ERROR_U_OBJ, A_NH, nic);
				}
			} else { /* copy the NH to the report to return to DBupdate */
				if (tr->class_type==C_OA) {
					g_string_sprintfa(tr->error_script, "I[%d][%s]\n", A_OA, nic);
				} else {
					g_string_sprintfa(tr->error_script, "I[%d][%s]\n", A_NH, nic);
				}
				UT_free(nic);
			}
		} /* if person/role */
		else if (tr->succeeded && (tr->class_type==C_KC) && tr->x509_commit) {
			/* is a key-cert, we have to update last assigned keycert_id in x509 table */
			res = NH_register_keycert_id(tr->keycert_id, tr->sql_connection, commit_now);
			if (res == -1) {
				tr->succeeded=0;
				tr->error |= ERROR_U_DBS;
				LG_log(ud_context, LG_SEVERE, "cannot allocate key");
				die;
			} else if (res == 0) { /* this will never happen ?!?!?! */
				tr->succeeded=0;
				tr->error |= ERROR_U_OBJ;
				LG_log(tr->src_ctx, LG_INFO, "[%ld] key [X509-%ld] wrong or already in use", tr->transaction_id,
				        tr->keycert_id);
				g_string_sprintfa(tr->error_script, "E[%d][%d:X509-%ld]:key wrong or already in use\n", ERROR_U_OBJ,
				        A_NH, tr->keycert_id);
			} else {
				/* copy the NH to the report to return to DBupdate */
				g_string_sprintfa(tr->error_script, "I[%d][X509-%ld]\n", A_NH, tr->keycert_id);
			}
		}
	} else {
		LG_log(tr->src_ctx, LG_INFO, "[%ld] object: unknown action", tr->transaction_id);
		tr->succeeded=0;
		tr->error|=ERROR_U_BADOP;
		return (tr->succeeded);
	}

	if (!IS_STANDALONE(tr->mode)) { /* not for loader*/
		/* update object_id and sequence_id fields */
		TR_update_id(tr);

		if (tr->succeeded) {
			/* checkpoint the TR  - we are going to commit*/
			CP_COMMIT(tr->action);
			TR_update_escript(tr);
			TR_update_status(tr);

			/* commit the transaction and checkpoint it */
			UD_commit(tr);
			/* Commit nic-handle modifications to the repository */
			NH_commit(tr->sql_connection);

			/* Commit keycert id modifications to the repository */
			NH_commit_keycert_id(tr->sql_connection);

			CP_COMMIT_NH_PASSED(tr->action);
			TR_update_status(tr);
			/* TR will be marked as clean in UD_create_serial() */
		} else {
			UD_rollback(tr);

			CP_ROLLBACK_PASSED(tr->action);
			TR_update_status(tr);

			/* rollback nic-handle modifications to the repository */
			NH_rollback(tr->sql_connection);

			CP_ROLLBACK_NH_PASSED(tr->action);
			TR_update_status(tr);
			/* Delete TR record if in update mode. Next time (if any) DBupdate tries to submit, we'll start from scratch */
			/* In NRTM mode we create serial anyway, so the record will be deleted  */
			/* after serial is created TR record will be deleted in */
		}
	}
	/* send an ack */
	UD_ack(tr);
	return (tr->succeeded);
} /* object_process() */

