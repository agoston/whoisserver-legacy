#include <rip.h>
#include "mm.h"
#include <stdlib.h>
#include <string.h>
#include <c-client.h>
#include <signal.h>



MM_header_t* mm_header_new(const gchar* from, const gchar* subject,
  const gchar* date, const gchar* reply_to, const gchar* cc,
  const gchar* message_id);

MM_mail_info_t* mm_mail_info_new(const gchar* from, const gchar* subject,
  const gchar* date, const gchar* reply_to, const gchar* cc,
  const gchar* message_id) ;

MM_content_t* mm_explode_mime(BODY* body, char* cnt);

void mm_content_free(MM_content_t* content);

void mm_header_free(MM_header_t* header);
