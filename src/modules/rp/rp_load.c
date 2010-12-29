/***************************************
 $Revision: 1.1 $

 Radix payload (rp) - user level functions for storing data in radix trees

 rp_load = loading the radix trees with data on startup

 Status: NOT REVIEWED, TESTED

 Design and implementation by: Marek Bukowy

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

/* logging */
extern LG_context_t *rp_context;

/* thread info storage */
GList *RP_threads = NULL;

static
int make_sql2pack(SQ_result_set_t *result, SQ_row_t *row, rp_upd_pack_t *pack, rp_attr_t attr, ip_space_t space, int colcount) {
    int conv = RP_OK;
    rp_uni_t *uniptr = &(pack->uni);
    char *idptr; /* initially set to the 0'th column */
    char *col[5];
    unsigned i;

    dieif(colcount>5); /* size of the col array */

    for (i = 0; i < colcount; i++) {
        col[i] = SQ_get_column_string_nocopy(result, row, i);
        if (col[i] == NULL) {
            die;
        }
    }

    idptr = col[0];

    pack->type = attr;
    pack->d.origin = NULL;
    switch (attr) {

    case A_IN:
        /*
         read 0-2 from inetnum
         0 - objectid
         1 - begin
         2 - end
         */
        uniptr->space = IP_V4;
        conv = IP_rang_f2b_v4(&(uniptr->u.in), col[1], col[2]);
        break;

    case A_RT:
        /*
         read 0-3 from route
         0 - objectid
         1 - prefix
         2 - prefix_length
         3 - origin
         */
        uniptr->space = IP_V4;
        if (NOERR(conv = IP_pref_f2b_v4(&(uniptr->u.rt), col[1], col[2]))) {
            pack->d.origin = UT_strdup(col[3]);
        }
        break;

    case A_DN:
        if (space == IP_V4) {
            /*
             read 0-3 from inaddr
             0 - objectid
             1 - prefix
             2 - prefix_length
             3 - domain
             */
            conv = IP_pref_f2b_v4(&(uniptr->u.rt), col[1], col[2]);
            uniptr->space = IP_pref_b2_space(&(uniptr->u.rt));
            pack->d.domain = UT_strdup(col[3]);
        } else {
            /* read 0-4 from ip6int
             0 - objectid
             1 - msb
             2 - lsb
             3 - prefix_length
             4 - domain
             */
            conv = IP_pref_f2b_v6(&(uniptr->u.rt), col[1], col[2], col[3]);
            uniptr->space = IP_pref_b2_space(&(uniptr->u.rt));

            pack->d.domain = UT_strdup(col[4]);
        }
        break;

    case A_I6:
        /*
         read 0-3 from inaddr
         0 - objectid
         1 - msb
         2 - lsb
         3 - prefix_length
         */
        conv = IP_pref_f2b_v6(&(uniptr->u.rt), col[1], col[2], col[3]);
        uniptr->space = IP_pref_b2_space(&(uniptr->u.rt));
        break;

    case A_R6:
        /*
         read 0-4 from route6
         0 - objectid
         1 - r6_msb
         2 - r6_lsb
         3 - prefix_length
         4 - origin
         */
        if (NOERR(conv = IP_pref_f2b_v6(&(uniptr->u.rt), col[1], col[2], col[3]))) {
            pack->d.origin = UT_strdup(col[3]);
        }
        uniptr->space = IP_pref_b2_space(&(uniptr->u.rt));
        break;

    default:
        /*    die; / * shouldn't have got here */
        conv = IP_INVARG;
    }

    if (sscanf(idptr, "%lu", &(pack->key)) < 1) {
        conv = IP_INVARG;
    }

    for (i = 0; i < colcount; i++) {
        /*    wr_free(col[i]);*/;
    }

    return conv;
}

static void *RP_sql_load_attr_space(void *arg) {
    RP_thread_info *inf = (RP_thread_info *)arg;
    rp_attr_t attr = inf->attr;
    ip_space_t space = inf->space;
    rp_regid_t reg_id = inf->reg_id;

    SQ_connection_t *con;
    SQ_row_t *row;
    SQ_result_set_t *result;

    int objnr = 0;
    rx_tree_t *mytree;
    rp_upd_pack_t pack;
    int colcount;
    char *load_query = (space == IP_V4) ? DF_attrcode_radix_load_v4(attr) : DF_attrcode_radix_load_v6(attr);
    char *srcnam = ca_get_srcname(reg_id);
    const char *attr_code;
    char activity[STR_M];

    dieif(load_query == NULL);
    dieif(RP_tree_get(&mytree, reg_id, space, attr) != RP_OK);

    LG_log(rp_context, LG_INFO, "loading using %s", load_query);

    attr_code = DF_get_attribute_code(attr);
    sprintf(activity, "%s/%s, query ", srcnam, attr_code);
    TA_setactivity(activity);

    con = SQ_get_connection_by_source_hdl(reg_id);
    if (SQ_execute_query(con, "LOCK TABLES route READ, inetnum READ, inet6num READ, inaddr_arpa READ, domain READ, ip6int READ, route6 READ", NULL)) {
        fprintf(stderr, "SQL ERROR %d: %s\n", SQ_errno(con), SQ_error(con));
        die;
    }

    if (SQ_execute_query(con, load_query, &result)) {
        fprintf(stderr, "SQL ERROR %d: %s\n", SQ_errno(con), SQ_error(con));
        die;
    } else {
        colcount = SQ_get_column_count(result);

        /* mytree is locked when created, so no need to acquire lock here */
        while ((row = SQ_row_next(result)) != NULL && SQ_errno(con) == 0) {
            dieif(!NOERR(make_sql2pack(result, row, &pack, attr, space, colcount)));

            if (!NOERR(RP_pack_node_l(RX_OPER_CRE, &pack, mytree))) {
                fprintf(stderr, "%d:\t%ld\n", objnr, pack.key);
                die;
            }

            /* free allocated memory */
            if (pack.d.origin != NULL) {
                UT_free(pack.d.origin);
                pack.d.origin = NULL;
            }

            objnr++;

            if (!(objnr & 1023)) {
                sprintf(activity, "%s/%s, %dK done ", srcnam, attr_code, objnr>>10);
                TA_setactivity(activity);
            }
        }
        /* XXX UNLOCK */
        TH_release_write_lockw(&(mytree->rwlock));
    }

    if (SQ_errno(con)) {
        fprintf(stderr, "SQL ERROR %d: %s\n", SQ_errno(con), SQ_error(con));
        die;
    }

    LG_log(rp_context, LG_INFO, "loaded %dK objects into %s/%s", objnr>>10, srcnam, attr_code);

    SQ_free_result(result);
    SQ_close_connection(con);
    UT_free(srcnam);

    TA_delete();

    inf->result = RP_OK;
    return NULL;
}


static void RP_sql_load_thread_add(rp_attr_t attr, ip_space_t space, rp_regid_t reg_id) {
    RP_thread_info *inf = malloc(sizeof(RP_thread_info));
    RP_threads = g_list_append(RP_threads, inf);
    inf->attr = attr;
    inf->space = space;
    inf->reg_id = reg_id;
    inf->result = -1;
}

int RP_sql_load_reg(rp_regid_t reg_id) {
    GList *p;

    /* init thread info */
    RP_sql_load_thread_add(A_RT, IP_V4, reg_id);
    RP_sql_load_thread_add(A_R6, IP_V6, reg_id);
    RP_sql_load_thread_add(A_IN, IP_V4, reg_id);
#ifndef NO_A_I6
    RP_sql_load_thread_add(A_I6, IP_V6, reg_id);
#endif
#ifndef NO_A_DN
    RP_sql_load_thread_add(A_DN, IP_V4, reg_id);
    RP_sql_load_thread_add(A_DN, IP_V6, reg_id);
#endif

    return RP_OK;
}

int RP_sql_load_start() {
    GList *p;

    /* start threads */
    for (p = g_list_first(RP_threads); p; p = g_list_next(p)) {
        RP_thread_info *inf = (RP_thread_info *)p->data;
        inf->thread_id = TH_create(RP_sql_load_attr_space, inf, FALSE);
    }

    return RP_OK;
}

int RP_sql_load_wait_until_finished() {
    GList *p;

    for (p = g_list_first(RP_threads); p; p = g_list_next(p)) {
        RP_thread_info *inf = (RP_thread_info *)p->data;
        pthread_join(inf->thread_id, NULL);
    }

    /* free alloc & return */
    wr_clear_list(&RP_threads);

    return RP_OK;
}

/*
 load the tree from an ascii file (short attr names).
 mainly for testing...
 */
int RP_asc_load(char *filename, int maxobj, int operation, rp_regid_t reg_id) {
    int err;
    FILE *fp;
    char buf[1024];
    char fulltext[65536];
    int objnr = 0;
    unsigned len, oldlen = 0, ranlen;
    char rangstr[IP_RANGSTR_MAX];
    int parsed = 0;
    int eor; /* end of record */

    if ((fp = fopen(filename, "r")) == NULL) {
        perror(filename);
        die;
    }

    do {
        fgets(buf, 128, fp);

        eor = (strlen(buf) <= 1 || feof(fp));

        if (strlen(buf) > 1) {
            len = strlen(buf);
            dieif( oldlen+len+1 > 65536 ); /* object too long */
            memcpy( fulltext+oldlen, buf, len);
            oldlen += len;

            fulltext[oldlen] = 0;
        }

        if (eor) { /* end of object: put into the database. */
            parsed++;

            /* see if it was just some whitespace junk and nothing more */
            if (*fulltext == 0) {
                continue; /* discard */
            }

            /* check if it's a radix object */
            do {
                char attrname[3];
                A_Type_t attrcode;

                if (fulltext[0] == '*' && fulltext[3] == ':') {
                    strncpy(attrname, fulltext+1, 2);
                    attrname[2] = 0;

                    if (strcmp(attrname, "XX") == 0) {
                        /* object deleted */
                        break;
                    }

                    if ((attrcode = DF_attribute_code2type(attrname)) == -1) {
                        fprintf(stderr, "discarding a non-object:\n%s\n", fulltext);
                        break;
                    }

                    if (DF_attrcode_has_radix_lookup(attrcode) == 0) {
                        /* no interest to radix */
                        break;
                    }

                    /* copy and translate the range */
                    ranlen = index(fulltext + 5, '\n') - fulltext - 5;
                    strncpy(rangstr, fulltext+5, ranlen);
                    rangstr[ranlen] = 0;

                    if (NOERR(err = RP_asc_node(operation, rangstr, attrcode, reg_id, fulltext, strlen(fulltext) + 1, 0L))) {
                        objnr++;
                    } else {
                        die; /* error putting into the radix tree */
                        return err;
                    }

                }
                /* CONSTCOND */
            } while (0);

            *fulltext = 0;
            oldlen = 0;
        }
    } while (!feof(fp) && objnr < maxobj);

    return RP_OK;
}
