/******************
  Copyright (c)                                        RIPE NCC

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
#include <sys/types.h>
#include <fcntl.h>

#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <stubs.h>
#include "ca_configFns.h"
#include "ca_dictionary.h"
#include "ca_macros.h"
#include "ca_srcAttribs.h"
#include "lg.h"
#include "ut_string.h"

int main (int argc, char** argv)
{
/* Initialise the linked list of sources. */

ca_dbSource_t *source_hdl;
char *source_name=NULL, *config_name=NULL, *property=NULL;
int errflg;
int c;
extern int optind;
extern char *optarg;
LG_context_t* lg;

        errflg=0;
        lg = LG_ctx_new();
	LG_ctx_add_appender(lg, LG_app_get_file_info_dump(stdout));
        if(argc<2)errflg++;

        while ((c = getopt(argc, argv, "s:c:p:?")) != EOF)
        switch (c) {
                case 's':
                source_name = optarg;
                break;  

                case 'c':
                config_name = optarg;
                break;

                case 'p':
                property = optarg;
                break;
                
                case '?':
                default :
                errflg++;
                break;
        }
        if (errflg) {
         fprintf(stderr,"usage: getvar -s <source> -c <configfile> -p <property>\n");
         fprintf(stderr,"list of properties:\n\tdatabase\tdatabase name\n"
                                             "\tdbhost\tsql server hostname\n"
                                             "\tdbport\tsql server port\n"
                                             "\ttrx_support\tsql transaction support\n"
                                             "\tdbuser\tsql user name\n"
                                             "\tdbpswd\tsql user password\n");
         exit (2);
        }

 UT_init(lg);
 ca_init(config_name);

 source_hdl = ca_get_SourceHandleByName(source_name);

 if(source_hdl == NULL) exit(1);

 if(strcasecmp(property, "dbname")==0){
  printf("%s", ca_get_srcdbname(source_hdl)); exit(0);
 } else 
 if(strcasecmp(property, "dbhost")==0){
  printf("%s",ca_get_srcdbmachine(source_hdl)); exit(0);
 } else
 if(strcasecmp(property, "dbport")==0){
  printf("%d",ca_get_srcdbport(source_hdl)); exit(0);
 } else
 if(strcasecmp(property, "dbuser")==0){
  printf("%s",ca_get_srcdbuser(source_hdl)); exit(0);
 } else
 if(strcasecmp(property, "dbpswd")==0){
  printf("%s",ca_get_srcdbpassword(source_hdl)); exit(0);
 } else
 if(strcasecmp(property, "trx_support")==0){
  printf("%s",ca_get_srctrx_support(source_hdl)); exit(0);
 } else {
  printf(""); exit(1);
 }

}
