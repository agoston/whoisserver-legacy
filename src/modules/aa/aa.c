/***************************************
  $Revision: 1.1 $

  access authorisation (aa). aa.c - functions to check access rights
  for less frequent clients (ripupdate, networkupdate, mirror).

  Status: NOT REVUED, NOT TESTED, 

  Design and implementation by: Marek Bukowy

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

#include "rip.h"

/* 
> +---------------+---------------------+------+-----+---------+-------+
> | Field         | Type                | Null | Key | Default | Extra |
> +---------------+---------------------+------+-----+---------+-------+
> | prefix        | int(10) unsigned    |      | PRI | 0       |       |
> | prefix_length | tinyint(3) unsigned |      | PRI | 0       |       |
> | source        | varchar(32)         |      | PRI |         |       |
> | ripupdate     | tinyint(3)          |      |     | 0       |       |
> | netupdate     | tinyint(3)          |      |     | 0       |       |
> | mirror        | tinyint(3)          |      |     | 0       |       |
> | comment       | longblob            | YES  |     | NULL    |       |
> +---------------+---------------------+------+-----+---------+-------+
*/

typedef struct {
  int ripupdate;
  int netupdate;
  int mirror;
} aa_rights;

void aa_parserow(SQ_result_set_t *result, aa_rights *rights)
{
  SQ_row_t *row;
  
  /* zero the rights - so if we don't get any results, we have a valid
   answer "no rights" */

  rights->ripupdate = 0;
  rights->netupdate = 0;
  rights->mirror    = 0;

  if ( (row = SQ_row_next(result)) != NULL ) {    
    /* read in the order of query */
    if( sscanf(SQ_get_column_string_nocopy(result, row, 0),
	       "%u", &rights->ripupdate ) < 1 ) { die; }
    if( sscanf(SQ_get_column_string_nocopy(result, row, 1),
	       "%u", &rights->netupdate ) < 1 ) { die; }
    if( sscanf(SQ_get_column_string_nocopy(result, row, 2),
	       "%u", &rights->mirror )    < 1 ) { die; }
  }
}



void aa_compose_query(ip_addr_t *address, char *source, char *buf, unsigned len)
{
snprintf(buf,len, "SELECT ripupdate, netupdate, mirror FROM aaa WHERE %u "  
" BETWEEN prefix AND (prefix+(1<<(32-prefix_length)))"
" AND source = '%s' "
" ORDER BY prefix_length DESC LIMIT 1" /* take the most specific entry */,
  IP_addr_b2v4_addr(address), source );
}



/* finds and fills in the struct */
void
aa_find(ip_addr_t *address, char *source, aa_rights *rights)
{
 SQ_result_set_t *result;
 SQ_connection_t *con=NULL;
 char buf[1024];

 /* get the query */
 aa_compose_query(address,source, buf, 1024);
 
 /* open the database */

 if( (con = AC_dbopen_admin()) == NULL ) {
   fprintf(stderr, "ERROR %d: %s\n", SQ_errno(con), SQ_error(con));
   die;
 }
 
 /* select the most specific entry */
 if( SQ_execute_query(con, buf, &result) == -1 ) {
   fprintf(stderr, "ERROR %d: %s\n", SQ_errno(con), SQ_error(con));
   die;
 }
 
 /* read in the rights from the resulting row */
 aa_parserow(result, rights);
 
 /* release everything */
 SQ_free_result(result);
 
 /* Close connection */
 SQ_close_connection(con);
}


int AA_can_networkupdate( ip_addr_t *address, char *source )
{ 
  aa_rights myrights;
  aa_find(address, source, &myrights);
  return (myrights.netupdate != 0);
}

int AA_can_ripupdate( ip_addr_t *address, char *source )
{ 
  aa_rights myrights;
  aa_find(address, source, &myrights);
  return (myrights.ripupdate != 0);
}

int AA_can_mirror( ip_addr_t *address, char *source )
{ 
  aa_rights myrights;
  aa_find(address, source, &myrights);
  return (myrights.mirror != 0);
}
