/***************************************
 $Revision: 1.16 $

 Functions to interface with RX module (create/update rx-nodes)

 Status: NOT REVUED, NOT TESTED

 Author(s):       Andrei Robachevsky

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

static long volatile UD_max_serial_id = -1;
static pthread_mutex_t serial_id_lock = PTHREAD_MUTEX_INITIALIZER;

void UD_set_global_update_lock(SQ_connection_t *con) {
    if (SQ_execute_query(con, "SELECT global_lock FROM update_lock WHERE global_lock = 0 FOR UPDATE", NULL)) {
        fprintf(stderr, "SQL ERROR %d: %s\n", SQ_errno(con), SQ_error(con));
        die;
    }
}

void UD_rx_refresh_set_serial(SQ_connection_t *con) {
    long min_serial = 0, max_serial = 0;
    PM_get_minmax_serial(con, &min_serial, &max_serial);

#ifdef DEBUG_QUERY
    fprintf(stderr, " *** max_serial set to %ld\n", max_serial);
#endif
    pthread_mutex_lock(&serial_id_lock);
    UD_max_serial_id = max_serial;
    pthread_mutex_unlock(&serial_id_lock);
}

void UD_update_radix_trees(SQ_connection_t *con, const ca_dbSource_t *source_hdl) {
    char query[STR_L];
    SQ_result_set_t *sql_result;
    SQ_row_t *sql_row;
    int sql_err;
    char *pkey;
    long object_type = -1, object_id = -1, operation = -1, serial_id = -1;
    rp_upd_pack_t pack;
    A_Type_t attr_type;
    rx_oper_mt rx_mode;

    pthread_mutex_lock(&serial_id_lock);

    // get new serials
    sprintf(query, "SELECT last.pkey, last.object_type, last.object_id, serials.operation, serials.serial_id FROM serials "
            "LEFT JOIN last ON last.object_id = serials.object_id AND last.sequence_id = serials.sequence_id "
            "WHERE last.object_type IN (3, 5, 6, 12, 19) AND serials.serial_id > %ld", UD_max_serial_id);

    if ((sql_err = SQ_execute_query(con, query, &sql_result))) {
        fprintf(stderr, "%s[%s]", SQ_error(con), query);
        die;
    }

    while ((sql_row = SQ_row_next(sql_result)) != NULL) {
        pkey = SQ_get_column_string_nocopy(sql_result, sql_row, 0);
        if (SQ_get_column_int(sql_result, sql_row, 1, &object_type)) {
            fprintf(stderr, "Error during SQ_get_column_int [%s]", query);
            die;
        }
        if (SQ_get_column_int(sql_result, sql_row, 2, &object_id)) {
            fprintf(stderr, "Error during SQ_get_column_int [%s]", query);
            die;
        }
        if (SQ_get_column_int(sql_result, sql_row, 3, &operation)) {
            fprintf(stderr, "Error during SQ_get_column_int [%s]", query);
            die;
        }
        if (SQ_get_column_int(sql_result, sql_row, 4, &serial_id)) {
            fprintf(stderr, "Error during SQ_get_column_int [%s]", query);
            die;
        }

        // update max serial id
        if (serial_id > UD_max_serial_id) UD_max_serial_id = serial_id;

        // calculate rx_mode; skip if operation is noop
        switch (operation) {
        case OP_DEL: rx_mode = RX_OPER_DEL; break;
        case OP_ADD:
        case OP_UPD: rx_mode = RX_OPER_CRE; break;
        default: continue;
        }

        // calculate attr_type for radix tree update
        switch (object_type) {
        case 3: attr_type = A_DN; break;
        case 5: attr_type = A_I6; break;
        case 6: attr_type = A_IN; break;
        case 12: attr_type = A_RT; break;
        case 19: attr_type = A_R6; break;
        default:
            die;
        }

#ifdef DEBUG_QUERY
        fprintf(stderr, "Op: %ld, type: %ld, pkey: %s\n", operation, object_type, pkey);
#endif

        // add new serials to radix trees
        if (RP_asc2pack( &pack, attr_type, pkey)) die;
        pack.key = object_id;
        if (RP_pack_node(rx_mode, &pack, source_hdl) != RX_OK) die;
    }

    pthread_mutex_unlock(&serial_id_lock);
    SQ_free_result(sql_result);
}

