/***************************************
  $Revision: 1.3 $

  Example code: main server code.

  Status: NOT REVUED, TESTED

  Author:       Chris Ottrey + pretty much everybody else involved :-)

  +html+ <DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL>
  +html+ </DL>

  ******************/ /******************
  Modification History:
        ottrey (09/03/1999) Created.
  ******************/ /******************
  Copyright (c) 1998                            RIPE NCC

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
#include <signal.h>
#include <unistd.h>

void segfault_handler(int sig) {
	// we print a nice message, including sig, to make things absolutely clear
	fprintf(stderr, " *** Signal %d caught, segfault handler starting\n", sig);
	die;
}

void install_signal_handler()
{
	struct sigaction sig, oldsig;
	sigset_t sset;

	memset(&sig, 0, sizeof(struct sigaction));
	sigemptyset(&sset);

	sig.sa_flags = 0;
	sig.sa_handler = SIG_DFL;

	/* change disposition for the signals to handler */
	/* this is to avoid signal non delivery in case signal is ignored by parent */

	/* SIGUSR1 will switch the updates on and off */
	sigaction(SIGUSR1, &sig, NULL);

	/* SIGINT and SIGTERM stop all servers by setting do_server to 0 */
	sigaction(SIGINT, &sig, NULL);
	sigaction(SIGTERM, &sig, NULL);

	/* SIGPIPE occurs at uncomfortable times on TCP sockets, so we ignore it.
	   In such a case we still get a -1 from the offending I/O operation, and
	   the errno of EPIPE. */
	sig.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sig, NULL);

	// set segfault handler
	memset(&sig, 0, sizeof(struct sigaction));
	memset(&oldsig, 0, sizeof(struct sigaction));
	sig.sa_handler = segfault_handler;
	sigaction(SIGSEGV, &sig, &oldsig);
	orig_segfault_handler = oldsig.sa_handler;
	sigaction(SIGABRT, &sig, &oldsig);

	/* now block the delivery of these signals in other threads */
	/* this will be inherited by other threads */
	sigemptyset(&sset);
	sigaddset(&sset, SIGTERM);
	sigaddset(&sset, SIGINT);
	sigaddset(&sset, SIGUSR1);
	sigaddset(&sset, SIGPIPE);
	pthread_sigmask(SIG_BLOCK, &sset, NULL);
}


static LG_context_t* sv_prepare_context(gchar* file, gchar* module) {
  LG_context_t* ctx;
  LG_appender_t* app;
  gchar log_line[100];

  ctx = LG_ctx_new();  //this is leaking, not serious
  app = LG_app_get_daily_info_dump(file);
  LG_ctx_add_appender(ctx, app);
  sprintf(log_line, "$TIMESTAMP whois_rip-$PID/$TID %s-$SEV-$SEVERITY $MESSAGE\n", module);
  app->formatter = LG_frm_general_prepared(log_line);

  return ctx;
}

static void sv_init_modules(void) {
//  LG_context_t* ctx;
//  LG_context_t* audit_ctx;
//  LG_appender_t* app;

  AC_init(sv_prepare_context(ca_get_ripaudit, "AC"));
  PC_init(sv_prepare_context(ca_get_ripmirlog, "PC"));
  PM_init(sv_prepare_context(ca_get_ripmirlog, "PM"));
  PW_init(sv_prepare_context(ca_get_ripaudit, "PW"),
          sv_prepare_context(ca_get_qrylog, "PW"));
  QC_init(sv_prepare_context(ca_get_allriperr, "QC"));
  QI_init(sv_prepare_context(ca_get_allriperr, "QI"),
          sv_prepare_context(ca_get_sqlog, "SQ"));
  RP_init(sv_prepare_context(ca_get_allriperr, "RP"));
  RX_init(sv_prepare_context(ca_get_allriperr, "RX"));
  SK_init(sv_prepare_context(ca_get_allriperr, "SK"));
  SQ_init(sv_prepare_context(ca_get_sqlog, "SQ"));
  SV_init(sv_prepare_context(ca_get_ripsvrlog, "SV"));
  UD_init(sv_prepare_context(ca_get_ripupdlog, "UD"));
}

int main(int argc, char **argv)
{
	extern char *optarg;
	char *prop_file_name;
	char *pid_file_name;
	char *result;
	int c, ret, errflg = 0;
	LG_context_t *boot_ctx;

	/* Initialize GLib library to be thread-safe */
	g_thread_init(NULL);

	/* parse command line options */
	prop_file_name = NULL;
	pid_file_name = NULL;
	if (argc < 5)
		errflg++;

	while ((c = getopt(argc, argv, "c:p:?")) != EOF)
		switch (c) {
		case 'c':
			prop_file_name = g_strdup(optarg);
			break;
		case 'p':
			pid_file_name = g_strdup(optarg);
			break;
		case '?':
		default:
			errflg++;
		}
	if (errflg || (prop_file_name == NULL) || (pid_file_name == NULL)) {
		fprintf(stderr, "usage: %s -c config -p pid_file\n", argv[0]);
		exit(2);
	}

	install_signal_handler();

	/* Create signal handling thread and block signals for others */
	/*   printf("Starting the signal handler\n"); */
	TH_create((void *(*)(void *))SV_signal_thread, NULL);

	/*  Set the constants. */
	/* fprintf(stderr,"Constants:\n");  */
	result = CO_set();
	UT_free(result);
	fprintf(stderr, "Configuration [%s]:\n", prop_file_name);
	boot_ctx = LG_ctx_new();	//this is leaking, not serious
	LG_ctx_add_appender(boot_ctx, LG_app_get_file_info_dump(stderr));
	UT_init(boot_ctx);
	ca_init(prop_file_name);
	g_free(prop_file_name);

	sv_init_modules();

	/*  Start the server */
	ret = SV_start(pid_file_name);

	if (ret != 0)
		return (1);
	else
		return (0);

}								/* main() */

