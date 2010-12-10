/***************************************
  $Revision: 1.12 $

  thread accounting (ta). ta.c - functions to keep track of activities
                            of threads within the server

  Status: NOT REVUED, TESTED, COMPLETE

  Design and implementation by: Marek Bukowy

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

#define TA_IMPL
#include "rip.h"


/*++++++++++++++++++++++++++++++++++++++

  Finds a thread by thread_id. Assumes locked list.

  ta_str_t *ta_findonly_l     returns a pointer to the thread's record

  GList **list                thread list

  pthread_t thread_id         thread id

  ++++++++++++++++++++++++++++++++++++++*/
static
ta_str_t *ta_findonly_l( GList **list, pthread_t thread_id )
{
  GList *item;

  /* try to find first */
  for(item = g_list_first(*list);
      item != NULL;
      item = g_list_next(item)) {
    ta_str_t *tas = (ta_str_t *) (item->data);

    if( tas->thread_id == thread_id ) {
      return tas;
    }
  }
  return NULL;
}


/*++++++++++++++++++++++++++++++++++++++

  Finds a thread by thread_id, or creates a new entry if there isn't one.
  Assumes locked list.

  ta_str_t *ta_findcreate_l   returns a pointer to the thread's record

  GList **list                thread list

  pthread_t thread_id         thread id

  ++++++++++++++++++++++++++++++++++++++*/
static
ta_str_t *ta_findcreate_l( GList **list, pthread_t thread_id )
{
  ta_str_t *newtas;

  if( (newtas = ta_findonly_l(list, thread_id)) == NULL) {
    
    /* not found => add */  /* zero everything*/
    newtas = (ta_str_t *)UT_calloc(1, sizeof(ta_str_t));
    newtas->thread_id = thread_id;
    
    *list = g_list_append( *list, newtas );
  }

  return newtas;
}



/*++++++++++++++++++++++++++++++++++++++
  
  finds and removes an entry for a thread given by thread_id.
  Assumes locked list.

  GList **list                thread list

  pthread_t thread_id         thread id

  ++++++++++++++++++++++++++++++++++++++*/
static
void ta_remove_l(GList **list, pthread_t thread_id )
{ 
 GList *item;

 for(item = g_list_first(*list);
     item != NULL;
     item = g_list_next(item)) {
   ta_str_t *tas = (ta_str_t *) (item->data);

   if( tas->thread_id == thread_id ) {
     *list = g_list_remove_link(*list, item);
     wr_clear_list( &item );
     break;
   }
 }
   
 return;
}


/*++++++++++++++++++++++++++++++++++++++
  
  sets the activity field in the given thread's record. 
  Truncates the string if too long.

  ta_str_t *tas        pointer to the thread's record

  char *activity       new value for the activity field
  ++++++++++++++++++++++++++++++++++++++*/
static
void ta_setactivity_l(ta_str_t *tas, char *activity)
{
  char *nl;

  strncpy(tas->activity, activity, TA_ACT_LEN-1);
  tas->activity[TA_ACT_LEN]=0;
  /* convert last newline to a space, if any */
  if( (nl=strrchr(tas->activity, '\n')) != NULL ) {
    *nl=' ';
  }
}


#define TA_HEADER "%-8s %15s %4s %4s %5s %5s %4s %6s %s\n"
#define TA_FORMAT "%-8s %15s %4d %4lu %5.1f %5.1f %4d %6.3f %s\n"


/*++++++++++++++++++++++++++++++++++++++
  
  prints a header for a list of threads to a specified buffer.
  The output is truncated if the buffer is too small.

  char *buf         pointer to the buffer

  unsigned length   buffer size
  ++++++++++++++++++++++++++++++++++++++*/
static 
void ta_print_header(char *buf, unsigned length)
{
  snprintf(buf, length, TA_HEADER,
	   "type", "from", "sock", "thr", "sess", "task", "#", 
	   "avg", "current"
	   ); 
}


/*++++++++++++++++++++++++++++++++++++++
  
  Formats the data from one thread's record to an entry for a list of
  threads.  Prints to a specified buffer. Calculates the average time
  spent per event as well as the lifetime of the thread. Checks the
  address of the peer on the socket. The output is truncated if the
  buffer is too small.
  
  ta_str_t *tas        pointer to the thread's record

  char *buf            pointer to the buffer
  
  unsigned length      buffer size
  
  ut_timer_t *reftime  current time 

  ++++++++++++++++++++++++++++++++++++++*/
static void ta_printone_l(ta_str_t *tas, char *buf, unsigned length, ut_timer_t *reftime) {
    ip_addr_t localaddr;
    char address[IP_ADDRSTR_MAX];
    float session, task; /* duration of the session/task */
    SK_getpeerip(tas->sock, &localaddr, &address);

    /* can be NULL for example if the socket has just closed
     or the file descriptor is not a socket */

    session = UT_timediff(&tas->sessionstart, reftime);
    task = UT_timediff(&tas->taskstart, reftime);

    snprintf(buf, length, TA_FORMAT ,
            tas->type,
            address ? address : "",
            tas->sock,
            (long unsigned)tas->thread_id,
            session,
            task,
            tas->tasks,
            (tas->tasks > 0) ? session / tas->tasks : 0,
            tas->activity);
}


/*++++++++++++++++++++++++++++++++++++++
  
  Public adding function - adds the current thread to the list,
  storing the given socket and type string along.

  int sock      associated socket (if any, or 0 if not)

  char *type    type string 
  
  ++++++++++++++++++++++++++++++++++++++*/
void TA_add(int sock, char *type)
{
  ta_str_t *newtas;
  
  /* lock the list */
  pthread_mutex_lock( &ta_mutex );
  
  /* find/create node and set peer/thread_id */
  newtas = ta_findcreate_l( &ta_list, pthread_self());
  newtas->sock = sock;
  newtas->tasks = 0;
  newtas->condat = NULL;
  UT_timeget( &newtas->sessionstart );
  UT_timeget( &newtas->taskstart ); /* just to get it a reasonable value */

  snprintf(newtas->type, TA_TYPE_LEN, type);
  ta_setactivity_l(newtas,"--");
  
  /* unlock */
  pthread_mutex_unlock( &ta_mutex );
}


/*++++++++++++++++++++++++++++++++++++++
  
  Public deletion function - deletes the current thread from the list.

  ++++++++++++++++++++++++++++++++++++++*/
void TA_delete(void)
{
  /* lock the list */
  pthread_mutex_lock( &ta_mutex );
  
  /* find & remove */
  ta_remove_l( &ta_list, pthread_self() );
  
  /* unlock */
  pthread_mutex_unlock( &ta_mutex );
}


/*++++++++++++++++++++++++++++++++++++++

  Public activity-setting function - sets the current activity string
  for the current thread.

  char *activity     new value
  
  ++++++++++++++++++++++++++++++++++++++*/
void TA_setactivity(char *activity)
{
  ta_str_t *newtas;

  /* lock the list */
  pthread_mutex_lock( &ta_mutex );
  
  /* find */
  newtas = ta_findcreate_l( &ta_list, pthread_self());
  
  /* set the activity field */
  ta_setactivity_l(newtas, activity);

  /* unlock */
  pthread_mutex_unlock( &ta_mutex );
}


/*++++++++++++++++++++++++++++++++++++++
  
  Public condat-setting function - associates a connection data
  structure with the current thread.

  sk_conn_st *condat    pointer to a connection data structure

  ++++++++++++++++++++++++++++++++++++++*/
void TA_setcondat(sk_conn_st *condat)
{
  ta_str_t *newtas;

  /* lock the list */
  pthread_mutex_lock( &ta_mutex );
  
  /* find */
  newtas = ta_findcreate_l( &ta_list, pthread_self());
  
  /* set the condat field */
  newtas->condat = condat;

  /* unlock */
  pthread_mutex_unlock( &ta_mutex );
}


/*++++++++++++++++++++++++++++++++++++++
  
   increments the event counter for the current thread.

  ++++++++++++++++++++++++++++++++++++++*/
void TA_increment(void)
{
  ta_str_t *newtas;

  /* lock the list */
  pthread_mutex_lock( &ta_mutex );
  
  /* find */
  newtas = ta_findcreate_l( &ta_list, pthread_self());
  /* increment task */
  newtas->tasks++;
  /* set task starting time */
  UT_timeget( &newtas->taskstart );

  /* unlock */
  pthread_mutex_unlock( &ta_mutex );
}


/*++++++++++++++++++++++++++++++++++++++
  
  resets the time and event counter of a specified thread.

  pthread_t thread_id     thread_id  

  ++++++++++++++++++++++++++++++++++++++*/
void TA_reset_counters(pthread_t thread_id)
{
  ta_str_t *tas;

  /* lock the list */
  pthread_mutex_lock( &ta_mutex );

  if( (tas = ta_findonly_l(&ta_list, thread_id)) != NULL) {
    UT_timeget( &tas->sessionstart );
    UT_timeget( &tas->taskstart ); /* just to get it a reasonable value */
    tas->tasks = 0;
  }

  pthread_mutex_unlock( &ta_mutex );
}


/*++++++++++++++++++++++++++++++++++++++
  
  Compiles a list of the threads' data records as text.

  char * TA_tostring      returns an allocated text, must be freed after use.

  ++++++++++++++++++++++++++++++++++++++*/
char * TA_tostring(void)
{
  GList *item;
  char *bigbuf = NULL;
  char smallbuf[TA_PRINT_LEN];
  ut_timer_t reftime;

  ta_print_header(smallbuf, TA_PRINT_LEN);
  bigbuf = (char *)UT_malloc(strlen(smallbuf)+2);
  strcpy(bigbuf, smallbuf);
  strcat(bigbuf, "\n");
  
  /* lock the list */
  pthread_mutex_lock( &ta_mutex );
  
  /* get reference time */
  UT_timeget( &reftime );
  
  /* iterate */
  for(item = g_list_first(ta_list);
      item != NULL;
      item = g_list_next(item)) {
    ta_str_t *tas = (ta_str_t *) (item->data);
    unsigned smalllen;
    unsigned biglen = ( bigbuf == NULL ) ? 0 : strlen(bigbuf);

    ta_printone_l(tas, smallbuf, TA_PRINT_LEN, &reftime);
    smalllen = strlen(smallbuf);

    bigbuf = (char *)UT_realloc(bigbuf, biglen+smalllen+3);
    
    strcat(bigbuf, smallbuf);
  }
  /* unlock */
  pthread_mutex_unlock( &ta_mutex );
  
  return bigbuf;
}



/*++++++++++++++++++++++++++++++++++++++
  
  Finds a thread of the matching type, socket file descriptor and thread id
  and executes the watchdog's triggers if it's defined.
  
  char *type                thread type string

  int sock                  socket #

  pthread_t thread_id       thread id

  ++++++++++++++++++++++++++++++++++++++*/
void TA_trigger(char *type, int sock, pthread_t thread_id)
{ 
  ta_str_t *tas;

  /* lock the list */
  pthread_mutex_lock( &ta_mutex );
  
  if( (tas = ta_findonly_l(&ta_list, thread_id)) != NULL
      && tas->sock == sock
      && strcmp(tas->type, type) == 0
      && tas->condat != NULL
      && tas->condat->sock == sock
      ) {
    SK_watchtrigger(tas->condat);
  }
  
  /* unlock */
  pthread_mutex_unlock( &ta_mutex );
  
}
