/***************************************
 $Revision: 1.1 $

 NIC handle manipulation module.

 Status: NOT REVUED, NOT TESTED

 Author(s):       Andrei Robachevsky

 ******************//******************
 Modification History:
 andrei (10/04/2000) Created.
 ******************//******************
 Copyright (c) 2000,2001,2002,2003               RIPE NCC
 
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
#ifndef _NH_H
#define _NH_H

/************************************************************
 *                                                           *
 * NHR is capable of handling nic-handles __ONLY__           *
 * of the following format:                                  *
 *                                                           *
 * Acceptable format is:                                     *
 * [A-Z][A-Z]*[1-9][0-9]*(-[A-Z][A-Z]*)?                     *
 *                                                           *
 ************************************************************/
#include <mysql_driver.h>

#define MAX_NH_LENGTH  64
#define MAX_NH_SPACE_LENGTH 4
#define MAX_NH_SOURCE_LENGTH 10

#define MAX_NIC_ID  999999
#define NULL_NIC_ID (0)
#define AUTO_NIC_ID (-1)
#define MIN_NIC_ID  (-2)
#define MAX_RANGE   (MAX_NIC_ID+1)
#define MIN_RANGE   NULL_NIC_ID

#define NH_DELETE 1000
#define NH_INSERT 2000 

typedef struct _range_t {
	long start;
	long end;
	char *space;
	char *source;
} range_t;

/* structure to save nic handle data */
typedef struct _nic_handle_t {
	long nic_id;
	char *space;
	char *source;
} nic_handle_t;

int NH_check(nic_handle_t *pnh, SQ_connection_t *sql_connection);
int NH_check_org(nic_handle_t *pnh, SQ_connection_t *sql_connection);
int NH_free(nic_handle_t *pnh, SQ_connection_t *sql_connection, int commit_now);
int NH_register(nic_handle_t *pnh, SQ_connection_t *sql_connection, int commit_now);
int NH_register_keycert_id(long keycert_id, SQ_connection_t *sql_connection, int commit_now);
int NH_register_org(nic_handle_t *pnh, SQ_connection_t *sql_connection, int commit_now);
int NH_assign_org_id(nic_handle_t *nh_ptr, SQ_connection_t *sql_connection);

char *NH_convert(nic_handle_t *pnh);
char *NH_convert_org(nic_handle_t *pnh);
int NH_parse(const char *nic, nic_handle_t **pnh);
int NH_parse_org(const char *nic, nic_handle_t **pnh);

void free_nh(nic_handle_t *nh_ptr);

int NH_comrol(SQ_connection_t *sql_connection, int thread_ins, int thread_del);
#define NH_commit(sql_connection)    NH_comrol(sql_connection, NH_INSERT, NH_DELETE)
#define NH_rollback(sql_connection)  NH_comrol(sql_connection, NH_DELETE, NH_INSERT)

int NH_comrol_keycert_id(SQ_connection_t *sql_connection, int thread_ins, int thread_del);
#define NH_commit_keycert_id(sql_connection) NH_comrol_keycert_id(sql_connection, NH_INSERT, NH_DELETE)
#define NH_rollback_keycert_id(sql_connection) NH_comrol_keycert_id(sql_connection, NH_DELETE, NH_INSERT)

long nh_get_keycert_id(SQ_connection_t *sql_connection);

#endif /* _NH_H */
