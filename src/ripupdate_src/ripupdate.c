/******************
  Copyright (c) 2002                                        RIPE NCC

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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


int main(int argc, char **argv)
{


int sockfd;
struct hostent *hptr;
struct sockaddr_in serv_addr;
struct in_addr  *paddr;
char line_buff[2048];
int nread;
FILE *networkR, *networkW;

 if(argc<3) {
	 fprintf(stderr, "Usage: %s host port\n", argv[0]);
	 exit(1);
 }
 fprintf(stderr, "Making connection to server %s port %d\n", argv[1], atoi(argv[2]));
 if ((sockfd=socket(AF_INET, SOCK_STREAM, 0))==-1){
     perror("socket");
     return(EXIT_FAILURE);
 }
 hptr=gethostbyname(argv[1]);
 paddr=(struct in_addr *)hptr->h_addr;
 bzero(&serv_addr, sizeof(serv_addr));
 serv_addr.sin_family=AF_INET;
 serv_addr.sin_port=htons(atoi(argv[2]));
 memcpy(&serv_addr.sin_addr, paddr, sizeof(struct in_addr));
 fprintf(stderr,"Trying %s port %d\n", inet_ntoa(serv_addr.sin_addr), atoi(argv[2]));
 if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==-1) {
   perror("connect");
   return(EXIT_FAILURE);
 }
 fprintf(stderr, "Connected. Sending object:\n");
 
 
 while(fgets(line_buff, sizeof(line_buff), stdin))
 {
  if(strncmp(line_buff, "%", 1)==0) break;
  write(sockfd, line_buff, strlen(line_buff));
 } 


 fprintf(stderr, "waiting for ack, Ctrl-C to exit\n"); 
 
while((nread=read(sockfd, line_buff, sizeof(line_buff)))>0)
 write(2, line_buff, nread );

 fprintf(stderr, "read %d bytes\n", nread);
 if(nread==-1) perror("read socket");
      
return(0);
}
