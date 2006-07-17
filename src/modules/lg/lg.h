/***************************************
  $Revision: 1.1 $

  Logging module.

  Status: REVIEWED, TESTED

 Author(s):       Tiago Antao

  ******************/ /******************
  Modification History:
        tiago (10/04/2003) Created.
  ******************/ /******************
  Copyright (c) 2003               RIPE NCC
 
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

#ifndef LG_H
#define LG_H

#include <stdio.h>
#include <pthread.h>
#include "glib.h"

typedef enum {
  LG_DEBUG  = 1,
  LG_INFO   = 2,
  LG_WARN   = 4,
  LG_ERROR  = 8,
  LG_SEVERE = 16,
  LG_FATAL  = 32,

  LG_FUNC   = 64,

  LG_ALL    = -1
} LG_level_t;

typedef struct LG_device {
  void (*destroy_func)(struct LG_device*);
  void (*write_func)(struct LG_device*, const char*, va_list);
  void* data;
} LG_device_t;  

typedef struct LG_formatter {
  //void (*destroy_func)(struct LG_formatter*);
  char* (*format_func)(const LG_level_t, void** data, const char*);
  void* data;
} LG_formatter_t;  


typedef struct {
  LG_level_t      level;
  LG_device_t*    device;
  LG_formatter_t* formatter;
} LG_appender_t;

typedef struct {
  GList* appenders;
  pthread_mutex_t lock;
} LG_context_t;

void LG_ctx_free(LG_context_t*);
LG_context_t* LG_ctx_new();
void LG_ctx_add_appender(LG_context_t*, LG_appender_t* );
void LG_ctx_lock(LG_context_t*);
void LG_ctx_unlock(LG_context_t*);


void LG_dev_free(LG_device_t*);
//LG_device* LG_dev_get_stdout();
LG_device_t* LG_dev_get_file(FILE*);
LG_device_t* LG_dev_daily(char* path);
void LG_dev_vfprintf(LG_device_t*, const char*, va_list);
void LG_dev_vasprintf(LG_device_t* dev, const char* message, va_list ap);

char* LG_frm_replicator(const LG_level_t, void** data, const char*);
LG_formatter_t* LG_frm_dbupdate_prepared();
LG_formatter_t* LG_frm_general_prepared(gchar* format_string);
char* LG_frm_dbupdate(const LG_level_t, void** data, const char*);
char* LG_frm_general(const LG_level_t, void** data, const char*);


void LG_app_free(LG_appender_t*);
void LG_app_set_level(LG_appender_t*, LG_level_t);
void LG_app_set_formatter(LG_appender_t*, LG_formatter_t*);
LG_appender_t* LG_app_get_file_info_dump(FILE*);
LG_appender_t* LG_app_get_filedes_dump(int fd);
LG_appender_t* LG_app_get_daily_info_dump(gchar* path);


void LG_log(LG_context_t*, LG_level_t, char*, ...);

void LG_set_module(const char* the_module);

/* ER legacy */

int NOERR(int a);
#define ERR(a) (!NOERR(a))

#endif /* LG_H */
