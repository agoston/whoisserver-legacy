/***************************************
  $Revision: 1.26 $

  Socket module - routines facilitating calls to socket library.

  Status: NOT REVUED, TESTED

  Basic code adapted by Chris Ottrey from 
      http://www.ibrado.com/sock-faq/sfaq.html#faq65 - sample source code.
  ******************/ /******************
  Modification History:
        ottrey (08/03/1999) Created from sockhelp.c.
        ottrey (08/03/1998) Heavily butchered.
        joao   (22/06/1999) Modified socket creation and accepts.
	marek  (December 2000) Added connection function w/timeout.
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

/* logging */
extern LG_context_t *sk_context;

/* SK_atoport() */
/*++++++++++++++++++++++++++++++++++++++
   Take a service name, and a service type, and return a port number.  If the
   service name is not found, it tries it as a decimal number.  The number
   returned is byte ordered for the network.

  char *service   Service name (or port number).

  char *proto     Protocol (eg "tcp").

  Author:
        ottrey.

  ++++++++++++++++++++++++++++++++++++++*/
int SK_atoport(const char *service, const char *proto) {
  unsigned port;
  long int lport;
  struct servent *serv;
  char *errpos;
  struct servent result;
  char buffer[STR_XXL];

  /* First try to read it from /etc/services */

#ifdef __linux__
  if(getservbyname_r(service, proto, &result, buffer, sizeof(buffer), &serv) < 0) serv = NULL;
#else  
  serv = getservbyname_r(service, proto, &result, buffer, sizeof(buffer));
#endif
  
  if (serv != NULL)
    port = serv->s_port;
  else { /* Not in services, maybe a number? */
    lport = strtol(service,&errpos,0);
    if ( (errpos[0] != 0) || (lport < 1) || (lport > 65535) )
      return -1; /* Invalid port address */
    port = htons(lport);
  }
  return port;
} /* SK_atoport() */


/* SK_close() */
/*++++++++++++++++++++++++++++++++++++++
  
  int SK_close     wrapper around closing the socket. Returns the value
                   returned by close(2)

  int socket       socket to be closed

  Author:
        ottrey
  ++++++++++++++++++++++++++++++++++++++*/
int SK_close(int socket) {
  LG_log(sk_context, LG_DEBUG, "Closing socket... %d", socket);

  return close(socket);
}

/* SK_getsock() */
/*++++++++++++++++++++++++++++++++++++++

   int  SK_getsock        This function creates a socket and binds to it. 
                          Returns the number of the created 
			  descriptor/listening socket.

   int socket_type        SOCK_STREAM or SOCK_DGRAM (TCP or UDP sockets)

   unsigned  port         The port to listen on. Ports < 1024 are
                          reserved for the root user. Host byte order.

   int backlog            Size of the backlog queue to be set on that
                          socket.

   uint32_t bind_address  Address to bind to, in network order.

  Authors:
        ottrey,
	joao,
	marek (added htons conversion for port).

  ++++++++++++++++++++++++++++++++++++++*/
int  SK_getsock(int socket_type, unsigned h_port, int backlog, 
		uint32_t bind_address) {
  struct sockaddr_in address;
  int listening_socket;
  int reuse_addr = 1;
  u_short port = htons(h_port);

  /* Setup internet address information.  
     This is used with the bind() call */
  memset((char *) &address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = port;
  address.sin_addr.s_addr = bind_address;

  /* Map all of the signals and exit routine */

  listening_socket = socket(AF_INET, socket_type, 0);
  if (listening_socket < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse_addr, sizeof(reuse_addr));

  if (bind(listening_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
    fprintf(stderr, 
        "error %d with bind() to %s:%d; %s\n",
        errno,
        inet_ntoa(address.sin_addr),
        h_port,
        strerror(errno));
    close(listening_socket);
    exit(EXIT_FAILURE);
  }


  if (socket_type == SOCK_STREAM) {
    listen(listening_socket, backlog); /* Queue up to five connections before
                                  having them automatically rejected. */
  }

  return listening_socket;
} /* SK_getsock() */

/*++++++++++++++++++++++++++++++++++++++

   Wait for an incoming connection on the specified socket

   int	SK_accept_connection The socket for communicating to the client

   int  listening_socket     The socket that the server is bound to

  Authors:
	joao,
	marek.
  ++++++++++++++++++++++++++++++++++++++*/
int SK_accept_connection(int listening_socket) {
  int connected_socket = -1;
  int num_errors = 0;

#define MAX_ACCEPT_ERRORS 3

  for (;;) {
    
    LG_log(sk_context, LG_DEBUG, 
	      "Going to accept connections on socket : %d",listening_socket);

    connected_socket = accept(listening_socket, NULL, NULL);
    if (connected_socket < 0) {
      /* Either a real error occured, or blocking was interrupted for
         some reason.  Only abort execution if a real error occured. */
      switch(errno) {
      case EINTR:        /* Interrupted system call */
      case ECONNABORTED: /* Software caused connection abort */
	/* no warning */
	continue;    /* don't return - do the accept again */
      default: 
	/* special case: shutdown of the server - just return */
	if( CO_get_do_server() == 0 ) {
	  return -1;
	}
	else { /* real error */
	  if( ++num_errors < MAX_ACCEPT_ERRORS ) {
	    /* warn */
            LG_log(sk_context, LG_WARN, "(%d) %s", errno, strerror(errno));
	  }
	  else {
	    /* crash */
            LG_log(sk_context, LG_SEVERE, 
	        "too many accept() errors (maximum is %d)", MAX_ACCEPT_ERRORS);
	    die;
	  }
	}
      }
    } 
    else { /* success */
       LG_log(sk_context, LG_DEBUG, "client connected on socket %d", 
	    connected_socket
	    );

       return connected_socket;
    }

  }
}


/*++++++++++++++++++++++++++++++++++++++
  
  int SK_connect       wrapper around connect(), doing non-blocking 
                            connection with timeout 

  int *sock         pointer to the storage for socket descriptor 
   
  char *hostname    host to connect to

  int port          port to connect to

  int timeout       in seconds

  Author: marek

  ++++++++++++++++++++++++++++++++++++++*/
int SK_connect(int  *sock, char *hostname, unsigned int port, unsigned int timeout)
{
  struct sockaddr_in sin;
  struct hostent *hp;
  int s;
  int flags;
  struct timeval ptm;
  fd_set rset, wset;
  int gs, sel, er, erlen=sizeof(er);
  int error;
  struct hostent result;
  char aliasbuf[8192]; /* Stevens, UNIX net. prog., p.304 */

  /* look up the host name */
#ifdef __linux__
  er = (gethostbyname_r(hostname,  &result, aliasbuf, 
			sizeof(aliasbuf), &hp, &error) < 0 );
#else /* default is Solaris implementation */                             
  hp = gethostbyname_r(hostname,  &result, aliasbuf, 
		       sizeof(aliasbuf), &error);
  er = ( hp == NULL ); 
#endif      
  
  if( er || (hp == NULL) ) {
    return SK_BADHOST;
  }
  
  /* create a socket */
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    return SK_SOCKET;
  }
  
  /* bind to it */
  bzero((caddr_t)&sin, sizeof (sin));
  sin.sin_family = hp->h_addrtype;
  if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    close(s);
    return SK_BIND;
  }
  bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
  sin.sin_port=htons(port);

  /* connect in non-blocking mode */
  flags = fcntl(s, F_GETFL, 0);
  fcntl(s, F_SETFL, flags | O_NONBLOCK );

  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0
      && errno != EINPROGRESS ) {
    close(s);
    return SK_CONNECT;
  }
  
  /* now wait for success */
  FD_ZERO( &rset );
  FD_SET( s, &rset );
  wset = rset;
  ptm.tv_usec = 0;
  ptm.tv_sec = timeout;
  
  if( (sel=select(s+1,  &rset, &wset, NULL, &ptm)) == 0 ) {
    /* timeout */
    close(s);
    return SK_TIMEOUT;
  }
  if (sel < 0) {
    close(s);
    return SK_CONNECT;
  }
  
  gs = getsockopt(s, SOL_SOCKET, SO_ERROR, &er, &erlen);
  
  if( gs < 0 || er ) {   /* Stevens code, p.411 is exceptionally crappy */
    close(s);
    return SK_CONNECT;
  } /* if error */
  
  fcntl(s, F_SETFL, flags);
  *sock = s;
  
  return SK_OK;
}


/* XXX: deprecated SK_read(), SK_gets(), and SK_getc(), since these 
        functions are unused, and do not work with the buffering added
        to SK_cd_gets() 
        - Shane, 2002-05-13 */

#if 0
/* SK_read() */
/*++++++++++++++++++++++++++++++++++++++

   This is just like the read() system call, except that it will make
   sure that all your data goes through the socket.

   int    SK_read   Returns the number of bytes read.

   int    sockfd    The socket file descriptor.

   char   *buf      The buffer to be read from the socket.

   size_t count     The number of bytes in the buffer.

  Author:
        ottrey

  ++++++++++++++++++++++++++++++++++++++*/
int SK_read(int sockfd, char *buf, size_t count) {
  size_t bytes_read = 0;
  int this_read;

  while (bytes_read < count) {
    do
      this_read = read(sockfd, buf, count - bytes_read);
    while ( (this_read < 0) && (errno == EINTR) );
    if (this_read < 0)
      return this_read;
    else if (this_read == 0)
      return bytes_read;
    bytes_read += this_read;
    buf += this_read;
  }

  return count;

} /* SK_read() */
#endif /* 0 */


/* SK_write() */
/*++++++++++++++++++++++++++++++++++++++

   int SK_write   Returns:
                     -1 on error
		      0 on timeout 
		      1 on success (all bytes written)

   int sockfd                      The socket file descriptor.

   const char *buf                 The buffer to be written to the socket.

   int count                       The number of bytes in the buffer.

   const struct timeval *timeout   Maximum time to wait between each 
                                   write() call, or NULL for "forever".

   int *count_sent                 Set to the number of bytes sucessfully
                                   written.  This value is always valid, 
				   although it will be less than the 
				   total number of bytes to send if 
				   the function returns -1 or 0.  NULL
				   may be sent if the caller does not
				   care about the number of bytes sent on
				   failure.

  ++++++++++++++++++++++++++++++++++++++*/

int 
SK_write(int sockfd, 
         const char *buf,
	 int count,
	 const struct timeval *timeout, 
         int *count_sent) 
{
  int     local_count_sent;    /* only used if caller passes NULL */
  fd_set  fds;
  int     select_ret;
  struct  timeval tv;
  int     write_ret;
  
  /* copious logging never hurt anybody (well, except people with slow
     computers or small hard disks) */
  LG_log(sk_context, LG_DEBUG, 
	    "SK_write = { sockfd=[%d], buf=[%s], count=[%d]", 
	    sockfd, buf, count);

  /* allow caller to pass NULL if it doesn't care about the count sent */
  if (count_sent == NULL) {
      count_sent = &local_count_sent;
  }

  /* loop around until we send all of our data, or we get a timeout or
     disconnect */
  *count_sent = 0;
  while (*count_sent < count) {

    /* first, call select() and see if we can write without blocking */
    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);
    if (timeout == NULL) {
        select_ret = select(sockfd+1, NULL, &fds, NULL, NULL);
    } else {
        /* make a copy of timeout, because it's value is undefined 
	   on return from select() */
        tv = *timeout;
        select_ret = select(sockfd+1, NULL, &fds, NULL, &tv);
    }
    /* check for error */
    if (select_ret < 0) {
        /* log the error and return "timeout" */
        LG_log(sk_context, LG_WARN, "(%d) %s", errno, strerror(errno));
        return -1;
    }
    /* check for timeout */
    if ((select_ret == 0) || !FD_ISSET(sockfd, &fds)) {
        return -1;
    }


    /* at this point we can safely write */
    write_ret = write(sockfd, buf, count - *count_sent);

    /* if write failed, assume other side disconnected */
    if (write_ret <= 0) {
        return 0;
    }

    /* update our current status */
    *count_sent += write_ret;
    buf += write_ret;

  }

  /* all bytes sent, return success */
  return 1;
} /* SK_write() */


#if 0
/* SK_gets() */
/*++++++++++++++++++++++++++++++++++++++

   This function reads from a socket, until it recieves a linefeed
   character.  It fills the buffer "str" up to the maximum size "count".

   int SK_gets      Returns the total_count of bytes read.

   int    sockfd    The socket file descriptor.

   char   *str      The buffer to be written from the socket.

   size_t count     The number of bytes in the buffer.


  Authors:
        ottrey,
	marek (modified for meaningful error codes).

  Side Effects:
        This function will return -1 if the socket is closed during the read operation.

        Note that if a single line exceeds the length of count, the extra data
        will be read and discarded!  You have been warned.

  ++++++++++++++++++++++++++++++++++++++*/
int SK_gets(int  sockfd, char *str, size_t count) {
  int bytes_read;
  int total_count = 0;
  char *current_position;
  char last_read = 0;

  int control_c = 0;

  current_position = str;
  while (last_read != 10) {

    bytes_read = read(sockfd, &last_read, 1);
    if (bytes_read <= 0) {
      /* The other side may have closed unexpectedly */
      return SK_DISCONNECT; 
      /* Is this effective on other platforms than linux? */
    }
    if ( (total_count < count) && (last_read != 10) && (last_read !=13) ) {
      *current_position = last_read;
      current_position++;
      total_count++;
    }

    if (last_read == -1) {
      bytes_read = read(sockfd, &last_read, 1);
      if (last_read == -12) {
        ER_dbg_va(FAC_SK, ASP_SK_GEN,"Client pressed Control-c");
        control_c = 1;
        ER_dbg_va(FAC_SK, ASP_SK_GEN,"returning SK_INTERRUPT");
        return SK_INTERRUPT;
      }
    }
  }
  if (count > 0) {
    *current_position = 0;
  }

  return total_count;

} /* SK_gets() */
#endif /* 0 */


/* SK_puts() */
/*++++++++++++++++++++++++++++++++++++++

   This function writes a character string out to a socket.

   int SK_puts  The total_count of bytes written, 
                or -1 on hangup, error, or timeout

   int    sockfd    The socket file descriptor.

   char   *str      The buffer to be written from the socket.

   const struct timeval *timeout   Maximum time to wait between each 
                                   write() call, or NULL for "forever".

  ++++++++++++++++++++++++++++++++++++++*/
int 
SK_puts(int sockfd, const char *str, const struct timeval *timeout) 
{
  int count_sent;
  if (SK_write(sockfd, str, strlen(str), timeout, &count_sent) <= 0) {
      return -1;
  } else {
      return count_sent;
  }
} /* SK_puts() */

/* SK_putc() */
/*++++++++++++++++++++++++++++++++++++++
  
  This function writes a single character out to a socket.

   int SK_putc       Returns the number of characters written.

   int sockfd        socket

   char ch           character

   const struct timeval *timeout   Maximum time to wait between each 
                                   write() call, or NULL for "forever".

  ++++++++++++++++++++++++++++++++++++++*/
int 
SK_putc(int sockfd, char ch, const struct timeval *timeout) 
{
  int count_sent;
  SK_write(sockfd, &ch, 1, timeout, &count_sent);
  return count_sent;
}/* SK_putc() */

#if 0
/*++++++++++++++++++++++++++++++++++++++

   This function reads a single character from a socket.

   returns EOF when no character can be read. 

  ++++++++++++++++++++++++++++++++++++++*/
int SK_getc(int  sockfd) {
  char ch;

  if( read(sockfd, &ch, 1) <= 0 ) {
    return EOF;
  }
  else {
    return ch;
  }
}/* SK_getc() */
#endif /* 0 */

/* SK_getpeername() */
/*++++++++++++++++++++++++++++++++++++++

  This function will tell you who is at the other end of a connected stream socket.
  
  char *SK_getpeername     Returns allocated string with the IP in it, 
                           or "--" if the descriptor is not a socket,
			   or NULL on error.

  int    sockfd            The socket or file descriptor.

  +html+ <PRE>
  Authors:
        ottrey,
	marek (modified error handling, made MT-Safe).
  +html+ </PRE>

  ++++++++++++++++++++++++++++++++++++++*/
char *SK_getpeername(int  sockfd) 
{
  char *hostaddress=NULL;
  struct sockaddr_in addr_in;
  int namelen=sizeof(addr_in);
 
  if (getpeername(sockfd, (struct sockaddr *)&addr_in, &namelen) == 0) {

    hostaddress = (char *)UT_malloc(INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(addr_in.sin_addr),  hostaddress, INET_ADDRSTRLEN);
  }
  else {
    int er = errno;
    
    if( er == ENOTSOCK ) {
      hostaddress = UT_strdup("--");
    }
    else {
      /* XXX: hack to avoid crash in the case where peer disconnects */
      /* To fix this, the socket interface needs to deal with a structure
         containing not only the file descriptor, but also the address of
	 the host and the peer.  Other goodies, like the current time, 
	 time of last operation, and last errno could also be included
	 for good measure. */
      hostaddress = UT_strdup("127.0.0.1");
    }
  }

  return hostaddress;
  
} /* SK_getpeername() */


/* SK_getpeerip */
/*++++++++++++++++++++++++++++++++++++++
  
  This function will check the ip of the connected peer and store it in the
  ip_addr_t structure defined in the IP module.

  int SK_getpeerip    returns 0 on success, -1 on failure.

  int  sockfd         The socket descriptor (file will result in -1)

  ip_addr_t *ip       Pointer to where the address should be stored.

  +html+ <PRE>
  Author:
	marek
  +html+ </PRE>
  ++++++++++++++++++++++++++++++++++++++*/

int SK_getpeerip(int  sockfd, ip_addr_t *ip) {
  struct sockaddr_in addr_in;
  int namelen=sizeof(addr_in);
  int ret=-1;

  memset(& addr_in, 0, sizeof(struct sockaddr_in));

  if (getpeername(sockfd, (struct sockaddr *)(& addr_in), &namelen) != -1) {
    ret=0;
    IP_addr_s2b(ip, &addr_in, namelen);
  } else {
      /* XXX: hack to avoid crash in the case where peer disconnects */
      /* To fix this, the socket interface needs to deal with a structure
         containing not only the file descriptor, but also the address of
	 the host and the peer.  Other goodies, like the current time, 
	 time of last operation, and last errno could also be included
	 for good measure. */
      ret = 0;
      addr_in.sin_family = AF_INET;
      addr_in.sin_addr.s_addr = INADDR_LOOPBACK;
      addr_in.sin_port = 0;
      IP_addr_s2b(ip, &addr_in, namelen);
  }
  
  return ret;
}

