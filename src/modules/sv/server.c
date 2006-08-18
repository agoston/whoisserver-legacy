/***************************************
  $Revision: 1.11 $

  Example code: A server for a client to connect to.

  Status: NOT REVUED, NOT TESTED

  Authors:       Chris Ottrey, Joao Damas,
                 heavy rewrite by Andrei Robachevsky, Marek Bukowy


  ******************/ /******************
  Modification History:
        ottrey (02/03/1999) Created.
        ottrey (08/03/1999) Modified.
        joao   (22/06/1999) Modified.
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

#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/poll.h>
#include <pwd.h>
#include <sys/prctl.h>

/* Listening sockets */
int SV_whois_sock;
int SV_config_sock;
int SV_mirror_sock;

/* each updatable source has its own update thread and its own socket */
#define MAX_SOURCES 100
int SV_update_sock[MAX_SOURCES];

/* when the process shuts down, it writes to this file descriptor */
int SV_shutdown_send_fd = -1;

/* server threads should check this file descriptor, and terminate
   as soon as possible if data becomes available on it */
int SV_shutdown_recv_fd = -1;

/*+ Server starting time +*/
time_t SV_starttime;
/* the filename where we store the PID of the server */
char *SV_pidfile;

static LG_context_t *sv_context;

void
SV_init (LG_context_t *ctx)
{
  sv_context = ctx;
}

/* Logging results */
static void log_print(const char *arg) {
  fprintf(stderr,arg);
} /* log_print() */


/* counters - by marek */
typedef struct {
  int count;
  pthread_mutex_t lock; /*+ Mutex lock.Used for synchronizing changes.+*/
  pthread_cond_t  cond; /*+ condition variable +*/
} svr_counter_t;


/* structure passed to every running server */
typedef struct {
  void (*function)(int);
  int conn_sock;
  ip_addr_t *conn_ip;			/* ip of the actual client */
  int accept_sock;
  int limit;         /* limit for the number of concurrent connections */
  svr_counter_t *counter; /* number of active clients */
  GHashTable *conn_ipnum;		/* how many times each IP has connected */
  pthread_mutex_t *conn_lock;	/* for synchronizing hashtable accesses */
  char *name;
} svr_args;


/*++++++++++++++++++++++++++++++++++++++
  function to operate on the counter structures -
  takes the increment (can be negative), changes the value
  using the locks and everything,

  int
  counter_add            returns the new value.

  svr_counter_t *cst     counter structure

  int incval             increment value (can be negative)

  Author:
    marek
  ++++++++++++++++++++++++++++++++++++++*/
static
int
counter_add( svr_counter_t *cst, int incval )
{
    int newval;

    /* add under mutex */
    pthread_mutex_lock( &(cst->lock) );
    cst->count += incval;
    newval = cst->count;
    pthread_mutex_unlock(&(cst->lock) );

    /* now - signal the change of value to the waiting thread */
    pthread_cond_signal( &(cst->cond) );

    return newval;
}


/*++++++++++++++++++++++++++++++++++++++

  int
  counter_state         returns the current value of a counter

  svr_counter_t *cst    counter

  Author:
    marek

  ++++++++++++++++++++++++++++++++++++++*/
static
int
counter_state( svr_counter_t *cst )
{
  return counter_add( cst, 0 );
}


/*++++++++++++++++++++++++++++++++++++++
  waits until the counter is in the range [0-limit].
  unless the limit is 0, in which case the check is disabled.

  int counter_wait      returns the new value of the counter after wait

  svr_counter_t *cst    counter

  int limit             limit / range, or 0 to disable the check

  Author:
    marek
  ++++++++++++++++++++++++++++++++++++++*/
static
int counter_wait(svr_counter_t *cst, int limit )
{
  int newval;

  pthread_mutex_lock( &(cst->lock) );

  if( limit != 0 ) {
    while( cst->count >= limit ) {
      pthread_cond_wait( &(cst->cond), &(cst->lock));
    }
  }

  newval = cst->count;
  pthread_mutex_unlock(&(cst->lock) );

  return newval;
}

/*++++++++++++++++++++++++++++++++++++++

  Initialize the radix tree - all trees are added to the forest in
  a locked state.  As each tree finishes loading (from radix_load()),
  it is unlocked.

  ++++++++++++++++++++++++++++++++++++++*/
static void
radix_init(void)
{
    int i;
    ca_dbSource_t *source_hdl;

    TH_init_read_write_lockw(&rx_forest_rwlock);
    for(i=0; (source_hdl = ca_get_SourceHandleByPosition(i))!=NULL ; i++){
        dieif( RP_init_trees( source_hdl ) != RP_OK );
    }
}

/*++++++++++++++++++++++++++++++++++++++

  Load the radix tree - all trees are initialized from the appropriate
  SQL tables.  As each tree finishes loading (from radix_load()), it is
  unlocked.  They must be created by the radix_init() function.

  ++++++++++++++++++++++++++++++++++++++*/
static void
radix_load(void)
{
    int i;
    ca_dbSource_t *source_hdl;

    for(i=0; (source_hdl = ca_get_SourceHandleByPosition(i))!=NULL ; i++){
        dieif( RP_sql_load_reg( source_hdl ) != RP_OK );
    }
}


/************************************************************
*  int SV_sleep()                                           *
*                                                           *
* sleeps till shutdown request comes                        *
* but at most <delay> seconds                               *
*                                                           *
* Returns:                                                  *
* 0 - timeout                                               *
* 1 - shutdown                                              *
*                                                           *
************************************************************/

int SV_sleep(int delay)
{
 int do_server;
 int elapsed_time=0;

 while((do_server=CO_get_do_server()) && (elapsed_time<delay))
 {
  sleep(TIME_SLICE);
  elapsed_time+=TIME_SLICE;
 }
 if(do_server)return(0); else return(1);
}

/*++++++++++++++++++++++++++++++++++++++

  Handle signals.

  Changes the flags:
  	do_nrtm
  	do_update
  	do_whoisd

  More:
  +html+ <PRE>
  Author:
        andrei
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
void *SV_signal_thread() {
char print_buf[STR_M];
sigset_t sset;
int sigReceived;
int do_update;

	sigemptyset(&sset);
	/* SIGTERM and SIGINT are used to shutdown the server */
	/* SIGUSR1 is used to pause/resume updates - rarely used as we have PC command */
	sigaddset(&sset, SIGTERM);
	sigaddset(&sset, SIGINT);
	sigaddset(&sset, SIGUSR1);
	/* This is a bit confusing, but is needed */
	/* For more information on signal handling in */
	/* threads see for example "Multithreading Programming */
	/* Techniques" by Shashi Prasad, ISBN 0-07-912250-7, pp. 94-101 */

	/* XXX If one needs to handle more signals, don't forget to */
	/* block them in other threads in install_signal_handler() in whois_rip.c */
	pthread_sigmask(SIG_BLOCK, &sset, NULL);

	for(;;)
	{
#ifdef HAVE_THR_SIGWAIT
         _thr_sigwait(&sset, &sigReceived);
#else
         sigwait(&sset, &sigReceived);
#endif
	 sprintf(print_buf, "Signal received [%d]\n", sigReceived);
	 log_print(print_buf); strcpy(print_buf, "");
	 /*	 fprintf(stderr, "Signal received [%d]\n", sigReceived); */
	 switch (sigReceived)
	 {
	   case SIGINT:
	   case SIGTERM:
	   /* SIGINT and SIGTERM stop all servers */
	        SV_shutdown();
                pthread_exit((void *)0);
  	        break;

  	   case SIGUSR1:
  	   /* SIGUSR1 will switch the updates on and off */
  	        do_update=CO_get_do_update();
  	        if(do_update)do_update=0; else do_update=1;
  	   	sprintf(print_buf, "%d", do_update);
		CO_set_const("UD.do_update", print_buf);
		if(do_update)
		  sprintf(print_buf, "Starting updates\n");
		else
		  sprintf(print_buf, "Stopping updates\n");
		log_print(print_buf); strcpy(print_buf, "");
		/*		fprintf(stderr, "Stopping updates (SIGTERM received)\n"); */
  	   	break;
  	 }
  	}
} /* SV_signal_thread() */


/* SV_do_child() */
/*++++++++++++++++++++++++++++++++++++++

  Handle whois/config/mirror connections. Takes a pointer to the
  service description structure, containing a connected socket, limit
  of active threads, pointer to the counter of them. Does not stop to
  obey the limits, assumes this to be checked and assumes that it is
  already counted.  Decrements the counter on exit.

  Precondition: the counter must be incremented before this function is called.

  void *SV_do_child     Actually, does not return anything useful. Just NULL.

  void *varg            service description structure.

  Author:
    marek
  ++++++++++++++++++++++++++++++++++++++*/
void *SV_do_child(void *varg)
{
  svr_args *args = (svr_args *) varg;
  int sock = args->conn_sock;
  int curclients;

  LG_log(sv_context, LG_DEBUG,
	    "%s: Child thread [%d]: Socket number = %d",
	    args->name, pthread_self(), sock);

  curclients = counter_state( args->counter ); /* already added */
  LG_log(sv_context, LG_DEBUG,
	    "%s threads++ = %d", args->name, curclients);

  TA_add(sock, args->name);

  args->function(sock);

  /* TA_delete must come first - otherwise the server would crash
     when trying to report address of a closed socket */
  TA_delete();
  SK_close(sock);

	/* decrement the simultaneous connection counter */
	if (args->limit > 0) {
		gpointer orig_key, client_conn_num;
		int i;

		pthread_mutex_lock(args->conn_lock);
		g_hash_table_lookup_extended(args->conn_ipnum, args->conn_ip, &orig_key, &client_conn_num);
		i = (int)client_conn_num;

		if (i == 1) {	/* remove the key from the hashtable & free memory */
			g_hash_table_remove(args->conn_ipnum, args->conn_ip);
			UT_free(orig_key);

		} else {	/* decrease the num_conn */

			client_conn_num = (void*)(i-1);
			g_hash_table_insert(args->conn_ipnum, args->conn_ip, client_conn_num);
		}
		pthread_mutex_unlock(args->conn_lock);
	}

  /* update the global thread counter. */
  curclients = counter_add( args->counter, -1);
  LG_log(sv_context, LG_DEBUG,
	    "%s threads-- = %d", args->name, curclients);

  UT_free(args);

  return NULL; /* exit the thread */
} /* SV_do_child */


/* main_loop() */
/*++++++++++++++++++++++++++++++++++++++

  Waits for an incoming connection on the and spawns a new thread to
  handle it.  Takes a pointer to the service description structure
  containing the number of the listening socket, limit of active
  threads, pointer to the counter of them, and the function to call
  with a connected socket.  Increments the counter before starting
  a client thread to run SV_do_child().

  void *arg      pointer to the service description structure.

  More:
  +html+ <PRE>
  Author:
        ottrey
	joao
	andrei (do_server)
	marek (rewritten/simplified/added limits)
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
static void  *main_loop(void *arg) {
  svr_args *args = (svr_args *)arg;
  svr_args *argcopy;
  char loopname[32];
  int children;
  char chnum[16];
  struct pollfd pfd[2];
	ip_addr_t *clientip;
	acl_st clientacl;

	clientip = UT_malloc(sizeof(ip_addr_t));	/* always keep an instance allocated */

  snprintf(loopname, 32, "s-%s", args->name);
  TA_add(0, loopname);

  pfd[0].fd = args->accept_sock;
  pfd[0].events = POLLIN;
  pfd[1].fd = SV_shutdown_recv_fd;
  pfd[1].events = POLLIN;

  while( CO_get_do_server() != 0 ) {
    /* check the number of clients, do not proceed until it's below limit */
    children = counter_wait( args->counter, args->limit );
    snprintf(chnum, 16, "%d", children);
    TA_setactivity(chnum); /* display the current number of children */

    /* check for input */
    if (poll(pfd, 2, -1) == -1) {
        LG_log(sv_context, LG_ERROR, "%s got error %d on poll; %s",
               args->name, errno, strerror(errno));
        continue;
    }

    /* see if we're shutting down */
    if (pfd[1].revents != 0) break;

    /* wait for new connections */
    args->conn_sock = SK_accept_connection(args->accept_sock);
    if (args->conn_sock == -1) {
      continue;
    }

    /* check whether the client already reached the simultaneous connection limit,
     * and if so, deny access without starting a thread */
		if (args->limit > 0) {
			gpointer orig_key, client_conn_num;

			SK_getpeerip(args->conn_sock, clientip);
			AC_check_acl(clientip, NULL, &clientacl);
			pthread_mutex_lock(args->conn_lock);

			if (g_hash_table_lookup_extended(args->conn_ipnum, clientip, &orig_key, &client_conn_num)) {
				int i = (int)client_conn_num;
				args->conn_ip = orig_key;
				if (i >= clientacl.maxconn) {
					/* close the connection without further warning */
					char buf[IP_ADDRSTR_MAX];
					pthread_mutex_unlock(args->conn_lock);
					IP_addr_b2a(clientip, buf, sizeof(buf));

					SK_close(args->conn_sock);
					/* FIXME: this should go to the log as well, but now I wanted to make it DOS-resistant,
					 * so it's important to handle as many refused connections/sec as possible */
					//fprintf(stderr, "Refused connection from %s (reason: too many simultaneous connections)\n", buf);
					continue;
				}
				client_conn_num = (void*)(i+1);
			} else {
				args->conn_ip = clientip;
				clientip = UT_malloc(sizeof(ip_addr_t));		/* always keep an instance allocated */
				client_conn_num = (void*)1;
			}

			g_hash_table_insert(args->conn_ipnum, args->conn_ip, client_conn_num);
			pthread_mutex_unlock(args->conn_lock);
		}

    LG_log(sv_context, LG_DEBUG, "%s: starting a new child thread", loopname);
    TA_increment();
    /* incrementing argset->counter here - to avoid race condition and
       ensure a _more_correct_ value of current clients also for unlimited
       or infrequent connections. Does not really matter otherwise.

       NOTE: this architecture implies that higher values can be
       displayed for infrequent threads, because there's no way
       to change it when threads are exiting while this thread is
       blocked in call to accept(). If this call was in the child thread,
       the number would be an underestimation instead. I prefer over-e.
    */
    counter_add( args->counter, 1);

    /* Start a new thread. will decrement counter when exiting */

    /* now. There's a race condition - argset must be copied in SV_do_child
       and can be reused here only afterwards. To avoid it, we make a copy
       and expect SV_do_child to free it after use.
       Caveat: the counter remains where it was, we just copy the pointer.
    */
    argcopy = UT_malloc( sizeof(svr_args) );
    memcpy( argcopy, args, sizeof(svr_args) );
    TH_create( SV_do_child, (void *)argcopy);
  }

  TA_delete();
  LG_log(sv_context, LG_DEBUG, "Exiting from the main loop");

  return NULL;
} /* main_loop() */

/* SV_concurrent_server() */
/*++++++++++++++++++++++++++++++++++++++

  This is the routine that creates the main threads.

  int     sock        The socket to connect to.

  int     limit       Limit of active clients (0 == no limit)

  void *  do_function The function to call for each type of service

  More:
  +html+ <PRE>
  Author:
        ottrey
	joao
	marek
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
static
void SV_concurrent_server(int sock, int limit,  char *name,
			  void do_function(int))
{
  svr_args *args;

  args = (svr_args *)UT_calloc(1, sizeof(svr_args));

  args->accept_sock=sock;
  args->limit=limit;
  args->name=name;
  args->function=do_function;
  args->counter = (svr_counter_t *)UT_calloc(1, sizeof(svr_counter_t));

  if (limit > 0) {		/* should we enforce total and per-source limits? */
  	args->conn_ipnum = g_hash_table_new_full(ip_addr_t_pointer_hash, ip_addr_t_pointer_equals, NULL, NULL);
  	args->conn_lock = UT_malloc(sizeof(pthread_mutex_t));
  	pthread_mutex_init(args->conn_lock, NULL);
  }

  pthread_mutex_init( &(args->counter->lock), NULL );
  pthread_cond_init(  &(args->counter->cond), NULL );
  args->counter->count = 0;

  /* Start a new thread. */
  TH_create(main_loop, (void *)args);

} /* SV_concurrent_server() */

/* SV_start() */
/*++++++++++++++++++++++++++++++++++++++

  Start the server.

  More:
  +html+ <PRE>
  Authors:
        ottrey
        joao
  +html+ </PRE>
  +html+ Starts up the server.
  +html+ <OL>
  +html+   <LI> Create sockets on the necessary ports (whois, config and mirror)
  +html+   <LI> Start new threads for each service.
  +html+ </OL>
  +html+ <A HREF=".DBrc">.properties</A>

  ++++++++++++++++++++++++++++++++++++++*/
int SV_start(char *pidfile) {
  int whois_port = -1;
  int config_port = -1;
  int mirror_port = -1;
  int update_port = -1;
  int update_mode = 0;
  int pid_fd;
  int nwrite;
  struct timeval tval;
  char starttime[128];
  char server_pid[16];
  ca_dbSource_t *source_hdl;
  char *source_name;
  int source;
  char *db_host, *db_name, *db_user, *db_passwd;
  int db_port;
  SQ_connection_t *db_connection;
  int shutdown_pipe[2];
  int retval=1;
  struct rlimit rlim;
  struct pollfd ufds;

  /* Store the starting time */
  gettimeofday(&tval, NULL);
  SV_starttime = tval.tv_sec;/* seconds since Jan. 1, 1970 */

  /* Log the starting time */
  ctime_r(&SV_starttime, starttime);
  LG_log(sv_context, LG_INFO, "Server is started %s", starttime);

  /* Create our shutdown pipe */
  if (pipe(shutdown_pipe) != 0) {
      LG_log(sv_context, LG_SEVERE,
                "error %d creating shutdown pipe; %s", errno, strerror(errno));
      return(-1);
  }
  SV_shutdown_send_fd = shutdown_pipe[1];
  SV_shutdown_recv_fd = shutdown_pipe[0];

  /* Store the PID of the process */
  SV_pidfile = pidfile;
  /* create the file read-writable by the process owner */
  if((pid_fd=open(SV_pidfile,  O_CREAT | O_TRUNC | O_WRONLY, 0600))==-1) {
      LG_log(sv_context, LG_ERROR, "cannot open pid file %s", SV_pidfile);
      return(-1);
  }
  sprintf(server_pid, "%d", (int)getpid());
  nwrite=write(pid_fd, server_pid, strlen(server_pid) );
  close(pid_fd);
  if(nwrite != strlen(server_pid)) {
      LG_log(sv_context, LG_ERROR, "cannot write to pid file %s", SV_pidfile);
      return(-1);
  }


  /* Initialise modules */
  /* XXX: must be handled previously!!! */
  /*SK_init();*/
  /*PW_init();*/

  /* Initialise the access control list. */
  AC_build();
  AC_acc_load();
  AC_persistence_init();
  /* explicitly start the decay & persistence threads */
  TH_create((void *(*)(void *))AC_decay, NULL);
  TH_create((void *(*)(void *))AC_persistence_daemon, NULL);

  /* Get port information for each service */
  whois_port  = ca_get_svwhois_port;
  LG_log(sv_context, LG_INFO, "whois port is %d", whois_port);
/*  ER_dbg_va(FAC_SV, ASP_SV_PORT, "whois port is %d", whois_port); */

  config_port = ca_get_svconfig_port;
  LG_log(sv_context, LG_INFO, "config port is %d", config_port);
/*  ER_dbg_va(FAC_SV, ASP_SV_PORT, "config port is %d", config_port); */


  mirror_port = ca_get_svmirror_port;
  LG_log(sv_context, LG_INFO, "mirror port is %d", mirror_port);
/*  ER_dbg_va(FAC_SV, ASP_SV_PORT, "mirror port is %d", mirror_port);*/


  /* 6. Create a socket on the necessary ports/addresses and bind to them. */
  /* whois socket */
  SV_whois_sock = SK_getsock(SOCK_STREAM, whois_port, 128, INADDR_ANY);
  fprintf(stderr,"Whois  port: %d\n",whois_port);
  /* config interface socket */
  SV_config_sock = SK_getsock(SOCK_STREAM, config_port, 5, INADDR_ANY);
  fprintf(stderr,"Config port: %d\n",config_port);
  /* nrt socket */
  SV_mirror_sock = SK_getsock(SOCK_STREAM,mirror_port, 128, INADDR_ANY);
  fprintf(stderr,"Mirror port: %d\n",mirror_port);
  /* per source update ports */
  for (source=0;
       (source_hdl = ca_get_SourceHandleByPosition(source))!=NULL;
       source++){
    update_mode = ca_get_srcmode(source_hdl);
    if(IS_UPDATE(update_mode)) {
      update_port = ca_get_srcupdateport(source_hdl);
      SV_update_sock[source] =
        SK_getsock(SOCK_STREAM, update_port, 128, INADDR_ANY);
      fprintf(stderr,"Update port: %d for %s\n", update_port, source_hdl);
    } else {
      SV_update_sock[source] = 0;
    }
  }

  /* (after 6). Drop root privileges, if any */
  if (getuid() != 0) {
    fprintf(stderr,"Not running as root, keeping current uid and gid\n");
  } else {
    struct passwd *passwent;
    char *procuser;
    int lenproc;

    fprintf(stderr,"*Running* as root, dropping privileges:\n");
    procuser=strdup(ca_get_processuser);

    /* chop off any trailing newline */
    lenproc = strlen(procuser);
    if ((lenproc > 0) && (procuser[lenproc-1] == '\n'))
    {
      procuser[lenproc-1] = '\0';
    }

    passwent = getpwnam(procuser);
    if (passwent == NULL) {
      fprintf(stderr," getpwnam failed for %s - %s\n",
        procuser,strerror(errno));
      die;
    }
    if ((setgid(passwent->pw_gid)) != 0) {
      fprintf(stderr,"can't setgid(%d)\n",passwent->pw_gid);
      die;
    }
    fprintf(stderr,"Gid: %d ",passwent->pw_gid);
    if ((setegid(passwent->pw_gid)) != 0) {
      fprintf(stderr,"can't setegid(%d)\n",passwent->pw_gid);
      die;
    }
    fprintf(stderr,"Egid: %d ",passwent->pw_gid);
    if ((setuid(passwent->pw_uid)) != 0) {
      fprintf(stderr,"can't setuid(%d)\n",passwent->pw_uid);
      die;
    }
    fprintf(stderr,"Uid: %d ",passwent->pw_uid);
    if ((seteuid(passwent->pw_uid)) != 0) {
      fprintf(stderr,"can't seteuid(%d)\n",passwent->pw_uid);
      die;
    }
    fprintf(stderr,"Euid: %d ",passwent->pw_uid);
    fprintf(stderr,".\n");
  }

  /* Set Dumpable flag - Linux specific TODO */
  /* Removed 20060321 - add it prior to distribution */
  //if (prctl(PR_SET_DUMPABLE,1,0,0,0)<0) {
  //  fprintf(stderr,"Warning: Can't set DUMPABLE status\n");
  //}

/*  SV_whois_sock = SK_getsock(SOCK_STREAM,whois_port,whois_addr); */

  /* Check every Database and create sockets */
  /* we need first to create and bind all of them */
  /* so that in case of failure we do not start any */
  /* update thread */
  fprintf(stderr, "Check the DB\n");
  for(source=0; (source_hdl = ca_get_SourceHandleByPosition(source))!=NULL ; source++){
     /* check for crash and recover if needed */
     /* make a connection to a database */
     db_host = ca_get_srcdbmachine(source_hdl);
     db_port = ca_get_srcdbport(source_hdl);
     db_name = ca_get_srcdbname(source_hdl);
     db_user = ca_get_srcdbuser(source_hdl);
     db_passwd = ca_get_srcdbpassword(source_hdl);
     db_connection=SQ_get_connection(db_host, db_port, db_name, db_user, db_passwd);
     /* now check TR record */
     TR_recover(db_connection, source_hdl, sv_context);
     /* free resources */
     SQ_close_connection(db_connection);
     UT_free(db_host);
     UT_free(db_name);
     UT_free(db_user);
     UT_free(db_passwd);
  }
  SV_update_sock[source+1]=-1; /* end of socket array */

  /* Initialise the radix trees
     already can allow socket connections, because the trees will
     be created locked, and will be unlocked when loaded */
  radix_init();

  /* Now.... accept() calls block until they get a connection
     so to listen on more than one port we need more
     than one thread */

  /* Create master thread for whois threads */
  /* The max number of threads for queries has been increased from 64 to 128 (Engin 20040614) */
  SV_concurrent_server(SV_whois_sock, 128, "whois", PW_interact);
  /* Create master thread for config threads */
  SV_concurrent_server(SV_config_sock, 0, "config", PC_interact);
  /* Create master thread for mirror threads */
  SV_concurrent_server(SV_mirror_sock, 0, "mirror", PM_interact);

  /* Load the radix trees - this must be done before the
     updates are allowed to proceed */
  radix_load();

  /* Walk through the sources and */
  /* run update thread for every source with CANUPD == 'y' */

   for(source=0; (source_hdl = ca_get_SourceHandleByPosition(source))!=NULL ; source++){
     source_name= ca_get_srcname(source_hdl);

     fprintf(stderr,"Source [%s] Mode STATIC\n", source_name);
     LG_log(sv_context, LG_INFO, "Source [%s] Mode STATIC", source_name);
     UT_free(source_name); /* because ca_* functions return copies */
   }
  /* terminate the thread */
  /* XXX not return becase then we terminate the whole process */

  /* Loop until the server goes down */
  ufds.fd = SV_shutdown_recv_fd;
  ufds.events = POLLIN | POLLPRI;
  ufds.revents = 0;
  while (1) {

    retval = poll(&ufds, 1, 500);
    if ((retval != 0)&&(ufds.revents != 0)) {
      fprintf(stderr,"Whois server going down.\n");
      pthread_exit(NULL);
      return(1);
    }
  }

  /* we will never reach this point */
  // return(1);
} /* SV_start() */

/* SV_switchdynamic() */
/*++++++++++++++++++++++++++++++++++++++

  Turn on dynamic updates

  ++++++++++++++++++++++++++++++++++++++*/
void SV_switchdynamic() {
  int dynamic_status = CO_get_dynamic();
  static int already_set = 0;
  int source;
  ca_dbSource_t *source_hdl;
  char *source_name;
  int update_mode = 0;

  if (dynamic_status == 1) {
    /* switch update and nrtm modes on */
    for(source=0;
        (source_hdl = ca_get_SourceHandleByPosition(source))!=NULL ;
        source++) {
      update_mode = ca_get_srcmode(source_hdl);
      source_name= ca_get_srcname(source_hdl);
      if(IS_UPDATE(update_mode)) {
        /* run RIPupdate thread */
        fprintf(stderr,"Source [%s] Mode UPDATE [port=%d]\n",
            source_name, ca_get_srcupdateport(source_hdl));
        LG_log(sv_context, LG_INFO, "Source [%s] Mode UPDATE [port=%d]",
            source_name, ca_get_srcupdateport(source_hdl));
        TH_create((void *(*)(void *))UD_do_updates, (void *)source);
      } else if (IS_NRTM_CLNT(update_mode)) {
        /* start NRTM client */
        fprintf(stderr,"Source [%s] Mode NRTM\n", source_name);
        LG_log(sv_context, LG_INFO, "Source [%s] Mode NRTM", source_name);
        TH_create((void *(*)(void *))UD_do_nrtm, (void *)source);
      } else {
        /* notify STATIC sources */
        fprintf(stderr,"Source [%s] Mode already STATIC\n", source_name);
        LG_log(sv_context, LG_INFO, "Source [%s] Mode already STATIC",
            source_name);
      }
    }
    already_set = 1;
  } else {
    if (already_set == 1) {
      fprintf(stderr,"Cannot revert to static mode from dynamic mode\n");
      LG_log(sv_context, LG_INFO,
          "Cannot revert to static mode from dynamic mode");
    }
  }
} /* SV_switchdynamic() */

/* SV_shutdown() */
/*++++++++++++++++++++++++++++++++++++++

  Shutdown the server.

  More:
  +html+ <PRE>
  Authors:
        andrei
  +html+ </PRE>
  +html+ Stops the server.
  +html+ <OL>
  +html+   <LI> Close listening sockets (whois, config, mirror and updates)
  +html+   <LI> Stop all threads by triggering do_server variable.
  +html+ </OL>
  +html+ <A HREF=".DBrc">.properties</A>

  ++++++++++++++++++++++++++++++++++++++*/
void SV_shutdown() {
char print_buf[STR_M];
time_t shutdowntime;
struct timeval tval;
char shuttime[100];

#if 0
int source;
#endif /* 0 */

 sprintf(print_buf, "%d", 0);
 /* Stop updates */
 CO_set_const("UD.do_update", print_buf);
 /* Stop all servers */
 CO_set_const("SV.do_server", print_buf);
 sprintf(print_buf, "Stopping all servers\n");
 fprintf(stderr, print_buf);
 /*log_print(print_buf); */
 strcpy(print_buf, "");

 /* wait for all updates to complete */
 /* XXX may be changed with blocking interface for stop updates */
 sleep(5);

 /* Store the shutdown time */
  gettimeofday(&tval, NULL);
  shutdowntime = tval.tv_sec;/* seconds since Jan. 1, 1970 */

  /* Log the sshutdown time */
  ctime_r(&shutdowntime, shuttime);
  LG_log(sv_context, LG_INFO, "Server shutdown %s", shuttime);



 /* Wake up all sleeping threads */
 fprintf(stderr, "Going to wake sleeping threads up\n");

 /* Delete the pid file to indicate normal shutdown */
 if(unlink(SV_pidfile)==-1) {
      LG_log(sv_context, LG_ERROR, "cannot delete pid file %s:", SV_pidfile);
 }

 /* write to shutdown pipe */
 if (write(SV_shutdown_send_fd, "", 1) != 1) {
      LG_log(sv_context, LG_ERROR, "error %d writing to shutdown pipe; %s",
                errno, strerror(errno));
 }


/* don't do this, it's not polite  ;) */
#if 0
 /* CLose all listening sockets, so accept call exits */
 close(SV_whois_sock);
 close(SV_config_sock);
 close(SV_mirror_sock);
 for (source=0; SV_update_sock[source]!=-1; source++)
	 if(SV_update_sock[source]!=0)close(SV_update_sock[source]);
#endif /* 0 */

} /* SV_shutdown() */
