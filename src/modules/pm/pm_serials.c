/***************************************
 $Revision: 1.1 $

 Near real-time mirror server module (pm).  NRTM  protocol.

 Status: NOT REVUED, NOT TESTED

 +html+ <DL COMPACT>
 +html+ <DT>Online References:
 +html+ <DD><UL>
 +html+ </UL>
 +html+ </DL>
 +html+ <PRE>
 Author:
 andrei
 +html+ </PRE>
 
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

#include <stdio.h>

/* defined in protocol_serials.c */
extern LG_context_t *pm_context;

/************************************************************
 * Gets the min and max serial number.
 * 
 * Returns:
 *  min & max serial number
 *  in case of an error, it dies (only sql errors possible)
 * 
 * Note: 
 *  min serial= MIN(serial_id)+1
 *  MIN(serial_id) represents legacy RIPE.CURRENSERIAL of the snapshot
 *************************************************************/
void PM_get_minmax_serial(SQ_connection_t *sql_connection, long *min, long *max) {
	char query[STR_M];
	SQ_result_set_t *sql_result;
	SQ_row_t *sql_row;
	int sql_err;

	/* get the lock to ensure that queries are not stopped */
	/* which means access to the database is allowed */
	PW_record_query_start();

	sprintf(query, "SELECT min(serial_id),max(serial_id) FROM serials");
	sql_err = SQ_execute_query(sql_connection, query, &sql_result);

	if (sql_err) {
		LG_log(pm_context, LG_SEVERE, "%s[%s]", SQ_error(sql_connection), query);
		die;
	}

	if ((sql_row = SQ_row_next(sql_result)) != NULL) {
		if (SQ_get_column_int(sql_result, sql_row, 0, min) || SQ_get_column_int(sql_result, sql_row, 1, max)) {
			LG_log(pm_context, LG_SEVERE, "Error during SQ_get_column_int [%s]", query);
			die;
		}
		*min++;		/* this is the old behavior - it seems sort of stupid, but it doesn't matter after all */
	}

	if (sql_result) {
		SQ_free_result(sql_result);
		sql_result=NULL;
	}

	/* release the lock */
	PW_record_query_end();
}

/************************************************************
 * Returns text block corresponding to the requested serial
 *
 * Returns:
 *  operation (ADD/DEL) and text object
 *  NULL in case of an error
 * 
 * Fills *object_type, *operation, *timestamp if not NULL
 *
 * Note:
 *  returned string should be freed by the caller
 *************************************************************/
char *PM_get_serial_object(SQ_connection_t *sql_connection, long serial_number, long *object_type, unsigned *timestamp,
        int *operation) {
	
	SQ_result_set_t * sql_result;
	SQ_row_t *sql_row;
	char *sql_str;
	char query[STR_L];
	int sql_err;
	int location;
	long locop[2]; // array to hold location and operation

	/* get the lock to ensure that queries are not stopped */
	/* which means access to the database is allowed */
	PW_record_query_start();

	sprintf(query, "SELECT atlast,operation FROM serials WHERE serial_id = %d", serial_number);
	get_fields_int_noalloc(sql_connection, query, locop);
	location = locop[0];
	if (operation)
		*operation = locop[1];
	/* reset timestamp */
	if (timestamp) *timestamp = 0;

	switch (location) {
		/* history */
		case 0:
			/* if the operation is delete, and we need to get the timestamps, a little bit different strategy is
			 * needed because of the bad database design. In this case, there are two entries with the same
			 * sequence_id and object_id in the serials table, both with atlast=0. The referred entry in the
			 * history table is the object as it was created/updated, and the timestamp there reflects the 
			 * creation/update time, not the deletion time.
			 * Deletion time can be digged from the last table, looking up object_id with sequence_id = 0.
			 * Following some twisted logic, this is how deletions are done in the RIPE DB.
			 * FIXME: Weird, bad design. Should be fixed.
			 * agoston, 2008-01-22
			 * 
			 * FIXME: real-world unearthed even more shit. NRTM updates database in a different way: when
			 * recreating a previously deleted pkey, it simply keeps the object_id, and updates (deletes)
			 * the sequence_id=0 record in the last table to the following sequence_id, puts the object
			 * in there, etc..., so it basically overwrites the deletion placeholder record in the last table,
			 * keeping the old object_id.
			 * It's funny as this proves that NRTM is NOT a proper replication - luckily this bug keeps the
			 * serials the same, but the sql DB itself is vastly different.
			 * As a workaround, I've changed the inner joins to left join. Of course this will result in
			 * timestamp being NULL in NRTM delete+re-create cases, but that data is lost anyway (as the 
			 * placeholder last record is overwritten).
			 * 
			 * agoston, 2008-01-30 */
			if ((locop[1] == OP_DEL) && timestamp) {
				sprintf(query, "SELECT history.object, history.object_type, last.timestamp FROM serials "
					"LEFT JOIN last ON serials.object_id = last.object_id AND last.sequence_id = 0 "
					"LEFT JOIN history ON serials.object_id = history.object_id AND serials.sequence_id = history.sequence_id "
					"WHERE serials.serial_id=%ld", serial_number);

			} else {

				sprintf(query, "SELECT history.object, history.object_type, history.timestamp FROM history, serials "
					"WHERE serials.serial_id=%ld "
					"AND serials.object_id=history.object_id "
					"AND serials.sequence_id=history.sequence_id", serial_number);
				
			}
			break;
			
		/* last */
		case 1:
			sprintf(query, "SELECT last.object, last.object_type, last.timestamp FROM last, serials "
				"WHERE serials.serial_id=%ld "
				"AND serials.object_id=last.object_id "
				"AND serials.sequence_id=last.sequence_id", serial_number);
			break;
			
		/* failed_transaction */	
		case 2:
			sprintf(query, "SELECT object FROM failed_transaction WHERE serial_id=%ld ", serial_number);
			/* return some meaningful value - in this case, UINT_MAX */
			if (timestamp) *timestamp = -1;
			break;

		default:
			return (NULL);
	}

	sql_err = SQ_execute_query(sql_connection, query, &sql_result);

	if (sql_err) {
		LG_log(pm_context, LG_SEVERE, "%s[%s]", SQ_error(sql_connection), query);
		die;
	}

	if ((sql_row = SQ_row_next(sql_result)) != NULL) {
		sql_str = SQ_get_column_string(sql_result, sql_row, 0);
		if (location < 2) {
			if (object_type && SQ_get_column_int(sql_result, sql_row, 1, object_type)) {
				LG_log(pm_context, LG_SEVERE, "Error during SQ_get_column_int [%s]", query);
				die;
			}
			/* (*timestamp) is left at zero if timestamp is NULL */ 
			if (timestamp && (SQ_get_column_unsigned(sql_result, sql_row, 2, timestamp) < -1)) {
				LG_log(pm_context, LG_SEVERE, "Error during SQ_get_column_int [%s]", query);
				die;
			}
		}
	} else
		sql_str=NULL;

	if (sql_result) {
		SQ_free_result(sql_result);
		sql_result=NULL;
	}

	/* release the lock */
	PW_record_query_end();

	return sql_str;
}

/************************************************************
 * Fills supplied buffer with information about the source   *
 *                                                           *
 * Returns text block corresponding to the requested source  *
 * Format:                                                   *
 * <source>:<can_mirror>:min_serial-max_serial               *
 * source - name of the source (e.g. RIPE, RADB, etc.)       *
 * can_mirror                                                * 
 *    'Y' if the client is allowed to mirror the source      *
 *    'N' if not                                             *
 *    'N' if there is no serials (then the range starts at 0)*    
 *************************************************************/
void pm_get_source_info(GString *gbuff, ip_addr_t *client_address, char *source, ca_dbSource_t *source_hdl) {

	char *db_host = ca_get_srcdbmachine(source_hdl);
	int db_port = ca_get_srcdbport(source_hdl);
	char *db_name = ca_get_srcdbname(source_hdl);
	char *db_user = ca_get_srcdbuser(source_hdl);
	char *db_passwd = ca_get_srcdbpassword(source_hdl);
	int version = ca_get_srcnrtmprotocolvers(source_hdl);
	SQ_connection_t *db_connection;
	long min_serial, max_serial;
	char can_mirror;

	/* Connect to the database */
	db_connection=SQ_get_connection(db_host, db_port, db_name, db_user, db_passwd);
	PM_get_minmax_serial(db_connection, &min_serial, &max_serial);
	/* We don't need this anymore - just don't start dynamic mode if the server crashes on a serial 
	 * agoston, 2007-12-21 */
	/* max_serial -= SAFE_BACKLOG; */

	/* If it cannot be mirrored at all - N, but range starts with 0 */
	/* If the client is allowed to mirror - Y         */
	/* Otherwise - N                                  */
	if (min_serial>max_serial) {
		can_mirror='N';
		min_serial=0;
	} else {
		switch (AA_can_mirror(client_address, source)) {
			default:
			case AA_MIRROR_DENY:
				can_mirror='N';
				break;
			case AA_MIRROR_PUBLIC:
				can_mirror='X';
				break;
			case AA_MIRROR_FULL:
				can_mirror='Y';
				break;
		}
	}
	g_string_sprintfa(gbuff, "%s:%d:%c:%lu-%lu\n", source, version, can_mirror, min_serial, max_serial);

	UT_free(db_host);
	UT_free(db_name);
	UT_free(db_user);
	UT_free(db_passwd);
	SQ_close_connection(db_connection);
}

/************************************************************
 * Fills supplied buffer with information about the sources  *
 *                                                           *
 *                                                           * 
 * Note:                                                     *
 *  returned GString should be freed by the caller           *
 *************************************************************/
GString *PM_get_nrtm_sources(ip_addr_t *client_address, char *source) {
	GString *gbuff=g_string_sized_new(STR_L);
	int nsource;
	ca_dbSource_t *source_hdl;

	if (source) {
		source_hdl = ca_get_SourceHandleByName(source);
		if (source_hdl)
			pm_get_source_info(gbuff, client_address, source, source_hdl);
	} else
		for (nsource=0; (source_hdl = ca_get_SourceHandleByPosition(nsource))!=NULL; nsource++) {
			source=ca_get_srcname(source_hdl);
			pm_get_source_info(gbuff, client_address, source, source_hdl);
			UT_free(source);
		}
	/* one extra line, another one will be put bt PW or PM */
	g_string_sprintfa(gbuff, "\n");
	return (gbuff);
}
