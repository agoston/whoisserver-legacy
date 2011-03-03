/***************************************
  $Revision: 1.4 $

  Protocol whois module (pw).  Whois protocol.

  Status: NOT REVUED, TESTED

  ******************/ /******************
  Filename            : protocol_whois.c
  Authors             : ottrey@ripe.net - framework and draft implementation
                        marek@ripe.net - rewritten and extended.
  OSs Tested          : Solaris 2.6
  ******************/ /******************
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

#include <stdio.h>
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

/*+ Maximum size of input that can be recieved from the client. +*/
#define MAX_INPUT_SIZE  1024

/* logging context */
LG_context_t *pw_context;
LG_context_t *query_context;

/*++++++++++++++++++++++++++++++++++++++

void
PW_display_file     opens a file and displays its contents to the
                    connection described in conn. structure.


sk_conn_st *condat  pointer to connection structure

char *filename      file name

  ++++++++++++++++++++++++++++++++++++++*/
void PW_display_file(sk_conn_st *condat, char *filename)
{
  char *buffer;
  struct stat sb;
  int bytes;
  int p;
  int fpint;

  /* open our file */
  fpint = open(filename, O_RDONLY);
  if (fpint == 0) {
    LG_log(pw_context, LG_ERROR, "open() failure \"%s\" : %s (%d)",
	   filename, strerror(errno), errno);
    return;
  }

  /* get the size of the file */
  if (fstat(fpint, &sb) != 0) {
    LG_log(pw_context, LG_ERROR, "fstat() failure \"%s\" : %s (%d)",
           filename, strerror(errno), errno);
    close(fpint);
    return;
  }

  /* allocate a buffer for the file */
  buffer = UT_malloc(sb.st_size+1);

  /* read the file contents */
  bytes = read(fpint, buffer, sb.st_size);
  close(fpint);

  /* can't read more bytes that we asked for */
  dieif(bytes > sb.st_size);


  /* remove any newlines (actually any whitespace) at the end of the file */
  /* we do this because we can have ONLY ONE newline at the end of the */
  /* output - any more violates our OBJECT, "\n", OBJECT, "\n" format */
  p = bytes-1;
  while ((p>=0) && isspace((int)buffer[p])) {
      p--;
  }

  /* NUL-terminate our string */
  buffer[p+1] = '\0';

  /* output our resulting buffer */
  SK_cd_printf(condat, "%s\n\n", buffer);
/*  SK_cd_puts(condat, buffer);*/

  /* and enough blank lines */
/*  SK_cd_puts(condat, "\n\n");*/

  /* free the allocated memory */
  UT_free(buffer);
}/* PW_display_file */


/*++++++++++++++++++++++++++++++++++++++

  static void
  pw_log_query              logs the query to a file after it has finished.
                            Takes many parameters to have access to as much
			    information as possible, including the original
			    query, accounting, response time, status of the
			    client connection, etc.


  Query_environ *qe       query environment

  Query_command *qc       query command structure

  acc_st *copy_credit     numbers of objects returned / referrals made
                          during this query
                          (calculated as original credit assigned before
			  the query minus what's left after the query).

  ut_timer_t begintime    time the processing began

  ut_timer_t endtime      time the processing finished

  char *hostaddress       text address of the real IP

  char *input             original query (trailing whitespaces chopped off)

  ++++++++++++++++++++++++++++++++++++++*/
static
void pw_log_query(Query_environ *qe,
		   Query_command *qc,
		   acc_st *copy_credit,
		   ut_timer_t begintime,
		   ut_timer_t endtime,
		   char *hostaddress,
		   char *input)
{
  char *qrystat = AC_credit_to_string(copy_credit);
  float elapsed;
  char *qrytypestr = "";

  if (qc && (qc->query_type != QC_REAL)) {
      qrytypestr = QC_get_qrytype(qc->query_type);
  }

  elapsed = UT_timediff( &begintime, &endtime);

  /* log the connection/query/#results/time/denial to file */
  LG_log(query_context, LG_INFO, "<%s> %s%s %.2fs [%s] --  %s",
	    qrystat,
	    qe->condat.rtc ? "INT " : "",
	    qrytypestr,
	    elapsed, hostaddress, input
	    );
  UT_free(qrystat);
} /* pw_log_query */




/*++++++++++++++++++++++++++++++++++++++

  void
  PW_process_qc          processes the query commands determined in QC,
                         This is where all the real action of the query
			 part is invoked.

  Query_environ *qe      query environment

  Query_command *qc      query command structure

  acc_st *acc_credit     credit assigned to this IP

  acl_st *acl_eip        current acl record applicable to this IP

  char *hostaddress      IP address of the query

  ++++++++++++++++++++++++++++++++++++++*/
void PW_process_qc(Query_environ *qe, Query_command *qc, acc_st *acc_credit, acl_st *acl_eip, char *hostaddress) {
	GList *qitem;
	Query_instructions *qis=NULL;
	int err;

	switch (qc->query_type) {
	case QC_PARERR:
		/* parameter error. relevant error message is already printed */
		/* force disconnection on error */
		qe->k = 0;
		break;
	case QC_NOKEY:
		/* no key (this is OK for some operational stuff, like -k) */
		/* we still need an extra newline to hand control back to the
		 client in the "-k" scenerio */
		if (qe->k) {
			/* XXX: in the query new code this newline is not necessary! */
			/*        SK_cd_puts(&(qe->condat), "\n");*/
			break;
		}
		/* if not -k, treat as an empty query, and FALLTHROUGH */
	case QC_EMPTY:
		/* The user didn't specify a key, so
		 - print moron banner
		 - force disconnection of the user. */
	{
		char *rep = ca_get_pw_err_nokey;
		SK_cd_printf(&(qe->condat), "%s\n\n", rep);
		UT_free(rep);
	}
		qe->condat.rtc = SK_NOTEXT;
		break;
	case QC_HELP:
	case QC_SYNERR: {
		char *rep = ca_get_pw_help_file;
		PW_display_file( &(qe->condat), rep);
		UT_free(rep);
	}
		break;
	case QC_TEMPLATE:
		switch (qc->q) {
		case QC_Q_SOURCES:
			/* print source & mirroring info */
		{
			GString *srcs = PM_get_nrtm_sources( &qe->condat.rIP, NULL);
			SK_cd_puts(&(qe->condat), srcs->str);
			g_string_free(srcs, TRUE);
		}
			break;
		case QC_Q_TYPES:
			/* print available types */
		{
			int i;
			char * const*class_names = DF_get_class_names();
			GString *types = g_string_new("");
			for (i=0; class_names[i] != NULL; i++) {
				g_string_append(types, class_names[i]);
				g_string_append_c(types, '\n');
			}
			g_string_append_c(types, '\n');
			SK_cd_puts(&(qe->condat), types->str);
			g_string_free(types, TRUE);
		}
			break;
		case QC_Q_VERSION:
			SK_cd_puts(&(qe->condat), "% whois-server-" VERSION "\n\n");
			break;
		default:
			/* EMPTY */
			;
		} /* -q */

		if (qc->t >= 0) {
			SK_cd_printf(&(qe->condat), "%s\n", DF_get_class_template(qc->t));
		}
		if (qc->v >= 0) {
			SK_cd_puts(&(qe->condat), DF_get_class_template_v(qc->v));
			/* no need for newline here, because it's all broken for any */
			/* automated processor at this point anyway */
		}
		break;

	case QC_FILTERED: {
		char *rep = ca_get_pw_k_filter;
		SK_cd_puts(&(qe->condat), rep);
		UT_free(rep);
	}
		/* FALLTROUGH */
	case QC_REAL:

	    /* Log query instructions */
#ifdef DEBUG_QUERY
        {
            char *temp = QC_query_command_to_string(qc);
            fprintf(stderr, "\n---\nRunning query: %s\n", temp);
            free(temp);
        }
#endif

	    qis = QI_new(qc, qe);

		/* go through all sources,
		 stop if connection broken - further action is meaningless */
		for (qitem = g_list_first(qe->sources_list); qitem && !qe->condat.rtc; qitem = g_list_next(qitem)) {

			/* QI will decrement the credit counters */
			PW_record_query_start();
			err = QI_execute(qitem->data, qis, qe, acc_credit, acl_eip);
			PW_record_query_end();
			if ( !NOERR(err) ) {
				if (err == QI_CANTDB) {
					SK_cd_puts(&(qe->condat), "% WARNING: Failed to make connection to ");
					SK_cd_puts(&(qe->condat), (char *)qitem->data);
					SK_cd_puts(&(qe->condat), " database.\n\n");
				}
				break; /* quit the loop after any error */
			}/* if error*/

		}/* for every source */

		QI_free(qis);

		if (AC_credit_isdenied(acc_credit) ) {
			/* host reached the limit of returned contact information */
			char *rep = ca_get_pw_fmt_limit_reached;
			SK_cd_printf(&(qe->condat), rep, hostaddress);
			UT_free(rep);
		}

		break;
	default:
		die;
	}
} /* PW_process_qc */

/*
   Occasionally, we need to pause queries to the Whois database.  This
   occurs, for instance, when the database is reloaded for one of the
   databases we mirror without using NRTM.

   The way this works is the caller of PW_stopqueries() gets a "write
   lock" on queries.  Each query gets a "read lock".  The lock mechanism
   that favors writers is used.

   This means that no new read locks can start once PW_stopqueries() is
   called, and that it doesn't return until all read locks have been
   released.  At this point, queries are stopped and the caller can
   proceed to munge about the database safely.

   XXX: This is not the best possible solution, because on a very slow
   query (for instance one with a very common person name), the thread
   calling PW_stopqueries() as well as ALL query threads cannot proceed
   until that thread completes.  An alternative with one lock per
   database was considered, and may be pursued in the future, but for
   now it is not a big problem, since operation occurs normally, just
   possibly with a delay in response for some users.

   PW_startqueries() releases the write lock, and queries proceed
   normally.
 */

/* pause queries using a thread lock that favors writers */
static rw_lock_t queries_lock;

/* PW_stopqueries() */
void
PW_stopqueries()
{
    TH_acquire_write_lockw(&queries_lock);
}

/* PW_startqueries() */
void
PW_startqueries()
{
    TH_release_write_lockw(&queries_lock);
}

/* PW_record_query_start() */
void
PW_record_query_start()
{
    TH_acquire_read_lockw(&queries_lock);
}

/* PW_record_query_end() */
void
PW_record_query_end()
{
    TH_release_read_lockw(&queries_lock);
}

/*++++++++++++++++++++++++++++++++++++++

	void PW_run_query			runs the query, makes accounting stuff and logs the query.

	Query_environ *qe      query environment

    Query_command *qc      query command structure

    acc_st *acc_credit     credit assigned to this IP

    acl_st *acl_ip		  acl record for this IP

    char *hostaddress     IP address of the host as a string

    char *input			  query as a string. may be empty string ""

++++++++++++++++++++++++++++++++++++++*/
void PW_run_query(Query_environ *qe, Query_command *qc, acc_st *acc_credit, acl_st *acl_ip, char *hostaddress, char *input) {

    /* time */
    ut_timer_t begintime;
    ut_timer_t endtime;
    acc_st copy_credit;

    mysql_thread_init();

    /* save accounting copy */
    copy_credit = *acc_credit;

    UT_timeget(&begintime);

    /* ** ACTUAL PROCESSING IS HERE ** */
    PW_process_qc(qe, qc, acc_credit, acl_ip, hostaddress);

    /* increase the number of queries */
    if (qc->query_type == QC_REAL) {
        copy_credit.queries++;
    }

    /* now, do some accounting stuff */

    /* calc. the credit used, result  into copy_credit
     This step MUST NOT be forgotten. It must complement
     the initial calculation of a credit, otherwise accounting
     will go bgzzzzzt.
     */

    AC_acc_addup(&copy_credit, acc_credit, ACC_MINUS);

    /* can say 'nothing found' only if:
     - the query did not just cause denial
     - was a 'real' query
     - nothing was returned
     */

    if (!AC_credit_isdenied(&copy_credit) && (qc->query_type == QC_REAL || qc->query_type == QC_FILTERED) && copy_credit.private_objects + copy_credit.public_objects + copy_credit.referrals == 0) {

        /* now: if the rtc flag is zero, the query ran to completion */
        if (qe->condat.rtc == 0) {
            char *rep = ca_get_pw_notfound;
            SK_cd_puts(&(qe->condat), rep);
            UT_free(rep);
            SK_cd_puts(&(qe->condat), "\n");
        } else {
            /* something happened. Hope for working socket and display message
             (won't hurt even if socket not operable)
             */
            char *rep = ca_get_pw_connclosed;
            SK_cd_puts(&(qe->condat), rep);
            UT_free(rep);
        }
    } /* if credit is denied */

    /* LOGGING */

    UT_timeget(&endtime);

    /* query logging */
    pw_log_query(qe, qc, &copy_credit, begintime, endtime, hostaddress, input);

    /* Commit the credit. This will deny if bonus limit hit and clear the copy */

    /* to avoid increasing the number of (simultaneous) connections (is already committed in PW_interact) */
    copy_credit.sim_connections = 0;
    copy_credit.connections = 0;

    /* ATTENTION: copy_credit is zeroed here !!! */
    AC_commit(&(qe->eIP), &copy_credit, acl_ip);

    /* end-of-result -> ONE empty line */
    SK_cd_puts(&(qe->condat), "\n");

    mysql_thread_end();
}

/*+++++++++++++++++++++++++++++++++++

	void PW_log_denied_query		logs denied queries

    Query_environ *qe		query environment

    Query_command *qc		query command structure (or NULL pointer)

    char *hostaddress		IP address of the querying host as a string

    char *input				query as a string. may be empty string ""

+++++++++++++++++++++++++++++++++++++*/
void PW_log_denied_query(Query_environ *qe, Query_command *qc, char *hostaddress, char *input)
{

  acc_st tmp_acc;
  ut_timer_t begintime;
  ut_timer_t endtime;

  /* denied queries are so short! - count begin time and enrd time. */
  UT_timeget(&begintime);
  UT_timeget(&endtime);

  /* zero object counters */
  memset(&tmp_acc, 0, sizeof(acc_st));

  /* this will give us <0+0+0 **DENIED **>, as we return NO objects */
  tmp_acc.denials = 1;

  /* LOG */
  pw_log_query(qe, qc, &tmp_acc, begintime, endtime, hostaddress, input);
}


/*++++++++++++++++++++++++++++++++++++++

  void
  PW_interact             Main loop for interaction with a single client.
                          The function sets up the accounting for the client,
			  invokes parsing, execution, logging and accounting
			  of the query.

  ++++++++++++++++++++++++++++++++++++++*/
void PW_interact(svr_args *args) {
    char input[MAX_INPUT_SIZE];
    int read_result;
    Query_environ *qe = NULL;
    Query_command *qc = NULL;
    int deny = 0;
    GList *message_node;

    /* Initialize the query environment. */
    qe = QC_environ_new(args->conn_sock);

    /* this is main loop. it runs at least once and
     exits if one of these is true:
     (-1). sim_conn > maxconn, checked at server.c::main_loop()
     1. sim_conn > threshold
     2. real IP is _permanently_ denied
     3. not a persistent connection
     4. broken socket
     5. whoisd off
     */
    do {

        /* main variables:
         acc_credit 			used to store real IP accounting for normal queries.
         pass_credit 			used to store passed IP accounting for normal queries.
         acl_rip				stores real IP acl record
         acl_eip				stores passed IP acl record (if needed).
         */
        acc_st acc_credit, pass_credit;
        acl_st acl_rip, acl_eip;

        /* refresh acl and accounting copy */
        AC_check_acl(&(qe->condat.rIP), &acc_credit, &acl_rip);

        /* The hard limit of simultaneous connections is checked in server.c:main_loop() to avoid the overhead of creating threads
         * So here we are only checking for the soft limits (a.k.a. the ones which return an error message) */
        if (args->act_conn_num > acl_rip.threshold) {

            /* threshold exceeded: show the message and prepare to drop connection */
            char *banner = ca_get_pw_banner;
            char *conndeny = ca_get_pw_fmt_acl_conndeny;
            SK_cd_printf(&(qe->condat), "%s\n", banner);
            SK_cd_printf(&(qe->condat), conndeny, qe->condat.rIPs);
            UT_free(conndeny);
            UT_free(banner);

            PW_log_denied_query(qe, NULL, qe->condat.rIPs, "");

            /* keep in mind */
            deny = 1;

        } else if (acl_rip.deny) {
            /* permanently denied, show the message and count denials */

            char *banner = ca_get_pw_banner;
            char *permdeny = ca_get_pw_fmt_acl_permdeny;
            SK_cd_printf(&(qe->condat), "%s\n", banner);
            SK_cd_printf(&(qe->condat), permdeny, qe->condat.rIPs);
            UT_free(permdeny);
            UT_free(banner);

            /* increase denial count */
            AC_commit_denials(&(qe->condat.rIP), &acl_rip);

            PW_log_denied_query(qe, NULL, qe->condat.rIPs, "");

            /* keep in mind */
            deny = 1;
        } else {
            /* okay, we actually have to read the query now */

            /* print the greeting */
            char *rep = ca_get_pw_banner;
            SK_cd_printf(&(qe->condat), "%s\n", rep);
            UT_free(rep);

            TA_setactivity("waiting for query");

            /* Read input */
            read_result = SK_cd_gets(&(qe->condat), input, MAX_INPUT_SIZE);

            /* check to see if we have a complete line */
            dieif(read_result >= MAX_INPUT_SIZE);
            if ((read_result > 0) && (input[read_result - 1] != '\n')) {
                /* rejection message */
                char *errmsg = ca_get_pw_err_linetoolong;
                SK_cd_printf(&(qe->condat), "%s\n\n", errmsg);
                UT_free(errmsg);

                PW_log_denied_query(qe, NULL, qe->condat.rIPs, "");

                /* keep in mind */
                deny = 1;

            } else { /* line fits in buffer */

                /* remove leading and trailing whitespace (including "\n") */
                g_strstrip(input);

                TA_setactivity(input);
                TA_increment();

                qc = QC_create(input, qe);

                /* output any messages from parsing */
                message_node = qc->parse_messages;
                while (message_node != NULL) {
                    SK_cd_printf(&(qe->condat), "%s\n", (char*) message_node->data);
                    message_node = g_list_next(message_node);
                }

                /* ADDRESS PASSING: check if -V option has passed IP in it */
                if (!STRUCT_EQUAL(qe->eIP, IP_ADDR_UNSPEC)) {
                    if (!acl_rip.trustpass) {
                        /* host not authorised to pass addresses with -V */

                        /* display the deny banner */
                        char *rep = ca_get_pw_fmt_acl_addrpass;
                        SK_cd_printf(&(qe->condat), rep, qe->condat.rIPs);
                        UT_free(rep);

                        /* commit denial count */
                        AC_commit_denials(&(qe->condat.rIP), &acl_rip);
                        PW_log_denied_query(qe, qc, qe->condat.rIPs, input);

                    } else {

                        /* check ACL. Get the proper acl record. Calculate credit */
                        AC_check_acl(&(qe->eIP), &pass_credit, &acl_eip);

                        if (acl_eip.deny) {

                            /* passed IP permanently denied */
                            char *rep = ca_get_pw_fmt_acl_permdeny;
                            SK_cd_printf(&(qe->condat), rep, qe->eIPs);
                            UT_free(rep);

                            /* increase denial count */
                            AC_commit_denials(&(qe->eIP), &acl_eip);
                            PW_log_denied_query(qe, qc, qe->eIPs, input);

                            /* keep in mind */
                            deny = 1;

                        } else {
                            /* allowed to query from passed IP */
                            PW_run_query(qe, qc, &pass_credit, &acl_eip, qe->eIPs, input);
                        }
                    }
                } else {
                    /* set effective IP to real IP */
                    qe->eIP = qe->condat.rIP;
                    strcpy(qe->eIPs, qe->condat.rIPs);

                    /* allowed to query from real IP */
                    PW_run_query(qe, qc, &acc_credit, &acl_rip, qe->eIPs, input);
                }

                QC_free(qc);

                /* clear eIP for next query */
                memset(&(qe->eIP), 0, sizeof(qe->eIP));
                qe->eIPs[0] = 0;
            }
        }
    } while (qe->k && qe->condat.rtc == 0 && deny == 0 && CO_get_whois_suspended() == 0);

    QC_environ_free(qe);
}


/* to be called before any other PW functions */
void PW_init(LG_context_t *pw_ctx, LG_context_t *query_ctx) {
    TH_init_read_write_lockw(&queries_lock);
    pw_context = pw_ctx;
    query_context = query_ctx;
}

