#ifndef _MM_H_
#define _MM_H_

#include <stdio.h>
#include <glib.h>

typedef enum {
  MM_PLAIN,
  MM_MULTIPART_ALTERNATIVE,
  MM_MULTIPART_SIGNED,
  MM_MULTIPART_MIXED,
  MM_SIGNATURE,
  MM_MESSAGE
} MM_type;

typedef struct {
  gchar* from;
  gchar* subject;
  gchar* date;
  gchar* reply_to;
  gchar* cc;
  gchar* message_id;
} MM_header_t;

typedef struct {
  MM_type type;
  gchar* content;
  gchar* bulk_content;
  GList* parts;
} MM_content_t;

typedef struct {
  MM_header_t* header;
  MM_content_t* content;
} MM_mail_info_t;

void MM_init(LG_context_t* ctx);
MM_mail_info_t* MM_extract_mail_info(const gchar*);
const gchar* MM_mail_info_header(MM_mail_info_t*, const gchar* header);
void MM_mail_info_free(MM_mail_info_t*);

/* Should never be used after a free_mail_info associated with part
MM_part_info_t* MM_mail_info_extract_part_info(MM_mail_info_t*);
const gchar* MM_part_info_to_char(MM_part_info_t*);
int MM_part_info_size(MM_part_info_t*);
gchar* MM_part_info_type(MM_part_info_t*);
MM_part_info_t* MM_part_info_child(MM_part_info_t*, int pos);
void MM_part_info_free(MM_part_info_t*);
*/


#endif
