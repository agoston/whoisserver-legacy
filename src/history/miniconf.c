/***************************************
  $Revision: 1.1 $

  Miniconf.  miniconf.c - functions to deal with simple configuration files.

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
#include <string.h>
#include <rip.h>
#include "miniconf.h"

/*
  miniconf_read - Reads a configuration file and returns its contents

  The order of the parameters in the file _must_ match the order in
  the conf structure.

  Returns 1 if OK, 0 if not OK

  NOTE: completely initialize conf (ie malloc _everything_!) before
  calling this function.
 */
int miniconf_read(miniconf_conf *conf, int conf_size, char *file){
  FILE* f;
  char  line[160];
  char* space_pos;
  int   current_conf;

  current_conf = 0;
  f = fopen(file, "r");

  if (f == NULL) {
    return 0;
  }
  while (fgets(line, 160, f) != NULL && current_conf<conf_size) {
    if (line[0] != '#') {
      space_pos = index(line, ' ');
      if (space_pos == NULL) {
	return 0;
      }
      if (strncmp(line, conf[current_conf].name, space_pos - line) != 0) {
        return 0;
      }
      if (strlen(space_pos + 1) >= conf[current_conf].max_size) {
        return 0;
      }
      strcpy(conf[current_conf].value, space_pos + 1);
      conf[current_conf].value[strlen(space_pos + 1) - 1] = 0;
      current_conf++;
    }
  }

  return 1;
}

void miniconf_create_param(miniconf_conf* conf, char* name, int size) {
  conf->name = UT_malloc(strlen(name) + 1);
  strcpy(conf->name, name);
  conf->max_size = size;
  conf->value = UT_malloc(size);
}
