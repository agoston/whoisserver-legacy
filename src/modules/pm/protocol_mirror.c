/***************************************

 Protocol mirror module (pm). 

 Status: NOT REVIEWED, TESTED

 ******************//******************
 Filename            : protocol_mirror.c
 Author              : andrei
 OSs Tested          : Solaris
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

#define MIN_ARG_LENGTH  6
#define NRTM_DELIM "-:"

#define MAX_OPT_ARG_C 3

#define Q_QUERY      0x01
#define G_QUERY      0x02
#define K_QUERY      0x04

#define IS_Q_QUERY(a)          ((a)&Q_QUERY)
#define IS_G_QUERY(a)          ((a)&G_QUERY)
#define IS_PERSISTENT(a)       ((a)&K_QUERY)

LG_context_t *pm_context;

/* object types considered private, in order: mntner, person, role, irt, organisation */
const int PM_PRIVATE_OBJECT_TYPES[] = {9, 10, 11, 17, 18};

void PM_init(LG_context_t *ctx) {
	pm_context = ctx;
}

/*
 * parses input and fills nrtm_q_t structure
 *
 * Returns:
 *  -1 in case of garbage
 *  1  in case of -q sources
 *  2  in case of valid -g
 *  3  in case of -k
 */
static int parse_request(char *input, nrtm_q_t *nrtm_q) {
	int res=0, err=0;
	int opt_argc;
	int c;
	gchar **opt_argv;
	getopt_state_t *gst= NULL;

	/* Create the arguments. */
	/* This allows only a maximum of MAX_OPT_ARG_C words in the query. */
	opt_argv = g_strsplit(input, " ", MAX_OPT_ARG_C+1);

	/* Determine the number of arguments. */
	for (opt_argc=0; opt_argv[opt_argc] != NULL; opt_argc++)
		;

	dieif( (gst = mg_new(0)) == NULL);

	while ((c = mg_getopt(opt_argc, opt_argv, "kq:g:", gst)) != EOF) {
		switch (c) {
		case 'k':
			res |= K_QUERY; /* persistent connection */
			break;

		case 'q':
			if (gst->optarg != NULL) {
				char *token, *cursor = gst->optarg;

				res |= Q_QUERY;
				err=strncmp(cursor, "sources", 7);
				if (err!=0)
					break;
				cursor+=7;
				g_strchug(cursor);
				token=cursor;
				/* if no sourses are specified - put NULL in nrtm_q->source and list them all */
				if ((*token=='\0') || (*token=='\n')|| ((int)*token==13))
					nrtm_q->source=NULL;
				else {
					cursor=index(token, ' ');
					if (cursor)
						nrtm_q->source=g_strndup(token, (cursor-token));
					else {
						cursor=index(token, 13); /* search for ctrl-M - telnet loves this */
						if (cursor)
							nrtm_q->source=g_strndup(token, (cursor-token));
						else {
							cursor=index(token, '\n');
							if (cursor)
								nrtm_q->source=g_strndup(token, (cursor-token));
							else
								nrtm_q->source=g_strdup(token);
						}
					}
				}
				/* if source was specified - convert it to an upper case */
				if (nrtm_q->source)
					g_strup(nrtm_q->source);
			} else
				err=1;
			break;

		case 'g':
			if (gst->optarg != NULL) {
				char *cursor = gst->optarg;
				char **tokens;

				res |= G_QUERY;
				g_strdelimit(cursor, NRTM_DELIM, ':');
				tokens=g_strsplit(cursor, ":", 5);
				if (tokens==NULL) {
					err=1;
					break;
				}

				if (tokens[0]) {
					/* first token is source name */
					nrtm_q->source=g_strdup(tokens[0]);
					/* convert it to an upper case */
					g_strup(nrtm_q->source);
					if (tokens[1]) {
						/* second token is version number */
						nrtm_q->version=atoi(tokens[1]);
						if (tokens[2]) {
							/* this is first serial */
							nrtm_q->first=atol(tokens[2]);
							if (nrtm_q->first>0) {
								if (tokens[3]) {
									/* this is last serial */
									nrtm_q->last=atol(tokens[3]);
									if (nrtm_q->last==0)
										if (strncasecmp(tokens[3], "LAST", 4)!=0)
											err=1;
								} else
									err=1;
							} else
								err=1;
						} else
							err=1;
					} else
						err=1;
				} else
					err=1;
				g_strfreev(tokens);

			} else
				err=1;

			break;
		default:
			err=1;
			break;
		} /* switch */
	} /* while there are arguments */

	UT_free(gst);
	g_strfreev(opt_argv);

	if (err)
		return (-1);
	else
		return (res);

}

/* PM_interact() */
/*++++++++++++++++++++++++++++++++++++++
 Interact with the client.

 int sock Socket that client is connected to.

 More:
 +html+ <PRE>
 Authors:
 ottrey
 andrei

 +html+ </PRE><DL COMPACT>
 +html+ <DT>Online References:
 +html+ <DD><UL>
 +html+ </UL></DL>

 ++++++++++++++++++++++++++++++++++++++*/
void PM_interact(int sock) {
	char input[MAX_PM_INPUT_SIZE+1];
	char buff[STR_L];
	ca_dbSource_t *source_hdl;
	int read_result;
	int parse_result;
	ip_addr_t address;

	char *hostaddress=NULL;
	sk_conn_st condat;
	nrtm_q_t nrtm_q;
	long current_serial;
	long oldest_serial;
	long object_type;
	aa_mirror_right mirror_perm;

	char *object;
	int operation;

	char *db_host;
	int db_port;
	char *db_name;
	char *db_user;
	char *db_pswd;
	int protocol_version;

	GString *gbuff;

	SQ_connection_t *sql_connection;
	int persistent_connection;

	/* make a record for thread accounting */
	TA_add(sock, "nrtm_srv");

	/* Get the IP of the client */
	hostaddress = SK_getpeername(sock);

	/* initialise the connection structure */
	memset( &condat, 0, sizeof(sk_conn_st));
	/* initialise the nrtm structure */
	memset( &nrtm_q, 0, sizeof(nrtm_q_t));
	/* set the connection data: both rIP and eIP to real IP */
	condat.sock = sock;
	condat.ip = hostaddress;
	SK_getpeerip(sock, &(condat.rIP));
	memcpy( &(condat.eIP), &(condat.rIP), sizeof(ip_addr_t));

	/* Read input */
	read_result = SK_cd_gets(&(condat), input, MAX_PM_INPUT_SIZE);

	/* read_result < 0 is an error and connection should be closed */
	if (read_result < 0) {
		/* log the fact, rtc was set */
	}

	parse_result = parse_request(input, &nrtm_q);

	if (parse_result < 0) {
		LG_log(pm_context, LG_DEBUG, "[%s] -- Garbage received: %s", hostaddress, input);
		/* log the fact and exit */
		/* Free the hostaddress */
		sprintf(buff, "\n%%ERROR:405: syntax error\n\n\n");
		SK_cd_puts(&condat, buff);
		/*      SK_cd_close(&(condat)); */
		UT_free(hostaddress);
		UT_free(nrtm_q.source);
		return;
	}

	LG_log(pm_context, LG_DEBUG, "[%s] -- input: [%s]", hostaddress, input);

	/* this is -q sources query  - answer and return */
	if (IS_Q_QUERY(parse_result)) {

		gbuff=PM_get_nrtm_sources(&(condat.rIP), nrtm_q.source);
		SK_cd_puts(&condat, gbuff->str);
		/* end-of-result one extra line (2 in total) */
		SK_cd_puts(&condat, "\n");
		/* Free allocated memory  */
		g_string_free(gbuff, TRUE);
		UT_free(hostaddress);
		UT_free(nrtm_q.source);
		/*      SK_cd_close(&(condat)); */
		return;
	} else if (IS_G_QUERY(parse_result)) {
		if (IS_PERSISTENT(parse_result))
			persistent_connection=1;
		else
			persistent_connection=0;
	} else {
		LG_log(pm_context, LG_DEBUG, "[%s] -- Syntax error: %s", hostaddress, input);
		/* log the fact and exit */
		/* Free the hostaddress */
		sprintf(buff, "\n%%ERROR:405: syntax error\n\n\n");
		SK_cd_puts(&condat, buff);
		/*      SK_cd_close(&(condat)); */
		UT_free(hostaddress);
		UT_free(nrtm_q.source);
		return;

	}

	/* otherwise this is -g query */

	LG_log(pm_context, LG_DEBUG, "[%s] -- input parsed: %s:%d:%ld-%ld", hostaddress, nrtm_q.source, nrtm_q.version,
	        nrtm_q.first, nrtm_q.last);

	source_hdl = ca_get_SourceHandleByName(nrtm_q.source);
	if (source_hdl == NULL) {
		LG_log(pm_context, LG_DEBUG, "[%s] --  Unknown source %s", hostaddress, nrtm_q.source);
		sprintf(buff, "\n%%ERROR:403: unknown source\n\n\n");
		SK_cd_puts(&condat, buff);
		UT_free(hostaddress);
		UT_free(nrtm_q.source);
		return;
	}

	/* check if the client is authorized to mirror */
	SK_getpeerip(sock, &address);
	if (!(mirror_perm = AA_can_mirror(&address, nrtm_q.source))) {
		LG_log(pm_context, LG_DEBUG, "[%s] --  Not authorized to mirror the source %s", hostaddress, nrtm_q.source);
		sprintf(buff, "\n%%ERROR:402: not authorized to mirror the database\n\n\n");
		SK_cd_puts(&condat, buff);
		UT_free(hostaddress);
		UT_free(nrtm_q.source);
		return;
	}

	/* get protocol version of the source */
	protocol_version = ca_get_srcnrtmprotocolvers(source_hdl);

	/* XXX this is compatibility mode where we don't care about the protocol version */
#if 0
	/* compare to the version requested */
	if(nrtm_q.version != protocol_version) {
		ER_inf_va(FAC_PM, ASP_PM_ERESP,"[%s] --  Source does not support requested protocol %d", hostaddress, nrtm_q.version);
		sprintf(buff, "\n%%ERROR:404: version %d of protocol is not supported\n\n\n", nrtm_q.version);
		SK_cd_puts(&condat, buff);
		free(hostaddress);
		free(nrtm_q.source);
		/*     SK_cd_close(&(condat)); */
		return;
	}
#endif

	/* get database */
	db_name = ca_get_srcdbname(source_hdl);
	/* get database host*/
	db_host = ca_get_srcdbmachine(source_hdl);
	/* get database port*/
	db_port = ca_get_srcdbport(source_hdl);
	/* get database user*/
	db_user = ca_get_srcdbuser(source_hdl);
	/* get database password*/
	db_pswd = ca_get_srcdbpassword(source_hdl);

	sql_connection = SQ_get_connection(db_host, db_port, db_name, db_user, db_pswd);
	if (!sql_connection) {
		LG_log(pm_context, LG_ERROR, " database='%s' [%d] %s", db_name, SQ_errno(sql_connection),
		        SQ_error(sql_connection));
		return;
	}
	LG_log(pm_context, LG_DEBUG, "[%s] --  Made SQL connection to %s@%s", hostaddress, db_name, db_host);

	/* free copies of the variables */
	UT_free(db_host);
	UT_free(db_name);
	UT_free(db_user);
	UT_free(db_pswd);

	/* Not to consume the last serial which may cause crash */
	PM_get_minmax_serial(sql_connection, &oldest_serial, &current_serial);
	current_serial -= SAFE_BACKLOG;

	if ((current_serial==-1) || (oldest_serial==-1)) {
		LG_log(pm_context, LG_ERROR, " database='%s' [%d] %s", db_name, SQ_errno(sql_connection),
		        SQ_error(sql_connection));
		/* Free the hostaddress */
		/*      SK_cd_close(&(condat)); */
		/* close the connection to SQL server */
		SQ_close_connection(sql_connection);
		UT_free(hostaddress);
		UT_free(nrtm_q.source);
		return;
	}

	/* zero indicates that LAST keyword has been used */
	if (nrtm_q.last==0)
		nrtm_q.last=current_serial;
	/* for persistent connections end of range has no meaning */
	if (persistent_connection)
		nrtm_q.last=current_serial;

	if ((nrtm_q.first>nrtm_q.last) || (nrtm_q.first<oldest_serial)|| (nrtm_q.last>current_serial)||(nrtm_q.first<=0)
	        || (nrtm_q.last<=0)) {
		LG_log(pm_context, LG_DEBUG, "[%s] --  Invalid range: %ld-%ld [%ld-%ld]", hostaddress, nrtm_q.first,
		        nrtm_q.last, oldest_serial, current_serial);
		/* write error message back to the client */
		sprintf(buff, "\n%%ERROR:401: invalid range: Not within %ld-%ld\n\n\n", oldest_serial, current_serial);
		SK_cd_puts(&condat, buff);
		/*      SK_cd_close(&(condat)); */

		/* close the connection to SQL server */
		SQ_close_connection(sql_connection);

		/* Free the hostaddress */
		UT_free(hostaddress);
		UT_free(nrtm_q.source);
		return;
	}

	current_serial=nrtm_q.first;

	/* print banner */
	{
		/* get the header string */
		char *resp_header = ca_get_pw_resp_header;
		SK_cd_puts(&condat, "\n");
		SK_cd_puts(&condat, resp_header);
		UT_free(resp_header);
		SK_cd_puts(&condat, "\n");
	}

	sprintf(buff, "%%START Version: %d %s %ld-%ld\n\n", nrtm_q.version, nrtm_q.source, nrtm_q.first, nrtm_q.last);
	SK_cd_puts(&condat, buff);

	/* make a record for thread accounting */
	TA_setactivity(buff);

	/* FIXME: This is crap, but we will redo nrtm completely after the radix tree removal anyway.
	 * It will do until then... */ 
	/*************************** MAIN LOOP ****************************/
	/* now start feeding client with data */
	do {

		/************ ACTUAL PROCESSING IS HERE ***********/
		/* this call will block if queries are paused */
		object=PM_get_serial_object(sql_connection, current_serial, &object_type, &operation);

		/* there is a probability that mirroring interferes with HS cleanup */
		/* in such case serial may be deleted before it is read by mirror client */
		/* null object will be returned in this case and we need to break the loop */
		if (object==NULL)
			break;

		/* filter private object types if it is not authorized */
		if (mirror_perm == AA_MIRROR_PUBLIC) {
			int i = 0;
			for (; i < sizeof(PM_PRIVATE_OBJECT_TYPES)/sizeof(*PM_PRIVATE_OBJECT_TYPES); i++) {
				if (object_type == PM_PRIVATE_OBJECT_TYPES[i]) {
					/* don't process this one */
					operation = OP_NOOP;
					break;
				}
			}
		}

		/* FIXME: OP_UPD is a defined operation, but not used in serials table - left unhandled here, too */
		switch (operation) {
		case OP_ADD:
			SK_cd_puts(&condat, "ADD\n\n");
			SK_cd_puts(&condat, object);
			SK_cd_puts(&condat, "\n");
			break;
		
		case OP_DEL:
			SK_cd_puts(&condat, "DEL\n\n");
			SK_cd_puts(&condat, object);
			SK_cd_puts(&condat, "\n");
			break;

		case OP_NOOP:
		}

		UT_free(object);
		current_serial++;

		/* for real-time mirroring we need some piece of code */
		if (persistent_connection && (condat.rtc == 0)) {
			while (((nrtm_q.last = SQ_get_max_id(sql_connection, "serial_id", "serials") - SAFE_BACKLOG)<current_serial)
			        && (CO_get_do_server()==1))
				sleep(1);
		}

	} /* do while there are more serials, connection was not reset and XXX do_server is on*/
	while ((current_serial<=nrtm_q.last) && (condat.rtc == 0)&& (CO_get_do_server()==1));
	/*******************************************************************/

	sprintf(buff, "%%END %s\n\n\n", nrtm_q.source);
	SK_cd_puts(&condat, buff);

	LG_log(pm_context, LG_INFO, "[%s] -- <%s:%ld-%ld (%ld)> ", hostaddress, nrtm_q.source, nrtm_q.first, nrtm_q.last,
	        nrtm_q.last-nrtm_q.first+1);

	/* make a record for thread accounting */
	TA_delete();

	/* close the connection to SQL server */
	SQ_close_connection(sql_connection);
	/* Free the hostaddress */
	UT_free(hostaddress);
	UT_free(nrtm_q.source);

} /* PM_interact() */
