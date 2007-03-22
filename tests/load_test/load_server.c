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

int arg_port, numthreads;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

#define NUMBER_OF_CONNECTIONS 256
int sockfds[NUMBER_OF_CONNECTIONS];
int numsock = 0;
int numconns = 0;

void *startup(void *arg) {
	int sock;
	size_t len;
	char buf[1024];
	
	while (1) {
		pthread_mutex_lock(&lock);
		while (numsock <= 0) {
		        pthread_cond_wait(&cond, &lock);
                }
                
                numsock--;
                sock = sockfds[numsock];
                
		numconns++;
		if (numconns > 1000) {
			numconns -= 1000;
			fprintf(stderr, ".");
		}
		pthread_mutex_unlock(&lock);

		if (len = read(sock, &buf, 1024)) {
        		write(sock, buf, len);
                }
			
		close(sock);
	}
}

int main(int argc, char **argv) {
	int i, mainsock;
	pthread_attr_t attr;

	if (argc < 3) {
		printf("%s <numthreads> <serverport>\n\n", argv[0]);
		exit(1);
	}

	numthreads = strtol(argv[1], NULL, 10);
	arg_port = strtol(argv[2], NULL, 10);

	mainsock = SK_getsock(NULL, arg_port, SOCK_STREAM, 128);
	if (mainsock < 0) {
	        fprintf(stderr, "SK_getsock(): couldn't allocate socket");
	        exit(0);
        }

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, 1048576);
	printf("Creating %d threads, listening %d...\n", numthreads, arg_port);

	for (i = 0; i < numthreads; i++) {
		pthread_t dummy;
		int res = pthread_create(&dummy, &attr, startup, NULL);
		if (res != 0) {
			perror("pthread_create");
			exit(1);
		}
	}

	printf("Thread creation done. Each . represents 1000 served queries.\nPress ctrl-c to exit.\n");

	while (1) {
	        int actsock = SK_accept_connection(mainsock);
	        if (actsock < 0) {
	                sleep(1);
	                continue;
                }
                
                pthread_mutex_lock(&lock);
                if (numsock < NUMBER_OF_CONNECTIONS) {
                        sockfds[numsock++] = actsock;
                        pthread_cond_signal(&cond);
                } else {
                        fprintf(stderr, "Dropping connection (queue full)");
                        close(actsock);
                        sleep(1);
                }
                pthread_mutex_unlock(&lock);
	}

	return 0;
}
