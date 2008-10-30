/***************************************
 $Revision: 1.1 $

 Wrapper for NRTM client

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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/poll.h>

/* here we store sockets for update threads */
/* they are from SV module */
extern int SV_update_sock[];

/* here is the shutdown pipe for update threads */
/* also from SV module (apologies for breaking encapsulation) */
extern int SV_shutdown_recv_fd;

/* Response time to swtching updates on and off */
#define TIMEOUT 60

/* timeout between successive attempts to establish connection with server */
#define PM_CONNECTION_TIMEOUT 10

/* Maximum number of objects(serials) we can consume at a time */
#define SBUNCH 1000

/* Timeout in seconds when reading (writing) from DBupdate */
#define STREAM_TIMEOUT 120

/* logging */
LG_context_t *ud_context;

void UD_init(LG_context_t *ctx) {
	ud_context = ctx;
}

/************************************************************
 * int get_NRTM_fd()                                         *
 *                                                           *
 * Gets the NRTM stream                                      *
 *                                                           *
 * First tries to request the serials from the NRTM server   *
 * If the name of the server appears to be not a network name*
 * it tries to open the file with this name                  *
 *                                                           *
 * nrtm - pointer to _nrtm structure                         *
 * upto_last - if==1 then requests to download serials using *
 * LAST keyword                                              *
 *                                                           *
 * Returns:                                                  *
 * A file descriptor for a data stream                       *
 * -1 - error                                                *
 *                                                           *
 ************************************************************/
int get_NRTM_fd(struct _nrtm *nrtm, int upto_last, char *source) {
	int sockfd;
	struct hostent *hptr;
	struct sockaddr_in serv_addr;
	struct in_addr *paddr;
	char host_info[STR_XXL];
	GString *line_buff;
	int fd;
	int ret;
	struct hostent result;
	int error;
	int network;

	int so_keepalive = 1;

	/* Check if it is a network stream or a file */
	if ((ret = SK_connect(&sockfd, nrtm->server, nrtm->port, 30)) == SK_OK) { /* this is a network stream */

		/* set keepalive option so we can reconnect in case there was no connectivity */
		/* with primary. See bug 100 for details. Default timeout is 2hours           */
		if ((error = setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &so_keepalive, sizeof(so_keepalive))) == -1) {
			LG_log(ud_context, LG_ERROR, "cannot set KEEPALIVE socket option");
			close(sockfd);
			return -1;
		}

		/* Request all available serials (upto LAST), or SBUNCH of them */
		line_buff = g_string_sized_new(STR_L);
		if (upto_last == 1)
			g_string_sprintf(line_buff, "-g %s:%d:%ld-LAST\n", source, nrtm->version, nrtm->current_serial + 1);
		else if (upto_last == -1) /* persistent mirroring */
			g_string_sprintf(line_buff, "-k -g %s:%d:%ld-LAST\n", source, nrtm->version, nrtm->current_serial + 1);
		else
			g_string_sprintf(line_buff, "-g %s:%d:%ld-%ld\n", source, nrtm->version, nrtm->current_serial + 1,
			        nrtm->current_serial + SBUNCH);

		ret = SK_write(sockfd, line_buff->str, line_buff->len, NULL, NULL);
		g_string_free(line_buff, TRUE);
		if (ret != 1) {
			LG_log(ud_context, LG_ERROR, "cannot write");
			close(sockfd);
			return (-1);
		}
		fd = sockfd;
		network = 1;
		/*fprintf(stderr, "Returning stream pointer\n"); */
	} else if (ret == SK_BADHOST) { /* this is a file stream */

		network = 0;
		close(sockfd);
		/*fprintf(stderr, "Trying file ...\n");*/
		if ((fd = open(nrtm->server, O_RDONLY, 0666)) == -1) {
			LG_log(ud_context, LG_ERROR, "cannot open");
			return -1;
		}
	} else { // real error
		LG_log(ud_context, LG_ERROR, "cannot connect to %s:%d; neither tcp connection nor file", nrtm->server,
		        nrtm->port);
		return -1;
	}

	return fd;
}

/************************************************************
 *  void UD_do_nrtm()                                        *
 *                                                           *
 * Processes NRTM stream                                     *
 *                                                           *
 * It cycles requesting objects from the NRTM server,        *
 * processing them and then sleeping a specified amount of   *
 * time.                                                     *
 *                                                           *
 * It starts by requesting SBUNCH number of serials and does *
 * so untill no serials are received (actually a warning     *
 * is received saying that the requested range is invalid)   *
 * This approach avoids excessive load on the NRTM server    *
 *                                                           *
 * After that it requests serials using LAST keyward keeping *
 * almost in sync with the server                            *
 *                                                           *
 ************************************************************/

void UD_do_nrtm(void *arg) {
	int source = (int)arg;
	UD_stream_t ud_stream;
	struct _nrtm *nrtm;
	int nrtm_delay;
	int do_update = 1;
	int do_server;
	int nrtm_fd;
	int num_ok;
	int upto_last;
	GString *ta_activity;
	ca_dbSource_t *source_hdl = ca_get_SourceHandleByPosition(source);
	char *db_host, *db_name, *db_user, *db_passwd;
	int db_port;
	/* get source we are going to mirror */
	char *source_name = ca_get_srcname(source_hdl);
	LG_context_t *src_ctx;

	{ /* set up the logging path */
		char *er_ud_def;
		GString *er_def;
		LG_appender_t *app;

		er_ud_def = ca_get_ripupdlog; /* something like 'RIPUPDLOG basename' */
		er_def = g_string_new("");
		g_string_sprintf(er_def, "%s_%s", er_ud_def, source_name);
		UT_free(er_ud_def);

		fprintf(stderr, "[%s]\n", er_def->str);
		app = LG_app_get_daily_info_dump(er_def->str);
		g_string_free(er_def, TRUE);

		src_ctx = LG_ctx_new();
		LG_app_set_formatter(app, LG_frm_general_prepared("$TIMESTAMP whois_rip-$PID/$TID $SEVERITY $MESSAGE\n"));
		LG_ctx_add_appender(src_ctx, app);
	}

	/* load the dictionary */
	rpsl_load_dictionary(RPSL_DICT_CORE);

	nrtm = UT_calloc(1, sizeof(struct _nrtm));
	if (nrtm == NULL) {
		LG_log(ud_context, LG_SEVERE, "cannot allocate memory");
		die;
	}
	/* get mode of operation: protected/unprotected (dummy) */
	memset(&ud_stream, 0, sizeof(ud_stream));
	ud_stream.source_hdl = source_hdl;
	ud_stream.ud_mode = ca_get_srcmode(source_hdl);
	nrtm_delay = ca_get_srcnrtmdelay(source_hdl);
	/* Zero delay means persistent connection */
	if (nrtm_delay == 0)
		ud_stream.ud_mode |= B_PERSIST_MIRR;

	fprintf(stderr, "Mode of operation:\n");
	if (IS_DUMMY_ALLOWED(ud_stream.ud_mode))
		fprintf(stderr, "* dummy allowed\n");
	else
		fprintf(stderr, "* dummy not allowed\n");

	if (IS_UPDATE(ud_stream.ud_mode))
		fprintf(stderr, "* DBupdate\n");
	else if (IS_NRTM_CLNT(ud_stream.ud_mode)) {

		if (IS_PERSIST_MIRR(ud_stream.ud_mode))
			fprintf(stderr, "* NRTM: persistent conection\n");
		else
			fprintf(stderr, "* NRTM\n");
	} else
		fprintf(stderr, "* STATIC\n");

	if (IS_STANDALONE(ud_stream.ud_mode))
		fprintf(stderr, "* running standalone\n");
	else
		fprintf(stderr, "* running as a server\n");

	if (IS_NO_NHR(ud_stream.ud_mode))
		fprintf(stderr, "* NHR is not maintained\n");
	else
		fprintf(stderr, "* NHR is maintained\n");

	/* get mirror server */
	nrtm->server = ca_get_srcnrtmhost(source_hdl);

	/* get mirror port */
	nrtm->port = ca_get_srcnrtmport(source_hdl);
	printf("XXX nrtm_port=%d\n", nrtm->port);

	/* get mirror version */
	nrtm->version = ca_get_srcnrtmprotocolvers(source_hdl);

	/* get error log facility */
	/*   logfilename=ca_get_srcnrtmlog(source_hdl); */

	db_host = ca_get_srcdbmachine(source_hdl);
	db_port = ca_get_srcdbport(source_hdl);
	db_name = ca_get_srcdbname(source_hdl);
	db_user = ca_get_srcdbuser(source_hdl);
	db_passwd = ca_get_srcdbpassword(source_hdl);

	/* Connect to the database */
	LG_log(ud_context, LG_DEBUG, "%s making SQL connection to %s@%s ...", UD_TAG, db_name, db_host);
	ud_stream.db_connection = SQ_get_connection(db_host, db_port, db_name, db_user, db_passwd);

	if (!ud_stream.db_connection) {
		LG_log(ud_context, LG_SEVERE, "no connection to SQL server");
		die;
	}

	ud_stream.num_skip = 0;
	ud_stream.load_pass = 0;
	ud_stream.nrtm = nrtm;

	if (IS_PERSIST_MIRR(ud_stream.ud_mode))
		upto_last = -1; /* the faster the better */
	else
		upto_last = 0; /* let's start gradually if the backlog is > SBUNCH (1000) serials */

	/*+++ main cycle +++*/

	do {
		do_update = CO_get_do_update();
		if (do_update) {

			/* Check connection to the database and try to reconnect */
			if (SQ_ping(ud_stream.db_connection)) {
				LG_log(src_ctx, LG_DEBUG, "%s connection to SQL server timed out - reistablishing", UD_TAG);
			}

			/* get current serial */
			nrtm->current_serial = SQ_get_max_id(ud_stream.db_connection, "serial_id", "serials");

			if (nrtm->current_serial == -1) {
				LG_log(ud_context, LG_SEVERE, "cannot obtain current serial: %ld", nrtm->current_serial);
				die;
			}

			LG_log(src_ctx, LG_INFO, "%s connecting to NRTM server [%s:%d] (current serial=%ld)", UD_TAG, nrtm->server,
			        nrtm->port, nrtm->current_serial);

			/* Get file descriptor of the data stream (RPSL format, use mirror reflector to convert if needed) */
			nrtm_fd = get_NRTM_fd(nrtm, upto_last, source_name);
			if (nrtm_fd == -1) {
				LG_log(src_ctx, LG_INFO, "%s Cannot open data stream. Trying...", UD_TAG);
				SV_sleep(PM_CONNECTION_TIMEOUT);
				continue;
			}

			/* make a record for thread accounting */
			TA_add(nrtm_fd, "nrtm_clnt");
			ta_activity = g_string_sized_new(STR_M);
			g_string_sprintf(ta_activity, "[%s]%ld->", source_name, nrtm->current_serial);
			TA_setactivity(ta_activity->str);

			ud_stream.condat.sock = nrtm_fd;
			ud_stream.log.num_ok = 0;
			ud_stream.log.num_failed = 0;

			LG_log(src_ctx, LG_DEBUG, "%s starting processing stream", UD_TAG);

			/***************** process stream ****************/

			num_ok = UD_process_stream(&ud_stream, src_ctx);

			/***************** process stream ****************/

			LG_log(src_ctx, LG_DEBUG, "%s processing stream finished", UD_TAG);

			/* close the socket of the NRTM stream */
			close(ud_stream.condat.sock);

			LG_log(src_ctx, LG_INFO, "%s forwarded to serial:%ld", UD_TAG, (nrtm->current_serial + num_ok));

			/* set activity for thread record */
			g_string_sprintf(ta_activity, "[%s]->%ld", source_name, (nrtm->current_serial + num_ok));
			TA_setactivity(ta_activity->str);
			g_string_free(ta_activity, TRUE);

			/* if we are NOT in persistent mode */
			if (!IS_PERSIST_MIRR(ud_stream.ud_mode)) {
				/* Now we can process serials in normal way (upto LAST) */
				if (num_ok == 0)
					upto_last = 1;
				/* get delay */
				nrtm_delay = ca_get_srcnrtmdelay(source_hdl);
			} else
				/* we need to delay the next attempt not to have a birst of requests */
				nrtm_delay = TIMEOUT;
			/* sleep the delay seconds or untill the shutdown requested */
			SV_sleep(nrtm_delay);

		} /* if do_updates */
		else
			SV_sleep(TIMEOUT);

		TA_delete();

	} while ((do_server = CO_get_do_server())); /* main cycle */

	/*   fclose(ud_stream.log.logfile);*/
	UT_free(source_name);
	/* free data associated with nrtm structure */
	if (nrtm) {
		UT_free(nrtm->server);
		UT_free(nrtm);
	}

	/* That's all. Close connection to the DB */
	SQ_close_connection(ud_stream.db_connection);
	UT_free(db_host);
	UT_free(db_name);
	UT_free(db_user);
	UT_free(db_passwd);

	LG_log(src_ctx, LG_DEBUG, "%s NRTM client stopped", UD_TAG);

	LG_ctx_free(src_ctx);
} /* UD_do_nrtm() */

/************************************************************
 *  void UD_do_updates()                                     *
 *                                                           *
 * Processes updates                                         *
 *                                                           *
 * It cycles accepting connections and processing them       *
 * (interactive server). This assures that there is only     *
 * one write thread per database/source.                     *
 *                                                           *
 ************************************************************/

void UD_do_updates(void *arg) {
	int source = (int)arg;
	int listening_socket = SV_update_sock[source];
	int connected_socket;
	UD_stream_t ud_stream;
	int do_update=1;
	int num_ok;
	ca_dbSource_t *source_hdl = ca_get_SourceHandleByPosition(source);
	char *db_host, *db_name, *db_user, *db_passwd;
	int db_port;
	ip_addr_t address;
	char *source_name;
	LG_context_t *src_ctx;
	struct pollfd pfd[2];

	source_name = ca_get_srcname(source_hdl);

#if 0
	{ /* set up the lohgging path */
		/* get source we are going to update */
		int res;
		char *er_ud_def = ca_get_er_ud_def; /* something like 'RIPUPDLOG basename' */
		GString *er_def;
		char *erret = NULL;

		er_def = g_string_sized_new(256);
		g_string_sprintf(er_def, "%s %s", er_ud_def, source_name);
		if( (res = ER_macro_spec(er_def->str, &erret)) != 0 ) {
			fputs(erret, stderr);
			die;
			/* or some other error handling */
		}
		UT_free(erret); /* the response is allocated and must be freed */
		g_string_free(er_def, TRUE);
		UT_free(er_ud_def);
	}
#endif /* 0 */
	{ /* set up the logging path */
		char *er_ud_def;
		GString *er_def;
		LG_appender_t *app;

		er_ud_def = ca_get_ripupdlog; /* something like 'RIPUPDLOG basename' */
		er_def = g_string_new("");
		g_string_sprintf(er_def, "%s_%s", er_ud_def, source_name);
		UT_free(er_ud_def);

		fprintf(stderr, "[%s]\n", er_def->str);
		app = LG_app_get_daily_info_dump(er_def->str);
		g_string_free(er_def, TRUE);

		src_ctx = LG_ctx_new();
		LG_app_set_formatter(app, LG_frm_general_prepared("$TIMESTAMP whois_rip-$PID/$TID $SEVERITY $MESSAGE\n"));
		LG_ctx_add_appender(src_ctx, app);
	}

	/* load the dictionary */
	rpsl_load_dictionary(RPSL_DICT_CORE);

	/* get mode of operation: protected/unprotected (dummy) */
	memset(&ud_stream, 0, sizeof(ud_stream));
	ud_stream.source_hdl=source_hdl;
	ud_stream.ud_mode=ca_get_srcmode(source_hdl);

	fprintf(stderr, "Mode of operation:\n");
	if (IS_DUMMY_ALLOWED(ud_stream.ud_mode))
		fprintf(stderr, "* dummy allowed\n");
	else
		fprintf(stderr, "* dummy not allowed\n");
	if (IS_UPDATE(ud_stream.ud_mode))
		fprintf(stderr, "* DBupdate\n");
	else
		fprintf(stderr, "* NRTM\n");
	if (IS_STANDALONE(ud_stream.ud_mode))
		fprintf(stderr, "* running standalone\n");
	else
		fprintf(stderr, "* running as a server\n");

	/* get the database */
	db_host = ca_get_srcdbmachine(source_hdl);
	db_port = ca_get_srcdbport(source_hdl);
	db_name = ca_get_srcdbname(source_hdl);
	db_user = ca_get_srcdbuser(source_hdl);
	db_passwd = ca_get_srcdbpassword(source_hdl);

	/* Connect to the database */
	LG_log(src_ctx, LG_DEBUG, "%s making SQL connection to %s@%s ...", UD_TAG, db_name, db_host);
	ud_stream.db_connection=SQ_get_connection(db_host, db_port, db_name, db_user, db_passwd);

	if (!ud_stream.db_connection) {
		LG_log(ud_context, LG_SEVERE, "no connection to SQL server\n");
		die;
	}

	ud_stream.condat.rd_timeout.tv_sec=STREAM_TIMEOUT;
	ud_stream.num_skip=0;
	ud_stream.load_pass=0;
	ud_stream.nrtm=NULL;

	/* set up poll structure */
	pfd[0].fd = listening_socket;
	pfd[0].events = POLLIN;
	pfd[1].fd = SV_shutdown_recv_fd;
	pfd[1].events = POLLIN;

	/*+++ main cycle +++*/

	do { /* be alive while do_server is 1. do_server is turned off by SIGINT */

		/* make a record for thread accounting */
		TA_add(listening_socket, "update");
		TA_setactivity("waiting");

		/* check for input */
		if (poll(pfd, 2, -1) == -1) {
			LG_log(ud_context, LG_ERROR, "UD_do_updates got error %d on poll; %s", errno, strerror(errno));
			continue;
		}

		/* shutdown */
		if (pfd[1].revents != 0)
			break;

		/* accept connection */
		connected_socket = SK_accept_connection(listening_socket);
		if (connected_socket==-1)
			continue;

		/* check if the client is authorised to update */
		SK_getpeerip(connected_socket, &address);
		if (!AA_can_ripupdate(&address, source_name)) {
			char *hostaddress;
			sk_conn_st condat;
			char buff[STR_L];

			memset( &condat, 0, sizeof(sk_conn_st));
			condat.wr_timeout.tv_sec=STREAM_TIMEOUT;
			condat.sock = connected_socket;
			SK_getpeerip(connected_socket, &(condat.rIP));
			memcpy( &(condat.eIP), &(condat.rIP), sizeof(ip_addr_t));
			hostaddress = SK_getpeername(connected_socket);
			condat.ip = hostaddress;

			LG_log(src_ctx, LG_INFO, "[%s] --  Not authorized to update the source %s", hostaddress, source_name);
			sprintf(buff, "\n%%ERROR:406: not authorized to update the database\n\n\n");
			SK_cd_puts(&condat, buff);
			SK_cd_close(&(condat));

			UT_free(hostaddress);
			/* start the next loop */
			continue;
		}

		/* make a record for thread accounting */
		TA_delete(); /* Delete 'waiting' record */
		TA_add(connected_socket, "update");

		ud_stream.condat.sock = connected_socket;
		ud_stream.condat.rtc = 0;

		LG_log(src_ctx, LG_DEBUG, "%s Connection accepted...", UD_TAG);

		ud_stream.log.num_ok=0;
		ud_stream.log.num_failed=0;

		while ((do_update=CO_get_do_update())!=1) {
			TA_setactivity("suspended");
			/* if shutdown was requested - break */
			if (SV_sleep(3*TIME_SLICE))
				break;
		}

		if (do_update) {
			/* Check connection to the database and try to reconnect*/
			if (SQ_ping(ud_stream.db_connection)) {
				LG_log(ud_context, LG_SEVERE, "%s", SQ_error(ud_stream.db_connection));
				die;
			}

			LG_log(src_ctx, LG_DEBUG, "%s starting processing object", UD_TAG);

			/***************** process stream ****************/

			num_ok=UD_process_stream(&ud_stream, src_ctx);

			/***************** process stream ****************/

			LG_log(src_ctx, LG_DEBUG, "%s processing object finished", UD_TAG);

			/* close the socket of the NRTM stream */
			close(ud_stream.condat.sock);

		} /* if do_update*/

		TA_delete();

	} while (CO_get_do_server()); /* main cycle */

	/*   fclose(ud_stream.log.logfile); */
	/* That's all. Close connection to the DB */
	SQ_close_connection(ud_stream.db_connection);
	UT_free(db_host);
	UT_free(db_name);
	UT_free(db_user);
	UT_free(db_passwd);
	UT_free(source_name);

	LG_ctx_free(src_ctx);

	LG_log(ud_context, LG_INFO, "%s update server stopped", UD_TAG);
} /* UD_do_update() */

