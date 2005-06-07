/******************
  Copyright (c) 2002                              RIPE NCC

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

/*
 * Forward declarations
 */
int command_help(char *input, GString *output, sk_conn_st *condat); 
int command_purify(char *input, GString *output, sk_conn_st *condat);
int command_quit(char *input, GString *output, sk_conn_st *condat);
int command_set(char *input, GString *output, sk_conn_st *condat);
int command_save(char *input, GString *output, sk_conn_st *condat);
int command_show(char *input, GString *output, sk_conn_st *condat);
int command_stop(char *input, GString *output, sk_conn_st *condat);
int set_acl(char *input, GString *output, sk_conn_st *condat);
int set_auto_save(char *input, GString *output, sk_conn_st *condat);
int set_const(char *input, GString *output, sk_conn_st *condat);
int set_consts(char *input, GString *output, sk_conn_st *condat);
int set_counter(char *input, GString *output, sk_conn_st *condat);
int set_err(char *input, GString *output, sk_conn_st *condat);
int set_macro(char *input, GString *output, sk_conn_st *condat);
int set_nodeny(char *input, GString *output, sk_conn_st *condat);
int set_props(char *input, GString *output, sk_conn_st *condat);
int set_updates(char *input, GString *output, sk_conn_st *condat);
int set_queries(char *input, GString *output, sk_conn_st *condat);
int set_initrx(char *input, GString *output, sk_conn_st *condat);
int set_dynamic(char *input, GString *output, sk_conn_st *condat);
int save_access_tree(char *input, GString *output, sk_conn_st *condat);
int show_access(char *input, GString *output, sk_conn_st *condat);
int show_acl(char *input, GString *output, sk_conn_st *condat);
int show_auto_save(char *input, GString *output, sk_conn_st *condat);
int show_const(char *input, GString *output, sk_conn_st *condat);
int show_consts(char *input, GString *output, sk_conn_st *condat);
int show_err(char *input, GString *output, sk_conn_st *condat);
int show_macros(char *input, GString *output, sk_conn_st *condat);
int show_props(char *input, GString *output, sk_conn_st *condat);
int show_threads(char *input, GString *output, sk_conn_st *condat);
int show_whois(char *input, GString *output, sk_conn_st *condat);
int show_uptime(char *input, GString *output, sk_conn_st *condat);
int show_dynamic(char *input, GString *output, sk_conn_st *condat);
int stop_query(char *input, GString *output, sk_conn_st *condat);
