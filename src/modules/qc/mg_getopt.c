/*
 * Copyright (c) 1987, 1993, 1994
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/* This code has been modified from the original UC version by 
   Marek Bukowy, RIPE NCC
   to be thread-safe
*/

#include "rip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

getopt_state_t *
mg_new(int optind)
{
  getopt_state_t *state;

  if( (state=UT_calloc(1, sizeof(getopt_state_t))) != NULL ) {
    state->place = EMSG;
    state->optind = optind;
    state->optreset = 1;
  }
  return state;
}


/*
 * getopt --
 *      Parse argc/argv argument vector.
 */
int
mg_getopt( int            nargc,
	   char * const   *nargv,
	   const char     *ostr,
	   getopt_state_t *state)
{
  char *oli;                              /* option letter list index */

  if (state->optreset || !*state->place) {              /* update scanning pointer */
    state->optreset = 0;

    /* handle wrongly tokenised space (grrrrr...) - skip empty fields */
    while(state->optind < nargc &&  *(nargv[state->optind]) == '\0') {
      state->optind++;
    }
    if (state->optind >= nargc || *(state->place = nargv[state->optind]) != '-') {
      state->place = EMSG;
      return (-1);
    }
    if (state->place[1] && *++(state->place) == '-') {      /* found "--" */
      ++state->optind;
      state->place = EMSG;
      return (-1);
    }
  }                                       /* option letter okay? */
  if ((state->optopt = (int)*state->place++) == (int)':' ||
      !(oli = strchr(ostr, state->optopt))) {
    /*
     * if the user didn't specify '-' as an option,
     * assume it means -1.
     */
    if (state->optopt == (int)'-')
      return (-1);
    if (!*state->place)
      ++state->optind;
    return (BADCH);
  }
  if (*++oli != ':') {                    /* don't need argument */
    state->optarg = NULL;
    if (!*state->place)
      ++state->optind;
  }
  else {                                  /* need an argument */
    if (*state->place)                     /* no white space */
      state->optarg = state->place;
    else if (nargc <= ++state->optind) {   /* no arg */
      state->place = EMSG;
      if (*ostr == ':')
	return (BADARG);
      
      return (BADCH);
    }
    else                            /* white space */
      state->optarg = nargv[state->optind];
    state->place = EMSG;
    ++state->optind;
  }
  return (state->optopt);                        /* dump back option letter */
}
