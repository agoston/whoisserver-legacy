/******************
  Copyright (c) 2001                              RIPE NCC
 
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

#if 0
We have received errors both from select() and fcntl() claiming that 
they were called on bad file descriptors in "impossible" situations.
The theory is that some other thread has closed the file descriptor.
This can happen, for instance, if another thread attempts to call
close() twice on the same integer.  For example:

   ret_val = 0;
   connect(fd, &addr, sizeof(addr));
     .
     .
     .
   if (error) {
       close(fd);
       ret_val = -1;
       goto get_out;
   }
     .
     .
     .
   get_out:
       close(fd);          /* OOOPS!!!  Bogus second call to close() */
       return ret_val;

In an effort to detect this condition, we introduce this wrapper, which
sets the file descriptor to -1 (an illegal file number) while closing.  
This means that subsequent calls can detect the -1 value and issue an
appropriate warning.

Shane, 2001-07-02
#endif /* 0 */

#include "rip.h"

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

/* undefine our wrappers so we can use the real functions */
#undef close
#undef fclose

/* logging */
extern LG_context_t *ut_context;

int 
fdwrap_close (int *fd, const char *file, int line)
{
    int open_fd;
    int ret_val;

    fprintf(stderr, 
           "fdwrap_close(%p => %d, \"%s\", %d)", fd, *fd, file, line);

    /* save the passed file descriptor, and set to -1 */
    open_fd = *fd;
    *fd = -1;

    /* check to see if we've closed this already */
    if (open_fd < 0) {
        /* report error */
        fprintf(stderr,
            "close() called on closed file descriptor %d from %s:%d",
            *fd, file, line);

        /* return error */
        errno = EBADF;
        ret_val = -1;
    } else {
        /* the usual case, call close() */
        ret_val = close(open_fd);
    }

    fprintf(stderr, "fdwrap_close = %d", ret_val);
    return ret_val;
}

int 
fdwrap_fclose (FILE **fp, const char *file, int line)
{
    FILE *open_fp;
    int ret_val;

    fprintf(stderr,
           "fdwrap_fclose(%p => %p, \"%s\", %d)", fp, *fp, file, line);

    /* save the passed file handle, and set to NULL */
    open_fp = *fp;
    *fp = NULL;

    /* check to see if we've closed this already */
    if (open_fp == NULL) {
        /* report error */
        fprintf(stderr,
            "fclose() called on closed file handle %p from %s:%d",
            *fp, file, line);

        /* return error */
        errno = EBADF;
        ret_val = EOF;
    } else {
        /* the usual case, call fclose() */
        ret_val = fclose(open_fp);
    }

    fprintf(stderr, "fdwrap_fclose = %d", ret_val);
    return ret_val;
}

