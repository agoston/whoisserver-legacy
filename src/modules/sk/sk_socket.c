/***************************************
  $Revision: 1.1 $

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

/* SK_setsockflags() */
/*++++++++++++++++++++++++++++++++++++++

  int SK_setsockflags     sets socket flags using fcntl()

  int socket							socket to be closed
  int flags								value to be ORed with existing flags

	RETURNS									socket on success
													-1 on error (errno is set accordingly)
  ++++++++++++++++++++++++++++++++++++++*/
int SK_setsockflags(int socket, int flags) {
	int old_flags;

  if ((old_flags = fcntl(socket, F_GETFL, 0)) == -1) {
  	return -1;
  }

  if (fcntl(socket, F_SETFL, old_flags | flags) == -1) {
  	return -1;
  }

	return socket;
}

/* SK_clearsockflags() */
/*++++++++++++++++++++++++++++++++++++++

  int SK_clearsockflags     sets socket flags using fcntl()

  int socket							socket to be closed
  int flags								value to be ~ANDed with existing flags

	RETURNS									socket on success
													-1 on error (errno is set accordingly)
  ++++++++++++++++++++++++++++++++++++++*/
int SK_clearsockflags(int socket, int flags) {
	int old_flags;

  if ((old_flags = fcntl(socket, F_GETFL, 0)) == -1) {
  	return -1;
  }

  if (fcntl(socket, F_SETFL, old_flags & ~flags) == -1) {
  	return -1;
  }

	return socket;
}

/* SK_getsock() */
/*++++++++++++++++++++++++++++++++++++++

	Binds to the given host and port in an AF-independent manner.

	char* node				the address to bind to; NULL for any addr
										if hostname, it will be looked up
	char* port				the service/port to bind on
	int socket_type		SOCK_STREAM or SOCK_DGRAM (TCP or UDP sockets)
	int backlog       Size of the backlog queue to be set on the socket(s).

	RETURNS 					socket id

  ++++++++++++++++++++++++++++++++++++++*/
int SK_getsock(const char* node, unsigned port, int socket_type, int backlog) {
    struct addrinfo hints, *res, *ai_iter;
    gint error, sock = -1, sock_opt, flags;
    char portbuf[16], hostnamebuf[256];

    /* Prepare addrinfo struct for getaddrinfo */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = socket_type;

    /* This should give us values for listening */
		snprintf(portbuf, 16, "%u", port);
    error = getaddrinfo(node, portbuf, &hints, &res);
    if (error != 0) {
			LG_log(sk_context, LG_FATAL, "SK_getsock(%s, %d): getaddrinfo(): %s", node, port, gai_strerror(error));
      die;
    }

		/* is there an IPv6 address? */
		for (ai_iter = res; ai_iter != NULL; ai_iter = ai_iter->ai_next) {
			if (ai_iter->ai_family == AF_INET6) {
//				inet_ntop(ai_iter->ai_family, &((struct sockaddr_in6*)(ai_iter->ai_addr))->sin6_addr, hostnamebuf, sizeof(hostnamebuf));
//				fprintf(stderr, "Trying to bind on v6 address '%s' ... ", hostnamebuf);
		   	sock = socket(ai_iter->ai_family, ai_iter->ai_socktype, ai_iter->ai_protocol);
		    if (sock >= 0) break;
//		    fprintf(stderr, "failed.\n");
			}
		}

		/* if no IPv6 addresses found, fall back to IPv4 */
		if (sock < 0) {
			for (ai_iter = res; ai_iter != NULL; ai_iter = ai_iter->ai_next) {
				if (ai_iter->ai_family == AF_INET) {
//					inet_ntop(ai_iter->ai_family, &((struct sockaddr_in*)(ai_iter->ai_addr))->sin_addr, hostnamebuf, sizeof(hostnamebuf));
//					fprintf(stderr, "Trying to bind on v4 address '%s' ... ", hostnamebuf);
			   	sock = socket(ai_iter->ai_family, ai_iter->ai_socktype, ai_iter->ai_protocol);
			    if (sock >= 0) break;
//			    fprintf(stderr, "failed.\n");
				}
			}
		}

		/* if no v4 address found either, bail out */
		if (sock < 0) {
			LG_log(sk_context, LG_FATAL, "SK_getsock(%s, %d): No v6 nor v4 address found or supported", node, port);
			die;
		}

		res = ai_iter;

    sock_opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *) &sock_opt, sizeof(sock_opt)) == -1) {
			LG_log(sk_context, LG_FATAL, "SK_getsock(%s, %d): setsockopt(): %d (%s)", node, port, errno, strerror(errno));
			die;
    }

	  if ((bind(sock, res->ai_addr, res->ai_addrlen)) == -1) {
			LG_log(sk_context, LG_FATAL, "SK_getsock(%s, %d): bind(): %d (%s)", node, port, errno, strerror(errno));
			die;
	  }

		if (socket_type == SOCK_STREAM) {
			if (listen(sock, backlog) == -1) {
				LG_log(sk_context, LG_FATAL, "SK_getsock(%s, %d): listen(): %d (%s)", node, port, errno, strerror(errno));
				die;
			}
		}

    return sock;
}

/*++++++++++++++++++++++++++++++++++++++

   Wait for an incoming connection on the specified socket

   int  listening_socket      The socket that the server is bound to

   RETURNS										The socket for communicating to the client

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
				} else {		/* real error */
				  if( ++num_errors < MAX_ACCEPT_ERRORS ) { /* warn */
	          LG_log(sk_context, LG_WARN, "(%d) %s", errno, strerror(errno));
					} else {	/* crash */
            LG_log(sk_context, LG_SEVERE,
	        	"too many accept() errors (maximum is %d)", MAX_ACCEPT_ERRORS);
	    			die;
				  }
				}
      }
    } else { /* success */
      LG_log(sk_context, LG_DEBUG, "client connected on socket %d", connected_socket);
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

  ++++++++++++++++++++++++++++++++++++++*/
int SK_connect(int  *sock, char *hostname, unsigned int port, unsigned int timeout) {
    struct addrinfo hints, *res, *ai_iter;
    gint error;
    char portbuf[16];

    /* Prepare addrinfo struct for getaddrinfo */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    /* This should give us values for listening */
		snprintf(portbuf, 16, "%u", port);
    error = getaddrinfo(hostname, portbuf, &hints, &res);
    if (error != 0) {
			LG_log(sk_context, LG_ERROR, "SK_connect(%s, %d): getaddrinfo(): %s", hostname, port, gai_strerror(error));
			return SK_BADHOST;
    }

		/* connect using v4, then v6 address(es) */
		for (ai_iter = res; ai_iter != NULL; ai_iter = ai_iter->ai_next) {
			if (ai_iter->ai_family == AF_INET) {
				if (SK_connect_inner(sock, ai_iter, timeout, hostname, port) == SK_OK) {
					return SK_OK;
				}
			}
		}

		/* if no IPv4 addresses found, fall back to IPv6 */
		for (ai_iter = res; ai_iter != NULL; ai_iter = ai_iter->ai_next) {
			if (ai_iter->ai_family == AF_INET6) {
				if (SK_connect_inner(sock, ai_iter, timeout, hostname, port) == SK_OK) {
					return SK_OK;
				}
			}
		}

		/* if not v6 address found either, bail out */
		LG_log(sk_context, LG_ERROR, "SK_connect(%s, %d): No v6 nor v4 address found", hostname, port);
		*sock = -1;
		return SK_CONNECT;
}

/* Helper function for SK_connect
 * Does the actual connecting based on *res
 * hostname and port is only used for logging
 *
 * Returns: SK_* error code, SK_OK if everything went fine,
 * 					in which case it fills *retsock */
int SK_connect_inner(int *retsock, struct addrinfo *res, int timeout, char *hostname, int port) {
		int sock, flags, sel, gs, er, erlen=sizeof(er);
	  struct timeval ptm;
  	fd_set rset, wset;

   	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
			LG_log(sk_context, LG_ERROR, "SK_connect(%s, %d): socket(): %d (%s)", hostname, port, errno, strerror(errno));
			return SK_SOCKET;
    }

    if (SK_setsockflags(sock, O_NONBLOCK) < 0) {
			LG_log(sk_context, LG_ERROR, "SK_connect(%s, %d): setsockflags(): %d (%s)", hostname, port, errno, strerror(errno));
			close(sock);
			return SK_SOCKET;
    }

	  if ((connect(sock, res->ai_addr, res->ai_addrlen)) < 0 && errno != EINPROGRESS) {
			LG_log(sk_context, LG_ERROR, "SK_connect(%s, %d): connect(): %d (%s)", hostname, port, errno, strerror(errno));
			close(sock);
			return SK_CONNECT;
	  }

	  /* waiting for timeout */
	  FD_ZERO( &rset );
	  FD_SET( sock, &rset );
	  wset = rset;
	  ptm.tv_usec = 0;
	  ptm.tv_sec = timeout;

		if ((sel = select(sock + 1, &rset, &wset, NULL, &ptm)) == 0) {	/* timeout */
			LG_log(sk_context, LG_ERROR, "SK_connect(%s, %d): connect(): timeout", hostname, port);
			close(sock);
			return SK_TIMEOUT;
		}

		if (sel < 0) {
			LG_log(sk_context, LG_ERROR, "SK_connect(%s, %d): select(): %d (%s)", hostname, port, errno, strerror(errno));
			close(sock);
			return SK_CONNECT;
		}

		gs = getsockopt(sock, SOL_SOCKET, SO_ERROR, &er, &erlen);

		if (gs < 0 || er) {
			LG_log(sk_context, LG_ERROR, "SK_connect(%s, %d): select(): %d (%s)", hostname, port, er, strerror(er));
			close(sock);
			return SK_CONNECT;
		}

    if (SK_clearsockflags(sock, O_NONBLOCK) < 0) {
			LG_log(sk_context, LG_ERROR, "SK_connect(%s, %d): clearsockflags(): %d (%s)", hostname, port, errno, strerror(errno));
			close(sock);
			return SK_SOCKET;
    }

    *retsock = sock;
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
  struct sockaddr_storage ss;
  int namelen=sizeof(ss);

  memset(&ss, 0, sizeof(ss));

  if (getpeername(sockfd, (struct sockaddr *)&ss, &namelen) == 0) {
    hostaddress = (char *)UT_malloc(INET6_ADDRSTRLEN);
		if (getnameinfo((struct sockaddr *)&ss, namelen, hostaddress, INET6_ADDRSTRLEN, NULL, 0, 0) != 0) {
			/* Something really-really bad thing happened */
			LG_log(sk_context, LG_FATAL, "SK_getpeername(): getnameinfo(): %d (%s)", errno, strerror(errno));
			die;
		}
  } else {
    int er = errno;

    if (er == ENOTSOCK) {
      hostaddress = UT_strdup("--");
    } else {
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
  struct sockaddr_storage ss;
  int namelen=sizeof(ss);
  int ret=-1;

  memset(&ss, 0, sizeof(ss));

  if (getpeername(sockfd, (struct sockaddr *)&ss, &namelen) == 0) {
    ret=0;
    IP_addr_s2b(ip, &ss, namelen);
  } else {
      /* XXX: hack to avoid crash in the case where peer disconnects */
      /* To fix this, the socket interface needs to deal with a structure
         containing not only the file descriptor, but also the address of
	 the host and the peer.  Other goodies, like the current time,
	 time of last operation, and last errno could also be included
	 for good measure. */
	 		struct sockaddr_in addr_in;

      ret = 0;
      addr_in.sin_family = AF_INET;
      addr_in.sin_addr.s_addr = INADDR_LOOPBACK;
      addr_in.sin_port = 0;
      IP_addr_s2b(ip, &addr_in, sizeof(addr_in));
  }

  return ret;
}

