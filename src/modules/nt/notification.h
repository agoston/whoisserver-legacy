/***************************************
  $Revision: 1.1 $

  NT (Notifications) module

  Status: NOT REVIEWED, NOT TESTED

  Author(s):       Engin Gunduz

  ******************/ /******************
  Modification History:
        engin (06/07/2000) Created.
  ******************/ /******************
  Copyright (c) 2000                              RIPE NCC
 
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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H 


#include <stdarg.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <glib.h>
#include "rt.h"

#define ATTRIBUTE    0
#define MNTNER_OBJ   1
#define DIFF_BUFFER_SIZE 1025
#define DIFF_READ_SIZE 1024
/* TODO absolute path */
#define DIFF_COMMAND "/usr/bin/diff"

typedef struct {
      char *key; 
      RT_context_t *value;
} ctx_data_t;


/*  codes used to indicate type of notification message */ 
typedef enum
{
  UP_NOTIF=0, 
  UP_FRWD,
  UP_FRWD_CREATE,
  UP_FRWD_MODIFY,
  UP_FRWD_DELETE,
  UP_FRWD_POLICY,
  UP_ACK
} notif_codes;

/* convert the enum to a printable string */
#define UP_notif2str(r) \
  (((r) == UP_NOTIF)         ? "UP_NOTIF" : \
   ((r) == UP_FRWD)          ? "UP_FRWD" : \
   ((r) == UP_FRWD_CREATE)   ? "UP_FRWD_CREATE" : \
   ((r) == UP_FRWD_MODIFY)   ? "UP_FRWD_MODIFY" : \
   ((r) == UP_FRWD_DELETE)   ? "UP_FRWD_DELETE" : \
   ((r) == UP_FRWD_POLICY)   ? "UP_FRWD_POLICY" : \
   ((r) == UP_ACK)           ? "UP_ACK" : \
                               "UNKNOWN" )

/* convert the enum to a printable string for log file seperaters */
#define UP_notif2log_str(r) \
  (((r) == UP_NOTIF)         ? "NOTIF" : \
   ((r) == UP_FRWD)          ? "FRWD" : \
   ((r) == UP_FRWD_CREATE)   ? "FRWD CREATE" : \
   ((r) == UP_FRWD_MODIFY)   ? "FRWD MODIFY" : \
   ((r) == UP_FRWD_DELETE)   ? "FRWD DELETE" : \
   ((r) == UP_FRWD_POLICY)   ? "FRWD POLICY" : \
   ((r) == UP_ACK)           ? "ACK" : \
                               "UNKNOWN" )


/* function prototypes */

void NT_write_all_ntfs(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                             options_struct_t *options, source_data_t *source_data,
                             rpsl_object_t *postproc_obj, rpsl_object_t *old_obj,
                             GList *mntner_used );

void NT_write_all_frwds(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                             options_struct_t *options, source_data_t *source_data,
                             rpsl_object_t *postproc_obj, rpsl_object_t *old_obj,
                             GList *mntner_used  );

void NT_process_acknowledgement(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                                  options_struct_t *options);

void NT_process_notifications(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                                  options_struct_t *options, int type);

void NT_forw_create_req(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                             options_struct_t *options, char *operation,
                             rpsl_object_t *object, GList *credentials);

void NT_forw_policy_fail(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                             options_struct_t *options, char *operation,
                             rpsl_object_t *object, char *reason, 
                             GList *credentials);


#endif
