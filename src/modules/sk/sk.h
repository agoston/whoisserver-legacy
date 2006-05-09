#ifndef SK_H
#define SK_H
/***************************************
  $Revision: 1.1 $

  Socket module (sk)

  Status: NOT REVUED, NOT TESTED

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

#include "stubs.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

/* #include <arpa/inet.h> */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include <signal.h>
#include <stdio.h>

#include "iproutines.h"
#include "lg.h"

/* maximum amount of input data to buffer */
#define INPUT_BUF_LEN 2048

/* connection data -> helps keep track of all errors etc */
typedef struct {
  int            sock;         /* socket descriptor # */
  struct timeval rd_timeout;   /* preset timeout values */
  struct timeval wr_timeout;
  unsigned short rtc;          /* RTC flags (reason-to-close) */

  pthread_t      watchdog;     /* thread id of the watchdog associated */
  pthread_t      killthis;     /* thread to be killed by watchdog */
  void * (*execthis)(void *);  /* function to be called if watchdog triggers */
  void *         execargs;     /* argument to be passed to that function */
  pthread_mutex_t watchmutex;

  unsigned char  lasterr;      /* timeout, interrupt, etc. */
  ip_addr_t rIP;               /* real IP */
  ip_addr_t eIP;               /* effective IP */
  char *ip;                    /* text of the eIP */

  char rd_buf[INPUT_BUF_LEN];  /* buffer for input */
  int rd_buf_len;              /* number of characters in input buffer */
} sk_conn_st;

/* error values */
#define SK_OK        0
#define SK_BADHOST   1
#define SK_CONNECT   2
#define SK_BIND      3
#define SK_SOCKET    4

/* reasons to close: socket-wise .... */
#define SK_DISCONNECT  8
#define SK_INTERRUPT  16
#define SK_TIMEOUT    32

/* ... and user-wise: */
#define SK_NOTEXT     64


#ifdef __cplusplus
extern "C" {
#endif


int SK_atoport(const char *service, const char *proto);
int SK_close(int  socket);
int SK_setsockflags(int socket, int flags);
int SK_clearsockflags(int socket, int flags);
int SK_getsock(const char* node, unsigned port, int socket_type, int backlog);
int SK_connect(int *sock, char *hostname, unsigned port, unsigned timeout);
int SK_accept_connection(int listening_socket);
int SK_write(int sockfd,
             const char *buf,
	     int count,
	     const struct timeval *timeout,
	     int *count_sent);
int SK_puts(int  sockfd, const char *str, const struct timeval *timeout);
int SK_putc(int  sockfd, char ch, const struct timeval *timeout);
char *SK_getpeername(int sockfd);
int SK_getpeerip(int sockfd, ip_addr_t *ip);

void SK_cd_make(sk_conn_st *condat, int sock, unsigned  timeout);
void SK_cd_free(sk_conn_st *condat);
int SK_cd_puts(sk_conn_st *condat, const char *str);
int SK_cd_gets(sk_conn_st *condat, char *str, size_t count);
int SK_cd_close(sk_conn_st *condat);
int SK_cd_printf(sk_conn_st *condat, char *txt, ...)
#ifdef __GNUC__  /* let gcc check the format string for problems */
     __attribute__ ((format (printf, 2, 3)))
#endif
     ;
void SK_init(LG_context_t *ctx);

int SK_watchstart(sk_conn_st *condat);
int SK_watchstop(sk_conn_st *condat);
void SK_watch_setkill(sk_conn_st *condat, pthread_t killthis);
void SK_watch_setexec( sk_conn_st *condat, void *(*function)(void *), void *args);
void SK_watch_setclear(sk_conn_st *condat);
void SK_watchexec(sk_conn_st *condat);
void SK_watchkill(sk_conn_st *condat);
void SK_watchtrigger(sk_conn_st *condat);
void SK_init (LG_context_t *ctx);

/* INTERNAL USE ONLY */
int sk_fill_rd_buf(sk_conn_st *condat);
#define sk_rd_buf_full(condat) (((condat)->rd_buf_len) >= sizeof((condat)->rd_buf))



#ifdef __cplusplus
}
#endif

#endif /* SK_H */
