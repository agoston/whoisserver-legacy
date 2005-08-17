/***************************************
  $Revision: 1.1 $

  Example code: A thread.

  Status: NOT REVUED, NOT TESTED

 Authors:       Chris Ottrey
		Joao Damas

  +html+ <DL COMPACT>
  +html+ <DT>Online References:
  +html+ <DD><UL>
  +html+ </UL>
  +html+ </DL>
 
  ******************/ /******************
  Modification History:
        ottrey (02/03/1999) Created.
        ottrey (08/03/1999) Modified.
        ottrey (17/06/1999) Stripped down.
        joao   (22/06/1999) Redid thread startup
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

#include <pthread.h>       /* Posix thread library */
#include <stdio.h>
#include <strings.h>

//typedef struct th_args {
//	void *function;
//	int sock;
//} th_args;

/*************************************************
*                                                *
*     Readers Writes Locks that favor READERS    *
*                                                *
*************************************************/
/* TH_acquire_read_lock() */
/*++++++++++++++++++++++++++++++++++++++

  Aquire a readers lock.

  rw_lock_t *prw_lock Readers writers lock.

  Reference: "Multithreaded Programming Techniques - Prasad p.192"
  More:
  +html+ <PRE>
  Author:
        ottrey
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
void TH_acquire_read_lock(rw_lock_t *prw_lock) { 
  struct timespec to;
  int err;

  to.tv_sec = time(NULL) + CONDWAITTIMEOUT; /* Wait for a second, at most */
  to.tv_nsec = 0;
  pthread_mutex_lock(&prw_lock->rw_mutex);

  while (prw_lock->rw_count < 0) {
    err = pthread_cond_timedwait(&prw_lock->rw_cond, &prw_lock->rw_mutex, &to);
    if (err == ETIMEDOUT) {
      break;
    }
  }

  ++prw_lock->rw_count;
  pthread_mutex_unlock(&prw_lock->rw_mutex);

} /* TH_acquire_read_lock() */

/* TH_release_read_lock() */
/*++++++++++++++++++++++++++++++++++++++

  Release a readers lock.

  rw_lock_t *prw_lock Readers writers lock.

  Reference: "Multithreaded Programming Techniques - Prasad p.192"
  More:
  +html+ <PRE>
  Author:
        ottrey
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
void TH_release_read_lock(rw_lock_t *prw_lock) { 
  pthread_mutex_lock(&prw_lock->rw_mutex);

  --prw_lock->rw_count;

  if (!prw_lock->rw_count) {
    pthread_cond_signal(&prw_lock->rw_cond);
  }

  pthread_mutex_unlock(&prw_lock->rw_mutex);

} /* TH_release_read_lock() */

/* TH_acquire_write_lock() */
/*++++++++++++++++++++++++++++++++++++++

  Aquire a writers lock.

  rw_lock_t *prw_lock Readers writers lock.

  Reference: "Multithreaded Programming Techniques - Prasad p.192"
  More:
  +html+ <PRE>
  Author:
        ottrey
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
void TH_acquire_write_lock(rw_lock_t *prw_lock) { 
  struct timespec to;
  int err;

  to.tv_sec = time(NULL) + CONDWAITTIMEOUT; /* Wait for a second, at most */
  to.tv_nsec = 0;

  pthread_mutex_lock(&prw_lock->rw_mutex);

  while (prw_lock->rw_count != 0) {
    err = pthread_cond_timedwait(&prw_lock->rw_cond, &prw_lock->rw_mutex, &to);
    if (err == ETIMEDOUT) {
      break;
    }
  }

  prw_lock->rw_count = -1;
  pthread_mutex_unlock(&prw_lock->rw_mutex);

} /* TH_acquire_write_lock() */

/* TH_release_write_lock() */
/*++++++++++++++++++++++++++++++++++++++

  Release a writers lock.

  rw_lock_t *prw_lock Readers writers lock.

  Reference: "Multithreaded Programming Techniques - Prasad p.192"
  More:
  +html+ <PRE>
  Author:
        ottrey
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
void TH_release_write_lock(rw_lock_t *prw_lock) { 
  pthread_mutex_lock(&prw_lock->rw_mutex);
  prw_lock->rw_count = 0;
  pthread_mutex_unlock(&prw_lock->rw_mutex);
  pthread_cond_broadcast(&prw_lock->rw_cond);

} /* TH_release_write_lock() */

/* TH_init_read_write_lock() */
/*++++++++++++++++++++++++++++++++++++++

  Initialize a readers/writers lock.

  rw_lock_t *prw_lock Readers writers lock.

  Side effect: the lock is set to open(?)

  Reference: "Multithreaded Programming Techniques - Prasad p.192"
  More:
  +html+ <PRE>
  Author:
        ottrey
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
void TH_init_read_write_lock(rw_lock_t *prw_lock) { 
  pthread_mutex_init(&prw_lock->rw_mutex, NULL);
  pthread_cond_init(&prw_lock->rw_cond, NULL);
  pthread_cond_init(&prw_lock->w_cond, NULL);
  prw_lock->rw_count = 0;
  prw_lock->w_count = 1; /* just in case one uses wrong interface */

} /* TH_init_read_write_lock() */

/*************************************************
*                                                *
*     Readers Writes Locks that favor WRITERS    *
*                                                *
*************************************************/
/* TH_acquire_read_lockw() */
/*++++++++++++++++++++++++++++++++++++++

  Aquire a readers lock.

  rw_lock_t *prw_lock Readers writers lock.

  Reference: "Multithreaded Programming Techniques - Prasad p.192"
  More:
  +html+ <PRE>
  Author:
        ottrey
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
void TH_acquire_read_lockw(rw_lock_t *prw_lock) { 
  struct timespec to;
  int err;

  to.tv_sec = time(NULL) + CONDWAITTIMEOUT; /* Wait for a second, at most */
  to.tv_nsec = 0;

  pthread_mutex_lock(&prw_lock->rw_mutex);

  while (prw_lock->w_count != 0) {
    err = pthread_cond_timedwait(&prw_lock->w_cond, &prw_lock->rw_mutex, &to);
    if (err == ETIMEDOUT) {
      break;
    }
  }

  ++prw_lock->rw_count;
  pthread_mutex_unlock(&prw_lock->rw_mutex);

} /* TH_acquire_read_lockw() */

/* TH_release_read_lockw() */
/*++++++++++++++++++++++++++++++++++++++

  Release a readers lock.

  rw_lock_t *prw_lock Readers writers lock.

  Reference: "Multithreaded Programming Techniques - Prasad p.192"
  More:
  +html+ <PRE>
  Author:
        ottrey
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
void TH_release_read_lockw(rw_lock_t *prw_lock) { 
  pthread_mutex_lock(&prw_lock->rw_mutex);

  --prw_lock->rw_count;

  if (!prw_lock->rw_count) {
    pthread_cond_signal(&prw_lock->rw_cond);
  }

  pthread_mutex_unlock(&prw_lock->rw_mutex);

} /* TH_release_read_lockw() */

/* TH_acquire_write_lockw() */
/*++++++++++++++++++++++++++++++++++++++

  Aquire a writers lock.

  rw_lock_t *prw_lock Readers writers lock.

  Reference: "Multithreaded Programming Techniques - Prasad p.192"
  More:
  +html+ <PRE>
  Author:
        ottrey
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
void TH_acquire_write_lockw(rw_lock_t *prw_lock) { 
  struct timespec to;
  int err;

  to.tv_sec = time(NULL) + CONDWAITTIMEOUT; /* Wait for a second, at most */
  to.tv_nsec = 0;

  pthread_mutex_lock(&prw_lock->rw_mutex);

 /* check for writers */
  while (prw_lock->w_count != 0) {
    err = pthread_cond_timedwait(&prw_lock->w_cond, &prw_lock->rw_mutex, &to);
    if (err == ETIMEDOUT) {
      break;
    }
  }

  prw_lock->w_count = 1;
 
 /* wait until all readers are gone */
  while (prw_lock->rw_count != 0) {
    err = pthread_cond_timedwait(&prw_lock->rw_cond, &prw_lock->rw_mutex, &to);
    if (err == ETIMEDOUT) {
      break;
    }
  }
 
  pthread_mutex_unlock(&prw_lock->rw_mutex);

} /* TH_acquire_write_lockw() */

/* TH_release_write_lockw() */
/*++++++++++++++++++++++++++++++++++++++

  Release a writers lock.

  rw_lock_t *prw_lock Readers writers lock.

  Reference: "Multithreaded Programming Techniques - Prasad p.192"
  More:
  +html+ <PRE>
  Author:
        ottrey
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
void TH_release_write_lockw(rw_lock_t *prw_lock) { 
  pthread_mutex_lock(&prw_lock->rw_mutex);
  prw_lock->w_count = 0;
  pthread_mutex_unlock(&prw_lock->rw_mutex);
  pthread_cond_broadcast(&prw_lock->w_cond);

} /* TH_release_write_lockw() */

/* TH_init_read_write_lockw() */
/*++++++++++++++++++++++++++++++++++++++

  Initialize a readers/writers lock.

  rw_lock_t *prw_lock Readers writers lock.

  Side effect: the lock is set to open(?)

  Reference: "Multithreaded Programming Techniques - Prasad p.192"
  More:
  +html+ <PRE>
  Author:
        ottrey
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
void TH_init_read_write_lockw(rw_lock_t *prw_lock) { 
  pthread_mutex_init(&prw_lock->rw_mutex, NULL);
  pthread_cond_init(&prw_lock->rw_cond, NULL);
  pthread_cond_init(&prw_lock->w_cond, NULL);
  prw_lock->rw_count = 0;
  prw_lock->w_count = 0;

} /* TH_init_read_write_lockw() */



/*************************************************
*                                                *
*     Other functions                            *
*                                                *
*************************************************/


int TH_get_id(void) {

  return (int)pthread_self();

} /* TH_get_id() */

/* TH_to_string() */
char *TH_to_string(void) {
  char tmp[STR_L];
  char thread_info_buffer[STR_XL];

  strcpy(thread_info_buffer, "Thread = { ");

  sprintf(tmp, "[pthread_self] = \"%lu\" ", (long unsigned)pthread_self());
  strcat(thread_info_buffer, tmp);
  
  /*
  thread_name = (char *)pthread_getspecific(Name);

  if (thread_name == NULL ) {
    sprintf(tmp, "[Name] = \"%s\" ", "didn't work!");
  }
  else {
    sprintf(tmp, "[Name] = \"%s\" ", thread_name);
  }
  strcat(thread_info_buffer, tmp);
  */
  
  strcat(thread_info_buffer, "}");
  
  return UT_strdup(thread_info_buffer);
} /* TH_to_string() */


/*++++++++++++++++++++++++++++++++++++++

  This is the routine that creates a thread. 

   More:
  +html+ <PRE>
  Author:
        ottrey
	joao
	andrei
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
pthread_t TH_create(void *do_function(void *), void *arguments ) {
  pthread_t tid;
  pthread_attr_t attr;
  int ret;

#if defined(SCO)
  size_t ssize;
#endif

    /* Start a new thread. */
    pthread_attr_init(&attr);     /* initialize attr with default attributes */
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

#if defined(SCO)
    /*********
      For SCO, we need to increase the stack size, because the default is
      exceedingly small.

      Note: see also modules/sk/cd_watchdog.c
     *********/
    dieif(pthread_attr_getstacksize(&attr, &ssize) != 0);
    if (ssize > 0) {
        dieif(pthread_attr_setstacksize(&attr, ssize * 4) != 0);
    }
#endif

    ret = pthread_create(&tid, &attr, do_function, arguments);
    if( ret !=0 ) die;
    pthread_attr_destroy(&attr);

    return tid;

} /* TH_run() */



