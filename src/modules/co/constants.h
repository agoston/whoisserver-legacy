#ifndef READ_CONSTANTS
#define READ_CONSTANTS

/***************************************
  $Revision: 1.1 $

  Constants module (co) - this _should_ eventually get merged in with the
  config module.

  Status: NOT REVUED, NOT TESTED

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

#include <glib.h>

int CO_get_max_threads();
char *CO_get_whois_port();
char *CO_get_config_port();
char *CO_get_mirror_port();
char *CO_get_update_port();
char *CO_get_host();
char *CO_get_user();
char *CO_get_password();
int CO_get_database_port();
char *CO_get_database();
char *CO_get_query();
char *CO_get_in_query();
char *CO_get_rt_query();
int CO_get_authenticate();
int CO_get_whois_suspended();
char *CO_get_welcome();
char *CO_get_prompt();
int CO_get_clear_screen();
int CO_get_sleep_time();
int CO_get_accounting();
int CO_get_query_logging();
char *CO_get_query_logfile();
int CO_get_comnd_logging();
char *CO_get_comnd_logfile();
int CO_get_instr_logging();
char *CO_get_instr_logfile();
int CO_get_tests_logging();
char *CO_get_tests_logfile();
int CO_get_thread_logging();
char *CO_get_thread_logfile();
int CO_get_socket_logging();
char *CO_get_socket_logfile();
int CO_get_config_logging();
char *CO_get_config_logfile();
char *CO_get_config_file();
int CO_get_dynamic();


char *CO_get_nrtm_host();
char *CO_get_nrtm_port();
int CO_get_nrtm_version();
int CO_get_nrtm_delay();
char *CO_get_nrtm_cserialfile();
char *CO_get_nrtm_logfile();
int CO_get_do_nrtm();
int CO_get_update_mode();
int CO_get_do_update();
int CO_get_do_server();


char *CO_to_string(void);
char *CO_const_to_string(const char *name);
int CO_set_const(const char *name, const char *value);
char *CO_set(void);

char * CO_sources_to_string(void);
char *CO_sources_list_to_string(GList *sources_list);
char * const *CO_get_sources(void) ;
const char *CO_get_source(int index);
const char *CO_get_source_database(int index);

#endif /* READ_CONSTANTS */
