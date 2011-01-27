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
#include "class.h"
#include "attribute.h"

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

/* CA_NRTM_HISTORY_ACCESS_LIMIT */
unsigned history_access_limit = 0;
gchar **PM_DUMMY_ADD_ATTR = NULL;
char *main_source = NULL;

/* hash to store placeholder object per class (used in nrtm v1 and v2 backward compatibility mode only)
 * key = classname (char *), value = placeholder object blob (char *)
 *
 * The dummy object would have a more convenient place in the rip.config file, however, that would mean that the object referenced
 * from dummified object cannot be queried for in the master database.
 *
 * TODO: Once NRTM v1+2 is dropped, we don't need to load this object into memory, so we can drop this cache too. Woohooo!
 *
 * FIXME: The RPSL implementation support a single RPSL schema definition only. This means that NRTM can dummify only
 * one source, and that is the main source (first UPDSOURCE). Hence, dummification should never be called for any other source.
 * agoston, 2010-04-20 */
GHashTable *placeholders = NULL;

/* Initializes variables to use in dummify_object().
 * Need to be executed before any calls to dummify_object() are made.
 *
 * agoston, 2009-08-11 */
static void dummify_init() {
    /* parse dummify config options
     * it is required during the full lifespan of the whoisserver process, so there is no mechanism to
     * free() for the structures allocated here - agoston, 2007-10-31 */
    char *dummy_add_attr = ca_get_dummy_add_attr;
    history_access_limit = ca_get_nrtm_history_access_limit;

    PM_DUMMY_ADD_ATTR = g_strsplit(dummy_add_attr, "\n", 0);
    free(dummy_add_attr);

    /* Load all placeholder objects for quick returning in backwards compatibility (nrtm v1, v2 mode)
     no free() hooks defined, as all values are coming directly, without making a copy */
    placeholders = g_hash_table_new_full(g_int_hash, g_int_equal, NULL, NULL);

    /* get DB connection - only main (first UPDSOURCE) DB is supported, see FIXME above */
    ca_updDbSource_t **upd_source_hdl;
    upd_source_hdl = ca_get_UpdSourceHandle(CA_UPDSOURCE);
    if (upd_source_hdl[0] == NULL) {
        LG_log(pm_context, LG_FATAL,"dummify_init(): There must be at least one updsource defined in the config file");
        die;
    }

    main_source =  upd_source_hdl[0]->name;
    SQ_connection_t *db_connection = SQ_get_connection_by_source_name(main_source);

    int i;
    char **class_names = DF_get_class_names();
    for (i = 0; class_names[i]; i++) {
        const class_t *classinfo = class_lookup(class_names[i]);
        if (classinfo->dummify_type == DUMMIFY_PLACEHOLDER) {
            /* get the singleton from the DB (for the major DB only, see above) */
            char query[STR_M];
            SQ_result_set_t *sql_result;
            SQ_row_t *sql_row;
            int sql_err;

            /* FIXME: person and role share the exact same nic-hdl repo, and are interchangable, but there is no metadata or interface
             * to get this info. So now I'll just hack this info here, but should we do a major schema change that touches this area,
             * and/or such metadata/api would become available, that should be introduced here - agoston, 2010-04-21 */
            if (!strcmp(classinfo->name, "person") || !strcmp(classinfo->name, "role")) {
                sprintf(query, "SELECT object FROM last WHERE pkey = '%s' and (object_type = 10 or object_type = 11)", classinfo->dummify_singleton);
            } else {
                sprintf(query, "SELECT object FROM last WHERE pkey = '%s' and object_type = %d", classinfo->dummify_singleton, classinfo->id);
            }
            sql_err = SQ_execute_query(db_connection, query, &sql_result);

            if (sql_err) {
                LG_log(pm_context, LG_SEVERE, "%s [%s]", SQ_error(db_connection), query);
                fprintf(stderr, "%s [%s]", SQ_error(db_connection), query);
                die;
            }

            if ((sql_row = SQ_row_next(sql_result)) != NULL) {
                char *object_blob = SQ_get_column_string(sql_result, sql_row, 0);
                g_hash_table_insert(placeholders, (gpointer)&(classinfo->id), object_blob);
            } else {
                fprintf(stderr, "Placeholder object for class %s, %s, was not found in source %s (meaning, the following query gave no results: %s)\n",
                        class_names[i], classinfo->dummify_singleton, main_source, query);
                /* die only if main source name can be found at the end of the dummy object
                 * so whois-server wouldn't whine for a missing placeholder while testing/etc... */
                if (g_str_has_suffix(classinfo->dummify_singleton, main_source)) {
                    die;
                } else {
                    fprintf(stderr, " *** Ignoring above error for source %s\n", main_source);
                }
            }

            if (sql_result) {
                SQ_free_result(sql_result);
                sql_result=NULL;
            }
        }
    }

    SQ_close_connection(db_connection);
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
	for (opt_argc=0; opt_argv[opt_argc]; opt_argc++);

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

/* Internal helper function to delete attributes from an rpsl object during dummification.
 *
 * Takes: rpsl object
 *        rpsl attribute
 *        GList pointer to current attribute in attribute list*/
void pm_dummify_delete_attr(rpsl_object_t *obj, rpsl_attr_t *act_attr, GList **gli) {
    /* We use the _internal call as the normal one does some extra, and, in this case, surplus
     * checking.
     *
     * During call, the only possible error is trying to remove a mandatory attribute - but since
     * we already checked that, we don't care about errors here. - agoston, 2007-10-29 */
    *gli = (*gli)->prev;
    rpsl_attr_delete(rpsl_object_remove_attr_internal(obj, act_attr->num, NULL));
}

/* Replaces a placeholder rpsl attribute with its proper value (or removes if optional)
 * Behavior can be configured in the xml files in src/defs/
 *
 * Takes: rpsl object
 *        rpsl attribute
 *        GList pointer to current attribute in attribute list
 *
 * Returns:
 *      0 if no replacement was done
 *      1 if attribute was replaced with placeholder value or was deleted */
int pm_dummify_replace_placeholder_attribute(rpsl_object_t *obj, rpsl_attr_t *act_attr, GList **gli)
{
    const class_t *actclass;
    /* get the attribute info - this is bad, but it also doesn't make much sense to include
     * dummification info into librpsl */
    attribute_t *attrinfo = (attribute_t *) act_attr->attr_info;

    if (!attrinfo)
    {
        /* this happens if there was an rpsl error parsing this attribute, for example, the attribute was
         * deprecated :) */
        return 0;
    }

    /* first check if it's a foreign key AND the foreign object type is placeholder */
    if (attrinfo->foreignkey_class_offset >= 0 &&
        (actclass = class_lookup_id(attrinfo->foreignkey_class_offset))->dummify_type == DUMMIFY_PLACEHOLDER)
    {
        /* if mandatory attribute, replace with placeholder; if not, remove */
        if (rpsl_attr_is_required(obj, act_attr->lcase_name))
        {
            const char *placeholder = actclass->dummify_singleton;
            rpsl_attr_replace_value(act_attr, placeholder);
        }
        else
        {
            pm_dummify_delete_attr(obj, act_attr, gli);
        }
        return 1;
    }

    return 0;
}

/* Replaces an rpsl attribute with its filtered counterpart (or removes if optional)
 * Behavior can be configured in the xml files in src/defs/
 *
 * Takes: rpsl object
 *        rpsl attribute
 *        primary attribute value (aka key) of the rpsl object (for printf()ing)
 *        GList pointer to current attribute in attribute list
 *
 * Returns: 0 if no replacement was done
 *          1 if attribute was filtered or deleted */
int pm_dummify_replace_filtered_attribute(rpsl_object_t *obj, rpsl_attr_t *act_attr, char *prim_val, GList **gli)
{
    /* get the attribute info - this is bad, but it also doesn't make much sense to include
     * dummification info into librpsl */
    attribute_t *attrinfo = (attribute_t *) act_attr->attr_info;

    if (!attrinfo)
    {
        /* this happens if there was an rpsl error parsing this attribute, for example, the attribute was
         * deprecated :) */
        return 0;
    }

    /* check if this attribute is marked for filtering */
    if (rpsl_attr_is_required(obj, act_attr->lcase_name))
    {
        if (attrinfo->dummify)
        {
            char buf[STR_L];
            snprintf(buf, STR_L, attrinfo->dummify, prim_val);
            rpsl_attr_replace_value(act_attr, buf);
            return 1;
        }
    }
    else
    {
        pm_dummify_delete_attr(obj, act_attr, gli);
        return 1;
    }
    return 0;
}

/* adds the text defined as DUMMY_ADD_ATTR in rip.config to the rpsl object as remarks
 *
 * Takes: rpsl object
 *
 * Returns: NULL on OK
 *          rpsl_error_t* on error (should be freed by caller using rpsl_error_free()) */
rpsl_error_t *pm_dummify_add_attr(rpsl_object_t *obj)
{
    rpsl_error_t error;
    char **actline;

    for (actline = PM_DUMMY_ADD_ATTR; *actline && **actline; actline++)
    {
        rpsl_attr_t *newattr;
        char buf[STR_L];

        snprintf(buf, STR_L, "remarks: %s", *actline);
        newattr = rpsl_attr_init(buf, NULL);
        if (!rpsl_object_append_attr(obj, newattr, &error))
        {
            /* we make a copy, as this should never happen and we want to avoid
             * surplus heap allocation in often-called function */
            rpsl_error_t *retval = malloc(sizeof (rpsl_error_t));
            memcpy(retval, &error, sizeof (rpsl_error_t));
            return retval;
        }
    }
    return NULL;
}

/* PUBLIC NRTM STREAM DESIGN
 *
 * The RIPE DB contains quite a bit of personal data. So far, NRTM access gave full access to all private data, and all
 * history.  DP-TF found this is not acceptable.  We had to limit access to private data and history.
 *
 * Limiting history is easier: we simply look up if the beginning of the NRTM requested range has been added within a
 * limited amount of time (NRTM_HISTORY_ACCESS_LIMIT in rip.config).  If not, we simply refuse the NRTM request.
 *
 * Limiting personal data is a lot more complicated.  Ideally, we only want to drop data fields, and keep
 * primary/foreign keys like mntner or nic-hdl.  But since even the keys identifying personal data are considered
 * personal, we have to have 3 levels of dummification:
 *
 * 1. Remove object type
 *
 * In this case, we remove all occurances of an object type, and replace all references to it with a dummy placeholder
 * object (thus, we keep referential integrity).
 *
 * You can set this in attributes.xml, element foreignkey and classes.xml, element dummify/placeholder.
 *
 * 2. Dummify object
 *
 * In this case, we keep all objects of an object type, but we replace all mandatory, non-key attributes with a
 * dummified string, while dropping all optional attributes.  This is similar in result to #1, but end user retains the
 * possibility to look up the dummified object in the RIPE DB.
 *
 * You can set this in classes.xml, element dummify/filter, and attributes.xml, element dummify.
 *
 * 3. Leave alone
 *
 * These objects themselves are not to be changed, they don't contain any data that should be masked.  However, they
 * might still contain reference to such data (like tech-c or admin-c attributes that contain nic-hdls).  As such,
 * these objects have to be scanned and the references replaced to point to the placeholder object.
 *
 * We do the dummification in a generic way. For each object, we first parse it using the RPSL module. This hurts
 * performance big time, as it does a lot of surplus processing and error checking, however, we are more flexible about
 * future changes, handling outdated/erronous objects in the database, and also, we handle corner cases like line
 * continuation out of the box, error-free.  Side note: the RPSL library is somewhat dated, so don't be surprised on
 * some quirks in the code below.
 *
 * Using the RPSL module also means that objects which have been stuffed into the database earlier, not passing the
 * current syntax rules will produce an error.  On such rpsl parser errors, we return NULL, which marks that the object
 * could not be dummified.  These cases must be handled by the caller; for example, the NRTM server will skip the
 * object.  This will normally not be a problem as we also don't want to give object history through public NRTM
 * stream.  Any attempt to try to go back more than two weeks into the past should give an error.
 *
 * After having the processed object structure, we check the object class dummify settings, and:
 * - if placeholder, return NULL;
 * - if filter, iterate through the attributes
 *    - remove all non-mandatory attributes
 *    - replace all attributes marked as filter with attribute's filter settings (if both dummify and foreignkey has
 *          been defined for an attribute in attributes.xml, dummification takes precedence)
 *    - replace all attributes that have a foreign key set pointing to a placeholder object with the placeholder value
 *    - return;
 * - if neither, iterate through the attributes
 *    - replace all attributes that have a foreign key set pointing to a placeholder object with the placeholder value
 *    - return;
 *
 * After finishing the dummification, we return the flat object for the nrtm server to send to the client or NULL
 * on dummification error or when we don't want to return anything.
 *
 * There is also option to add a message to objects that have been filtered. See DUMMY_ADD_ATTR in rip.config
 * for details.
 *
 * Note that dummify_init() must be called before any calls are made to dummify_object().
 *
 * agoston, 2010-04-19
 * */
char *PM_dummify_object(char *object)
{
    const GList *errors = NULL;
    gboolean quit = FALSE;
    rpsl_object_t *obj = NULL;
    GList *gli = NULL;
    gchar *prim_val = NULL;
    char *ret = NULL;
    GHashTable *dummified_attribs = NULL;
    class_t *classinfo = NULL;
    rpsl_error_t *error;

    /* parse the object */
    obj = rpsl_object_init(object);
    errors = rpsl_object_errors(obj);

    for (; errors; errors = errors->next)
    {
        rpsl_error_t *acterr = (rpsl_error_t *) errors->data;
        if (acterr->level >= RPSL_ERRLVL_ERROR)
        {
            LG_log(pm_context, LG_ERROR, "rpsl_object_init failed: %s (at attribute %d)", acterr->descr, acterr->attr_num);
            quit = TRUE;
        }
    }

    /* if there *was* an rpsl error, but not major (meaning the data structures are still filled),
     * let's continue running.
     * if there's a serious error, we skip this object */
    if (quit && !(obj && obj->attributes && obj->attributes->data))
    {
        goto dummify_abort;
    }
    else
    {
        quit = FALSE;
    }

    /* get the class info - this is bad, but the only quick way to get the classinfo structure */
    classinfo = (class_t *) obj->class_info;
    if (classinfo->dummify_type == DUMMIFY_PLACEHOLDER)
    {
        /* we skip it and let the caller take care of proper action toward the client */
        goto dummify_abort;
    }

    /* init hash containing the already dummified attribs (so we don't have
     * hundreds of the same changed: lines, for example */
    dummified_attribs = g_hash_table_new_full(g_str_hash, g_str_equal, free, NULL);

    /* the primary key is not necessarily the first attribute of the object, e.g. person */
    prim_val = rpsl_object_get_key_value(obj);

    /* iterate through attributes
     * OPTME: this could be speeded up considerably if we wouldn't do metadata examination for every
     * dummified object, but on whois-server startup store what decision to make on each attribute of each class,
     * and simply execute that previously made decision here - agoston, 2009-08-25
     */
    for (gli = g_list_first(obj->attributes); gli; gli = g_list_next(gli))
    {
        rpsl_attr_t *act_attr = (rpsl_attr_t *) gli->data;

        /* MAIN DUMMIFICATION ALGORITHM */
        if (g_hash_table_lookup(dummified_attribs, act_attr->lcase_name))
        {
            /* if we already replaced this attribute with its placeholder value, then remove */
            pm_dummify_delete_attr(obj, act_attr, &gli);
        }
        else
        {
            /* make a copy of the attribute name, as the attribute could end up deleted during the dummification
             * process, and then we would have nothing to use as a key to the dummified_attribs hash */
            gchar *attr_name = strdup(act_attr->lcase_name);

            if (pm_dummify_replace_placeholder_attribute(obj, act_attr, &gli))
            {
                /* if we replaced the attribute with placeholder, mark it in the dummified attributes hash,
                 * so we will remove any more occurences of this attribute in the future */
                g_hash_table_insert(dummified_attribs, attr_name, attr_name);
            }
            else if (classinfo->dummify_type == DUMMIFY_FILTER)
            {
                if (pm_dummify_replace_filtered_attribute(obj, act_attr, prim_val, &gli))
                {
                    /* if we dummified an attribute, mark it in the dummified attributes hash,
                     * so we will remove any more occurences of this attribute in the future */
                    g_hash_table_insert(dummified_attribs, attr_name, attr_name);
                }
                else
                {
                    free(attr_name);
                }
            }
            else
            {
                free(attr_name);
            }
        }
    }

    /* append the additional attributes */
    if ((error = pm_dummify_add_attr(obj)))
    {
        fprintf(stderr, "Error in dummify_object()::rpsl_object_append_attr(): %s\n", error->descr);
        LG_log(pm_context, LG_FATAL, "Error in dummify_object()::rpsl_object_append_attr(): %s", error->descr);
        rpsl_error_free(error);
        free(error);
        goto dummify_abort;
    }

    ret = rpsl_object_get_text(obj, RPSL_STD_COLUMN);

dummify_abort:

    if (dummified_attribs)
        g_hash_table_destroy(dummified_attribs);

    if (prim_val)
        free(prim_val);

    if (obj)
        rpsl_object_delete(obj);

    return ret;
}

/* Interact with the client.

    NRTM is a very simple protocol - the client requests a range of serials, to which the server responds with the
    corresponding amount of changes in the RIPE Database, each prepended with 'ADD' or 'DEL'.

    When we introduced massive filtering of NRTM data, we introduced object types that are never to be sent to the
    public.  These can be adjusted in the schema description located in src/defs.  At the moment, these are person and
    role.  All references to these objects have been replaced with a placeholder object (DUMY-RIPE), and the object
    themselves are never sent to the public.

    But this new implementation conflicted with NRTM version 2. Since this old version of the NRTM stream does not
    include the serials, a client asking for persistent connection would have no idea which serials the server passed
    to it, rendering NRTM v2 unusable.  To circumvent this, we implemented a backwards compatibility that would replace
    all person and role objects with an 'update' against DUMY-RIPE.  This will help keeping the serials in sync, and
    gives time to end users to switch to NRTM v3.

    NRTM v3 adds a serial ID after the operation ('ADD' or 'DEL'). This way, an NRTM client will know if there are gaps
    in the stream, and has the correct serials in its own database which it can base future nrtm requests on.
    
    Users are encouraged to exclusively use NRTM v3.
    
    Note that there is also a very old version 1 supported still, but it is considered to be exactly the same as
    version 2 in this code.
    
    agoston, 2010-04-19

Arguments:
    int sock		Socket that client is connected to
*/
void PM_interact(svr_args *args) {
    char input[MAX_PM_INPUT_SIZE + 1];
    char buff[STR_L];
    ca_dbSource_t *source_hdl;
    int read_result;
    int parse_result;

    sk_conn_st condat;
    nrtm_q_t nrtm_q;
    long current_serial;
    long oldest_serial;
    long object_type = -1;
    unsigned timestamp;
    aa_mirror_right mirror_perm;
    int check_history_limit = 1;
    char *object;
    int operation;
    GString *gbuff;
    SQ_connection_t *sql_connection;
    int persistent_connection;

    /* make a record for thread accounting */
    TA_add(args->conn_sock, "nrtm_srv");

    /* initialise the nrtm structure */
    memset(&nrtm_q, 0, sizeof (nrtm_q_t));

    /* init the connection structure, set timeout for reading the query */
    SK_cd_make(&condat, args->conn_sock, 0);

    /* Read input */
    read_result = SK_cd_gets(&condat, input, MAX_PM_INPUT_SIZE);

    parse_result = parse_request(input, &nrtm_q);

    if (parse_result < 0)
    {
        LG_log(pm_context, LG_DEBUG, "[%s] -- Garbage received: %s", condat.rIPs, input);
        sprintf(buff, "\n%%ERROR:405: syntax error\n\n\n");
        SK_cd_puts(&condat, buff);
        goto error_return;
    }

    LG_log(pm_context, LG_DEBUG, "[%s] -- input: [%s]", condat.rIPs, input);

    /* this is -q sources query  - answer and return */
    if (IS_Q_QUERY(parse_result))
    {

        gbuff = PM_get_nrtm_sources(&(condat.rIP), nrtm_q.source);
        SK_cd_puts(&condat, gbuff->str);
        /* end-of-result one extra line (2 in total) */
        SK_cd_puts(&condat, "\n");
        /* Free allocated memory  */
        g_string_free(gbuff, TRUE);
        goto error_return;
    }
    else if (IS_G_QUERY(parse_result))
    {
        if (IS_PERSISTENT(parse_result))
            persistent_connection = 1;
        else
            persistent_connection = 0;
    }
    else
    {
        LG_log(pm_context, LG_DEBUG, "[%s] -- Syntax error: %s", condat.rIPs, input);
        sprintf(buff, "\n%%ERROR:405: syntax error\n\n\n");
        SK_cd_puts(&condat, buff);
        goto error_return;
    }

    /* otherwise this is -g query */

    LG_log(pm_context, LG_DEBUG, "[%s] -- input parsed: %s:%d:%ld-%ld", condat.rIPs, nrtm_q.source, nrtm_q.version, nrtm_q.first, nrtm_q.last);

    /* check if source exists */
    source_hdl = ca_get_SourceHandleByName(nrtm_q.source);
    if (source_hdl == NULL)
    {
        LG_log(pm_context, LG_DEBUG, "[%s] --  Unknown source %s", condat.rIPs, nrtm_q.source);
        sprintf(buff, "\n%%ERROR:403: unknown source %s\n\n\n", nrtm_q.source);
        SK_cd_puts(&condat, buff);
        goto error_return;
    }

    /* check if the client is authorized to mirror */
    if (!(mirror_perm = AA_can_mirror(&(condat.rIP), nrtm_q.source)))
    {
        LG_log(pm_context, LG_DEBUG, "[%s] --  Not authorized to mirror the source %s", condat.rIPs, nrtm_q.source);
        sprintf(buff, "\n%%ERROR:402: not authorized to mirror the database\n\n\n");
        SK_cd_puts(&condat, buff);
        goto error_return;
    }

    /* Check if requested dummification of non-main database
     * See FIXME of variable placeholders for further details */
    if ((mirror_perm == AA_MIRROR_PUBLIC) && strcmp(main_source, nrtm_q.source)) {
        LG_log(pm_context, LG_DEBUG, "[%s] --  Dummified mirroring of source %s is not supported (try source %s)", condat.rIPs, nrtm_q.source, main_source);
        sprintf(buff, "\n%%ERROR:404: Dummified mirroring of source %s is not supported (try source %s)\n\n\n", nrtm_q.source, main_source);
        SK_cd_puts(&condat, buff);
        goto error_return;
    }

    /* check if requested nrtm version is supported */
    if (nrtm_q.version < 1 || nrtm_q.version > 3)
    {
        LG_log(pm_context, LG_DEBUG, "[%s] -- NRTM version mismatch: %s", condat.rIPs, input);
        sprintf(buff, "\n%%ERROR:406: NRTM version mismatch\n\n\n");
        SK_cd_puts(&condat, buff);
        goto error_return;
    }
    
    /* check for deprecated NRTM version */
    if (nrtm_q.version < 3)
    {
        sprintf(buff, "%%WARNING: NRTM version %d is deprecated, please consider migrating to version 3!\n\n", nrtm_q.version);
        SK_cd_puts(&condat, buff);
    }

    sql_connection = SQ_get_connection_by_source_hdl(source_hdl);

    PM_get_minmax_serial(sql_connection, &oldest_serial, &current_serial);
    if ((current_serial == -1) || (oldest_serial == -1))
    {
        LG_log(pm_context, LG_ERROR, " source='%s' [%d] %s", nrtm_q.source, SQ_errno(sql_connection), SQ_error(sql_connection));
        SQ_close_connection(sql_connection);
        goto error_return;
    }

    /* zero indicates that LAST keyword has been used */
    if (nrtm_q.last == 0)
        nrtm_q.last = current_serial;
    /* for persistent connections end of range has no meaning */
    if (persistent_connection)
        nrtm_q.last = current_serial;

    if ((nrtm_q.first > nrtm_q.last) || (nrtm_q.first < oldest_serial) || (nrtm_q.last > current_serial) || (nrtm_q.first <= 0) || (nrtm_q.last <= 0)) {
        LG_log(pm_context, LG_DEBUG, "[%s] --  Invalid range: %ld-%ld [%ld-%ld]", condat.rIPs, nrtm_q.first,
               nrtm_q.last, oldest_serial, current_serial);
        /* write error message back to the client */
        sprintf(buff, "\n%%ERROR:401: invalid range: Not within %ld-%ld\n\n\n", oldest_serial, current_serial);
        SK_cd_puts(&condat, buff);

        /* close the connection to SQL server */
        SQ_close_connection(sql_connection);
        goto error_return;
    }

    current_serial = nrtm_q.first;

    /* print banner */
    {
        /* get the header string */
        char *resp_header = ca_get_pw_resp_header;
        SK_cd_puts(&condat, "\n");
        SK_cd_puts(&condat, resp_header);
        UT_free(resp_header);
        SK_cd_puts(&condat, "\n");
    }

    sprintf(buff, "%%START Version: %d %s %ld-%ld%s\n\n", nrtm_q.version, nrtm_q.source, nrtm_q.first, nrtm_q.last, (mirror_perm < AA_MIRROR_FULL) ? " FILTERED" : "");
    SK_cd_puts(&condat, buff);

    /* make a record for thread accounting */
    TA_setactivity(buff);

    /* FIXME: This is the most basic solution possible. There's a lot of room for optimization.
     * If we would encounter performance problems, this is the entry point for nrtm optimizations.
     * agoston, 2007-11-15 */
    /*************************** MAIN LOOP ****************************/
    do
    {

        /* get the object corresponding to current serial */
        object = PM_get_serial_object(sql_connection, current_serial, &object_type, &timestamp, &operation);

        /* if we can't get it for any reason (error, missing), we skip this serial */
        if (object != NULL)
        {

            /* filter private object types if it is not authorized */
            if (mirror_perm == AA_MIRROR_PUBLIC)
            {
                /* check access history limit only if we actually been able to get a timestamp from the DB
                 * for DBs filled by NRTM, timestamp of delete operation is lost due to a bug, check
                 * PM_get_serial_object() for more info
                 * agoston, 2008-01-30 */
                if (check_history_limit && timestamp)
                {
                    /* check if timestamp is within the limits set in rip.config */
                    if (history_access_limit && (time(NULL) - timestamp > history_access_limit))
                    {
                        sprintf(buff, "%% Your request has been denied to protect private data.\n"
                                "%% (Requesting serials older than %d days will be rejected)\n", history_access_limit / 86400);
                        SK_cd_puts(&condat, buff);
                        free(object);
                        break;
                    }
                    else
                    {
                        /* we don't want to let user start the nrtm stream, and then, at some point (because of a buggy
                         * timestamp for example) deny him a few of the objects, making the slave database completely
                         * out of sync. The decision to (dis)allow the user should be consistent throughout the nrtm
                         * connection.
                         * agoston, 2008-01-21 */
                        check_history_limit = 0;
                    }
                }

                /* check for invalid object type. Normally, this should never pass, but for some reason,
                 * in the practice it did.
                 * My suspicion is that mysql 4.0.24 on cello doesn't always do what it was supposed to, but it's hard to prove :(
                 * agoston, 2009-06-02 */
                if (object_type < 0)
                {
                    fprintf(stderr, "object_type < 0 for %ld\n", current_serial);
                    LG_log(pm_context, LG_ERROR, "object_type < 0 for %ld", current_serial);
                }

                /* dummify private objects */
                char *newobj;
                newobj = PM_dummify_object(object);
                free(object);

                if (newobj)
                {
                    object = newobj;
                }
                else
                {
                    object = NULL;
                    operation = -1;
                }
            }

            /* OP_UPD is a defined operation, but not used in serials table - left unhandled here, too.
             *
             * NRTM versions have slighly different formats. v1 and v2 look exactly the same, but (according
             * to a comment in whois source code) updates are atomic (whatever that means). v3 has the serial id
             * appended to the operation. See design notes in the function header comment. */
            switch (operation)
            {
            case OP_ADD:
                switch (nrtm_q.version)
                {
                case 3:
                    sprintf(buff, "ADD %ld\n\n", current_serial);
                    break;

                default:
                    sprintf(buff, "ADD\n\n");
                    break;
                }
                SK_cd_puts(&condat, buff);
                SK_cd_puts(&condat, object);
                SK_cd_puts(&condat, "\n");
                break;

            case OP_DEL:
                switch (nrtm_q.version)
                {
                case 3:
                    sprintf(buff, "DEL %ld\n\n", current_serial);
                    break;

                default:
                    sprintf(buff, "DEL\n\n");
                    break;
                }
                SK_cd_puts(&condat, buff);
                SK_cd_puts(&condat, object);
                SK_cd_puts(&condat, "\n");
                break;

            default:
                /* in this case, don't send back anything and skip serial.
                 * the possible reasons are: rpsl error, missing object or invalid operation */

                /* compatibility mode for NRTM v1 and v2: we send back the placeholder object as an ADD,
                 * to avoid holes in the serials - agoston, 2010-04-21 */
                if (nrtm_q.version < 3) {
                    int id = (int)object_type;
                    char *object = (char *)g_hash_table_lookup(placeholders, &id);
                    if (object) {
                        sprintf(buff, "ADD\n\n");
                        SK_cd_puts(&condat, buff);
                        SK_cd_puts(&condat, object);
                        SK_cd_puts(&condat, "\n");
                    }
                }
                break;
            }

            if (object) free(object);
        }

        current_serial++;

        /* for real-time mirroring we need some piece of code */
        if (persistent_connection && (condat.rtc == 0))
        {
            while (((nrtm_q.last = SQ_get_max_id(sql_connection, "serial_id", "serials")) < current_serial)
                && (CO_get_do_server() == 1))
                sleep(2);
        }

    } while ((current_serial <= nrtm_q.last) && (condat.rtc == 0) && (CO_get_do_server() == 1));

    sprintf(buff, "%%END %s\n\n\n", nrtm_q.source);
    SK_cd_puts(&condat, buff);

    LG_log(pm_context, LG_INFO, "[%s] -- <%s:%d:%ld-%ld (%ld)> ", condat.rIPs, nrtm_q.source, nrtm_q.version, nrtm_q.first, nrtm_q.last, nrtm_q.last - nrtm_q.first + 1);

    /* make a record for thread accounting */
    TA_delete();

    /* close the connection to SQL server */
    SQ_close_connection(sql_connection);

error_return:
    SK_cd_free(&condat);
    UT_free(nrtm_q.source);
}
