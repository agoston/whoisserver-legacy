#ifndef READ_THREAD
#define READ_THREAD

/***************************************
  $Revision: 1.1 $

  Thread module (th)

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
#include <pthread.h>       /* Posix thread library */
/*
typedef struct _rwlock {
  pthread_mutex_t rw_mutex;
  pthread_cond_t rw_cond;
  int rw_count;
} rw_lock_t;
*/
/* structure for writers favouring functions */
typedef struct _rwlock {
  pthread_mutex_t rw_mutex;
  pthread_cond_t rw_cond;
  pthread_cond_t w_cond;
  int rw_count;
  int w_count;
} rw_lock_t;

/*
typedef struct _wd_args_t {
  int connected_socket;
  pthread_t tid;
} wd_args_t;  
*/

/* Timeout for locks in seconds */
#define CONDWAITTIMEOUT 1

/* Readers writers lock functions favoring readers */
void TH_acquire_read_lock(rw_lock_t *prw_lock);
void TH_release_read_lock(rw_lock_t *prw_lock);
void TH_acquire_write_lock(rw_lock_t *prw_lock);
void TH_release_write_lock(rw_lock_t *prw_lock);
void TH_init_read_write_lock(rw_lock_t *prw_lock);

/* Readers writers lock functions favoring writers */
void TH_acquire_read_lockw(rw_lock_t *prw_lock);
void TH_release_read_lockw(rw_lock_t *prw_lock);
void TH_acquire_write_lockw(rw_lock_t *prw_lock);
void TH_release_write_lockw(rw_lock_t *prw_lock);
void TH_init_read_write_lockw(rw_lock_t *prw_lock);

int TH_get_id(void);
char *TH_to_string(void);
pthread_t TH_create(void *do_function(void *), void *arguments );

/* 	
void TH_hdl_signal(void);
void TH_watchdog(wd_args_t *wd_args);
void TH_run(int sock, void *do_function(void *));
void TH_run1(int sock, void *do_function(void *));
void TH_run2(void *function(void *));
*/ 
#endif /* READ_THREAD */
