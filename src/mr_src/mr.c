/***************************************
  $Revision: 1.2 $

  Wrapper for NRTM client

  Status: NOT REVUED, NOT TESTED

 Author(s):       Andrei Robachevsky

  ******************/ /******************
  Modification History:
        andrei (17/01/2000) Created.
  ******************/ /******************
  Copyright (c) 2000                             RIPE NCC
 
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

#include <stdio.h> 
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 256
#define TIMEOUT 60
/* number of outstanding connections (backlog queue length) */
#define BACKLOG 10

extern char *optarg;


int main(int argc, char **argv)
{
	char input[MAX_INPUT_SIZE], output[MAX_INPUT_SIZE + 1];
	char *mserver = NULL;
	int listen_port = 0, connect_port = 0;
	int listening_socket, client_socket, server_socket;
	sk_conn_st client_conn;
	int res;
	struct timeval timeout;
	int c;
	int errflg = 0;
	int pid;
	char *filter_name = "./ripe2rpsl";
	char *source_name = NULL;
	LG_context_t *null_ctx;

	/* NULL logging for these, since we don't care */
	null_ctx = LG_ctx_new();
	SK_init(null_ctx);
	UT_init(null_ctx);

	if (argc < 4)
		errflg++;

	while ((c = getopt(argc, argv, "l:h:p:f:s:?")) != EOF)
		switch (c) {
		case 'l':
			listen_port = atoi(optarg);
			break;
		case 'h':
			mserver = optarg;
			break;
		case 'p':
			connect_port = atoi(optarg);
			break;
		case 'f':
			filter_name = optarg;
			break;
		case 's':
			source_name = optarg;
			break;
		case '?':
		default:
			errflg++;
			break;
		}
	if (errflg) {
		fprintf(stderr, "usage: mr -l listen_port -h mirror_server -p port [-f convertor] [-s alt_source]\n");
		exit(2);
	}

	listening_socket = SK_getsock(NULL, listen_port, SOCK_STREAM, BACKLOG);
	bzero(&timeout, sizeof(timeout));
	timeout.tv_sec = TIMEOUT;

	while (1) {
		client_socket = SK_accept_connection(listening_socket);
		if (client_socket == -1) {
			fprintf(stderr, "cannot accept client\n");
			continue;
		}
		fprintf(stderr, "client connected\n");
		SK_cd_make(&client_conn, client_socket, 0);
		/* get the input from the client */
		SK_cd_gets(&client_conn, input, MAX_INPUT_SIZE);
		fprintf(stderr, "input:[%s]\n", input);

		/* replace the source in -g query if alternative source specified */
		if ((source_name != NULL) && (strstr(input, "-g") != NULL)) {
			char buf[10];
			char *rest;
			fprintf(stderr, "alternative source specified:[%s]\n", source_name);
			rest = strdup(strstr(input, ":") + 1);
			sprintf(input, "-g %s:%s", source_name, rest);
			fprintf(stderr, "input changed to:[%s]\n", input);
			free(rest);
		}

		if (SK_connect(&server_socket, mserver, connect_port, 30) != SK_OK) {
			fprintf(stderr, "Connection to server %s:%p failed, retrying in %d seconds...", mserver, connect_port, TIMEOUT);
			sleep(TIMEOUT);
			continue;
		}

		fprintf(stderr, "Sending Invitation");

		sprintf(output, "%s\n", input);
		res = SK_puts(server_socket, output, &timeout);
		if (res < 0) {
			perror("write");
			sleep(TIMEOUT);
			continue;
		}
		fprintf(stderr, "...sent \n");

		if ((pid = fork()) == 0) {
			close(listening_socket);
			if (dup2(server_socket, 0) == -1)
				perror("dup2-serv");;	/* provide input from the mirror server */
			if (dup2(client_socket, 1) == -1)
				perror("dup2-clnt");;	/* direct output to the client */
			fprintf(stderr, "Executing convertor: %s\n", filter_name);
			execlp(filter_name, filter_name, NULL);
			fprintf(stderr, "Cannot execute %s\n", filter_name);
		}
		fprintf(stderr, "waiting for convertor to finish...\n");
		wait(&pid);				/* wait untill conversion finishes */
		fprintf(stderr, "...converting stream done\n");

		close(server_socket);
		close(client_socket);
	}							/* main loop */
}
