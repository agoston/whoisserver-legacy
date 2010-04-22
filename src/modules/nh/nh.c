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

#include "rip.h"

#include <glib.h>
#include <stdio.h>
#include <strings.h>
#include <glib.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

/*+ String sizes +*/
#define STR_S   63
#define STR_M   255
#define STR_L   1023
#define STR_XL  4095
#define STR_XXL 16383
#define STR_XXXL 65535

/*
 CREATE TABLE nic_hdl (
 thread_id(11) DEFAULT '0' NOT NULL,
 range_id int(10) unsigned DEFAULT '0' NOT NULL auto_increment, 
 range_start int(10) DEFAULT '0' NOT NULL,
 range_end int(10) DEFAULT '0' NOT NULL,
 space char(4) DEFAULT '' NOT NULL,
 source char(10) DEFAULT '' NOT NULL,
 PRIMARY KEY (range_id, range_start, range_end)
 );

 */

#define get_min_range(prange, sql_connection) get_range(MIN_NIC_ID, prange, sql_connection)
static long get_range(long nic_id, range_t *prange, SQ_connection_t *sql_connection);
static long nh_update_keycert_id(long old_keycert_id, long keycert_id, SQ_connection_t *sql_connection, int commit_now);
static long get_range_org(long nic_id, range_t *prange, SQ_connection_t *sql_connection);
static long get_range_org_by_space(range_t *prange, SQ_connection_t *sql_connection);
static long update_range(long range_id, range_t *p_newrange, SQ_connection_t *sql_connection, int commit_now);
static long update_range_org(long range_id, range_t *p_newrange, SQ_connection_t *sql_connection, int commit_now);
static long create_range(range_t *p_range, SQ_connection_t *sql_connection, int commit_now);
static long create_range_org(range_t *p_range, SQ_connection_t *sql_connection, int commit_now);

/************************************************************
 * int NH_convert()                                          *
 *                                                           *
 * Converts space & nic_id into a database nic-handle        *
 *                                                           *
 * Returns:                                                  *
 * The newly allocated string containing nic handle          *
 * The string should be freed when no longer used            *
 *                                                           *
 * NULL in case of failure                                   *
 *                                                           *
 ************************************************************/
char *NH_convert(nic_handle_t *nh_ptr) {
	char *nic_id=NULL;
	char *nic_components[4];
	char *nic_handle;
	int nc=0;
	/* Check for special cases */
	/* Is is and AUTO nic-handle ? */
	if (nh_ptr->nic_id == AUTO_NIC_ID)
		return (NULL);
	if (nh_ptr->space) {
		nic_components[nc]=nh_ptr->space;
		nc++;
	}
	/* No nic-id ? */
	if (nh_ptr->nic_id != NULL_NIC_ID) {
		nic_id = g_strdup_printf("%ld", nh_ptr->nic_id);
		nic_components[nc]=nic_id;
		nc++;
	}

	/* No source ? */
	if (nh_ptr->source) {
		nic_components[nc]=nh_ptr->source;
		nc++;
	}
	nic_components[nc]=NULL;
	nic_handle = g_strjoinv(NULL, nic_components);
	UT_free(nic_id);
	return (nic_handle);
}

/************************************************************
 * int NH_convert_org()                                      *
 *                                                           *
 * Converts space & nic_id (org_id) of an org object         *
 * into a database nic-handle                                *
 *                                                           *
 * Returns:                                                  *
 * The newly allocated string containing nic handle          *
 * The string should be freed when no longer used            *
 *                                                           *
 * NULL in case of failure                                   *
 *                                                           *
 ************************************************************/

char *NH_convert_org(nic_handle_t *nh_ptr) {
	char *nic_id=NULL;
	char *nic_components[8];
	char *nic_handle;
	int nc=0;
	/* Check for special cases */
	/* Is it and AUTO nic-handle ? */
	if (nh_ptr->nic_id == AUTO_NIC_ID) {
		return (NULL);
	}

	/* Does it have a 'space'? (mandatory for org_ids) */
	if (nh_ptr->space == NULL) {
		return (NULL);
	}

	/* No nic-id (number)? It is mandatory for org_ids, so return NULL */
	if (nh_ptr->nic_id == NULL_NIC_ID) {
		return (NULL);
	}

	/* No space? It is mandatory for org_ids, so return NULL */
	if (nh_ptr->space == NULL) {
		return (NULL);
	}

	nic_components[nc]=strdup("ORG-");
	nc++;
	nic_components[nc]=nh_ptr->space;
	nc++;

	nic_id = g_strdup_printf("%ld", nh_ptr->nic_id);
	nic_components[nc]=nic_id;
	nc++;

	nic_components[nc]=nh_ptr->source;
	nc++;

	nic_components[nc]=NULL;
	nic_handle = g_strjoinv(NULL, nic_components);
	UT_free(nic_id);
	return (nic_handle);
}

/************************************************************
 * int NH_parse()                                            *
 *                                                           *
 * Parse a nic handle as supplied by DBupdate                *
 * The format is: <space>[<nic_id>|*][SOURCE]                *
 * Also extracts nic_id and space for regular nic-handles    *
 *                                                           *
 * Acceptable format is:                                     *
 * [A-Z][A-Z]*[1-9][0-9]*(-[A-Z][A-Z]*)?                     *
 *                                                           *
 * Returns:                                                  *
 * >0 - success                                              *
 *  0 - AUTO NIC                                             *
 * -1  - error                                               *
 *                                                           *
 ************************************************************/
int NH_parse(const char *nic, nic_handle_t **nh_ptr_ptr) {
	const char *ptr;
	int res = 1;
	nic_handle_t *nh_ptr;

	*nh_ptr_ptr=NULL;
	if (!(nh_ptr=UT_calloc(1, sizeof(nic_handle_t))))
		die;

	ptr=nic;

	/* extract space */
	while (isalpha((int)*ptr))
		ptr++;

	/* sanity check - space <=4 characters */
	if ((ptr-nic)>MAX_NH_SPACE_LENGTH) {
		UT_free(nh_ptr);
		return (-1);
	}

	if (!(nh_ptr->space=UT_malloc(ptr-nic+1)))
		die;
	strncpy(nh_ptr->space, nic, ptr-nic);
	*(nh_ptr->space+(ptr-nic))='\0';

	/* If there are no digits, then this is no nic-hdl */
	/* We reserve NULL_NIC_ID for such pretty identifiers */
	if (*ptr == '\0') {
		nh_ptr->nic_id=NULL_NIC_ID;
		nh_ptr->source=NULL;
	} else {
		/* Check if it is and AUTO nic */
		if (*ptr == '*') {
			/* For AUTO nic_id we reserve AUTO_NIC_ID */
			nh_ptr->nic_id=AUTO_NIC_ID;
			res=0;
			ptr++;
		} else {
			nic=ptr;
			/* convert digits (if any) and store first invalid characted in ptr */
			if (isdigit((int)*nic)) {
				nh_ptr->nic_id=(int)strtol(nic, (char **)&ptr, 10);
			} else {
				/* There were no digits at all */
				nh_ptr->nic_id=NULL_NIC_ID;
			}
		}
		/* sanity check - if the ID does not exeed NH_MAX_ID */
		if (nh_ptr->nic_id > MAX_NIC_ID) {
			if (nh_ptr) {
				UT_free(nh_ptr->space);
				UT_free(nh_ptr);
			}
			return (-1);
		}
		/* check if there is any suffix */
		if (*ptr == '\0')
			nh_ptr->source=NULL;
		/* Copy suffix into source */
		else {
			/* sanity check - suffix does not exceed the length */
			if (strlen(ptr)>MAX_NH_SOURCE_LENGTH) {
				if (nh_ptr) {
					UT_free(nh_ptr->space);
					UT_free(nh_ptr);
				}
				return (-1);
			}
			if (!(nh_ptr->source=UT_malloc(strlen(ptr)+1)))
				die;
			strcpy(nh_ptr->source, ptr);
		}
	}
	*nh_ptr_ptr=nh_ptr;
	return (res);
}

/************************************************************
 * int NH_parse_org()                                        *
 *                                                           *
 * Parse an organisation object NIC handle (orgID)           *
 * as supplied by DBupdate                                   *
 * The format is: ORG-<space>(<nic_id>|*)SOURCE              *
 * Also extracts nic_id and space for regular nic-handles    *
 *                                                           *
 * Acceptable format is:                                     *
 * ORG-[A-Z][A-Z]*[1-9][0-9]*(-[A-Z][A-Z]*)                  *
 *                                                           *
 * Returns:                                                  *
 * >0 - success                                              *
 *  0 - AUTO NIC                                             *
 * -1  - error                                               *
 *                                                           *
 ************************************************************/
int NH_parse_org(const char *nic, nic_handle_t **nh_ptr_ptr) {
	const char *ptr;
	int res = 1;
	nic_handle_t *nh_ptr;

	*nh_ptr_ptr=NULL;
	if (!(nh_ptr=UT_calloc(1, sizeof(nic_handle_t))))
		die;

	ptr=nic;

	/* check if the first 4 chars are 'ORG-' */
	if (strncmp(ptr, "ORG-", 4) != 0) {
		UT_free(nh_ptr);
		return (-1);
	}
	ptr = ptr + 4;

	/* extract space */
	while (isalpha((int)*ptr))
		ptr++;

	/* sanity check - space <=4 characters */
	if ((ptr-nic)>(MAX_NH_SPACE_LENGTH + 4)) {
		UT_free(nh_ptr);
		return (-1);
	}

	if (!(nh_ptr->space=UT_malloc(ptr-nic+1)))
		die;
	strncpy(nh_ptr->space, nic+4, ptr-nic-4);
	*(nh_ptr->space+(ptr-nic-4))='\0';

	/* If there are no digits, then this is not a valid orgID */
	if (*ptr == '\0') {
		UT_free(nh_ptr->space);
		UT_free(nh_ptr);
		return (-1);
	} else {
		/* Check if it is an AUTO orgID */
		if (*ptr == '*') {
			/* For AUTO nic_id we reserve AUTO_NIC_ID */
			nh_ptr->nic_id=AUTO_NIC_ID;
			res=0;
			ptr++;
		} else {
			nic=ptr;
			/* convert digits (if any) and store first invalid character in ptr */
			if (isdigit((int)*nic)) {
				nh_ptr->nic_id=(int)strtol(nic, (char **)&ptr, 10);
			} else {
				/* There were no digits at all */
				UT_free(nh_ptr->space);
				UT_free(nh_ptr);
				return (-1);
			}
		}
		/* sanity check - if the ID does not exceed NH_MAX_ID */
		if (nh_ptr->nic_id > MAX_NIC_ID) {
			if (nh_ptr) {
				UT_free(nh_ptr->space);
				UT_free(nh_ptr);
			}
			return (-1);
		}
		/* check if there is any suffix */
		/* which is mandatory           */
		if (*ptr == '\0') {
			if (nh_ptr) {
				UT_free(nh_ptr->space);
				UT_free(nh_ptr);
			}
			return (-1);
		} else { /* Copy suffix into source */
			/* sanity check - suffix does not exceed the length */
			if (strlen(ptr)>MAX_NH_SOURCE_LENGTH) {
				if (nh_ptr) {
					UT_free(nh_ptr->space);
					UT_free(nh_ptr);
				}
				return (-1);
			}
			if (!(nh_ptr->source=UT_malloc(strlen(ptr)+1)))
				die;
			strcpy(nh_ptr->source, ptr);
		}
	}
	*nh_ptr_ptr=nh_ptr;
	return (res);
}

/************************************************************
 * int NH_check()                                            *
 *                                                           *
 * Check a NIC handle in the repository                      *
 *                                                           *
 *                                                           *
 * Returns:                                                  *
 *  1 - success                                              *
 *  0 - error(nic_id exists or space is fully occupied)      *
 * -1 - error (eg. more than one object with the same        *
 *                 primary key)                              *
 *                                                           *
 ************************************************************/
int NH_check(nic_handle_t *nh_ptr, SQ_connection_t *sql_connection) {
	range_t range;
	long range_id;
	long nic_id=nh_ptr->nic_id;

	range.space=nh_ptr->space;
	if (nh_ptr->source)
		range.source=nh_ptr->source;
	else
		range.source="";

	if (nic_id == AUTO_NIC_ID) {
		/* NIC handle is an AUTO one */
		/* get first range (with min range_end) for a given space */
		range_id = get_min_range(&range, sql_connection);
		if (range_id<0)
			return (-1); /* in case of an error */

		if (range_id==0) {
			/* Nothing found */
			/* Allocate a hic-hdl in a new space with the first range {0-1} in it*/
			nic_id=1;
		} else {
			if (range.end == MAX_NIC_ID)
				return (0); /* space is fully occupied  */
			/* attach to range and may be join with next */
			nic_id = range.end+1;
		}
	}
	/* if not AUTO */
	else {
		range_id = get_range(nic_id, &range, sql_connection);
		if (range_id <0)
			return (-1); /* in case of an error */
		if (range_id!=0)
			return (0); /* this nic_id already exists */
	}
	nh_ptr->nic_id=nic_id;
	return (1);
}

/************************************************************
 * int NH_check_org()                                        *
 *                                                           *
 * Check an organisation object NIC handle in the repository *
 *                                                           *
 *                                                           *
 * Returns:                                                  *
 *  1 - success                                              *
 *  0 - error(nic_id exists or space is fully occupied)      *
 * -1 - error (eg. more than one object with the same        *
 *               primary key)                                *
 *                                                           *
 ************************************************************/
int NH_check_org(nic_handle_t *nh_ptr, SQ_connection_t *sql_connection) {
	range_t range;
	long range_id;
	long nic_id=nh_ptr->nic_id;

	/* XXX DEBUG */
	/*printf("DEBUG: NH_check_org: nh_ptr->space is [%s]\n", nh_ptr->space ? nh_ptr->space : "NULL");
	 printf("DEBUG: NH_check_org: nh_ptr->source is [%s]\n", nh_ptr->source ? nh_ptr->source : "NULL");
	 printf("DEBUG: NH_check_org: nh_ptr->nic_id is [%li]\n", nh_ptr->nic_id);*/

	range.space=nh_ptr->space;
	if (nh_ptr->source) {
		range.source=nh_ptr->source;
	} else {
		range.source="";
	}

	if (nic_id == AUTO_NIC_ID) {
		/* auto NIC handles are handled by NH_assign_org_id */
		return (-1); /* return error */
	}
	/* if not AUTO */
	else {
		range_id = get_range_org(nic_id, &range, sql_connection);
		if (range_id <0)
			return (-1); /* in case of an error */
		if (range_id!=0)
			return (0); /* this nic_id already exists */
	}
	nh_ptr->nic_id=nic_id;
	return (1);
}

/************************************************************
 * int NH_assign_org_id()                                    *
 *                                                           *
 * Assigns a NIC handle to an organisation object from the   *
 * repository                                                *
 *                                                           *
 *                                                           *
 * Returns:                                                  *
 *  1 - success                                              *
 *  0 - error(nic_id exists or space is fully occupied)      *
 * -1 - error (eg. more than one object with the same        *
 *               primary key, or it wasn't an AUTO orgID)    *
 *                                                           *
 ************************************************************/
int NH_assign_org_id(nic_handle_t *nh_ptr, SQ_connection_t *sql_connection) {
	range_t range = {-1, -1, NULL, NULL};
	long range_id;
	long nic_id=nh_ptr->nic_id;

	/* XXX DEBUG */
	/*printf("DEBUG: NH_assign_org_id: nh_ptr->space is [%s]\n", nh_ptr->space ? nh_ptr->space : "NULL");
	 printf("DEBUG: NH_assign_org_id: nh_ptr->source is [%s]\n", nh_ptr->source ? nh_ptr->source : "NULL");
	 printf("DEBUG: NH_assign_org_id: nh_ptr->nic_id is [%li]\n", nh_ptr->nic_id);*/

	range.space=nh_ptr->space;
	if (nh_ptr->source) {
		range.source=nh_ptr->source;
	} else {
		range.source="";
	}

	if (nic_id == AUTO_NIC_ID) {
		/* NIC handle is an AUTO one */
		/* get the range for a given space */
		range_id = get_range_org_by_space(&range, sql_connection);
		if (range_id<0)
			return (-1); /* in case of an error */

		if (range_id==0) {
			/* Nothing found */
			/* Allocate a hic-hdl in a new space with the first range {0-1} in it*/
			nic_id=1;
		} else {
			if (range.end == MAX_NIC_ID) {
				return (0); /* space is fully occupied  */
			}
			/* attach to range */
			nic_id = range.end+1;
		}
	}
	/* if not AUTO */
	else {
		/* This function must only be used to assign NIC handles */
		return (-1); /* return error */
	}
	nh_ptr->nic_id=nic_id;
	return (1);
}

/************************************************************
 * int NH_register()                                         *
 *                                                           *
 * Get a NIC handle from the repository                      *
 *                                                           *
 *                                                           *
 * Returns:                                                  *
 * 1 - success                                               *
 * 0  - nic_id already exists or space is fully occupied     *
 * -1 - error (f.e. more than one object with the same PK)   *
 *                                                           *
 ************************************************************/
int NH_register(nic_handle_t *nh_ptr, SQ_connection_t *sql_connection, int commit_now) {
	range_t range;
	long range_id;
	long nic_id=nh_ptr->nic_id;

	/* Yiu should check for nh first for AUTO nic-handles */
	if (nic_id == AUTO_NIC_ID) {
		return (0);
	};

	range.space=nh_ptr->space;
	if (nh_ptr->source)
		range.source=nh_ptr->source;
	else
		range.source="";

	range_id = get_range(nic_id, &range, sql_connection);
	if (range_id <0) {
//		fprintf(stderr, "NIC_ID %ld failure point #1", nic_id);
		return (-1);
	} /* in case of an error */
	if (range_id!=0) {
		return (0);
	} /* this nic_id already exists */

	/* check if we can attach to existing next range */
	range_id = get_range(nic_id+1, &range, sql_connection);
	if (range_id <0) {
//		fprintf(stderr, "NIC_ID %ld failure point #2", nic_id);
		return (-1);
	} /* in case of an error */

	if (range_id>0) {
		/* attach to range and may be join with previous */
		range.start-=1;
		range_id=update_range(range_id, &range, sql_connection, commit_now);
		if (range_id<=0) {
//			fprintf(stderr, "NIC_ID %ld failure point #3", nic_id);
			return (-1);
		}
	} else {
		/* check if we can attach to existing previous range */
		if (nic_id>0)
			range_id = get_range(nic_id-1, &range, sql_connection);
		else
			range_id=0; /* there is no previous range in this case (nic_id==0) */
		if (range_id <0) {
//			fprintf(stderr, "NIC_ID %ld failure point #4", nic_id);
			return (-1);
		} /* in case of an error */
		if (range_id>0) {
			/* attach to range and may be join with next */
			range.end+=1;
			range_id=update_range(range_id, &range, sql_connection, commit_now);
			if (range_id<=0) {
//				fprintf(stderr, "NIC_ID %ld failure point #5", nic_id);
				return (-1);
			}
		} else {
			/* If we cannot attach to any existing range - create new {nic_id-nic_id} */
			range.end=range.start=nic_id;
			range_id=create_range(&range, sql_connection, commit_now);
			if (range_id <=0) {
//				fprintf(stderr, "NIC_ID %ld failure point #6", nic_id);
				return (-1);
			} /* in case of an error */
		}
	}
	return (1);
}

/************************************************************
 * int NH_register_org()                                     *
 *                                                           *
 * Get an orgID (org object NIC handle) from the repository  *
 *                                                           *
 *                                                           *
 * Returns:                                                  *
 * 1 - success                                               *
 * 0  - nic_id already exists or space is fully occupied     *
 * -1 - error (eg. more than one object with the same primary*
 *             key)                                          *
 *                                                           *
 ************************************************************/
int NH_register_org(nic_handle_t *nh_ptr, SQ_connection_t *sql_connection, int commit_now) {
	range_t range;
	long range_id;
	long nic_id=nh_ptr->nic_id;

	/*XXX*/
	/*  printf("DEBUG: NH_register_org entered, nic_id=[%li]\n", nic_id);*/

	/* You should check for nh first for AUTO nic-handles */
	if (nic_id == AUTO_NIC_ID) {
		return (0);
	};

	range.space=nh_ptr->space;
	if (nh_ptr->source) {
		range.source=nh_ptr->source;
	} else {
		range.source="";
	}

	range_id = get_range_org(nic_id, &range, sql_connection);
	/*XXX*/
	/*  printf("DEBUG: NH_register_org get_range_org returned [%li]\n", range_id);*/

	if (range_id <0) {
		return (-1);
	} /* in case of an error */
	if (range_id!=0) {
		return (0);
	} /* this nic_id already exists */

	/* check if we can attach to previous existing range */
	if (nic_id>0) {
		range_id = get_range_org(nic_id-1, &range, sql_connection);
	} else {
		range_id=0; /* there is no previous range in this case (nic_id==0) */
	}
	if (range_id<0) {
		return (-1);
	} /* in case of an error */
	if (range_id>0) {
		/* attach to range */
		/*    printf("DEBUG: NH_register_org range.end is [%li]\n", range.end);*/
		range.end+=1;
		/*    printf("DEBUG: NH_register_org range.end is [%li]\n", range.end);*/
		range_id=update_range_org(range_id, &range, sql_connection, commit_now);
		/*XXX*/
		/*    printf("DEBUG: NH_register_org update_range_org returned [%li], range.end was [%li]\n", range_id, range.end);*/
		if (range_id<=0) {
			return (-1);
		}
	} else {
		/* create new range */
		range.end=nic_id;
		range_id=create_range_org(&range, sql_connection, commit_now);
		/*XXX*/
		/*    printf("DEBUG: NH_register_org create_range_org returned [%li]\n", range_id);*/

		if (range_id <=0) {
			return (-1);
		} /* in case of an error */
	}

	/*XXX*/
	/*  printf("DEBUG: NH_register_org will return success\n");*/
	return (1);
}

/*
 Free nic_handle_t structure 
 */
void free_nh(nic_handle_t *nh_ptr) {
	if (nh_ptr) {
		if (nh_ptr->space)
			UT_free(nh_ptr->space);
		if (nh_ptr->source)
			UT_free(nh_ptr->source);
		UT_free(nh_ptr);
	}
}

/***************************************************************
 * int NH_register_keycert_id()                                 *
 *                                                              *
 * register the new keycert_id in the x509 table                *
 * we need to do it only if it is bigger than the last assigned *
 * we never update; create new, then                            *
 * if commit -> delete old                                      *
 * if rollback -> delete new                                    *
 * to commit/rollback, use NH_keycert_id_commit                 * 
 *                                                              *
 *                                                              *
 * Returns:                                                     *
 * 1 - success                                                  *
 * -1 - cannot allocate key                                     *
 *                                                              *
 ***************************************************************/
int NH_register_keycert_id(long keycert_id, SQ_connection_t *sql_connection, int commit_now) {
	long last_keycert_id;

	last_keycert_id = nh_get_keycert_id(sql_connection);
	if (last_keycert_id < 0) {
		return -1;
	}
	if (keycert_id <= last_keycert_id) { /* no need to update */
		return 1;
	}
	/* update keycert_id */
	return nh_update_keycert_id(last_keycert_id, keycert_id, sql_connection, commit_now);
}

/************************************************************
 * long nh_get_keycert_id()                                     *
 *                                                           *
 * Searches for the last assigned keycert id for X509 object *
 *                                                           *
 * Returns:                                                  *
 * >=0 - id available, returns last assigned or 0 if none    *
 * <0 - DB error                                             *
 *                                                           *
 * **********************************************************/
long nh_get_keycert_id(SQ_connection_t *sql_connection) {
	return SQ_get_max_id(sql_connection, "keycert_id", "x509");
}

/************************************************************
 * long nh_update_keycert_id()                                  *
 *                                                           *
 * Updates the keycert_id in x509 table to last assigned one *
 *                                                           *
 * Returns:                                                  *
 * 1 - success                                               *
 * -1 - cannot allocate key                                  *
 *                                                           *
 *                                                           *
 * **********************************************************/

long nh_update_keycert_id(long old_keycert_id, long keycert_id, SQ_connection_t *sql_connection, int commit_now) {
	SQ_result_set_t *sql_result;
	GString *query;
	int sql_err;
	int num;

	query = g_string_sized_new(STR_L);
	if (commit_now) {
		/* if starting id, insert */
		if (old_keycert_id == 0) {
			g_string_sprintf(query, "INSERT INTO x509(thread_id, keycert_id) VALUES (0, %ld)", keycert_id);
		} else {
			g_string_sprintf(query, "UPDATE x509 SET keycert_id=%ld WHERE thread_id=0", keycert_id);
		}
	} else {
		g_string_sprintf(query, "INSERT INTO x509(thread_id, keycert_id) VALUES (%d, %ld)", NH_INSERT, keycert_id);
	}

	/* execute query */
	sql_err=SQ_execute_query(sql_connection, query->str, &sql_result);

	if (sql_err) {
		/* An error occured */
		g_string_free(query, TRUE);
		return (-1);
	}
	num = SQ_get_affected_rows(sql_connection);
	/* this should not happen */
	if (num==0)
		die;

	/* mark old one for deletion */
	if (!commit_now) {
		g_string_sprintf(query, "UPDATE x509 SET thread_id=%d WHERE thread_id=0;", NH_DELETE);
		/* execute query */
		sql_err=SQ_execute_query(sql_connection, query->str, &sql_result);

		if (sql_err) {
			/* An error occured */
			g_string_free(query, TRUE);
			return (-1);
		}
		num = SQ_get_affected_rows(sql_connection);
		if (old_keycert_id != 0) {
			/* this should not happen */
			if (num==0)
				die;
		}
	}

	return (1);

}

/************************************************************
 * int NH_comrol_keycert_id()                       *
 * commit keycert_id changes to x509 table                   *
 *                                                           *
 * Returns:                                                  *
 * >0 - success                                              *
 * -1 - SQL error                                            *
 *                                                           *
 * **********************************************************/
int NH_comrol_keycert_id(SQ_connection_t *sql_connection, int thread_ins, int thread_del) {
	GString *query;
	int sql_err;

	query = g_string_sized_new(STR_L);

	g_string_sprintf(query, "DELETE FROM x509 "
		"WHERE thread_id=%d ", thread_del);

	sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
	if (sql_err) {
		/* An error occured */
		g_string_free(query, TRUE);
		fprintf(stderr,"ERROR: %s\n", SQ_error(sql_connection));
		die;
	}

	g_string_sprintf(query, "UPDATE x509 "
		"SET thread_id=0 "
		"WHERE thread_id=%d ", thread_ins);

	sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
	g_string_free(query, TRUE);

	if (sql_err) {
		/* An error occured */
		fprintf(stderr,"ERROR: %s\n", SQ_error(sql_connection));
		die;
	}

	return (1);

}

/************************************************************
 * long get_range()                                          *
 *                                                           *
 * Searches for the range of the space containing            *
 * the specified nic_id                                      *
 *                                                           *
 * To request to search for the first (min) range, nic_id    *
 * should be set to MIN_NIC_ID                               *
 *                                                           *
 * Returns:                                                  *
 * >0 - range exists, returns range_id                       *
 * 0  - range does not exist                                 *
 * -1 - DB error (f.e. more than one object with the same PK)*
 *                                                           *
 * **********************************************************/
static long get_range(long nic_id, range_t *prange, SQ_connection_t *sql_connection) {
	SQ_result_set_t *sql_result;
	SQ_row_t *sql_row;
	char *sql_str;
	GString *query;
	long range_id=0;
	int sql_err;

	/*XXX*/
	/*printf("DEBUG: get_range: entered with nic_id=[%li]\n", nic_id);*/

	query = g_string_sized_new(STR_L);

	/* Define row numbers in the result of the query */
#define RANGE_ID 0
#define RANGE_START 1
#define RANGE_END 2

	if (nic_id==MIN_NIC_ID) {
		/* requesting the first (min) range */
		g_string_sprintf(query, "SELECT range_id, range_start, range_end "
			"FROM nic_hdl "
			"WHERE space='%s' "
			"AND source='%s' "
			"AND (range_start=0 "
			"OR  range_start=1) ", prange->space, prange->source);
	} else {

		g_string_sprintf(query, "SELECT range_id, range_start, range_end "
			"FROM nic_hdl "
			"WHERE space='%s' "
			"AND source='%s' "
			"AND range_start<=%ld "
			"AND range_end>=%ld ", prange->space, prange->source, nic_id, nic_id);
	}

	/* execute query */
	/* fprintf(stderr, "get_range[%s]\n", query->str); */
	sql_err=SQ_execute_query(sql_connection, query->str, &sql_result);
	g_string_free(query, TRUE);

	if (sql_err) {
		fprintf(stderr,"ERROR: %s\n", SQ_error(sql_connection));
		return (-1);
	}

	if ((sql_row = SQ_row_next(sql_result)) != NULL) {
		/* Object exists */
		sql_str = SQ_get_column_string(sql_result, sql_row, RANGE_ID);
		if (sql_str != NULL) {
			range_id = atol(sql_str);
			UT_free(sql_str);
		}
		sql_str = SQ_get_column_string(sql_result, sql_row, RANGE_START);
		if (sql_str != NULL) {
			prange->start = atoi(sql_str);
			UT_free(sql_str);
		}
		sql_str = SQ_get_column_string(sql_result, sql_row, RANGE_END);
		if (sql_str != NULL) {
			prange->end = atoi(sql_str);
			UT_free(sql_str);
		}

		/* We must process all the rows of the result */
		/* otherwise we'll have them as part of the next qry */
		while ( (sql_row = SQ_row_next(sql_result)) != NULL)
			range_id=-1;
	} else
		range_id=0; // object does not exist

	if (sql_result)
		SQ_free_result(sql_result);
	/*XXX*/
	/*printf("DEBUG: get_range: returning with range_id=[%li]\n", range_id);*/
	return (range_id);
}

/************************************************************
 * long get_range_org()                                      *
 *                                                           *
 * Searches for the range of the space containing            *
 * the specified source                                      *
 *                                                           *
 * Returns:                                                  *
 * >0 - range exists, returns range_id                       *
 * 0  - range does not exist                                 *
 * -1 - DB error (eg. more than one object with the same     *
 *                primary key)                               *
 *                                                           *
 * **********************************************************/
static long get_range_org(long nic_id, range_t *prange, SQ_connection_t *sql_connection) {
	SQ_result_set_t *sql_result;
	SQ_row_t *sql_row;
	char *sql_str;
	GString *query;
	long range_id=0;
	int sql_err;

	query = g_string_sized_new(STR_L);

	/* Define row numbers in the result of the query */
#define ORG_RANGE_ID 0
#define ORG_RANGE_END 1

	/*XXX*/
	/*printf("DEBUG: get_range_org: entered with nic_id=[%li]\n", nic_id);*/

	g_string_sprintf(query, "SELECT range_id, range_end "
		"FROM organisation_id "
		"WHERE space='%s' "
		"AND source='%s' "
		"AND range_end>=%ld ", prange->space, prange->source, nic_id);

	/* execute query */
	/* fprintf(stderr, "get_range[%s]\n", query->str); */
	sql_err=SQ_execute_query(sql_connection, query->str, &sql_result);
	g_string_free(query, TRUE);

	if (sql_err) {
		fprintf(stderr,"ERROR: %s\n", SQ_error(sql_connection));
		return (-1);
	}

	if ((sql_row = SQ_row_next(sql_result)) != NULL) {
		/* Object exists */
		sql_str = SQ_get_column_string(sql_result, sql_row, ORG_RANGE_ID);
		if (sql_str != NULL) {
			range_id = atol(sql_str);
			UT_free(sql_str);
		}
		sql_str = SQ_get_column_string(sql_result, sql_row, ORG_RANGE_END);
		if (sql_str != NULL) {
			prange->end = atoi(sql_str);
			UT_free(sql_str);
		}

		/* We must process all the rows of the result */
		/* otherwise we'll have them as part of the next qry */
		while ( (sql_row = SQ_row_next(sql_result)) != NULL)
			range_id=-1;
	} else {
		range_id=0; /* object does not exist */
	}

	if (sql_result)
		SQ_free_result(sql_result);
	/*XXX*/
	/*printf("DEBUG: get_range_org: returning with range_id=[%li]\n", range_id);*/
	return (range_id);
}

/************************************************************
 * long get_range_org_by_space()                             *
 *                                                           *
 * Searches for the range of the space containing            *
 * the specified source. Could be used assinging a new orgID *
 *                                                           *
 * Returns:                                                  *
 * >0 - range exists, returns range_id. Also changes         *
 *                prange->end                                *
 * 0  - range does not exist                                 *
 * -1 - DB error (eg. more than one object with the same     *
 *                primary key)                               *
 *                                                           *
 * **********************************************************/
static long get_range_org_by_space(range_t *prange, SQ_connection_t *sql_connection) {
	SQ_result_set_t *sql_result;
	SQ_row_t *sql_row;
	char *sql_str;
	GString *query;
	long range_id=0;
	int sql_err;

	query = g_string_sized_new(STR_L);

	/* Define row numbers in the result of the query */
#define ORG_RANGE_ID 0
#define ORG_RANGE_END 1

	/*XXX*/
	/*printf("DEBUG: get_range_org_by_space: entered\n");*/

	g_string_sprintf(query, "SELECT range_id, range_end "
		"FROM organisation_id "
		"WHERE space='%s' "
		"AND source='%s' ", prange->space, prange->source);

	/* execute query */
	/* fprintf(stderr, "get_range[%s]\n", query->str); */
	sql_err=SQ_execute_query(sql_connection, query->str, &sql_result);
	g_string_free(query, TRUE);

	if (sql_err) {
		fprintf(stderr,"ERROR: %s\n", SQ_error(sql_connection));
		return (-1);
	}

	if ((sql_row = SQ_row_next(sql_result)) != NULL) {
		/* Object exists */
		sql_str = SQ_get_column_string(sql_result, sql_row, ORG_RANGE_ID);
		if (sql_str != NULL) {
			range_id = atol(sql_str);
			UT_free(sql_str);
		}
		sql_str = SQ_get_column_string(sql_result, sql_row, ORG_RANGE_END);
		if (sql_str != NULL) {
			prange->end = atoi(sql_str);
			UT_free(sql_str);
		}

		/* We must process all the rows of the result */
		/* otherwise we'll have them as part of the next qry */
		while ( (sql_row = SQ_row_next(sql_result)) != NULL)
			range_id=-1;
	} else {
		range_id=0; /* object does not exist */
	}

	if (sql_result)
		SQ_free_result(sql_result);
	/*XXX*/
	/*printf("DEBUG: get_range_org: returning with range_id=[%li]\n", range_id);*/
	return (range_id);
}

/************************************************************
 * long update_range()                                       *
 *                                                           *
 * Updates the range by changing the boundaries              *
 * Deletes the range if nothing left                         *
 * Merges with neighbor ranges if there is no gap between    *
 *                                                           *
 * We never update range. We create a new one with specified * 
 * limits and mark old one(s) for deletion, so that we can   *
 * make commit/rollback properly. This is possible as the    * 
 * primary keys are (range_id, range_start, range_end)       *
 *                                                           *
 * To finalize changes make commit/rollback                  *
 *                                                           *
 * Returns:                                                  *
 * >0 - returns range_id on success                          *
 * -1 - error (f.e. more than one object with the same PK)   *
 *                                                           *
 ************************************************************/
static long update_range(long range_id, range_t *p_newrange, SQ_connection_t *sql_connection, int commit_now) {
	GString *query;
	range_t range;
	long prev_range_id, next_range_id;
	int num;
	int sql_err;

	query = g_string_sized_new(STR_L);

	/* Do range check */
	if ((p_newrange->end > MAX_RANGE ) || (p_newrange->start < MIN_RANGE ))
		return (-1);

	/* Check if the range collapses */
	if (p_newrange->end < p_newrange->start) {
		/* then delete the range */
		/* Do this by marking the range for deletion for further commit/rollback */
		if (commit_now)
			g_string_sprintf(query, "DELETE FROM nic_hdl "
				"WHERE range_id=%ld ", range_id);
		else
			g_string_sprintf(query, "UPDATE nic_hdl SET thread_id=%d "
				"WHERE range_id=%ld ", NH_DELETE, range_id);

		/*   fprintf(stderr, "update_range[%s]\n", query->str); */
		sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
		if (sql_err) {
			/* An error occured */
			g_string_free(query, TRUE);
			return (-1);
		}
		num = SQ_get_affected_rows(sql_connection);
		/* this should not happen */
		if (num==0)
			die;

	} else {
		/* update the range for the same space/source */
		range.space=p_newrange->space;
		range.source=p_newrange->source;
		/* Check if we can join with previous range of the same space */
		prev_range_id=get_range(p_newrange->start-1, &range, sql_connection);
		/* Check if such range exists and it is not ours (this happens when we are shrinking */
		if ((prev_range_id>0) && (prev_range_id!=range_id)) {
			/* acquire the previous range */
			/* mark it for deletion for commit/rollback */
			if (commit_now)
				g_string_sprintf(query, "DELETE FROM nic_hdl "
					"WHERE range_id=%ld ", prev_range_id);
			else
				g_string_sprintf(query, "UPDATE nic_hdl SET thread_id=%d "
					"WHERE range_id=%ld ", NH_DELETE, prev_range_id);

			/*    fprintf(stderr, "update_range[%s]\n", query->str); */
			sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
			if (sql_err) {
				/* An error occured */
				g_string_free(query, TRUE);
				return (-1);
			}
			num = SQ_get_affected_rows(sql_connection);
			/* this should not happen */
			if (num==0)
				die;

			/* expand the boundaries */
			p_newrange->start=range.start;
		}

		/* Check if we can join with next range of the same space */
		next_range_id=get_range(p_newrange->end+1, &range, sql_connection);
		/* Check if such range exists and it is not ours (this happens when we are shrinking) */
		if ((next_range_id>0) && (next_range_id!=range_id)) {
			/* acquire the next range */
			/* mark it for deletion for commit/rollback */
			if (commit_now)
				g_string_sprintf(query, "DELETE FROM nic_hdl "
					"WHERE range_id=%ld ", next_range_id);
			else
				g_string_sprintf(query, "UPDATE nic_hdl SET thread_id=%d "
					"WHERE range_id=%ld ", NH_DELETE, next_range_id);

			/*   fprintf(stderr, "update_range[%s]\n", query->str); */
			sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
			if (sql_err) {
				/* An error occured */
				g_string_free(query, TRUE);
				return (-1);
			}
			num = SQ_get_affected_rows(sql_connection);
			/* this should not happen */
			if (num==0)
				die;

			/* expand the boundaries */
			p_newrange->end=range.end;
		}

		/* Now make a larger range. Mark current for deletion and new for commit/rollback */
		if (commit_now)
			g_string_sprintf(query, "UPDATE nic_hdl "
				"SET range_start=%ld, range_end=%ld "
				"WHERE range_id=%ld", p_newrange->start, p_newrange->end, range_id);
		else {

			g_string_sprintf(query, "UPDATE nic_hdl SET thread_id=%d "
				"WHERE range_id=%ld ", NH_DELETE, range_id);
			/*   fprintf(stderr, "update_range[%s]\n", query->str); */
			sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
			if (sql_err) {
				/* An error occured */
				g_string_free(query, TRUE);
				return (-1);
			}
			num = SQ_get_affected_rows(sql_connection);
			/* this should not happen */
			if (num==0)
				die;

			g_string_sprintf(query, "INSERT nic_hdl "
				"SET thread_id=%d, range_id=%ld, space='%s', source='%s', range_start=%ld, range_end=%ld ", NH_INSERT,
			        range_id, p_newrange->space, p_newrange->source, p_newrange->start, p_newrange->end);
		}

		/*   fprintf(stderr, "update_range[%s]\n", query->str); */
		sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
		if (sql_err) {
			/* An error occured */
			g_string_free(query, TRUE);
			return (-1);
		}
		num = SQ_get_affected_rows(sql_connection);
		/* this should not happen */
		if (num==0)
			die;
	} /* update the range */

	g_string_free(query, TRUE);
	return (range_id);
}

/************************************************************
 * long update_range_org()                                   *
 *                                                           *
 * Updates the range by changing the boundaries              *
 * Never deletes the range as we don't allow reuse of orgIDs *
 * Never shrinks a range as we don't allow reuse of orgIDs   *
 *                                                           *
 *                                                           *
 * We never update range. We create a new one with specified * 
 * limits and mark old one(s) for deletion, so that we can   *
 * make commit/rollback properly. This is possible as the    * 
 * primary keys are (range_id, range_end)                    *
 *                                                           *
 * To finalize changes make commit/rollback                  *
 *                                                           *
 * Returns:                                                  *
 * >0 - returns range_id on success                          *
 * -1 - error (f.e. more than one object with the same PK)   *
 *                                                           *
 ************************************************************/

static long update_range_org(long range_id, range_t *p_newrange, SQ_connection_t *sql_connection, int commit_now) {
	GString *query;
	range_t range;
	long prev_range_id;
	int num;
	int sql_err;

	query = g_string_sized_new(STR_L);

	/* Do range check */
	if (p_newrange->end > MAX_RANGE)
		return (-1);

	/* update the range for the same space/source */
	range.space=p_newrange->space;
	range.source=p_newrange->source;
	/* Check if we can join with previous range of the same space */
	prev_range_id=get_range_org(p_newrange->start-1, &range, sql_connection);
	/* Check if such range exists */
	if ((prev_range_id>0) && (prev_range_id!=range_id)) {
		/* acquire the previous range */
		/* mark it for deletion for commit/rollback */
		if (commit_now)
			g_string_sprintf(query, "DELETE FROM organisation_id "
				"WHERE range_id=%ld ", prev_range_id);
		else
			g_string_sprintf(query, "UPDATE organisation_id SET thread_id=%d "
				"WHERE range_id=%ld ", NH_DELETE, prev_range_id);

		/*    fprintf(stderr, "update_range[%s]\n", query->str); */
		sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
		if (sql_err) {
			/* An error occured */
			g_string_free(query, TRUE);
			return (-1);
		}
		num = SQ_get_affected_rows(sql_connection);
		/* this should not happen */
		if (num==0)
			die;

		/* expand the boundaries */
		p_newrange->start=range.start;
	}

	/* Now make a larger range. Mark current for deletion and new for commit/rollback */
	if (commit_now) {
		g_string_sprintf(query, "UPDATE organisation_id "
			"SET range_end=%ld "
			"WHERE range_id=%ld", p_newrange->end, range_id);
	} else {

		g_string_sprintf(query, "UPDATE organisation_id SET thread_id=%d "
			"WHERE range_id=%ld ", NH_DELETE, range_id);
		/*   fprintf(stderr, "update_range[%s]\n", query->str); */
		sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
		if (sql_err) {
			/* An error occured */
			g_string_free(query, TRUE);
			return (-1);
		}
		num = SQ_get_affected_rows(sql_connection);
		/* this should not happen */
		if (num==0)
			die;

		g_string_sprintf(query, "INSERT organisation_id "
			"SET thread_id=%d, range_id=%ld, space='%s', source='%s', range_end=%ld ", NH_INSERT, range_id,
		        p_newrange->space, p_newrange->source, p_newrange->end);
	}

	/*   fprintf(stderr, "update_range[%s]\n", query->str); */
	sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
	if (sql_err) {
		/* An error occured */
		g_string_free(query, TRUE);
		return (-1);
	}
	num = SQ_get_affected_rows(sql_connection);
	/* this should not happen */
	if (num==0)
		die;

	g_string_free(query, TRUE);
	return (range_id);
}

/************************************************************
 * long create_range()                                       *
 *                                                           *
 * Creates a new range in a given name space                 *
 *                                                           *
 * To finalize changes make commit/rollback                  *
 *                                                           *
 * Returns:                                                  *
 * >0 - returns range_id on success                          *
 * -1 - error (f.e. more than one object with the same PK)   *
 *                                                           *
 ************************************************************/

static long create_range(range_t *p_range, SQ_connection_t *sql_connection, int commit_now) {
	GString *query;
	int sql_err, num;
	long range_id;

	query = g_string_sized_new(STR_L);

	/* get the next range_id */
	/* XXX we cannot use autoincrement with MyISAM tables */
	/* XXX because they keep the max inserted id even if  */
	/* XXX it was deleted later, thus causing gaps we don't want */

	range_id=SQ_get_max_id(sql_connection, "range_id", "nic_hdl") +1;

	if (commit_now)
		g_string_sprintf(query, "INSERT nic_hdl "
			"SET thread_id=0,  range_id=%ld, space='%s', source='%s', range_start=%ld, range_end=%ld ", range_id,
		        p_range->space, p_range->source, p_range->start, p_range->end);
	else
		g_string_sprintf(query, "INSERT nic_hdl "
			"SET thread_id=%d, range_id=%ld, space='%s', source='%s', range_start=%ld, range_end=%ld ", NH_INSERT,
		        range_id, p_range->space, p_range->source, p_range->start, p_range->end);

	/* fprintf(stderr, "create_range[%s]\n", query->str); */
	sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
	g_string_free(query, TRUE);

	if (sql_err) {
		/* An error occured */
		return (-1);
	}
	num = SQ_get_affected_rows(sql_connection);
	/* this should not happen */
	if (num==0)
		die;
	return (range_id);
}

/************************************************************
 * long create_range_org()                                   *
 *                                                           *
 * Creates a new range in a given name space                 *
 *                                                           *
 * To finalize changes make commit/rollback                  *
 *                                                           *
 * Returns:                                                  *
 * >0 - returns range_id on success                          *
 * -1 - error (eg. more than one object with the same        *
 *             primary key)                                  *
 *                                                           *
 ************************************************************/

static long create_range_org(range_t *p_range, SQ_connection_t *sql_connection, int commit_now) {
	GString *query;
	int sql_err, num;
	long range_id;

	query = g_string_sized_new(STR_L);

	/* get the next range_id */
	/* XXX we cannot use autoincrement with MyISAM tables */
	/* XXX because they keep the max inserted id even if  */
	/* XXX it was deleted later, thus causing gaps we don't want */

	range_id=SQ_get_max_id(sql_connection, "range_id", "organisation_id") + 1;

	if (commit_now) {
		g_string_sprintf(query, "INSERT organisation_id "
			"SET thread_id=0,  range_id=%ld, space='%s', source='%s', range_end=%ld ", range_id, p_range->space,
		        p_range->source, p_range->end);
	} else {
		g_string_sprintf(query, "INSERT organisation_id "
			"SET thread_id=%d, range_id=%ld, space='%s', source='%s', range_end=%ld ", NH_INSERT, range_id,
		        p_range->space, p_range->source, p_range->end);
	}

	/* fprintf(stderr, "create_range[%s]\n", query->str); */
	sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
	g_string_free(query, TRUE);

	if (sql_err) {
		/* An error occured */
		return (-1);
	}
	num = SQ_get_affected_rows(sql_connection);
	/* this should not happen */
	if (num==0)
		die;
	return (range_id);
}

/************************************************************
 * int NH_comrol()                                           *
 *                                                           *
 * Commits or rolls back changes to NHR                      *
 *                                                           *
 *                                                           *
 * Returns:                                                  *
 * >0 - success                                              *
 * -1 - SQL error                                            *
 *                                                           *
 ************************************************************/

int NH_comrol(SQ_connection_t *sql_connection, int thread_ins, int thread_del) {
	GString *query;
	int sql_err;

	query = g_string_sized_new(STR_L);

	/* For person/role objects */
	g_string_sprintf(query, "DELETE FROM nic_hdl "
		"WHERE thread_id=%d ", thread_del);

	sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
	if (sql_err) {
		/* An error occured */
		g_string_free(query, TRUE);
		fprintf(stderr,"ERROR: %s\n", SQ_error(sql_connection));
		die;
	}

	/* For organisation objects */
	g_string_sprintf(query, "DELETE FROM organisation_id "
		"WHERE thread_id=%d ", thread_del);

	sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);
	if (sql_err) {
		/* An error occured */
		g_string_free(query, TRUE);
		fprintf(stderr,"ERROR: %s\n", SQ_error(sql_connection));
		die;
	}

	/* For person/role objects */
	g_string_sprintf(query, "UPDATE nic_hdl "
		"SET thread_id=0 "
		"WHERE thread_id=%d ", thread_ins);

	sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);

	if (sql_err) {
		/* An error occured */
		fprintf(stderr,"ERROR: %s\n", SQ_error(sql_connection));
		die;
	}

	/* For organisation objects */
	g_string_sprintf(query, "UPDATE organisation_id "
		"SET thread_id=0 "
		"WHERE thread_id=%d ", thread_ins);

	sql_err=SQ_execute_query(sql_connection, query->str, (SQ_result_set_t **)NULL);

	if (sql_err) {
		/* An error occured */
		fprintf(stderr,"ERROR: %s\n", SQ_error(sql_connection));
		die;
	}

	g_string_free(query, TRUE);

	return (1);

}
