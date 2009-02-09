/***************************************
 $Revision: 1.1 $

 loader.c - core of the database loading.

 Status: NOT REVUED, TESTED

 ******************//******************
 Filename            : loader.c
 Authors             : Andrei Robachevsky
 OSs Tested          : Solaris 7,8
 ******************//******************
 Copyright (c) 2000                            RIPE NCC

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
#include "lg.h"

#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

void stop_updates() {
    char print_buf[STR_M];

    fprintf(stderr, "Updates interrupted..\n");
    sprintf(print_buf, "%d", 0);
    CO_set_const("UD.do_update", print_buf);
    return;
}

#if 0
void error_init(int argc, char ** argv) {
    char *slash;
    char progname[32];

    slash = strrchr(argv[0],'/');
    strncpy(progname, (slash != NULL) ? slash+1 : argv[0], 31);
    progname[31]=0;

    /* Initialize error module but do not process ER_DEF definitions from the config */
    ER_init(progname, 0);
#if 1
    /* ripupdlog: logs all update transactions */
    /* add one more definition */
    {
        char *err_msg = NULL;
        char *buf =
        "CREATE err_log { FORMAT SEVCHAR|FACSYMB|TEXTLONG|DATETIME SOCK 2 }"
        "( FAC ALL SEV E- )"
        "( FAC UD ASP 0xffffffff SEV I )"
        ;

        int parsres = ER_parse_spec(buf, &err_msg);

        if( parsres != 0 ) { /* print only on failure */
            puts(err_msg);
        }

        UT_free(err_msg);

        dieif( parsres != 0 );
    }
#endif

} /* error_init() */
#endif /* 0 */

/***********************************************
 ******* MAIN **********************************
 ***********************************************/

int main(int argc, char** argv) {
    int c;
    int fd;
    extern int optind;
    extern char *optarg;
    int errflg = 0;
    int dummy_allowed;
    int start_object;
    int num_ok, num_failed;
    long num_skip = 0;
    int load_pass = -1;
    UD_stream_t ud_stream;
    char *prop_file_name = NULL;
    char *source_name = NULL;
    ca_dbSource_t *source_hdl;
    char *db_host, *db_name, *db_user, *db_passwd;
    int db_port;
    char *co_result;
    LG_appender_t *app;
    LG_context_t *ctx;
    LG_context_t *stream_ctx;

    struct sigaction sig;

    num_ok = 0;
    num_failed = 0;
    dummy_allowed = 0;
    load_pass =

    start_object = 1;

    while ((c = getopt(argc, argv, "n:M:L:p:s:?")) != EOF)
        switch (c) {
            case 'n':
                num_skip = atol(optarg);
                break;
            case 'p':
                prop_file_name = optarg;
                break;
            case 'L':
                load_pass = atoi(optarg);
                dummy_allowed = 1;
                break;
            case 's':
                source_name = optarg;
                break;
            case '?':
            default:
                errflg++;
                break;
        }

    /* check that all parameters were specified */
    if (prop_file_name == NULL) {
        errflg++;
        fprintf(stderr, "configuration file is not specified (-p)\n");
    }
    if (load_pass == -1) {
        errflg++;
        fprintf(stderr, "load pass number is not specified (-L)\n");
    }
    if (source_name == NULL) {
        errflg++;
        fprintf(stderr, "source is not specified (-s)\n");
    }
    if (errflg) {
        fprintf(stderr, "usage: loader -L pass# [-n num_skip] -p properties -s source file\n");
        exit(2);
    }

    /* reach properly to SIGINT and SIGTERM */
    sig.sa_handler = stop_updates;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sig, NULL);
    sigaction(SIGTERM, &sig, NULL);

    co_result = CO_set();
    UT_free(co_result);

    /* set up logging */
    ctx = LG_ctx_new();
    app = LG_app_get_file_info_dump(stderr);
    LG_ctx_add_appender(ctx, app);
    RP_init(ctx);
    RX_init(ctx);
    SK_init(ctx);
    SQ_init(ctx);
    UD_init(ctx);
    UT_init(ctx);

    stream_ctx = LG_ctx_new();
    app = LG_app_get_file_info_dump(stdout);
    LG_ctx_add_appender(stream_ctx, app);

    /* 3a. Populate dictionary and load config */
    ca_init(prop_file_name);

    /* 4. initalise error system */
    /* XXX: from ER */
    /*  error_init(argc, argv);*/

    /* Zero the structure */
    memset(&ud_stream, 0, sizeof(ud_stream));

    /* set mode of operation: unprotected (dummy allowed), updates, standalone */
    ud_stream.ud_mode = (B_DUMMY | B_UPDATE | B_STANDALONE);

    /* get the source handle */
    source_hdl = ca_get_SourceHandleByName(source_name);

    /* if NO_NHR option was specified - this is to be done for foreign databases */
    if (IS_NO_NHR(ca_get_srcmode(source_hdl)))
        ud_stream.ud_mode |= B_NO_NHR;

    /* Connect to the database */
    db_host = ca_get_srcdbmachine(source_hdl);
    db_port = ca_get_srcdbport(source_hdl);
    db_name = ca_get_srcdbname(source_hdl);
    db_user = ca_get_srcdbuser(source_hdl);
    db_passwd = ca_get_srcdbpassword(source_hdl);

    fprintf(stderr, "D: Making SQL connection to %s@%s ...", db_name, db_host);

    /*  ud_stream.db_connection=SQ_get_connection2(); */
    ud_stream.db_connection = SQ_get_connection(db_host, db_port, db_name, db_user, db_passwd);

    if (!ud_stream.db_connection) {
        fprintf(stderr, "D: ERROR: no SQL connection\n");
        exit(1);
    }

    fprintf(stderr, "OK\n");

    ud_stream.nrtm = NULL;

    ud_stream.log.num_ok = 0;
    ud_stream.log.num_failed = 0;

    UT_free(db_host);
    UT_free(db_name);
    UT_free(db_user);
    UT_free(db_passwd);

    if (optind < argc)
        fd = open(argv[optind], O_RDONLY, 0666);
    else
        fd = 0;

    if (fd == -1) {
        fprintf(stderr, "Cannot open data stream. Exiting..\n");
        exit(1);
    }

    ud_stream.condat.sock = fd;
    ud_stream.num_skip = num_skip;
    ud_stream.load_pass = load_pass;

    /* load the dictionary */
    rpsl_load_dictionary(RPSL_DICT_CORE);

    /* Start to process the stream */

    fprintf(stderr, "starting processing stream\n");
    num_ok = UD_process_stream(&ud_stream, stream_ctx);
    fprintf(stderr, "processing stream finished\n");
    fprintf(stderr, "%d objects processed\n", num_ok);

    return (0);

} /* main() */

