/***************************************
 access authorisation (aa). aa.c - functions to check access rights
 for less frequent clients (ripupdate, mirror).
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

typedef struct {
	ip_prefix_t pref;
	char source[32];
	int ripupdate;
	int mirror;
} aa_rights;

/** The AAA cache itself - if it would get big, we should introduce an sql-tree instead */
GList *aaa[MAX_IPSPACE_ID+1];

pthread_rwlock_t aaa_lock;

void aa_compose_query(ip_space_t space, char *buf, unsigned len) {
	switch (space) {
	case IP_V4:
		snprintf(buf, len, "SELECT ripupdate, mirror, source, prefix_length, "
			"prefix FROM aaa "
			"ORDER BY prefix_length ASC");
		break;

	case IP_V6:
		snprintf(buf, len, "SELECT ripupdate, mirror, source, prefix_length, "
			"prefix1, prefix2, prefix3, prefix4 FROM aaa6 "
			"ORDER BY prefix_length ASC");
		break;

	default:
		die;
	}
}

/* Loads the aaa table into an in-memory buffer for fast access
 * Puts write lock on aaa_lock during its operation */
void AA_load() {
	int i;
	char buf[1024];
	SQ_result_set_t *result;
	SQ_connection_t *con = NULL;
	SQ_row_t *row;

	pthread_rwlock_wrlock(&aaa_lock);

	for (i = MIN_IPSPACE_ID; i <=MAX_IPSPACE_ID; i++) {
		/* clear the list */
		wr_clear_list(&aaa[i]);

		/* get the query */
		aa_compose_query(i, buf, sizeof(buf));

		/* open the database */
		if ((con = AC_dbopen_admin()) == NULL) {
			fprintf(stderr, "ERROR %d: %s\n", SQ_errno(con), SQ_error(con));
			die;
		}

		/* select the most specific entry */
		if (SQ_execute_query(con, buf, &result) == -1) {
			fprintf(stderr, "ERROR %d: %s\n", SQ_errno(con), SQ_error(con));
			die;
		}

		/* read in the rights from the resulting rows */
		while ((row = SQ_row_next(result)) != NULL) {
			aa_rights *rights = calloc(1, sizeof(aa_rights));

			dieif(sscanf(SQ_get_column_string_nocopy(result, row, 0), "%u", &rights->ripupdate) < 1);
			dieif(sscanf(SQ_get_column_string_nocopy(result, row, 1), "%u", &rights->mirror) < 1);
			dieif(sscanf(SQ_get_column_string_nocopy(result, row, 2), "%s", rights->source) < 1);

			/* now comes the prefix */
			switch (i) {
			case IP_V4:
				dieif(IP_pref_f2b_v4(&rights->pref, SQ_get_column_string_nocopy(result, row, 4),
				        SQ_get_column_string_nocopy(result, row, 3)) != IP_OK);
				break;

			case IP_V6:
				dieif(IP_pref_f2b_v6_32(&rights->pref, SQ_get_column_string_nocopy(result, row, 4),
				        SQ_get_column_string_nocopy(result, row, 5), SQ_get_column_string_nocopy(result, row, 6),
				        SQ_get_column_string_nocopy(result, row, 7), SQ_get_column_string_nocopy(result, row, 3))
				        != IP_OK);

				dieif(rights->pref.ip.space != IP_V6); /* v4 or v4-mapped address in acl6 table!!! */
				break;
			}

			aaa[i] = g_list_prepend(aaa[i], rights);
		}

		/* release everything */
		SQ_free_result(result);

		/* Close connection */
		SQ_close_connection(con);
	}

	pthread_rwlock_unlock(&aaa_lock);
}

void AA_init() {
	pthread_rwlockattr_t aaa_lock_attr;
	int i;

	for (i = MIN_IPSPACE_ID; i <=MAX_IPSPACE_ID; i++) {
		aaa[i] = NULL;
	}

	/* set the 'prefer writer' flag to the rwlock */
	pthread_rwlockattr_init(&aaa_lock_attr);
	pthread_rwlockattr_setkind_np(&aaa_lock_attr, PTHREAD_RWLOCK_PREFER_WRITER_NP);
	pthread_rwlock_init(&aaa_lock, &aaa_lock_attr);

	AA_load();
}

/* finds and fills in the struct */
aa_rights *aa_find(ip_addr_t * address, char *source) {
	GList *pitem;
	aa_rights *ret = NULL;

	pthread_rwlock_rdlock(&aaa_lock);

	/* items in aaa are sorted by prefix_length, in descending order, so just walking the list
	 * gives us the most specific matching prefix, which is what we want */
	for (pitem = g_list_first(aaa[address->space]); pitem != NULL; pitem = g_list_next(pitem)) {
		aa_rights *rights = (aa_rights *) pitem->data;
		if (!strcmp(source, rights->source) && IP_addr_in_pref(address, &rights->pref)) {
			ret = rights;
			break;
		}
	}

	pthread_rwlock_unlock(&aaa_lock);
	return ret;
}

/* returns the ripupdate field of the most specific aaa record for the given ip address and source
 * 0 = can't do updates through update port
 * 1 = can do updates through update port */
int AA_can_ripupdate(ip_addr_t *address, char *source) {
	aa_rights *myrights = aa_find(address, source);
	if (myrights)
		return myrights->ripupdate;
	return 0;
}

/* returns the mirror field of the most specific aaa record for the given ip address and source */
aa_mirror_right AA_can_mirror(ip_addr_t *address, char *source) {
	aa_rights *myrights = aa_find(address, source);
	if (myrights)
		return myrights->mirror;
	return 0;
}
