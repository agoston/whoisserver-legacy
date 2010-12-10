#ifndef READ_SERVER
#define READ_SERVER

/***************************************
  $Revision: 1.1 $

  Server module (sv)
  config module.

  Status: NOT REVUED, NOT TESTED

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

/* Interrupts/Locks */
#define MAX_LOCKS    4
#define LOCK_SHTDOWN 0 /* here we store the read end of the pipe */
#define WLOCK_SHTDOWN 1 /* here we store the write end of the pipe */
#define LOCK_UPDATES 2 /* here we store the read end of the pipe */
#define WLOCK_UPDATES 3 /* here we store the write end of the pipe */

/* Timeslice in seconds for sleep */
#define TIME_SLICE 1

typedef struct _wd_args_t {
  int connected_socket;
  pthread_t tid;
} wd_args_t;


typedef struct {
    int count;
    pthread_mutex_t lock; /*+ Mutex lock.Used for synchronizing changes.+*/
    pthread_cond_t cond; /*+ condition variable +*/
} svr_counter_t;

/* structure passed to every running server */
typedef struct svr_args {
    void (*function)(struct svr_args *);
    int conn_sock;
    ip_addr_t *act_conn_ip;  /* ip of the actual client */
    long act_conn_num;       /* the number of sim. connections from this IP at the time of starting handler thread */
    int accept_sock;
    int limit; /* limit for the number of concurrent connections */
    svr_counter_t *counter; /* number of active clients */
    GHashTable *conn_ipnum; /* how many times each IP has connected */
    pthread_mutex_t *conn_lock; /* for synchronizing hashtable accesses */
    char *name;
} svr_args;


void SV_init (LG_context_t *ctx);
int SV_start();
void SV_shutdown();
int SV_sleep(int sleeptime);
void *SV_signal_thread();
void SV_watchdog(wd_args_t *wd_args);
void SV_init (LG_context_t *ctx);
void SV_switchdynamic();

char *SV_command_on_die;



#endif /* READ_SERVER */
