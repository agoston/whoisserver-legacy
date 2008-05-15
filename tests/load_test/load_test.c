#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#include "SK.h"

int arg_port;
char *arg_hostname;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;
int numlines = 0;
FILE* infile;
FILE* logfile;
char* infilename;

void log_to_file(char *reason, char *actwork) {
        pthread_mutex_lock(&log_lock);
        fprintf(logfile, "%s: %s", reason, actwork);
        fflush(logfile);
        pthread_mutex_unlock(&log_lock);
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
		        log_to_file("connect", line);
			sleep(3);
		} 
		else 
		{
			
			write(sock, line, strlen(line));
			while (1) {
				FD_ZERO(&rset);
				ptm.tv_usec = 0;
				ptm.tv_sec = 30;
				FD_SET(sock, &rset);
				if ((sel = select(sock + 1, &rset, NULL, NULL, &ptm)) <= 0) {
					fprintf(stderr, "!");fflush(stderr);
					log_to_file("read", line);
					break;
				}
				
				if (read(sock, buf, 1024) <= 0) break;
			}

                        if (close(sock) < 0) {
                                perror("close");
                        }
		}
			
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
        if (!strcmp(infilename, "-")) {
                infile = stdin;
        } else {
                infile = fopen(infilename, "r");
        }
	infile = fopen(infilename, "r");
	numthreads = strtol(argv[2], NULL, 10);
	arg_hostname = argv[3];
	arg_port = strtol(argv[4], NULL, 10);
	logfile = fopen("load_test.log", "w+");

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
