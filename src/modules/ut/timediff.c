/******************
  Copyright (c) 2001                              RIPE NCC
 
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

/* difference between two times obtained with UT_store_time */

float UT_timediff( ut_timer_t *begintime, ut_timer_t *endtime )
{
  return ( endtime->tv_sec - begintime->tv_sec ) +
    1e-6 * ( endtime->tv_usec - begintime->tv_usec ) ;
}

void
UT_timeget(ut_timer_t *timer)
{
  gettimeofday( timer, NULL );
}

float UT_time_getvalue(ut_timer_t *timer){
  return timer->tv_sec + timer->tv_usec * 1e-6;
}

void UT_time_set(ut_timer_t *timer, long seconds, long useconds){
  timer->tv_sec = seconds;
  timer->tv_usec = useconds;
}
