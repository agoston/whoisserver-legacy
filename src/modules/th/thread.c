/* THIS MODULE IS OBSOLETE
 *
 * There is pthread_rwlock support in libc, there is absolutely no need
 * to simulate those using pthread_mutex calls
 *
 * for example on pthread_rwlock, see aa.c
 *
 * agoston, 2008-01-22
 */

/***************************************
  $Revision: 1.7 $

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

/**************************************************
*                                                 *
*     Readers/Writers lock that favors READERS    *
*                (recursive!)                     *
*                                                 *
**************************************************/
void TH_acquire_read_lock(rw_lock_t * prw_lock)
{
	pthread_t thread_id = pthread_self();

	pthread_mutex_lock(&prw_lock->rw_mutex);

	/* if we got an rw lock, we can get read lock as well */
	while ((prw_lock->rw_count > 0) && !pthread_equal(prw_lock->thread_id, thread_id)) {
		pthread_cond_wait(&prw_lock->rw_cond, &prw_lock->rw_mutex);
	}
	prw_lock->r_count++;

	pthread_mutex_unlock(&prw_lock->rw_mutex);

}


void TH_release_read_lock(rw_lock_t * prw_lock)
{
	pthread_mutex_lock(&prw_lock->rw_mutex);

	if (prw_lock->r_count > 0)
		prw_lock->r_count--;

	pthread_cond_signal(&prw_lock->rw_cond);

	pthread_mutex_unlock(&prw_lock->rw_mutex);
}


void TH_acquire_write_lock(rw_lock_t * prw_lock)
{
	pthread_t thread_id = pthread_self();

	pthread_mutex_lock(&prw_lock->rw_mutex);

	while ((prw_lock->r_count > 0) || ((prw_lock->rw_count > 0) && !pthread_equal(prw_lock->thread_id, thread_id))) {
		pthread_cond_wait(&prw_lock->rw_cond, &prw_lock->rw_mutex);
	}

	prw_lock->rw_count++;
	prw_lock->thread_id = thread_id;
	pthread_mutex_unlock(&prw_lock->rw_mutex);
}



void TH_release_write_lock(rw_lock_t * prw_lock)
{
	pthread_t thread_id = pthread_self();

	pthread_mutex_lock(&prw_lock->rw_mutex);

	if (pthread_equal(prw_lock->thread_id, thread_id) && (prw_lock->rw_count > 0))
		prw_lock->rw_count--;

	if (!prw_lock->rw_count) prw_lock->thread_id = 0;

	pthread_cond_broadcast(&prw_lock->rw_cond);
	pthread_mutex_unlock(&prw_lock->rw_mutex);
}



void TH_init_read_write_lock(rw_lock_t * prw_lock)
{
	pthread_mutex_init(&prw_lock->rw_mutex, NULL);
	pthread_cond_init(&prw_lock->rw_cond, NULL);
	pthread_cond_init(&prw_lock->w_cond, NULL);
	pthread_cond_init(&prw_lock->r_cond, NULL);
	prw_lock->rw_count = 0;
	prw_lock->r_count = 0;
	prw_lock->thread_id = 0;
}

/*************************************************
*                                                *
*     Readers/Writes Locks that favor WRITERS    *
*                (recursive!)                    *
*                                                *
*************************************************/
void TH_acquire_read_lockw(rw_lock_t * prw_lock)
{
	pthread_t thread_id = pthread_self();

	pthread_mutex_lock(&prw_lock->rw_mutex);

	/* if we got an rw lock, we can get read lock as well */
	while ((prw_lock->rw_count > 0) && !pthread_equal(prw_lock->thread_id, thread_id)) {
		pthread_cond_wait(&prw_lock->rw_cond, &prw_lock->rw_mutex);
	}
	prw_lock->r_count++;

	pthread_mutex_unlock(&prw_lock->rw_mutex);
}



void TH_release_read_lockw(rw_lock_t * prw_lock)
{
	pthread_mutex_lock(&prw_lock->rw_mutex);

	if (prw_lock->r_count > 0)
		prw_lock->r_count--;

	if ((prw_lock->rw_count > 0) && (prw_lock->r_count == 0))
		pthread_cond_signal(&prw_lock->r_cond);

	pthread_mutex_unlock(&prw_lock->rw_mutex);
}



void TH_acquire_write_lockw(rw_lock_t * prw_lock)
{
	pthread_t thread_id = pthread_self();

	pthread_mutex_lock(&prw_lock->rw_mutex);

	if (pthread_equal(prw_lock->thread_id, thread_id)) {
		prw_lock->rw_count++;

	} else {
		/* check for writers */
		while (prw_lock->rw_count > 0) {
			pthread_cond_wait(&prw_lock->w_cond, &prw_lock->rw_mutex);
		}

		prw_lock->rw_count++;
		prw_lock->thread_id = thread_id;

		/* wait until all readers are gone */
		while (prw_lock->r_count > 0) {
			pthread_cond_wait(&prw_lock->r_cond, &prw_lock->rw_mutex);
		}
	}

	pthread_mutex_unlock(&prw_lock->rw_mutex);
}



void TH_release_write_lockw(rw_lock_t * prw_lock)
{
	pthread_mutex_lock(&prw_lock->rw_mutex);

	if (prw_lock->rw_count > 0)
		prw_lock->rw_count--;

	if (!prw_lock->rw_count) {
		prw_lock->thread_id = 0;
		/* signal to an other thread waiting for write lock first */
		pthread_cond_signal(&prw_lock->w_cond);
		/* signal to other threads waiting for read lock */
		pthread_cond_broadcast(&prw_lock->rw_cond);
	}

	pthread_mutex_unlock(&prw_lock->rw_mutex);
}



void TH_init_read_write_lockw(rw_lock_t * prw_lock)
{
	pthread_mutex_init(&prw_lock->rw_mutex, NULL);
	pthread_cond_init(&prw_lock->rw_cond, NULL);
	pthread_cond_init(&prw_lock->w_cond, NULL);
	pthread_cond_init(&prw_lock->r_cond, NULL);
	prw_lock->rw_count = 0;
	prw_lock->r_count = 0;
	prw_lock->thread_id = 0;
}


/*************************************************
*                                                *
*     Other functions                            *
*                                                *
*************************************************/


inline int TH_get_id(void) {

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
pthread_t TH_create(void *do_function(void *), void *arguments)
{
	pthread_t tid;
	pthread_attr_t attr;
	int ret;
	size_t ssize;

	/* Start a new thread. */
	pthread_attr_init(&attr);	/* initialize attr with default attributes */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_attr_getstacksize(&attr, &ssize);
	/* don't know if this stack size will be enough, but if ssize is set so low, we shouldn't set it more than absolutely needed */
	if (ssize < 262144) {
		pthread_attr_setstacksize(&attr, 262144);
	}

	/* And we shouldn't use more than 1M, because we will run out of virtual address space on 32 bit systems
	 * (note that each thread has its own stack address space) */
	if (ssize > 1048576) {
		pthread_attr_setstacksize(&attr, 1048576);
	}

	ret = pthread_create(&tid, &attr, do_function, arguments);
	if (ret != 0) {
		fprintf(stderr, "pthread_create(): %d: %s\n", errno, strerror(errno));
		die;
	}
	pthread_attr_destroy(&attr);

	return tid;
}

/* This function is called for macros 'die' and 'dieif', both defined in stubs.h
 * We do a simple stderr output which tells the position of the die macro,
 * then call gdb to do a complete backtrace (at least that's the intended
 * and default feature; it can be modified in rip.config:COMMAND_ON_DIE)
 *
 * Note that automatic gdb backtrace output is only available on linux, as
 * gettid() is linux-specific.
 */

#if defined(__linux__) && !defined(__NR_gettid)
#include <asm/unistd.h>
#endif

#if defined(__linux__) && defined(__NR_gettid)
#define GETTID() syscall(__NR_gettid)
#else
#define GETTID() getpid()
#endif

volatile int dying = 0;

void do_nice_die(int line, char *file) {
	if (!dying) {
		dying = 1;
		char *command = ca_get_command_on_die;
		fprintf(stderr," *** died: +%d %s\n",line, file);

		if (command) {
			char buf[1024];
			fprintf(stderr, " *** Backtrace:\n");
			snprintf(buf, 1024, command, GETTID());
			system(buf);
		}
	}
	/* that's the legacy way to bail out - it generates a segfault, so core can be dumped if needed */
	//*((int*)NULL)=0;

	// this is the more 1337 way to dump core
	(*orig_segfault_handler)(11);
}
