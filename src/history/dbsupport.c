/***************************************
  $Revision: 1.1 $

  DB support.  dbsupport.c - SQL utility functions.

  Status: NOT REVIEWED, NOT TESTED, NOT COMPLETE

  Implementation by: Tiago Antao

  ******************/ /******************
  Copyright (c) 2002               RIPE NCC
 
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


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <rip.h>

SQ_connection_t* RIPE_conn;
SQ_connection_t* archive_conn;

/*
  prepare_string_attribute: Prepares a string to be SQL inserted.
*/
void prepare_string_attribute(char* dirty, char* clean){
  //printf("STRL: %d", strlen(dirty));
  while (*dirty) {
    if (*dirty == 39 || *dirty == 92) { // ' | \
      //      printf("DIRTY\n");
      *clean = *dirty;
      clean++;
    }
    if (*dirty == 10) { // \n
      *clean = '\\';
      clean++;
      *clean = 'n';
    }
    else {
      *clean = *dirty;
    }
    clean++;
    dirty++;
  }
  *clean = 0;
}


/*
  check_null: Checks if an attribute is NULL

  Technicaly this is wrong, as the content can be NULL (string)
*/
void check_null(SQ_result_set_t* rs, SQ_row_t* row,
		int attribute, char* result) {
  if (SQ_get_column_string_nocopy(rs,row,attribute) == NULL) {
    strcpy(result, "NULL");
  }
  else {
    strcpy(result, SQ_get_column_string_nocopy(rs,row,attribute));
  }
}


