#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

struct {
        int l_onoff;
        int l_linger;
} linger_struct = {1, 0};

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

int SK_getsock(const char *node, unsigned port, int socket_type, int backlog)
{
	struct addrinfo hints, *res, *ai_iter;
	int error, sock = -1, sock_opt, flags;
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
		fprintf(stderr, "SK_getsock(%s, %d): getaddrinfo(): %s", node, port, gai_strerror(error));
		return -1;
	}

	/* is there an IPv6 address? */
	for (ai_iter = res; ai_iter != NULL; ai_iter = ai_iter->ai_next) {
		if (ai_iter->ai_family == AF_INET6) {
		        inet_ntop(ai_iter->ai_family, &((struct sockaddr_in6*)(ai_iter->ai_addr))->sin6_addr, hostnamebuf, sizeof(hostnamebuf));
                        fprintf(stderr, "Trying to bind on v6 address '%s' ... ", hostnamebuf);
			sock = socket(ai_iter->ai_family, ai_iter->ai_socktype, ai_iter->ai_protocol);
			if (sock >= 0)
				break;
                        fprintf(stderr, "failed.\n");
		}
	}

	/* if no IPv6 addresses found, fall back to IPv4 */
	if (sock < 0) {
		for (ai_iter = res; ai_iter != NULL; ai_iter = ai_iter->ai_next) {
			if (ai_iter->ai_family == AF_INET) {
			        inet_ntop(ai_iter->ai_family, &((struct sockaddr_in*)(ai_iter->ai_addr))->sin_addr, hostnamebuf, sizeof(hostnamebuf));
			        fprintf(stderr, "Trying to bind on v4 address '%s' ... ", hostnamebuf);
			        sock = socket(ai_iter->ai_family, ai_iter->ai_socktype, ai_iter->ai_protocol);
			        if (sock >= 0)
				        break;
				        fprintf(stderr, "failed.\n");
			}
		}
	}

	/* if no v4 address found either, bail out */
	if (sock < 0) {
		fprintf(stderr, "SK_getsock(%s, %d): No v6 nor v4 address found or supported", node, port);
		return -1;
	}

	sock_opt = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&sock_opt, sizeof(sock_opt)) == -1) {
		fprintf(stderr, "SK_getsock(%s, %d): setsockopt(): %d (%s)", node, port, errno, strerror(errno));
		return -1;
	}

	if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (void *)&linger_struct, sizeof(linger_struct)) == -1) {
		fprintf(stderr, "SK_getsock(%s, %d): setsockopt(): %d (%s)", node, port, errno, strerror(errno));
		return -1;
	}
	
	if ((bind(sock, ai_iter->ai_addr, ai_iter->ai_addrlen)) == -1) {
		fprintf(stderr, "SK_getsock(%s, %d): bind(): %d (%s)", node, port, errno, strerror(errno));
		return -1;
	}

	if (socket_type == SOCK_STREAM) {
		if (listen(sock, backlog) == -1) {
			fprintf(stderr, "SK_getsock(%s, %d): listen(): %d (%s)", node, port, errno, strerror(errno));
			return -1;
		}
	}

	freeaddrinfo(res);

	return sock;
}

/*++++++++++++++++++++++++++++++++++++++

   Wait for an incoming connection on the specified socket

   int  listening_socket      The socket that the server is bound to

   RETURNS										The socket for communicating to the client

  ++++++++++++++++++++++++++++++++++++++*/
int SK_accept_connection(int listening_socket)
{
	int connected_socket = -1;
	int num_errors = 0;

#define MAX_ACCEPT_ERRORS 3

	for (;;) {

		connected_socket = accept(listening_socket, NULL, NULL);

		if (connected_socket < 0) {
			/* Either a real error occured, or blocking was interrupted for
			   some reason.  Only abort execution if a real error occured. */
			switch (errno) {
			case EINTR:		/* Interrupted system call */
			case ECONNABORTED:	/* Software caused connection abort */
				/* no warning */
				continue;		/* don't return - do the accept again */

			default:
				if (num_errors++ < MAX_ACCEPT_ERRORS) {	/* warn */
					fprintf(stderr, "(%d) %s", errno, strerror(errno));
				} else {	/* crash */
					fprintf(stderr, "too many accept() errors (maximum is %d)", MAX_ACCEPT_ERRORS);
					return -1;
				}
			}
		} else {				/* success */
                        int sock_opt = 1;
                	if (setsockopt(connected_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&sock_opt, sizeof(sock_opt)) == -1) {
                		fprintf(stderr, "SK_accept_connection: setsockopt(): %d (%s)", errno, strerror(errno));
                		return -1;
                	}

                	if (setsockopt(connected_socket, SOL_SOCKET, SO_LINGER, (void *)&linger_struct, sizeof(linger_struct)) == -1) {
                		fprintf(stderr, "SK_accept_connection: setsockopt(): %d (%s)", errno, strerror(errno));
                		return -1;
                	}
			return connected_socket;
		}
	}
}


int SK_connect_inner(int *retsock, struct addrinfo *res, int timeout, char *hostname, int port) {
	int sock, flags, sel, gs, er, erlen = sizeof(er);
	struct timeval ptm;
	fd_set rset, wset;

	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock < 0) {
		fprintf(stderr, "SK_connect(%s, %d): socket(): %d (%s)", hostname, port,
				errno, strerror(errno));
		return -1;
	}

	if (SK_setsockflags(sock, O_NONBLOCK) < 0) {
		fprintf(stderr, "SK_connect(%s, %d): setsockflags(): %d (%s)", hostname,
				port, errno, strerror(errno));
		close(sock);
		return -1;
	}

	if ((connect(sock, res->ai_addr, res->ai_addrlen)) < 0 && errno != EINPROGRESS) {
		fprintf(stderr, "SK_connect(%s, %d): connect(): %d (%s)", hostname, port,
				errno, strerror(errno));
		close(sock);
		return -1;
	}

	FD_ZERO(&rset);
	FD_SET(sock, &rset);
	wset = rset;
	ptm.tv_usec = 0;
	ptm.tv_sec = timeout;

	if ((sel = select(sock + 1, &rset, &wset, NULL, &ptm)) == 0) {	// timeout
		//fprintf(stderr, "SK_connect(%s, %d): connect(): timeout", hostname, port);
		fprintf(stderr, "*");
		close(sock);
		return -1;
	}

	if (sel < 0) {
		fprintf(stderr, "SK_connect(%s, %d): select(): %d (%s)", hostname, port,
				errno, strerror(errno));
		close(sock);
		return -1;
	}

	gs = getsockopt(sock, SOL_SOCKET, SO_ERROR, &er, &erlen);

	if (gs < 0 || er) {
		fprintf(stderr, "SK_connect(%s, %d): select(): %d (%s)", hostname, port, er,
				strerror(er));
		close(sock);
		return -1;
	}

	if (SK_clearsockflags(sock, O_NONBLOCK) < 0) {
		fprintf(stderr, "SK_connect(%s, %d): clearsockflags(): %d (%s)", hostname,
				port, errno, strerror(errno));
		close(sock);
		return -1;
	}
	
        int sock_opt = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *)&sock_opt, sizeof(sock_opt)) == -1) {
                fprintf(stderr, "SK_accept_connection: setsockopt(): %d (%s)", errno, strerror(errno));
                return -1;
        }

        if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (void *)&linger_struct, sizeof(linger_struct)) == -1) {
                fprintf(stderr, "SK_accept_connection: setsockopt(): %d (%s)", errno, strerror(errno));
                return -1;
        }
        
	*retsock = sock;
	return 0;
}


int SK_connect(char *hostname, unsigned int port, unsigned int timeout) {
	struct addrinfo hints, *res, *ai_iter;
	char portbuf[16];
	int socktemp, *sock = &socktemp, error;

	/*
	 * Prepare addrinfo struct for getaddrinfo 
	 */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	/*
	 * This should give us values for listening 
	 */
	snprintf(portbuf, 16, "%u", port);
	error = getaddrinfo(hostname, portbuf, &hints, &res);
	if (error != 0) {
		fprintf(stderr, "SK_connect(%s, %d): getaddrinfo(): %s", hostname,
				port, gai_strerror(error));
		return -1;
	}

	/*
	 * connect using v4, then v6 address(es) 
	 */
	for (ai_iter = res; ai_iter != NULL; ai_iter = ai_iter->ai_next) {
		if (ai_iter->ai_family == AF_INET) {
			if (SK_connect_inner(sock, ai_iter, timeout, hostname, port) == 0) {
                                freeaddrinfo(res);
				return socktemp;
			}
		}
	}

	/*
	 * if no IPv4 addresses found, fall back to IPv6 
	 */
	for (ai_iter = res; ai_iter != NULL; ai_iter = ai_iter->ai_next) {
		if (ai_iter->ai_family == AF_INET6) {
			if (SK_connect_inner(sock, ai_iter, timeout, hostname, port) == 0) {
                                freeaddrinfo(res);
				return socktemp;
			}
		}
	}

	/*
	 * if not v6 address found either, bail out 
	 */
	//fprintf(stderr, "SK_connect(%s, %d): No v6 nor v4 address found", hostname, port);
	freeaddrinfo(res);
	return -1;
}
