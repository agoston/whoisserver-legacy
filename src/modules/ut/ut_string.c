/***************************************
  $Revision: 1.9 $

  string utilities (ut).

  Status: NOT REVUED, NOT TESTED

  ******************/ /******************
  Filename            : stringutil.c
  Author              : marek@ripe.net
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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SPACE 0
#define WORD  1

/*+
   replacement g_strsplit function, to emulate its v1 behaviour.
+*/

/*
 * DEPRECATED
 * Wrapper for g_strsplit in glib v1
 */
gchar **ut_g_strsplit_v1(gchar *string, gchar *delimiter, gint max_tokens) {
    gchar **v2_result;
    gchar *string_trimmed;

    int len = strlen(string);
    if (string[len - 1] == delimiter[0]) {
        string_trimmed = strdup(string);
        string_trimmed[len - 1] = 0;
        v2_result = g_strsplit(string_trimmed, delimiter, max_tokens);
        g_free(string_trimmed);
    } else {
        v2_result = g_strsplit(string, delimiter, max_tokens);
    }

    return v2_result;
}

/*+
  compress by removing leading/trailing spaces and compressing multiple
  whitespaces to one.

  allocates a string and returns it - it must be freed
+*/


/*++++++++++++++++++++++++++++++++++++++
char *
ut_string_compress    removes leading/trailing whitespaces and compresses
                      multiple whitespaces to one. The result is an allocated
		      string and must be freed.

char *input           source string
  +++++++++++++++++++++++++++++++++++++*/
char *
ut_string_compress(char *input)
{
  char *inchr;
  char *copy = UT_malloc(strlen(input)+1);
  char *outchr = copy;
  char *lastchr;
  int nowexpected = WORD; /* skip initial spaces */

  /* find trailing space */
  lastchr = strchr(input, '\0');
  while( lastchr != input ) {
    lastchr--;
    if( !isspace((int)*lastchr) ) {
      break;
    }
  }

  /* copy the string word by word, inserting spaces */
  inchr=input;
  do {
    unsigned char ch = *inchr;

    if( isspace(ch) ) {
      if( nowexpected == WORD ) {
	continue; /* skip this whitespace */
      }
      else {
	ch = ' '; /* convert to a plain space */
	nowexpected = WORD; /* ignore any more spaces */
      }
    }
    else {
      nowexpected = SPACE;
    }

    *outchr = ch;
    outchr++;
  } while(inchr++ != lastchr);

  *outchr = '\0';

  return copy;
}

