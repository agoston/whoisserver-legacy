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
#include <stdio.h>

void LG_ctx_lock(LG_context_t* ctx) {
  pthread_mutex_lock(&(ctx->lock));
}

void LG_ctx_unlock(LG_context_t* ctx) {
  pthread_mutex_unlock(&(ctx->lock));
}

void LG_ctx_free(LG_context_t* ctx) {
  LG_appender_t* app;
  GList* l;

  l = g_list_first(ctx->appenders);
  while (l != NULL) {
    app = (LG_appender_t*) l->data;
    LG_app_free(app);
    l = g_list_next(l);
  }

  g_list_free(ctx->appenders);
  pthread_mutex_destroy(&(ctx->lock));
}

void LG_ctx_add_appender(LG_context_t* ctx, LG_appender_t* app) {
  ctx->appenders = g_list_prepend(ctx->appenders, app);
}

LG_context_t* LG_ctx_new() {
  LG_context_t* ctx;

  ctx = g_malloc(sizeof(LG_context_t));
  ctx->appenders = NULL;
  pthread_mutex_init(&(ctx->lock), NULL);

  return ctx;
}


void LG_dev_free(LG_device_t* dev) {
  if (dev->destroy_func) {
    dev->destroy_func(dev);
  }

  /* The following g_free() looks like unnecessary, because
     LG_dev_daily_destroy g_free()s dev->data (This is done in
     dev->destroy_func(dev) ).  So I will comment the below g_free(). EG 20031020 */
  /* g_free(dev->data);*/
  g_free(dev);
}

void LG_frm_free(LG_formatter_t* frm) {
  if (frm->data) {
    g_free(frm->data);
  }
  g_free(frm);
}

void LG_app_free(LG_appender_t* app) {
  LG_dev_free(app->device);
  LG_frm_free(app->formatter);
  g_free(app);
}

void LG_app_set_level(LG_appender_t* app, LG_level_t level) {
  app->level = level;
}

void LG_app_set_formatter(LG_appender_t* app, LG_formatter_t* frm) {
  if (app->formatter) {
    LG_frm_free(app->formatter);
  }
  app->formatter = frm;
}


void LG_log(LG_context_t* ctx, LG_level_t level, char* message, ...) {
  va_list ap;
  LG_appender_t* app;
  char* formated_message;
  GList* l;

  pthread_mutex_lock(&(ctx->lock));
  l = g_list_first(ctx->appenders);
  while (l != NULL) {
    app = (LG_appender_t*) l->data;
    if (app->level & level) {
      formated_message = app->formatter->format_func(level, &(app->formatter->data), message);
      va_start(ap, message);
      app->device->write_func(app->device, formated_message, ap);
      va_end(ap);
      g_free(formated_message);
    }
    l = g_list_next(l);
  }
  pthread_mutex_unlock(&(ctx->lock));

}

/* ER legacy */


int NOERR(int a) {
  return ((a & 0xFFFF) == 0 );          /* the error part is 0 */
}


