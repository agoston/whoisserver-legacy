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

#include <rip.h>
#include <sys/time.h>
#include <assert.h>

/*
LG_device_t* LG_dev_get_stdout() {
  LG_device_t* dev;

  dev = UT_malloc(sizeof(LG_device_t));
  dev->destroy_func = NULL;
  dev->write_func = &vprintf;
  
  return dev;
}
*/

LG_device_t* LG_dev_get_file(FILE* f) {
  LG_device_t* dev;

  dev = g_malloc(sizeof(LG_device_t));
  dev->destroy_func = NULL;
  dev->write_func = &LG_dev_vfprintf;
  dev->data = (void *)f;

  return dev;
}

LG_device_t* LG_dev_get_filedes(int fd) {
  LG_device_t* dev;

  dev = g_malloc(sizeof(LG_device_t));
  dev->destroy_func = NULL;
  dev->write_func = &LG_dev_vasprintf;
  dev->data = (void *)fd;

  return dev;
}

/** this function is only intended to be used with stdout or stderr!!! */
static pthread_mutex_t LG_dev_vfprintf_lock = PTHREAD_MUTEX_INITIALIZER;
void LG_dev_vfprintf(LG_device_t* dev, const char* message, va_list ap) {
  FILE* f;
  f = (FILE*) dev->data;
  
  pthread_mutex_lock(&LG_dev_vfprintf_lock);
  vfprintf(f, message, ap);
  fflush(f);
  pthread_mutex_unlock(&LG_dev_vfprintf_lock);
}

/** writes to a file descriptor, using a buffer. Locks the end of the file. */
void LG_dev_vasprintf(LG_device_t* dev, const char* message, va_list ap) {
	char *buf;
	int len;
	int fd = (int)dev->data;
	off_t currpos;
	
	len = vasprintf(&buf, message, ap);
	currpos = lseek(fd, 0, SEEK_END);
	lockf(fd, F_LOCK, 0);
	write(fd, buf, len);
	lseek(fd, currpos, SEEK_SET);
	lockf(fd, F_ULOCK, 0);
	free(buf);
}


LG_formatter_t* LG_frm_get_replicator() {
  LG_formatter_t* frm;

  frm = g_malloc(sizeof(LG_formatter_t));
  frm->format_func = &LG_frm_replicator;
  frm->data = NULL;

  return frm;
}

static char module[400];
LG_formatter_t* LG_frm_dbupdate_prepared() {
  LG_formatter_t* frm;
  int* indent;

  module[0] = 0;

  frm = g_malloc(sizeof(LG_formatter_t));
  frm->format_func = &LG_frm_dbupdate;
  frm->data = g_malloc(sizeof(int));
  indent = frm->data;
  *indent = 0;

  return frm;
}

LG_formatter_t* LG_frm_general_prepared(gchar* format_string) {
  LG_formatter_t* frm;

  frm = g_malloc(sizeof(LG_formatter_t));
  frm->format_func = &LG_frm_general;
  frm->data = g_strdup(format_string);

  return frm;
}

char* LG_frm_replicator(const LG_level_t level, void** data, const char* str) {
  gchar* ret;
  int    len;

  len = strlen(str);
  ret = g_malloc(len+1+1);
  memcpy(ret, str, len+1);
  ret[len] = '\n';
  ret[len+1] = 0;

  return ret;
}

gchar *stresc (const gchar *s)
{
  gchar *ret_val;
  gchar *p;
  
  ret_val = g_new(char, (strlen(s) * 4) + 1);
  p = ret_val;
  while (*s) 
  {
    if (isprint((int)*s)) 
    {
      *p = *s;
      p++;
    } 
    else 
    {
      sprintf(p, "\\x%02X", (*s & 0xFF));
      p += 4;
    }
    s++;
  }
  *p = '\0';
  return g_renew(char, ret_val, p - ret_val + 1);
}

char* LG_frm_general(const LG_level_t level, void** data, const char* str) {
  GString*     mystr;
  gchar*       ret;
  gchar*       frm;
  struct timeval curr_time;
  struct tm    ltime;
  gchar        buffer[100];
  gchar*       esc_str;
  int          return_val;

  frm = (char*)*data;
  mystr = g_string_new("");
  return_val = gettimeofday(&curr_time, NULL);
  assert( return_val == 0 );
  localtime_r(&curr_time.tv_sec, &ltime);

  while(*frm) {
    if(*frm == '$') {
      frm++;
      if (strstr(frm, "MESSAGE") == frm) {
        esc_str = stresc(str);
        g_string_append(mystr, esc_str);
        g_free(esc_str);
        frm += 7;
      }
      else if (strstr(frm, "TIMESTAMP") == frm) {
        sprintf(buffer, "%04d%02d%02d %02d:%02d:%02d", 
                ltime.tm_year+1900, ltime.tm_mon+1, ltime.tm_mday,
                ltime.tm_hour, ltime.tm_min, ltime.tm_sec);
        frm +=9;
        g_string_append(mystr, buffer);
      }
      else if (strstr(frm, "TIME") == frm) {
        sprintf(buffer, "%02d:%02d:%02d", 
                ltime.tm_hour, ltime.tm_min, ltime.tm_sec);
        frm +=4;
        g_string_append(mystr, buffer);
      }
      else if (strstr(frm, "DATE") == frm) {
        sprintf(buffer, "%04d-%02d-%02d", 
                ltime.tm_year+1900, ltime.tm_mon+1, ltime.tm_mday);
        frm +=4;
        g_string_append(mystr, buffer);
      }
      else if (strstr(frm, "PID") == frm) {
        sprintf(buffer, "%d", (int)getpid());
        g_string_append(mystr, buffer);
        frm +=3;
      }
      else if (strstr(frm, "TID") == frm) {
        sprintf(buffer, "%lu", (unsigned long)pthread_self());
        g_string_append(mystr, buffer);
        frm +=3;
      }
      else if (strstr(frm, "SEVERITY") == frm) {
        switch (level) {
        case LG_DEBUG:
          g_string_append(mystr, "DEBUG");
          break;
        case LG_INFO:
          g_string_append(mystr, "INFO");
          break;
        case LG_WARN:
          g_string_append(mystr, "WARN");
          break;
        case LG_ERROR:
          g_string_append(mystr, "ERROR");
          break;
        case LG_SEVERE:
          g_string_append(mystr, "SEVERE");
          break;
        default:
          g_string_append(mystr, "UNKNOWN");
        }
        frm += 8;
      }
      else if (strstr(frm, "SEV") == frm) {
        switch (level) {
        case LG_DEBUG:
          g_string_append(mystr, "D");
          break;
        case LG_INFO:
          g_string_append(mystr, "I");
          break;
        case LG_WARN:
          g_string_append(mystr, "W");
          break;
        case LG_ERROR:
          g_string_append(mystr, "E");
          break;
        case LG_SEVERE:
          g_string_append(mystr, "S");
          break;
        default:
          g_string_append(mystr, "U");
        }
        frm += 3;
      }
      else {
        g_string_append_c(mystr, '$');
      }
    }
    else {
      g_string_append_c(mystr, *frm);
      frm++;
    }
  }

  ret = mystr->str;
  g_string_free(mystr, FALSE);
  return ret;
}

void LG_set_module(const char* the_module) {
//not thread (or context btw) safe --> enough for dbupdate
  sprintf(module, "%s", the_module);
}

char* LG_frm_dbupdate(const LG_level_t level, void** data, const char* str) {
  char* ret;
  int indent_level;
  int current_indent;
  int i;
  GString* mystr;

  mystr = g_string_new(module);
  if (mystr->len) {
    g_string_append_c(mystr, ' ');
  }
  indent_level = *(int*)*data;
  current_indent = indent_level;
  if (str[0] == '>') {
    indent_level++;
    str++;
    current_indent++;
    if (level == LG_FUNC) {
      g_string_append_c(mystr, '\n');
    }
  }
  else if (str[0] == '<') {
    indent_level--;
    str++;
  }

  for (i=0; i<current_indent; i++) {
      g_string_append_c(mystr, ' ');
  }
  
  g_string_append(mystr, str);
  g_string_append_c(mystr, '\n');
  if (current_indent != indent_level && level == LG_FUNC) {
    g_string_append_c(mystr, '\n');
  }

  ret = mystr->str;
  g_string_free(mystr, FALSE);
  *(int*)*data = indent_level;

  return ret;
}



/*
LG_appender_t* LG_app_get_stdout_info_dump() {
  LG_appender_t* app;

  app = UT_malloc(sizeof(LG_appender_t));
  app->level = LG_INFO;
  app->device = LG_dev_get_stdout();
  app->formatter = LG_frm_get_replicator();

  return app;
}
*/

void LG_dev_vfprintf_daily(LG_device_t* dev, const char* message, va_list ap) {
  gchar* path;
  gchar* fname;
  int fint;
  time_t       clock;
  struct tm    the_time;
  gchar *log_message;

  path = (gchar*) dev->data;

  fname = g_malloc(strlen(path)+1+8+1);
  clock = time(NULL);
  localtime_r(&clock, &the_time);
  sprintf(fname, "%s.%04d%02d%02d", path, the_time.tm_year+1900,
    the_time.tm_mon+1, the_time.tm_mday);

  fint=open(fname,O_CREAT|O_WRONLY|O_APPEND, 0666);
  if (fint == -1) {
    fprintf(stderr,"opening %s ",fname);
    perror("failed");
    die;
  }

  lockf(fint, F_LOCK, 0);
  lseek(fint, 0, SEEK_END);
  log_message=g_strdup_vprintf(message, ap);
  write(fint,log_message,strlen(log_message));
  lockf(fint, F_ULOCK, 0);
  close(fint);
  g_free(log_message);
  g_free(fname);
}

void LG_dev_daily_destroy(LG_device_t* dev) {
  gchar* path;

  path = (gchar*)dev->data;
  g_free(path);
}

LG_device_t* LG_dev_daily(char* path) {
  LG_device_t* dev;
  gchar*       clean_path;

  /* This is an hack because of the CA module 8-(,
     its simpler this way.


  if (clean_path[strlen(path)-1] < 32) {
    clean_path[strlen(path)-1] = 0;
  }
  */
  clean_path  = g_strdup(path);  //TB removed (maybe)

  dev = g_malloc(sizeof(LG_device_t));
  dev->destroy_func = LG_dev_daily_destroy;
  dev->write_func = &LG_dev_vfprintf_daily;
  dev->data = (void *)clean_path;

  return dev;
}

LG_appender_t* LG_app_get_daily_info_dump(gchar* path) {
  LG_appender_t* app;

  app = g_malloc(sizeof(LG_appender_t));
  app->level = LG_INFO+LG_WARN+LG_ERROR+LG_SEVERE;
  app->device = LG_dev_daily(path);
  app->formatter = LG_frm_get_replicator();

  return app;
}

/** You MUSTN'T USE this for anything else than stdout or stderr!!! */ 
LG_appender_t* LG_app_get_file_info_dump(FILE* f) {
  LG_appender_t* app;

  app = g_malloc(sizeof(LG_appender_t));
  app->level = LG_INFO+LG_WARN+LG_ERROR+LG_SEVERE;
  app->device = LG_dev_get_file(f);
  app->formatter = LG_frm_get_replicator();

  return app;
}

/** this is just a normal appender, currently only used by dbupdate */
LG_appender_t* LG_app_get_filedes_dump(int fd) {
  LG_appender_t* app;

  app = g_malloc(sizeof(LG_appender_t));
  app->level = LG_INFO+LG_WARN+LG_ERROR+LG_SEVERE;
  app->device = LG_dev_get_filedes(fd);
  app->formatter = LG_frm_get_replicator();

  return app;
}

