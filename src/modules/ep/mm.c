#include "ep_internal.h"
#include "mm_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

LG_context_t* mm_ctx;

/*
 Initiates the MM module.
 */
void MM_init(LG_context_t* ctx) {
    //mail_link (&unixdriver);
    mail_link(&mmdfdriver);
    mm_ctx = ctx;
}

/*
 Extracts general mail and MIME information from a mail.

 stream - Mail in string format.

 return - Structured information about the mail.

 This is the fundamental entry point from the exterior
 (via ep_explode and ep_mail_driver).
 */
MM_mail_info_t *MM_extract_mail_info(const gchar *stream) {

    MAILSTREAM *mail_stream;
    BODY *body;
    //  STRINGLIST *cur;
    MM_mail_info_t *ret;
    //  long len;
    ENVELOPE *env;
    char *from = NULL;
    char *temp = NULL;
    char *header;
    char *date;
    char *reply_to;
    char *cc;
    char *message_id;
    char *t_header;
    char *subject;
    char *contents = NULL;
    char *tmp_file_name;
    char *tmp_file_prefix;
    int tmp_file_handle;
    int tmp_file_name_len;
    gchar ones[] = { 1, 1, 1, 1, '\n' };
    STRINGLIST list;
    gchar From[] = { 'F', 'r', 'o', 'm' };

    LG_log(mm_ctx, LG_FUNC, ">MM_extract_mail_info: entered");

    mail_stream = NIL;

    tmp_file_prefix = strdup("MM_extract_mail_info_temp_file");
    tmp_file_name_len = strlen(ep_temporary_directory) + strlen(tmp_file_prefix) + 256;
    tmp_file_name = (char *) malloc(tmp_file_name_len);
    g_snprintf(tmp_file_name, tmp_file_name_len, "%s/%s.%d.%d", ep_temporary_directory, tmp_file_prefix, time(NULL), (int) getpid());

    LG_log(ep_ctx, LG_DEBUG, "MM_extract_mail_info: using temporary file name: %s", tmp_file_name);
    tmp_file_handle = open(tmp_file_name, O_CREAT | O_EXCL | O_RDWR, 0644);
    if (tmp_file_handle == -1) {
        LG_log(mm_ctx, LG_FATAL, "MM_extract_mail_info: could not open temp file");
        LG_log(mm_ctx, LG_FUNC, "<MM_extract_mail_info: exiting with status EP_FATAL");
        ep_retval = EP_FATAL;
        return NULL;
    }

    free(tmp_file_prefix);

    /* write mail to temp file */
    /* Why do we do this??? EG */
    write(tmp_file_handle, ones, 5);
    write(tmp_file_handle, (char*) stream, (long) strlen(stream));
    close(tmp_file_handle);

    /* open mail stream in temp file */
    LG_log(mm_ctx, LG_DEBUG, "MM_extract_mail_info: Opening mail stream (c-client)");
    mail_stream = mail_open(NULL, tmp_file_name, NIL);
    if (mail_stream == NULL) {
        LG_log(ep_ctx, LG_ERROR, "MM_extract_mail_info: mail_open failed! Will bail out.");
        die;
    }
    unlink(tmp_file_name);
    free(tmp_file_name);
    if (!mail_stream) {
        LG_log(mm_ctx, LG_FATAL, "MM_extract_mail_info: mail_open failed");
        LG_log(mm_ctx, LG_FUNC, "<MM_extract_mail_info: exiting with status EP_FATAL");
        ep_retval = EP_FATAL;
        return NULL;
    }

    list.text.data = (unsigned char*) From;
    list.text.size = 4;
    list.next = NULL;

    /* get mail data from the mail stream */
    LG_log(mm_ctx, LG_DEBUG, "MM_extract_mail_info: fecthing structure from stream (c-client)");
    env = mail_fetch_structure(mail_stream, 1, &body, NIL);
    LG_log(mm_ctx, LG_DEBUG, "MM_extract_mail_info: body->type [%d] body->subtype [%s]", body->type, body->subtype);
    LG_log(mm_ctx, LG_DEBUG, "MM_extract_mail_info: fecthing header from stream (c-client)");
    header = mail_fetch_header(mail_stream, 1, NIL, &list, NIL, NIL);

    if (header && strlen(header) > 6) {
        from = g_malloc(strlen(header) + 1);
        temp = from;
        t_header = header;
        header += 6;
        while (*header != 0 && *header != '\n' && *header != '\r') {
            *temp = *header;
            header++;
            temp++;
        }
        *temp = 0;
        LG_log(mm_ctx, LG_DEBUG, "MM_extract_mail_info: from [%s]", from);
    } else {
        from = NULL;
    }

    /*
     from = g_malloc (strlen(env->sender->mailbox) + strlen(env->sender->host) + 2);
     sprintf(from, "%s@%s", env->sender->mailbox, env->sender->host);
     */
    subject = strdup(env->subject ? env->subject : "");
    date = strdup((char*) env->date ? (char*) env->date : "");
    message_id = strdup(env->message_id ? env->message_id : "");
    cc = NULL;
    if (env->cc) {
        cc = g_malloc(strlen(env->cc->mailbox) + (env->cc->host ? strlen(env->cc->host) : 0) + 2);
        sprintf(cc, "%s@%s", env->cc->mailbox, (env->cc->host ? env->cc->host : "" ));
    }
    reply_to = NULL;
    if (env->reply_to) {
        reply_to = g_malloc(strlen(env->reply_to->mailbox) + (env->reply_to->host ? strlen(env->reply_to->host) : 0) + 2);
        sprintf(reply_to, "%s@%s", env->reply_to->mailbox,
                (env->reply_to->host ? env->reply_to->host : "" ));
    }

    LG_log(mm_ctx, LG_DEBUG, "MM_extract_mail_info: fecthing content from stream (c-client)");
    contents = mail_fetchtext(mail_stream, 1);
    LG_log(mm_ctx, LG_DEBUG, "MM_extract_mail_info: contents [%s]", contents ? contents : "NULL");

    ret = mm_mail_info_new(from, subject, date, reply_to, cc, message_id);

    LG_log(mm_ctx, LG_DEBUG, "MM_extract_mail_info: Exploding MIME(MM)");
    ret->content = mm_explode_mime(body, contents);

    /* for some reason, mail_close() call causes a segfault. Since dbupdate is restarted for every update message,
     * it is acceptable to not free memory allocated by c_client. We also don't plan on keeping MM module in place
     * much longer, so it is not worth the fuss - agoston, 2011-03-17

    LG_log(mm_ctx, LG_DEBUG, "MM_extract_mail_info: closing mail stream");
    mail_close(mail_stream);
    LG_log(mm_ctx, LG_DEBUG, "MM_extract_mail_info: mail stream closed");
    */

    if (from) {
        g_free(from);
    }
    free(subject);
    free(date);
    free(message_id);
    if (cc) {
        g_free(cc);
    }
    if (reply_to) {
        g_free(reply_to);
    }
    LG_log(mm_ctx, LG_FUNC, "<MM_extract_mail_info: exiting");
    return ret;
}

/*
 Releases a mail_info structure.

 mail - Structure to be relasead.
 */
void MM_mail_info_free(MM_mail_info_t* mail) {
    mm_header_free(mail->header);
    if (mail->content) {
        mm_content_free(mail->content);
    }
    g_free(mail);
}

/*
 Creates a mail_info structure.

 from    - From: header value.
 subject - Subject: header value.

 return - a mail_info structure.
 */
MM_mail_info_t* mm_mail_info_new(const gchar* from, const gchar* subject, const gchar* date, const gchar* reply_to, const gchar* cc, const gchar* message_id) {
    MM_mail_info_t* ret;

    ret = g_malloc(sizeof(MM_mail_info_t));
    ret->header = mm_header_new(from, subject, date, reply_to, cc, message_id);

    return ret;
}

/*
 Creates a new content strcture.

 type         - MIME type.
 content      - Cleaned (ready to use) content.
 bulk_content - Bulk content (mainly for PGP signature verification).
 parts        - Subparts of this type.

 return - Content structure.

 content, bulk_content and parts can be NULL, it depends on the type.
 */
MM_content_t* mm_content_new(MM_type type, gchar* content, gchar* bulk_content, GList* parts) {
    MM_content_t* content_struc;

    content_struc = g_malloc(sizeof(MM_content_t));
    content_struc->type = type;
    content_struc->content = g_strdup(content);
    content_struc->bulk_content = g_strdup(bulk_content);
    content_struc->parts = parts;

    return content_struc;
}

/*
 Support function to free a GList of parts.

 part  - The part
 extra - paramater required by foreach function, ignored.
 */
void mm_content_parts_free(gpointer part, gpointer extra) {
    mm_content_free((MM_content_t*) part);
}

/*
 Frees a content structure.

 content - Content structure.
 */
void mm_content_free(MM_content_t* content) {
    if (!content)
        return;
    if (content->content) {
        g_free(content->content);
    }
    if (content->bulk_content) {
        g_free(content->bulk_content);
    }
    if (content->parts) {
        g_list_foreach(content->parts, mm_content_parts_free, NULL);
        g_list_free(content->parts);
    }
    g_free(content);
}

/*
 Frees a mail header structure.

 header - Mail header strcture.
 */
void mm_header_free(MM_header_t* header) {
    g_free(header->from);
    g_free(header->subject);
    g_free(header->date);
    g_free(header->reply_to);
    g_free(header->cc);
    g_free(header->message_id);
    g_free(header);
}

/*
 Returns the requested header information of a mail_info structure.

 mail - mail_info structure.

 header - required header.

 return - header.
 */
const gchar* MM_mail_info_header(MM_mail_info_t* mail, const gchar* header) {
    if (strcasecmp(header, "from"))
        return ((MM_mail_info_t*) mail)->header->from;
    if (strcasecmp(header, "subject"))
        return ((MM_mail_info_t*) mail)->header->subject;
    if (strcasecmp(header, "date"))
        return ((MM_mail_info_t*) mail)->header->date;
    if (strcasecmp(header, "reply_to"))
        return ((MM_mail_info_t*) mail)->header->reply_to;
    if (strcasecmp(header, "cc"))
        return ((MM_mail_info_t*) mail)->header->cc;
    if (strcasecmp(header, "message_id"))
        return ((MM_mail_info_t*) mail)->header->message_id;
    return NULL;
}

/*
 Creates a new header structure.

 from    - From: header.
 subject - Subject: header.

 return - header structure.
 */
MM_header_t* mm_header_new(const gchar* from, const gchar* subject, const gchar* date, const gchar* reply_to, const gchar* cc, const gchar* message_id) {
    MM_header_t* header;

    header = g_malloc(sizeof(MM_header_t));
    header->from = g_strdup(from);
    header->subject = g_strdup(subject);
    header->date = g_strdup(date);
    header->reply_to = g_strdup(reply_to);
    header->cc = g_strdup(cc);
    header->message_id = g_strdup(message_id);

    return header;
}

/*
 Processes a multipart/signed part.

 body    - Object with mail/MIME information structure.
 contents - The actual contents refered from body

 return - Content structure.
 */
MM_content_t* mm_process_signed(BODY* body, char* contents) {

    BODY* signed_content;
    BODY* signature;
    gchar* content;
    gchar* bulk_content;
    gchar* bulk_content_sig;
    //  int size;
    MM_content_t* signed_part;
    MM_content_t* signature_part;
    GList* sons;
    MM_content_t* content_struc;
    //MM_content_t* tmp;
    //  int x;

    LG_log(mm_ctx, LG_FUNC, ">mm_process_signed: entered");

    sons = NULL;
    signed_content = &body->nested.part->body;
    signature = &body->nested.part->next->body;

    bulk_content = g_strndup(contents + signed_content->mime.offset, signed_content->contents.text.size + signed_content->mime.text.size);
    content = g_strndup(contents + signed_content->contents.offset, signed_content->contents.text.size);

    LG_log(mm_ctx, LG_DEBUG, "mm_process_signed: explode content");
    signed_part = mm_explode_mime(signed_content, contents);
    g_free(content);

    /* the signature can be either PGP or X.509 */
    LG_log(mm_ctx, LG_DEBUG, "mm_process_signed: prepare signature");
    bulk_content_sig = g_strndup(contents + signature->mime.offset, signature->contents.text.size + signature->mime.text.size);
    content = g_strndup(contents + signature->contents.offset, signature->contents.text.size);
    signature_part = mm_content_new(MM_SIGNATURE, content, bulk_content_sig, NULL);
    g_free(content);

    sons = g_list_append(sons, signed_part);
    sons = g_list_append(sons, signature_part);

    content_struc = mm_content_new(MM_MULTIPART_SIGNED, NULL, bulk_content, sons);

    /* tmp = (MM_content_t*)(g_list_next(content_struc->parts)->data);
     printf("mm_process_signed: signature_part->content [\n%s\n]\n", tmp->content);
     printf("mm_process_signed: signature_part->bulk_content [\n%s\n]\n", tmp->bulk_content);
     printf("mm_process_signed: content_struc->bulk_content [\n%s\n]\n", content_struc->bulk_content); */
    g_free(bulk_content);

    LG_log(mm_ctx, LG_FUNC, "<mm_process_signed: exiting");
    return content_struc;
}

/*
 Processes a multipart/mixed-alternative part.

 body     - Object with mail/MIME information structure.
 contents - The actual contents refered from body

 return - Content structure.
 */
MM_content_t* mm_process_normal_multipart(BODY* body, char* contents, MM_type type) {

    GList* childeren;
    PART* part;
    MM_content_t* cnt;
    MM_content_t* content_struc;

    LG_log(mm_ctx, LG_FUNC, ">mm_process_normal_multipart: entered");

    part = body->nested.part;
    childeren = NULL;
    while (part) {
        cnt = mm_explode_mime(&part->body, contents);
        if (cnt) {
            childeren = g_list_append(childeren, cnt);
        }
        part = part->next;
    }

    content_struc = mm_content_new(type, NULL, NULL, childeren);

    LG_log(mm_ctx, LG_FUNC, "<mm_process_normal_multipart: exiting");
    return content_struc;
}

/*
 Processes a multipart/alternative part.

 body     - Object with mail/MIME information structure.
 contents - The actual contents refered from body

 return - Content structure.
 */
MM_content_t* mm_process_alternative(BODY* body, char* contents) {

    LG_log(mm_ctx, LG_FUNC, "mm_process_alternative: entered/exiting");
    return mm_process_normal_multipart(body, contents, MM_MULTIPART_ALTERNATIVE);
}

/*
 Processes a multipart/mixed part.

 body     - Object with mail/MIME information structure.
 contents - The actual contents refered from body

 return - Content structure.
 */
MM_content_t* mm_process_mixed(BODY* body, char* contents) {

    LG_log(mm_ctx, LG_FUNC, "mm_process_mixed: entered/exiting");
    return mm_process_normal_multipart(body, contents, MM_MULTIPART_MIXED);
}

/*
 Processes a multipart part.

 body     - Object with mail/MIME information structure.
 contents - The actual contents refered from body

 return - Content structure.

 Hello, I am only a dispatcher.
 */
MM_content_t* mm_process_multipart(BODY* body, char* contents) {

    MM_content_t *content_struc;

    LG_log(mm_ctx, LG_FUNC, ">mm_process_multipart: entered");
    LG_log(mm_ctx, LG_DEBUG, "mm_process_multipart: sub type [%s]", body->subtype);

    if (strcasecmp(body->subtype, "signed") == 0) {
        content_struc = mm_process_signed(body, contents);
    } else if (strcasecmp(body->subtype, "alternative") == 0) {
        content_struc = mm_process_alternative(body, contents);
    } else if (strcasecmp(body->subtype, "mixed") == 0) {
        content_struc = mm_process_mixed(body, contents);
    } else {
        content_struc = NULL;
    }

    LG_log(mm_ctx, LG_FUNC, "<mm_process_multipart: exiting");
    return content_struc;
}

/*
 Processes a message content.

 body     - Object with mail/MIME information structure.
 contents - The actual contents refered from body

 return - Content structure.
 */
MM_content_t* mm_process_message(BODY* body, char* contents) {
    gchar* message;

    MM_content_t* content;

    LG_log(mm_ctx, LG_FUNC, ">mm_process_message: entered");

    message = g_malloc(body->contents.text.size + 1);
    strncpy(message, contents + body->contents.offset, body->contents.text.size);
    message[body->contents.text.size] = 0;
    content = mm_content_new(MM_MESSAGE, message, NULL, NULL);
    g_free(message);

    LG_log(mm_ctx, LG_FUNC, "<mm_process_message: exiting");
    return content;
}

/*
 Strips \r's from a string.

 str - String.
 len - String size.

 return - The cleaned string.
 */
gchar* mm_r_stripper(gchar* str, int len) {
    gchar* return_str;
    gchar* pos;
    int i;

    return_str = g_malloc(len + 1);
    pos = return_str;

    for (i = 0; i < len; i++) {
        if (str[i] != 13) {
            *pos = str[i];
            pos++;
        }
    }
    *pos = 0;

    return return_str;
}

/*
 Quoted-printable decoder.

 dirty - String quoted-printable encoded.

 return - Decoded string.
 */
char* mm_qp_clean(char* dirty) {

    char* clean;
    char* cur_pos;
    unsigned char char_code;

    LG_log(mm_ctx, LG_FUNC, ">mm_qp_clean: entered");

    clean = g_malloc(strlen(dirty) + 1);
    cur_pos = clean;

    while (*dirty) {
        if (*dirty == '=') {
            if (*(dirty + 1) == '\n') {
                dirty++;
            }
            /* This protects against end of string boundaries (isalnum) */
            else if (isalnum((int) (*(dirty + 1))) && isalnum((int) (*(dirty + 2)))) {
                /* this is not completely correct, but is acceptable */
                char_code = 16 * ((*(dirty + 1) >= 'A') ? *(dirty + 1) - 'A' + 10 : *(dirty + 1) - '0');
                char_code += (*(dirty + 2) >= 'A') ? *(dirty + 2) - 'A' + 10 : *(dirty + 2) - '0';
                dirty++;
                dirty++;
                *cur_pos = (char) char_code;
                cur_pos++;
            }
        } else {
            *cur_pos = *dirty;
            cur_pos++;
        }
        dirty++;
    }
    *cur_pos = 0;

    LG_log(mm_ctx, LG_FUNC, "<mm_qp_clean: exiting");
    return clean;
}

/*
 Explodes MIME (translates from the MIME library format to our own).

 body     - Object with mail/MIME information structure.
 contents - The actual contents refered from body

 return - Content structure.
 */
MM_content_t* mm_explode_mime(BODY* body, char* contents) {

    MM_content_t* content_struc;
    char* content;
    char* old_content;

    LG_log(mm_ctx, LG_FUNC, ">mm_explode_mime: entered");

    if ((body->type == TYPETEXT && strcasecmp(body->subtype, "plain") == 0) || (body->type == TYPEAPPLICATION && strcasecmp(body->subtype, "pgp") == 0)) {
        LG_log(mm_ctx, LG_DEBUG, "mm_explode_mime: exploding leaf (text), body size is %i", body->contents.text.size);
        LG_log(mm_ctx, LG_DEBUG, "mm_explode_mime: type [%s] sub type [%s]", body->type == TYPETEXT ? "text" : "application", body->subtype);
        if (body->contents.text.size) {
            content = mm_r_stripper(contents + body->contents.offset, body->contents.text.size);

            if (body->encoding == ENCQUOTEDPRINTABLE) {
                LG_log(mm_ctx, LG_DEBUG, "mm_explode_mime: encoding is ENCQUOTEDPRINTABLE");
                old_content = content;
                content = mm_qp_clean(old_content);
                g_free(old_content);
            }
            LG_log(mm_ctx, LG_DEBUG, "mm_explode_mime: set up new content structure");
            content_struc = mm_content_new(MM_PLAIN, content, NULL, NULL);
            g_free(content);
        } else {
            LG_log(mm_ctx, LG_DEBUG, "mm_explode_mime: no body content");
            content_struc = NULL;
        }
    } else if (body->type == TYPEMULTIPART) {
        LG_log(mm_ctx, LG_DEBUG, "mm_explode_mime: exploding multipart");
        LG_log(mm_ctx, LG_DEBUG, "mm_explode_mime: type [multipart]");
        content_struc = mm_process_multipart(body, contents);
    } else if (body->type == TYPEMESSAGE) {
        LG_log(mm_ctx, LG_DEBUG, "mm_explode_mime: Exploding message inside message");
        LG_log(mm_ctx, LG_DEBUG, "mm_explode_mime: type [message]");
        content_struc = mm_process_message(body, contents);
    } else {
        LG_log(mm_ctx, LG_DEBUG, "mm_explode_mime: Unknown content");
        content_struc = NULL;
    }

    LG_log(mm_ctx, LG_FUNC, "<mm_explode_mime: exiting");
    return content_struc;
}

/*
 c-client event handling
 */
#include "mm_hook.c"
