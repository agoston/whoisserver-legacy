/* Symmetrical query server test

 Using logged query data and 2 query servers, it issues the same queries to them and compares the result.

 For usable result, the 2 query servers should run using the same mysql database, and the input queries should be sensible
 (meaning, there is no sense in replaying yesterday's queries on a 5 year old database).

 You can use extract.sh to produce the queries from the qry log files.

 Agoston, 2008-05-14
 */

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

#define BUF_SIZE 1048576
#define STACK_SIZE (1048576+2*BUF_SIZE)

int arg_port1, arg_port2;
char *arg_hostname1, *arg_hostname2;
pthread_mutex_t lock= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_lock= PTHREAD_MUTEX_INITIALIZER;
int numlines = 0;
FILE* infile;
FILE* logfile;
char* infilename;

void log_to_file(char *reason, char *actwork = NULL) {
    pthread_mutex_lock(&log_lock);
    fprintf(logfile, "%s: %s", reason, actwork ? actwork : "");
    fflush(logfile);
    pthread_mutex_unlock(&log_lock);
}

void log_to_file(char *hostname, int port, char *reason, char *actwork = NULL) {
    pthread_mutex_lock(&log_lock);
    fprintf(logfile, "[%s:%d] %s: %s", hostname, port, reason, actwork ? actwork : "");
    fflush(logfile);
    pthread_mutex_unlock(&log_lock);
}

int try_connect(char *hostname, int port) {
    int sock;

    for (;;) {
        sock = SK_connect(hostname, port, 30);
        if (sock < 0) {
            log_to_file(hostname, port, "connect timeout");
            sleep(3);
            continue;
        }
        return sock;
    }
}

int read_buf(int sock, char *buf, int &p) {
    int remaining_space = BUF_SIZE-p;
    
    /* we need to read the whole reply of whoisserver, as it has a nasty memleak otherwise :( 
     * agoston, 2008-06-03 */
    if (!remaining_space) {
        char devnull[4096];
        return read(sock, devnull, 4096);
    }
    
    int ret = read(sock, buf+p, remaining_space);

    if (ret <= 0) return 0;

    p += ret;
    return ret;
}

void *startup(void *arg) {
    int sock1, sock2;
    size_t len, linelen;
    char *line= NULL;
    char buf1[BUF_SIZE];
    char buf2[BUF_SIZE];
    int p1, p2;
    fd_set rset;
    struct timeval ptm;
    int sel;
    int maxfd;

    while (1) {
        pthread_mutex_lock(&lock);
        if ((linelen = getline(&line, &len, infile)) < 0) {
            fprintf(stderr, "\nThread reached end of input file, exiting...\n");
            return NULL;
        }
        numlines++;
        if (numlines> 1000) {
            numlines -= 1000;
            fprintf(stderr, ".");
            fflush(stderr);
        }
        pthread_mutex_unlock(&lock);

        sock1 = try_connect(arg_hostname1, arg_port1);
        sock2 = try_connect(arg_hostname2, arg_port2);

        write(sock1, line, linelen);
        write(sock2, line, linelen);

        p1 = p2 = 0;

        while (sock1 || sock2) {
            FD_ZERO(&rset);
            ptm.tv_usec = 0;
            ptm.tv_sec = 30;

            if (sock1) FD_SET(sock1, &rset);
            if (sock2) FD_SET(sock2, &rset);

            maxfd = (sock1 > sock2) ? sock1 + 1 : sock2 + 1;

            if ((sel = select(maxfd, &rset, NULL, NULL, &ptm)) <= 0) {
                fprintf(stderr, "!");
                fflush(stderr);
                if (sock1)
                    log_to_file(arg_hostname1, arg_port1, "read timeout", line);
                if (sock2)
                    log_to_file(arg_hostname2, arg_port2, "read timeout", line);
                continue;
            }

            if (FD_ISSET(sock1, &rset)) {
                if (read_buf(sock1, buf1, p1) == 0) {
                    close(sock1);
                    sock1 = 0;
                }
            }

            if (FD_ISSET(sock2, &rset)) {
                if (read_buf(sock2, buf2, p2) == 0) {
                    close(sock2);
                    sock2 = 0;
                }
            }
        }

        /* Compare results */
        if ((p1 != p2) || memcmp(buf1, buf2, p1)) {
            log_to_file("results differ", line);
            fprintf(stderr, "#");
            fflush(stderr);
        }

        free(line);
        line = NULL;
    }
}

int main(int argc, char **argv) {
    int numthreads, i;
    pthread_attr_t attr;

    if (argc < 7) {
        printf("%s <infile> <numthreads> <servername1> <serverport1> <servername2> <serverport2>\n\n"
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
    numthreads = strtol(argv[2], NULL, 10);
    arg_hostname1 = argv[3];
    arg_port1 = strtol(argv[4], NULL, 10);
    arg_hostname2 = argv[5];
    arg_port2 = strtol(argv[6], NULL, 10);
    logfile = fopen("load_test.log", "w+");

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, STACK_SIZE);
    printf("Creating %d worker threads...\n", numthreads);

    for (i = 0; i < numthreads; i++) {
        pthread_t dummy;
        int res = pthread_create(&dummy, &attr, startup, NULL);
        if (res != 0) {
            perror("pthread_create");
            exit(1);
        }
    }

    printf("Thread creation done. Each . represents 1000 queries. * means a connection error, ! means a read timeout, # means a comparison error.\n");
    for (;;)
        sleep(1000);
    return 0;
}
