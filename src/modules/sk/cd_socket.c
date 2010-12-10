/***************************************
  $Revision: 1.1 $

  Socket module - cd_socket.c - basic read/write socket routines defined
                                in terms of connection data structures
				with timeouts and storing information about
				broken connections.

  Status: NOT REVUED, TESTED

  Design and implementation by Marek Bukowy.

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

#include "rip.h"

/*+
 * -------------------------------------------------------------------
 * CD (connection data structure) varieties of the functions: 
 *   broken connections get registered in the connection structure
 *   as side effects.
 * by marek
 * -----------------------------------------------------------------
+*/

/* logging */
extern LG_context_t *sk_context;


/* SK_cd_make */
/*++++++++++++++++++++++++++++++++++++++
  
  Construct a connection data given the socket or file descriptor.
  Also performs the getpeername check and stores the IP in an allocated
  string.

  sk_conn_st *condat    pointer to where the data is to be stored.

  int  sock             The socket or file descriptor.

  unsigned timeout      Read timeout (used in SK_cd_gets) in seconds.
                        Value of 0 disables the timeout.
  ++++++++++++++++++++++++++++++++++++++*/
void SK_cd_make(sk_conn_st *condat, int sock, unsigned timeout) {
    memset(condat, 0, sizeof(sk_conn_st));

    condat->sock = sock;
    SK_getpeerip(sock, &(condat->rIP), &(condat->rIPs));
    condat->rd_timeout.tv_sec = timeout;
    condat->wr_timeout.tv_sec = timeout;
    condat->rd_buf_len = 0;
}


/*++++++++++++++++++++++++++++++++++++++
  Destroys the data allocated and anchored by the connection data structure.

  sk_conn_st *condat   Pointer to the connection data structure.

  ++++++++++++++++++++++++++++++++++++++*/
void SK_cd_free(sk_conn_st *condat)
{
}

/* SK_cd_puts() */
/*++++++++++++++++++++++++++++++++++++++

   This function writes a character string out to a socket, unless 
   the connection is broken. 

   int SK_cd_puts         Returns the total_count of bytes written, 
                          or inverted error codes (negative numbers):
			  (- SK_DISCONNECT) on broken connection,
			  (- SK_INTERRUPT)  on control-c received,
			  (- SK_TIMEOUT)    on timeout.

   sk_conn_st *condat     Pointer to the connection data structure.

   char   *str            The buffer to be written to the socket.

  More:
       if the connection structure has bad status for this connection
       from previous calls, no write will be attempted.

  +html+ <PRE>

  Side Effects:
       broken connections get registered in the connection structure 
	
  +html+ </PRE>

  ++++++++++++++++++++++++++++++++++++++*/
int SK_cd_puts(sk_conn_st *condat, const char *str) 
{
  int res;
  struct timeval *ptm;
  
  /* if we're not connected, return our status */
  if (condat->rtc != 0)  {
    return (-condat->rtc);
  }

  /* bad design to use 0 to mean "infinity", but we'll keep it because
     that's the current implementation - shane */
  ptm = &condat->rd_timeout;
  if ((ptm->tv_sec == 0) && (ptm->tv_usec == 0)) { /* if timeout 0, 
						      do blocking I/O */
    ptm = NULL;
  }

  /* use SK_puts() to do the actual work */
  res = SK_puts(condat->sock, str, ptm);

  /* if timed out (or some other error), then set the rtc variable */
  if (res < 0) {
    condat->rtc |= SK_DISCONNECT;
    res = -SK_DISCONNECT;
  }

  /* return documented value */
  return res;
} /* SK_cd_puts() */

/* fill the input buffer as much as possible */
int
sk_fill_rd_buf (sk_conn_st *condat)
{
    fd_set rset;
    struct timeval ptm_copy;
    struct timeval *ptm;
    int select_ret;
    int rd_buf_free;
    int read_ret;
    
    ptm = &(condat->rd_timeout);
    /* if timeout is 0, do blocking I/O - bogus, but that's how it is */
    if ((ptm->tv_sec == 0) && (ptm->tv_usec == 0)) {
        ptm = NULL;
    } else {
    	ptm_copy = condat->rd_timeout;
    	ptm = &ptm_copy;
    }
    FD_ZERO(&rset);
    FD_SET(condat->sock, &rset);

    /* wait for input to become available */
    select_ret = select(condat->sock + 1, &rset, NULL, NULL, ptm);
    if (select_ret < 0) {
        /* unfortunate, but bad things happen to good sockets - SK */
        LG_log(sk_context, LG_WARN, "(%d) %s", errno, strerror(errno));
        return -1;
    }
    if (select_ret == 0) {      
        condat->rtc |= SK_TIMEOUT;
        return -1;
    }

    /* calculate maximum amount to read */
    rd_buf_free = sizeof(condat->rd_buf) - (condat->rd_buf_len);

    /* read up to that much, if available */
    read_ret = read(condat->sock, 
                    condat->rd_buf + condat->rd_buf_len, 
                    rd_buf_free);
    if (read_ret <= 0) {
	condat->rtc |= SK_DISCONNECT;
        return -1;
    }
    condat->rd_buf_len += read_ret;

    /* return the fact that we got data */
    return read_ret;
}

#undef TEST_SK_FILL_RD_BUF

/* test sk_fill_rd_buf() function - full path coverage */
#ifdef TEST_SK_FILL_RD_BUF
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

int 
main ()
{
    sk_conn_st conn;
    int pfd[2];
    int tmp_fd;
    int bogus_fd;
    int i;
    int buf[INPUT_BUF_LEN+INPUT_BUF_LEN+100];
    int fill_buf_ret;

    /* test with bogus file descriptor and 0 timeout value */
    tmp_fd = open("/dev/null", O_RDONLY);
    if (tmp_fd == -1) {
        fprintf(stderr, "Error %d creating bogus file descriptor; %s\n", 
                errno, strerror(errno));
        exit(1);
    }
    bogus_fd = tmp_fd;
    close(tmp_fd);
    SK_cd_make(&conn, bogus_fd, 0);
    if (sk_fill_rd_buf(&conn) != -1) {
        fprintf(stderr, "Filling buffer with bogus file descriptor worked\n");
        exit(1);
    }

    /* create a pipe for further testing */
    if (pipe(pfd) != 0) {
        fprintf(stderr, "Error %d creating pipe; %s\n", errno, strerror(errno));
        exit(1);
    }
    /* real file descriptor and non-0 timeout value */
    SK_cd_make(&conn, pfd[0], 1);

    /* test timeout */
    if (sk_fill_rd_buf(&conn) != -1) {
        fprintf(stderr, "Buffer filling failed to timeout properly\n");
        exit(1);
    }
    if (conn.rtc != SK_TIMEOUT) {
        fprintf(stderr, "Timeout didn't set flag properly\n");
        exit(1);
    }

    /* test EOF input */
    close(pfd[1]);
    conn.rtc = 0;
    if (sk_fill_rd_buf(&conn) != -1) {
        fprintf(stderr, "Buffer filling failed to timeout properly\n");
        exit(1);
    }
    if (conn.rtc != SK_DISCONNECT) {
        fprintf(stderr, "Disconnect didn't set flag properly\n");
        exit(1);
    }

    /* test empty buffer reading various values */
    {
        int lengths[] = 
            { 1, 100, INPUT_BUF_LEN-1, INPUT_BUF_LEN, 
              INPUT_BUF_LEN+1, INPUT_BUF_LEN+100 };
        for (i=0; i<sizeof(lengths)/sizeof(lengths[0]); i++) {
            close(pfd[0]);
            close(pfd[1]);
            if (pipe(pfd) != 0) {
                fprintf(stderr, "Error %d creating pipe; %s\n", 
                        errno, strerror(errno));
                exit(1);
            }
            SK_cd_make(&conn, pfd[0], 0);
            memset(buf, i+1, lengths[i]);
            if (write(pfd[1], buf, lengths[i]) != lengths[i]) {
                fprintf(stderr, "Error %d writing to pipe; %s\n",
                        errno, strerror(errno));
                exit(1);
            }
            fill_buf_ret = sk_fill_rd_buf(&conn);
            if (fill_buf_ret == -1) {
                fprintf(stderr, "Error filling buffer\n");
                exit(1);
            }
            if (lengths[i] < INPUT_BUF_LEN) {
                if (fill_buf_ret != lengths[i]) {
                    fprintf(stderr, "Error filling buffer\n");
                    exit(1);
                }
                if (memcmp(buf, conn.rd_buf, lengths[i]) != 0) {
                    fprintf(stderr, "Error with buffer contents\n");
                    exit(1);
                }
            } else {
                if (fill_buf_ret != INPUT_BUF_LEN) {
                    fprintf(stderr, "Error filling buffer\n");
                    exit(1);
                }
                if (memcmp(buf, conn.rd_buf, INPUT_BUF_LEN) != 0) {
                    fprintf(stderr, "Error with buffer contents\n");
                    exit(1);
                }
            }
        }
    }

    return 0;
}
#endif 

/* SK_cd_gets() */
/*++++++++++++++++++++++++++++++++++++++

  Read from a socket, until a linefeed character is received or the buffer
  fills up to the maximum size "count". If the connection data has non-zero
  timeout value for reading, it is used here between calls to read 
  the next 1 character.

   int SK_cd_gets      Returns the total_count of bytes read, 
                       or inverted error codes (negative numbers):
		       (- SK_DISCONNECT) on broken connection,
		       (- SK_TIMEOUT)    on timeout.

   sk_conn_st *condat  connection data

   char   *str         The buffer to store the data received from
                       the socket.

   size_t count        size of the buffer.

  More:
       if the connection structure has bad status for this connection
       from previous calls, no read will be attempted.

  +html+ <PRE>
  Author:
        marek
	
  Side Effects:
       broken connections get registered in the connection structure.
       
  +html+ </PRE>

  ++++++++++++++++++++++++++++++++++++++*/
int SK_cd_gets(sk_conn_st *condat, char *str, size_t count) 
{
    char *eol;
    int line_len;
    int amt_to_copy;

    /* leave space for terminating '\0' */
    count--;

    /* we must always return a NUL-terminated string */
    str[0] = '\0';

    /* track current line length */
    line_len = 0;

    /* get input if none available */
    if (condat->rd_buf_len <= 0) {
        if (sk_fill_rd_buf(condat) == -1) {
            return line_len;
        }
    }

    for (;;) {
        /* we should always have something in the buffer here */
        dieif(condat->rd_buf_len <= 0);

        /* check for end of line */
        eol = memchr(condat->rd_buf, '\n', condat->rd_buf_len);

        /* figure out how much of the buffer to copy */
        if (eol == NULL) {
            /* no newline -> copy all available, if room */
            amt_to_copy = (count - line_len);
            if (amt_to_copy > condat->rd_buf_len) {
                amt_to_copy = condat->rd_buf_len;
            }
        } else {
            /* newline -> copy line if room */
            amt_to_copy = eol - condat->rd_buf + 1;
            if (amt_to_copy > (count - line_len)) {
                amt_to_copy = count - line_len;
            }
        }

        /* copy data to string */
        memcpy(str + line_len, condat->rd_buf, amt_to_copy);
        line_len += amt_to_copy;
        str[line_len] = '\0';

        /* remove data from input buffer */
        memmove(condat->rd_buf, 
                condat->rd_buf + amt_to_copy, 
                condat->rd_buf_len - amt_to_copy);
        condat->rd_buf_len -= amt_to_copy;

        /* if we got a newline or our buffer is full, exit */
        if ((eol != NULL) || (line_len >= count)) {
            return line_len;
        }

        /* otherwise get more input */
        if (sk_fill_rd_buf(condat) == -1) {
            return line_len;
        }
    }

} /* SK_cd_gets() */


/*++++++++++++++++++++++++++++++++++++++
  Wrapper around the close(2) system call, 

  int SK_cd_close         returns the error codes of close(2).

  sk_conn_st *condat      Pointer to the connection data structure.

    +html+ <PRE>
  Author:
        marek
    +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
int SK_cd_close(sk_conn_st *condat) {
  return SK_close(condat->sock);
} /* SK_cd_close() */


/* SK_cd_printf() */
/*++++++++++++++++++++++++++++++++++++++

  Printf-like function to print to socket/file specified by connection
  data structure. First writes the text to a temporary buffer, then
  uses SK_cd_puts to print it. Maintains a 2K static buffer, and allocates
  more memory if this is not enough.

  int SK_cd_printf       Returns the SK_cd_puts error code/return value.

  sk_conn_st *condat     Pointer to the connection data structure.

  char *txt              Format text to be written

  ...                    more arguments (like printf)

  
    +html+ <PRE>
  Author:
        marek
    +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/
int SK_cd_printf(sk_conn_st *condat, char *txt, ...)
{
#define SKBUFLEN 2047
  va_list   ap;
  char      buffer[SKBUFLEN+1];
  unsigned  len;
  int       buflen;
  char      *newbuf = NULL;
  char      *finalbuf = buffer; /* points to where the text REALLY is */
 
  /* vsnprintf returns the number of character it WOULD write if it could.
     So we assume the buffer to be of adequate size for most cases,
     and if it isn't, then we allocate to newbuf and call v*printf again 
  */
  va_start(ap, txt);
  len = vsnprintf(buffer, SKBUFLEN, txt, ap);
  va_end(ap);
  
  if (len > SKBUFLEN) {
    newbuf = (char *)UT_malloc(len+1);
    
    va_start(ap, txt);
    vsnprintf(newbuf, len+1, txt, ap);
    va_end(ap);   
    
    finalbuf = newbuf;
  } else if (len < 0) {
      /* in older versions of GNU libc -1 was returned if buffer was
         exceeded - so we have to search for an appropriately sized buffer */
      buflen = SKBUFLEN+1;
      for (;;) {
          buflen *= 2;
          newbuf = (char *)UT_malloc(buflen);

          va_start(ap, txt);
          len = vsnprintf(newbuf, buflen, txt, ap);
          va_end(ap);   

          /* exit when we finally have a big enough buffer */
          if (len > 0) {
              break;
          }

          UT_free(newbuf);
      }
  }
  /* terminate */
  finalbuf[len] = 0;

  /* reuse len */
  len = SK_cd_puts(condat, finalbuf);

  if(newbuf != NULL) {
    UT_free(newbuf);
  }

  return len;
} /* SK_cd_printf() */

