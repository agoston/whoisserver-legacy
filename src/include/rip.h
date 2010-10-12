#ifndef RIP_H
#define RIP_H

/***************************************
  Copyright (c) 2001                              RIPE NCC
 
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

/* config.h must be first */
#include <config.h>

/* Include these here to avoid warnings on FreeBSD. */
/* <sys/param.h> and <glib.h> both define MIN, but <glib.h> uses #undef */
/* first, so there are no errors or warnings if it is included second.  */
#include <sys/param.h>
#include <glib.h>

#include "au.h"
#include "lu.h"
#include "aa.h"
#include "access_control.h"
#include "bitmask.h"
#include "ca_adminAttribs.h"
#include "ca_configFns.h"
#include "ca_defs.h"
#include "ca_dictionary.h"
/*#include "ca_libs.h"*/   /* the status of the ca_libs.h file is unknown */
#include "ca_macros.h"
#include "ca_srcAttribs.h"
#include "comparisons.h"
#include "constants.h"
#include "cr.h"
#include "defs.h"
/* ER module is deprecated due to infinite complexity */
/*#include "er_arrays.h"*/
/*#include "er_aspects.h"*/
/*#include "er_facilities.h"*/
/*#include "er_formats.h"*/
/*#include "er_macro.h"*/
/*#include "er_paths.h"*/
/*#include "er_yacc_helper.h"*/
/* #include "erroutines.h" */
#include "fdwrap.h"
#include "mg_getopt.h"
#include "globaldefs.h"
#include "km.h"
#include "iproutines.h"
#include "ep.h"
#include "lg.h"
#include "memwrap.h"
#include "mm.h"
#include "mysql_driver.h"
#include "nh.h"
#include "numconv.h"
#include "pc_commands.h"
#include "pg.h"
#include "properties.h"
#include "protocol_config.h"
#include "protocol_mirror.h"
#include "protocol_whois.h"
#include "protocol_lookup.h"
#include "query_command.h"
#include "query_instructions.h"
#include "rp.h"
#include "rt.h"
#include "rt_dbupdate.h"
#include "rxroutines.h"
#include "server.h"
#include "sk.h"
#include "stubs.h"
#include "syntax_api.h"
#include "ta.h"
#include "thread.h"
#include "timediff.h"
#include "ud.h"
#include "ud_int.h"
#include "ud_tr.h"
#include "ut.h"
#include "ut_string.h"
#include "wh_queries.h"
#include "which_keytypes.h"
#include "dbupdate.h"
#include "up_util.h"
#include "up_pre_process.h"
#include "up_auto_nic.h"
#include "notification.h"

#endif /* RIP_H */
