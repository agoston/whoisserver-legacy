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

/* FIXME: NRTM protocol is basically undocumented and unspecified.
 * The only doc I could find googling 'site:ripe.net' is this proposal email:
 * http://ripe.net/ripe/maillists/archives/db-wg/2000/msg00201.html
 * agoston, 2007-10-10 */

#include "rip.h"
#include "class.h"

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
const int PM_PRIVATE_OBJECT_TYPES[] = { 9, 10, 11, 17, 18 };

gchar **PM_DUMMY_ADD_ATTR;
GHashTable *PM_DUMMY_ATTR;

/* Initializes variables to use in dummify_object().
 * Need to be executed before any calls to dummify_object() are made.
 * 
 * Also checks if all private object's mandatory attributes are covered in the rip.config file DUMMY_ATTR options
 * (and only those).
 * 
 * agoston, 2007-10-31 */
static void dummify_init() {
	gchar **lines, **actline;
	char *dummy_attr = ca_get_dummy_attr;
	char *dummy_add_attr = ca_get_dummy_add_attr;
	GHashTable *mandatory_attribs;
	int i;

	void collect_mandatory_attribs_callback(gpointer key, gpointer value, gpointer data) {
		class_attr_t *actattr = (class_attr_t *)value;
		if (actattr->choice == ATTR_MANDATORY) {
			g_hash_table_insert(mandatory_attribs, key, value);
		}
	}
	
	void log_leftover_mandatory_attribs_callback(gpointer key, gpointer value, gpointer data) {
		fprintf(stderr, "Attribute %s is defined as mandatory RPSL attribute of a private object type, but there is no DUMMY_ATTR configured for it in rip.config!\n", key);
		LG_log(pm_context, LG_FATAL, "Attribute %s is defined as mandatory RPSL attribute of a private object type, but there is no DUMMY_ATTR configured for it in rip.config!", key);
	}
	
	/* collect all mandatory options of private objects to perform sanity check during config parsing */
	mandatory_attribs = g_hash_table_new(g_str_hash, g_str_equal);
	for (i = 0; i < sizeof(PM_PRIVATE_OBJECT_TYPES)/sizeof(*PM_PRIVATE_OBJECT_TYPES); i++) {
		const class_t *actclass = class_lookup_id(PM_PRIVATE_OBJECT_TYPES[i]);
		g_hash_table_foreach(actclass->attr_hash, collect_mandatory_attribs_callback, NULL);
	}
	
	/* parse dummify config options
	 * it is required during the full lifespan of the whoisserver process, so there is no mechanism to
	 * free() the structures allocated here - agoston, 2007-10-31 */
	PM_DUMMY_ADD_ATTR = g_strsplit(dummy_add_attr, "\n", 0);
	PM_DUMMY_ATTR = g_hash_table_new(g_str_hash, g_str_equal);

	lines = g_strsplit(ca_get_dummy_attr, "\n", 0);
	for (actline = lines; *actline && **actline; actline++) {
		gchar *equal = strchr(*actline, '=');
		if (!equal) {
			fprintf(stderr, "No = sign found in one of the DUMMY_ATTR lines in the rip.config file\n");
			LG_log(pm_context, LG_FATAL, "No = sign found in one of the DUMMY_ATTR lines in the rip.config file");
			die;
		}

		*equal = 0;
		if (!g_hash_table_remove(mandatory_attribs, *actline)) {
			fprintf(stderr, "Attribute %s is not mandatory - there is no use of defining a DUMMY_ATTR line for it in rip.config!\n", *actline);
			LG_log(pm_context, LG_FATAL, "Attribute %s is not mandatory - there is no use of defining a DUMMY_ATTR line for it in rip.config!", *actline);
			die;
		}
		g_hash_table_insert(PM_DUMMY_ATTR, *actline, equal+1);
	}

	if (g_hash_table_size(mandatory_attribs) > 0) {
		g_hash_table_foreach(mandatory_attribs, log_leftover_mandatory_attribs_callback, NULL);
		die;
	}
	
	free(dummy_attr);
	free(dummy_add_attr);
	g_hash_table_destroy(mandatory_attribs);
}

void PM_init(LG_context_t *ctx) {
	pm_context = ctx;
	dummify_init();
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
					tokens=g_strsplit(cursor, ":", 3);
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
								/* this is the serial */
								char **serial_tokens = g_strsplit(tokens[2], "-", 2);
								nrtm_q->first=atol(serial_tokens[0]);
								if (nrtm_q->first>0) {
									if (serial_tokens[1]) {
										/* this is last serial */
										nrtm_q->last=atol(serial_tokens[1]);
										if (nrtm_q->last==0)
											if (strncasecmp(serial_tokens[1], "LAST", 4)!=0)
												err=1;
									} else
										err=1;
								} else
									err=1;
								g_strfreev(serial_tokens);
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

/* PUBLIC NRTM STREAM DESIGN
 * 
 * Instead of simply dropping private objects from the public nrtm stream, we decided to send dummified objects.
 * It has the following adventages:
 * - We can add a nice 'this is a dummy object, for the real stuff, use whois.ripe.net blabla' message to each
 *   dummified object, thus spare us *a lot* of support trouble
 * - There will be no gaps in serials
 * - 3rd party software interpreting the nrtm streams do not need modification to handle special cases
 * - We won't create a special case which will bite into our asses when we do some change
 * - All referential integrity will be maintained
 * 
 * We do the dummification in a generic way. For each object, we first parse it using the RPSL module. This sucks
 * big time performance-wise, as it does a lot of surplus processing and error checking, however, we are more flexible
 * about future changes, handling outdated/erronous objects in the database, and also, we handle corner cases like 
 * line continuation out of the box, error-free. Note however that the RPSL library's API is horrible, but there
 * was also no point in re-implementing one from scratch (I'd very much liked that, though).
 * 
 * This also means that objects which have been stuffed into the database earlier, not passing the current syntax
 * rules will produce an error. On such rpsl parser errors, we return NULL, which marks that the object could not 
 * be dummified. In this case, the NRTM server will return operation NOOP from the main loop.
 * This will normally not be a problem as we also don't want to give object history through public NRTM stream. Any
 * attempt to try to go back more than a week into the past should give an error.
 * 
 * After having the processed object structure, we iterate through the attributes. We discard any non-mandatory
 * attributes. From the mandatory ones, we keep the ones which maintain referential integrity. The remaining
 * attributes are dummified based on the corresponding entries in the rip.config file (read comments there).
 * 
 * After finishing the dummification, we return the flat object for the nrtm server to send to the client or NULL
 * on dummification error.
 * 
 * Note that dummify_init() must be called before any calls are made to dummify_object(). 
 * 
 * agoston, 2007-10-18   
 * */
static char *dummify_object(char *object) {
	const GList *errors = NULL;
	gboolean quit = FALSE;
	rpsl_attr_t *first_attr = NULL;
	rpsl_error_t error;
	rpsl_object_t *obj = NULL;
	int i, actoff;
	GList *gli = NULL;
	gchar *prim_val = NULL;
	char **actline;
	char *ret = NULL;

	/* parse the object */
	obj = rpsl_object_init(object);
	errors = rpsl_object_errors(obj);
	first_attr = (rpsl_attr_t *)obj->attributes->data;

	for (; errors; errors = errors->next) {
		rpsl_error_t *acterr = (rpsl_error_t *)errors->data;
		if (acterr->level >= RPSL_ERRLVL_ERROR) {
			LG_log(pm_context, LG_ERROR, "[%s: %s] rpsl_object_init failed: %s (at attribute %d)",
			        first_attr->lcase_name, first_attr->value, acterr->descr, acterr->attr_num);
			quit = TRUE;
		}
	}

	/* if there's a serious error, we skip this object */
	if (quit) {
		goto dummify_abort;
	}

	/* the primary key is not necessarily the first attribute of the object, e.g. person */
	prim_val = rpsl_object_get_key_value(obj);

	/* iterate through the attributes */
	actoff = 0;
	for (gli = g_list_first(obj->attributes); gli; actoff++, gli = g_list_next(gli)) {
		rpsl_attr_t *act_attr = (rpsl_attr_t *)gli->data;

		if (rpsl_attr_is_required(obj, act_attr->lcase_name)) {
			gchar *dummy_attr = g_hash_table_lookup(PM_DUMMY_ATTR, act_attr->lcase_name);;

			if (dummy_attr) {
				if (*dummy_attr) {
					char buf[STR_L];
					snprintf(buf, STR_L, dummy_attr, prim_val);
					rpsl_attr_replace_value(act_attr, buf);
				}
				/* if dummy_attr is empty, we leave the rpsl attrib alone */
			} else {
				/* mandatory attribute is not listed in config file - this is already checked in the
				 * init routine, so we die here */
				die;
			}
		} else {
			/* We use the _internal call as the normal one does some extra, and, in this case, surplus 
			 * checking.
			 * 
			 * During call, the only possible error is trying to remove a mandatory attribute - but since
			 * we already checked that, we don't care about errors here. - agoston, 2007-10-29 */
			gli = gli->prev;
			rpsl_object_remove_attr_internal(obj, actoff, NULL);
			actoff--;
		}
	}

	/* append the additional attributes */
	for (actline = PM_DUMMY_ADD_ATTR; *actline && **actline; actline++) {
		rpsl_attr_t *newattr;
		char buf[STR_L];

		snprintf(buf, STR_L, "remarks: %s", *actline);
		newattr = rpsl_attr_init(buf, NULL);
		if (!rpsl_object_append_attr(obj, newattr, &error)) {
			fprintf(stderr, "Error in dummify_object()::rpsl_object_append_attr(): %s\n", error.descr);
			LG_log(pm_context, LG_FATAL, "Error in dummify_object()::rpsl_object_append_attr(): %s", error.descr);
			rpsl_error_free(&error);
			goto dummify_abort;
		}
	}

	ret = rpsl_object_get_text(obj, RPSL_STD_COLUMN);

	dummify_abort:

	if (prim_val)
		free(prim_val);
	if (obj)
		rpsl_object_delete(obj);
	return ret;
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
		sprintf(buff, "\n%%ERROR:403: unknown source %s\n\n\n", nrtm_q.source);
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
	/* We don't need this anymore - just don't start dynamic mode if the server crashes on a serial 
	 * agoston, 2007-12-21 */
	/*current_serial -= SAFE_BACKLOG; */

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

	sprintf(buff, "%%START Version: %d %s %ld-%ld%s\n\n", nrtm_q.version, nrtm_q.source, nrtm_q.first, nrtm_q.last,
	        (mirror_perm < AA_MIRROR_FULL) ? " FILTERED" : "");
	SK_cd_puts(&condat, buff);

	/* make a record for thread accounting */
	TA_setactivity(buff);

	/* FIXME: This is the most basic solution possible (i.e. crap). There's a lot of room for optimization.
	 * Let's see how it works in real world first. If we would encounter performance problems, this is the
	 * entry point for nrtm optimizations.
	 * agoston, 2007-11-15 */
	/*************************** MAIN LOOP ****************************/
	/* now start feeding client with data */
	do {

		/************ ACTUAL PROCESSING IS HERE ***********/
		/* this call will block if queries are paused */
		object=PM_get_serial_object(sql_connection, current_serial, &object_type, &operation);

		/* there is a probability that mirroring interferes with HS cleanup */
		/* in such case serial may be deleted before it is read by mirror client */
		/* null object will be returned in this case and we need to break the loop */
		if (object==NULL) {
			/* The first serial, being only present so that the serials table is not empty, is set to
			 * OP_NOOP, as it serves no information - agoston, 2008-01-17 */
			if (operation != OP_NOOP) break;

		} else {
	
			/* filter private object types if it is not authorized */
			if (mirror_perm == AA_MIRROR_PUBLIC) {
				int i = 0;
				for (; i < sizeof(PM_PRIVATE_OBJECT_TYPES)/sizeof(*PM_PRIVATE_OBJECT_TYPES); i++) {
					if (object_type == PM_PRIVATE_OBJECT_TYPES[i]) {
						char *newobj;
	
						newobj = dummify_object(object);
						free(object);
	
						if (newobj) {
							object = newobj;
						} else {
							object = NULL;
							operation = -1;
						}
	
						break;
					}
				}
			}
		}

		/* OP_UPD is a defined operation, but not used in serials table - left unhandled here, too */
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
				/* don't do anything - this serial shouldn't be even asked for */
				break;

			case -1:
				SK_cd_puts(&condat, "%% INTERNAL ERROR. TERMINATING NRTM CONNECTION.\n");
				sprintf(buff, "%% SOURCE: %s; SERIAL: %d; PERMISSION: %d; IP: %s; TIME: %d\n\n", nrtm_q.source,
				        current_serial, mirror_perm, hostaddress, time(NULL));
				SK_cd_puts(&condat, buff);
				condat.rtc = SK_NOTEXT;
		}

		if (object)
			free(object);
		current_serial++;

		/* for real-time mirroring we need some piece of code */
		if (persistent_connection && (condat.rtc == 0)) {
			while (((nrtm_q.last = SQ_get_max_id(sql_connection, "serial_id", "serials")) < current_serial)
			        && (CO_get_do_server()==1))
				sleep(1);
		}

	} /* do while there are more serials, connection was not reset and XXX do_server is on*/
	while ((current_serial<=nrtm_q.last) && (condat.rtc == 0)&& (CO_get_do_server()==1));
	/*******************************************************************/

	sprintf(buff, "%%END %s\n\n\n", nrtm_q.source);
	SK_cd_puts(&condat, buff);
	SK_cd_close(&condat);

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
