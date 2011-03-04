/***************************************
  $Revision: 1.15 $

  Socket module - cd_watchdog.c - Socket watchdog - when activated, checks the
                                  socket for new data and discards it. If the
				  socket is closed, it triggers predefined
				  functions - executes a function and/or 
				  cancels a thread.

  Status: NOT REVUED, TESTED

  Design and implementation by Marek Bukowy.

  Modification history:
  marek  (August 2000) Created the watchdog part
  marek  (December 2000) Modified watchdog deactivation - 
                         replaced signals by pthread cancellation.
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

/* logging */
LG_context_t *sk_context;

/*+ Uncomment this to use watchdog deactivation by signal (may be risky)

   #define WATCHDOG_BY_SIGNAL
+*/

static pthread_once_t sk_init_once = PTHREAD_ONCE_INIT; 

#ifdef WATCHDOG_BY_SIGNAL

/*+ The signal version is complicated to cope with all timing situations.
  It uses a thread specific flag to see if the signal handler was invoked
  in case the signal arrives before select(3) is called in watchdog.
+*/

/* thread specific flag */
static pthread_key_t  sk_watch_tsd;

/*++++++++++++++++++++++++++++++++++++++
  initialisation for the SIGNAL cancellation mode 
  - initialises the thread specific flag.
  ++++++++++++++++++++++++++++++++++++++*/
static void sk_real_init(void)
{
  dieif( pthread_key_create( &sk_watch_tsd, NULL) != 0 );
}


/*++++++++++++++++++++++++++++++++++++++
  sk_watchdog signal handler - sets the thread-specific flag.

  int n      signal received. (not used)
  ++++++++++++++++++++++++++++++++++++++*/
static void func_sigusr(int n) {
#if 0
  /* just for debugging - we don't check the value here */
  int *tsd_flag = (int *) pthread_getspecific(sk_watch_tsd);
#endif

  /* 2000/12/18 MB:
     DEADLOCK has happened - the watchdog was just getting a mutex
     for the ER rwlock when a signal arrived and the execution of the
     pthread_mutex_lock function was interrupted AFTER the lock was
     grabbed. The this handler was invoked and tried to get that mutex
     again. As a result, everything stopped.

     Cures: 
     1. Not invoke this here:
     ER_dbg_va(FAC_SK, ASP_SK_GEN,"func_sigusr(%d) called", n);

     2. Not accept any signals during any pthread calls so that this
     does not happen again. Must be reimplemented with pthread_cancel 
     and all the signal stuff must go away. (Done, 2000/12/19).
  */
  /* set a thread-specific flag that the handler was invoked */
  
  pthread_setspecific(sk_watch_tsd, (void *)1 );
}

/*++++++++++++++++++++++++++++++++++++++
  watchdog (SIGNAL VERSION) - started as a separate thread. 

   Selects on the given socket; discards all input.
   whenever it sees end of file (socket closed), it
   * sets a corresponding flag in the condat structure, 
   * triggers the predefined actions (by SK_watchtrigger).

  void *arg    - pointer to the connection data structure
  ++++++++++++++++++++++++++++++++++++++*/
static
void *sk_watchdog(void *arg)
{
  sk_conn_st *condat = (sk_conn_st *) arg;
  int nready;
  int n;
  fd_set rset;
  char buff[STR_S];
  int socket = condat->sock;
  sigset_t sset;
  struct sigaction act;
  
  struct timeval timeout = { 1, 0 }; /* it's a timeout of 1 second */

  FD_ZERO(&rset);
  FD_SET(socket, &rset);

  sigemptyset(&sset);
  sigaddset(&sset, SIGUSR2);
  
  act.sa_handler = func_sigusr;
  act.sa_flags = 0;
  dieif(sigaction(SIGUSR2, &act, NULL) != 0);

  /* XXX in fact, it's unblocked already. Should be blocked on startup */
  dieif(pthread_sigmask(SIG_UNBLOCK, &sset, NULL) != 0);
  
  /* clear the handler's flag */
  pthread_setspecific(sk_watch_tsd, NULL);
  
  /* now ready for signal */
  pthread_mutex_unlock( & condat->watchmutex ); 

  /* hey, viva threaded signal handling! There is no way for select
     to unblock a blocked signal, It must be done by "hand" (above).

     Consequently, every once in a while, the signal will be delivered
     before the select starts :-/. So, we have to introduce a timeout
     for select and check if the signal was delivered anyway....aARGH!!!

     This adds a <timeout interval> to unlucky queries, about 0.1% of all.
  */

  while ((nready=select(socket+1, &rset, NULL, NULL, &timeout))!=-1) {
    
    LG_log(sk_context, LG_DEBUG, "select returned %d", nready);

    /* don't even try to read if we have been killed */
    if( errno == EINTR || pthread_getspecific(sk_watch_tsd) != NULL ) {
      break;
    }

    /* retry if the timeout has triggered */
    if( nready == 0 ) {
      continue;
    }

   /* There was some input or client half of connection was closed */
   /* Check for the latter */
    if (( n=read(socket, buff, sizeof(buff))) == 0) {
   /* Connection was closed by client */
   /* Now send a cancellation request to the whois thread. */
   /* mysql thread will be terminated by thread cleanup routine */
     
     /* call the actions: kill and exec (the SK_ functions called
	check if the action is defined. Will set the RTC flag on condat 
     */
     SK_watchtrigger(condat);

     /* quit */
     break;
   }
   /* Otherwise dump input and continue */

  }

  /* Exit the watchdog thread, passing NULL as we don't expect a join */
  pthread_exit(NULL);

  /* oh yes. Shouldn't compilers _recognize_ library functions ? */
  return NULL;
}


#else /* not WATCHDOG_BY_SIGNAL */


/*++++++++++++++++++++++++++++++++++++++
  watchdog (CANCEL VERSION) - started as a separate thread. 

   Selects on the given socket; discards all input.
   whenever it sees end of file (socket closed), it
   * sets a corresponding flag in the condat structure, 
   * triggers the predefined actions (by SK_watchtrigger).

  void *arg    - pointer to the connection data structure
  ++++++++++++++++++++++++++++++++++++++*/
// commented out to avoid compiler warning - agoston, 2011-03-03 //static
void *sk_watchdog(void *arg)
{
  sk_conn_st *condat = (sk_conn_st *) arg;
  int nready;
  struct timeval timeout;
  fd_set rset;
  
  /* this is to allow cancellation */
  pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

  /* now ready for the cancellation */
  pthread_mutex_unlock(&condat->watchmutex); 
  
  for (;;)
  {
    /* wait for activity on the socket, or a timeout */
    FD_ZERO(&rset);
    FD_SET(condat->sock, &rset);  
    timeout.tv_sec = 0;
    timeout.tv_usec = 50000;
    nready = select(condat->sock+1, &rset, NULL, NULL, &timeout);

    LG_log(sk_context, LG_DEBUG, "select returned %d", nready);
    /* quit on error */
    if( nready < 0 ) {
      break;
    }

    /* There was some input or client half of connection was closed. */
    /* We will use the sk_fill_rd_buf() so that we don't lose client
       query data.  If the client fills the buffer, or we get error
       or socket close, cancel our query. */
    if (nready != 0) 
    {
      if (sk_rd_buf_full(condat) || (sk_fill_rd_buf(condat) == -1))
      {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        SK_watchtrigger(condat);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        break;
      }
    }
  }
  
  return NULL; /* quit */
}


/*++++++++++++++++++++++++++++++++++++++
  initialisation for the PTHREAD_CANCEL mode is not needed.
  ++++++++++++++++++++++++++++++++++++++*/
static void sk_real_init(void) {
  /* EMPTY */
}

#endif  /* WATCHDOG_BY_SIGNAL */


/*++++++++++++++++++++++++++++++++++++++
   starts sk_watchdog thread unless already started,
   and registers its threadid in the condat structure

   dies if watchdog already running

   int SK_watchstart   Returns SK_OK on success.
   
   sk_conn_st *condat       pointer to the connection data structure

   The structure may (and normally, should) contain the predefined actions
   set by SK_watch_set... functions.
  ++++++++++++++++++++++++++++++++++++++*/
int
SK_watchstart(sk_conn_st *condat)
{
  dieif( condat->watchdog != 0 );

  /* init the mutex in locked state, watchdog will unlock it when 
     it's ready for signal/cancellation */
  pthread_mutex_init( & condat->watchmutex, NULL );
  pthread_mutex_lock( & condat->watchmutex ); 

/* 
   Linux doesn't seem to like getting signals in select(), which isn't
   technically allowed by POSIX.  The workaround in this case is simply
   to not create a watchdog for Linux.  This is probably acceptable
   because we will be changing the query path to perform queries in small
   chunks, so if a disconnect occurs it won't consume a lot of database
   resources in any case, even without a watchdog.

   SCO has a really small stack, so we don't want to create extra threads.
 */
#if !defined(__linux__) && !defined(SCO)
  /* NOT DETACHED! */
  pthread_create(&condat->watchdog, NULL, sk_watchdog, (void *) condat );
#endif /* __linux__ */

  return SK_OK;
}


/*++++++++++++++++++++++++++++++++++++++
  
  stops running sk_watchdog thread. 
  If it is not running ( == not registered in the connection struct), 
  it does nothing.

  int SK_watchstop    always succeeds (returns SK_OK)
  
  sk_conn_st *condat       pointer to the connection data structure
  ++++++++++++++++++++++++++++++++++++++*/
int
SK_watchstop(sk_conn_st *condat)
{
  void *res;

  if(condat->watchdog > 0) {
    int ret;

    /* wait until the watchdog is ready for signal */
    pthread_mutex_lock( & condat->watchmutex ); 

#ifdef WATCHDOG_BY_SIGNAL
    ret = pthread_kill(condat->watchdog, SIGUSR2);
#else
    ret = pthread_cancel(condat->watchdog);
#endif
    
    ret = pthread_join(condat->watchdog, &res);
    
    pthread_mutex_destroy( & condat->watchmutex ); 
    condat->watchdog = 0;
  }
  return SK_OK;
}


/*++++++++++++++++++++++++++++++++++++++

  void SK_watch_setkill       sets the thread id of the thread to be
                              cancelled by the watchdog watching this socket.
			      0 (default) means do not cancel anything.

  sk_conn_st *condat          pointer to the connection data structure.
  
  pthread_t killthis          thread id of the thread to be cancelled, or 0.
  ++++++++++++++++++++++++++++++++++++++*/
void
SK_watch_setkill(sk_conn_st *condat, pthread_t killthis)
{
  condat->killthis = killthis;
}


/*++++++++++++++++++++++++++++++++++++++
  
  void SK_watch_setexec       sets the function to be invoked by the watchdog 
                              watching this socket. NULL (default) means do
			      not invoke anything.
  
  sk_conn_st *condat          pointer to the connection data structure.
  
  void *(*function)(void *)   function to be invoked
  
  void *args                  argument to be passed to the function.

  ++++++++++++++++++++++++++++++++++++++*/
void
SK_watch_setexec( sk_conn_st *condat, void *(*function)(void *) , void *args)
{
  condat->execthis = function;
  condat->execargs = args;
}


/*++++++++++++++++++++++++++++++++++++++
  
  void SK_watch_setclear      clears the function and thread id fields so that
                              nothing gets cancelled or invoked by the 
			      watchdog.

  sk_conn_st *condat          pointer to the connection data structure.

  ++++++++++++++++++++++++++++++++++++++*/
void 
SK_watch_setclear(sk_conn_st *condat) 
{
  condat->execthis = NULL;
  condat->execargs = NULL;
  condat->killthis = 0;
}

/* call the function to be called if defined */


/*++++++++++++++++++++++++++++++++++++++

  void SK_watchexec          invokes the predefined function if defined.
                             (usually called from the watchdog). 
			     Also sets the reason-to-close
			     flag on this connection to SK_INTERRUPT.
  
  sk_conn_st *condat         pointer to the connection data structure.

  ++++++++++++++++++++++++++++++++++++++*/
void 
SK_watchexec(sk_conn_st *condat) 
{
  /* set the reason-to-close flag on this connection */
  condat->rtc |= SK_INTERRUPT;
  
  if( condat->execthis != NULL ) {
    condat->execthis(condat->execargs);
  } 
}

/* cancel the thread to be cancelled if defined */


/*++++++++++++++++++++++++++++++++++++++
  
  void SK_watchkill          cancels the predefined thread if defined.
                             (usually called from the watchdog). 
			     Also sets the reason-to-close
			     flag on this connection to SK_INTERRUPT.

  sk_conn_st *condat         pointer to the connection data structure.

  ++++++++++++++++++++++++++++++++++++++*/
void 
SK_watchkill(sk_conn_st *condat) {

  /* set the reason-to-close flag on this connection */
  condat->rtc |= SK_INTERRUPT;

  /* cancel thread if defined */
  if( condat->killthis != 0 ) {
    pthread_cancel(condat->killthis);
    /* The only possible error is ESRCH, so we do not care about it*/
  }
}


/*++++++++++++++++++++++++++++++++++++++
  
  void SK_watchtrigger       Wrapper around SK_watchkill and SK_watchexec.
                             First executes the function, then cancels the
			     thread.

  sk_conn_st *condat         pointer to the connection data structure.

  ++++++++++++++++++++++++++++++++++++++*/
void SK_watchtrigger(sk_conn_st *condat) 
{
     SK_watchexec(condat); 
     SK_watchkill(condat);    
}


/*++++++++++++++++++++++++++++++++++++++
  Initialisation function, should be called exactly once 
  (well, it ignores repeated calls). The actions depend on cancellation
  mode (signal or pthread_cancel).
  ++++++++++++++++++++++++++++++++++++++*/
void  
SK_init (LG_context_t *ctx)
{
  /* can be called only once */
  pthread_once( &sk_init_once, sk_real_init);

  sk_context = ctx;
}
