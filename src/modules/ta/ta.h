/******************
  Copyright (c) 2002                              RIPE NCC

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

#ifndef TA_H
#define TA_H

#include <glib.h>
#include <pthread.h>
#include <stdio.h>

#include "stubs.h"
#include "memwrap.h"
#include "sk.h"
#include "timediff.h"

/* thread activity monitor */
#define TA_TYPE_LEN 16
#define TA_ACT_LEN 256
#define TA_PRINT_LEN (TA_ACT_LEN+64)

typedef struct 
{  
  pthread_t  thread_id;                /* thread id */  
  ut_timer_t sessionstart;             /* time the session started */
  ut_timer_t taskstart;                /* time the last task started */
  int        sock;                     /* socket */
  sk_conn_st *condat;                  /* sk's connection data struct */
  char       type[TA_TYPE_LEN];
  char       activity[TA_ACT_LEN];     /* current activity (eg query) */
  int        tasks;                   /* number of activities(used to calculate the average) */
} ta_str_t;


#ifdef TA_IMPL
/* GLOBALs (private to the module)*/
GList *ta_list = NULL;
pthread_mutex_t ta_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* prototypes */
void TA_add(int  sock, char *type);
void TA_delete(void); 
void TA_setactivity(char *activity);
void TA_setcondat(sk_conn_st *condat);
char * TA_tostring(void);
void TA_increment(void);
void TA_trigger(char *type, int sock, pthread_t thread_id);
void TA_reset_counters(pthread_t thread_id);

#ifdef __cplusplus
}
#endif


#endif

