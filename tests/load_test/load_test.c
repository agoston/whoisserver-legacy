#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

int arg_port;
char *arg_hostname;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int numlines = 0;
FILE* infile;
char* infilename;

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

	/*
	 * waiting for timeout 
	 */
	FD_ZERO(&rset);
	FD_SET(sock, &rset);
	wset = rset;
	ptm.tv_usec = 0;
	ptm.tv_sec = timeout;

	if ((sel = select(sock + 1, &rset, &wset, NULL, &ptm)) == 0) {	/* timeout */
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
				return socktemp;
			}
		}
	}

	/*
	 * if not v6 address found either, bail out 
	 */
	//fprintf(stderr, "SK_connect(%s, %d): No v6 nor v4 address found", hostname, port);
	return -1;
}


void *startup(void *arg) {
	int sock;
	size_t len;
	char *line = NULL;
	char buf[1024];
	fd_set rset;
	struct timeval ptm;
	int sel;
	
	while (1) {
		pthread_mutex_lock(&lock);
		while (getline(&line, &len, infile) < 0) {
			fprintf(stderr, "\nThread reached end of file, reopening...\n");
			fclose(infile);
			infile = fopen(infilename, "r");
		}
		numlines++;
		if (numlines > 1000) {
			numlines -= 1000;
			fprintf(stderr, ".");fflush(stderr);
		}
		pthread_mutex_unlock(&lock);

		sock = SK_connect(arg_hostname, arg_port, 30);
		if (sock < 0) {
			sleep(3);
		} 
		else 
		{
			
			write(sock, line, strlen(line));
			while (1) {
				FD_ZERO(&rset);
				ptm.tv_usec = 0;
				ptm.tv_sec = 10;
				FD_SET(sock, &rset);
				if ((sel = select(sock + 1, &rset, NULL, NULL, &ptm)) <= 0) {
					fprintf(stderr, "!");fflush(stderr);
					break;
				}
				
				if (read(sock, buf, 1024) <= 0) break;
			}
		}
			
		close(sock);
		free(line);
		line = NULL;
	}
}

int main(int argc, char **argv) {
	int numthreads, i;
	pthread_attr_t attr;

	if (argc < 5) {
		printf("%s <infile> <numthreads> <servername> <serverport>\n\n"
			"Where infile is a file containing a '-k'-less query each line,\n"
			"and servername can be a hostname or a v4 or v6 address.\n", argv[0]);
		exit(1);
	}

	infilename = argv[1];
	infile = fopen(infilename, "r");
	numthreads = strtol(argv[2], NULL, 10);
	arg_hostname = argv[3];
	arg_port = strtol(argv[4], NULL, 10);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, 1048576);
	printf("Creating %d threads, connecting to server at %s:%d...\n", numthreads, arg_hostname, arg_port);

	for (i = 0; i < numthreads; i++) {
		pthread_t dummy;
		int res = pthread_create(&dummy, &attr, startup, NULL);
		if (res != 0) {
			perror("pthread_create");
			exit(1);
		}
	}
	
	printf("Thread creation done. Each . represents 1000 queries. * means a connect() timeout, ! means a read() timeout.\nPress ctrl-c to exit.\n");
	sleep(2147483647);
	return 0;
}
