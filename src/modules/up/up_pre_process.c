/***************************************
  $Revision: 1.3 $

  UP pre process checks

  Status: NOT REVIEWED, NOT TESTED

  Author(s):       Engin Gunduz, Denis Walker 

  ******************/ /******************
  Modification History:
        engin (15/12/2000) first Created.
        denis (31/08/2001) Modified for new parser API
        denis (11/11/2002) re-write for re-structured dbupdate
  ******************/ /******************
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


#include "config.h"
#include <assert.h>
#include "rip.h"

#include "iproutines.h"
#include "km.h"
#include "ca_defs.h"
#include "dbupdate.h"
#include "up_pre_process.h"
#include <glib.h>

/* these enum values and error messages must be kept in sync */
typedef enum
{
  UP_DATE_OK = 0,
  UP_DATE_SYNERR,    
  UP_DATE_FUTURE,    
  UP_DATE_TOOSMALL, 
  UP_DATE_INVMONTH, 
  UP_DATE_INVDAY,    
  UP_DATE_WRONGFORMAT  
} date_return_codes;

char *up_date_errmsgs[]=
{
  "OK",
  "Syntax error in date of changed: attribute",
  "Date in the future in changed: attribute",
  "Date is older than the database itself in changed: attribute",
  "Invalid month in date in changed: attribute",
  "Invalid day in date in changed: attribute",
  "Date must be in YYYYMMDD format in changed: attribute"
};



/* checks that each country in the 'country' attributes is valid
   Receives RT context
            LG context
            parsed object
            array of country code strings
   Returns  UP_FAIL if any country is found to be invalid
            UP_OK if all countries are valid
*/

int UP_check_country_attr(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj, char **countries)
{
  int retval = UP_OK; 
  int matched ;
  int ctry_idx;
  char *country_name;
  GList *country_list = NULL;
  GList *country_item = NULL;
 
  LG_log(lg_ctx, LG_FUNC,">UP_check_country_attr: entered\n");

  /* get country attributes from object */
  country_list = rpsl_object_get_attr(preproc_obj, "country");
  rpsl_attr_split_multiple(&country_list);
  
  if ( country_list == NULL )
  {
    /* can only fail if there is a country that is invalid */
    LG_log(lg_ctx, LG_DEBUG,"UP_check_country_attr: object contains no countries");
    LG_log(lg_ctx, LG_FUNC,"<UP_check_country_attr: exiting with value UP_OK");
    return UP_OK;		
  }

  /* check each country from the object and make sure it is in the countries list
     if any one of the countries from the object is not found, return UP_FAIL,
     after checking any remaining countries */
  for ( country_item = country_list; country_item != NULL ; country_item = g_list_next(country_item) )
  {
    country_name = rpsl_attr_get_clean_value( (rpsl_attr_t *)(country_item->data) );
    matched = 0 ;
    ctry_idx = 0 ;
    while(countries[ctry_idx])
    {
      if(strcasecmp(countries[ctry_idx++],country_name) == 0)
      {
        matched = 1;
        break;
      }
    }

    if ( ! matched )
    {
      /* found a country in the object that is not recognised */
      retval = UP_FAIL;
      LG_log(lg_ctx, LG_DEBUG,"UP_check_country_attr: country not recognised [%s]", country_name);
      RT_unknown_country(rt_ctx, country_name);
    }
    free(country_name);
  }
  rpsl_attr_delete_list(country_list);

  LG_log(lg_ctx, LG_FUNC,"<UP_check_country_attr: exiting with value [%s]\n", UP_ret2str(retval));
  return retval;
}


/* checks that the incoming object has correct "org:" attributes.
   The inetnums and inet6nums have to have an "org:" attribute if their
   "status:" attribute is ALLOCATED-BY-RIR, ALLOCATED PI,
   ALLOCATED PA or ALLOCATED UNSPECIFIED
   Also for AfriNic - SUB-ALLOCATED PA
   
   Receives RT context
            LG context
            parsed object
   Returns  UP_FAIL if an object does not have an "org:" attr when it has to have one
            UP_OK if optionality of "org:" attribute is OK
*/

int UP_check_org_attr(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj)
{
  int retval = UP_OK; 
  char *rir = NULL;
  const char *type = NULL;
  char *status_value = NULL;
  GList *status_attrs = NULL;
  GList *org_attrs = NULL;
 

  LG_log(lg_ctx, LG_FUNC,">UP_check_org_attr: entered");

  type = rpsl_object_get_class(preproc_obj);
  org_attrs = rpsl_object_get_attr(preproc_obj, "org");

  rir = ca_get_rir;
  assert(rir != NULL); /* should never fail */
  rir = UP_remove_EOLs(rir);
  LG_log(lg_ctx, LG_DEBUG,"UP_check_org_attr: rir [%s]", rir);

  if ( strncmp(type,"inetnum", strlen("inetnum")) == 0 )
  {
    /* if the object is an inetnum */
    status_attrs = rpsl_object_get_attr(preproc_obj, "status");
    if (status_attrs == NULL)
    {
      /* this should have been checked in RPSL syntax checks. Just return OK */
      retval = UP_OK;
    }
    else
    {
       status_value = rpsl_attr_get_clean_value(status_attrs->data);
       g_strup(status_value);
       if ( strncmp(status_value, "ALLOCATED PI", strlen("ALLOCATED PI")) == 0 
            || strncmp(status_value, "ALLOCATED PA", strlen("ALLOCATED PA")) == 0 
            || strncmp(status_value, "ALLOCATED UNSPECIFIED", strlen("ALLOCATED UNSPECIFIED")) == 0
            || ( ! strcasecmp(rir, "AFRINIC") && strncmp(status_value, "SUB-ALLOCATED PA", strlen("SUB-ALLOCATED PA")) == 0) )
       {
         LG_log(lg_ctx, LG_DEBUG,"UP_check_org_attr: inetnum requires org");
         if(org_attrs == NULL)
         {
           retval = UP_FAIL; /* this object must have an "org:" attribute! */
           LG_log(lg_ctx, LG_DEBUG,"UP_check_org_attr: no org found");
         }
         else
         {
           retval = UP_OK;
         }
       }
       else
       {
         retval = UP_OK;
       }
       free(status_value);
    }
  }
  else if ( strncmp(type,"inet6num", strlen("inet6num")) == 0 )
  {
    /* if the object is an inet6num */
    status_attrs = rpsl_object_get_attr(preproc_obj, "status");
    if (status_attrs == NULL)
    {
      /* this should have been checked in RPSL syntax checks. Just return OK */
      retval = UP_OK;
    }
    else
    {
       status_value = rpsl_attr_get_clean_value(status_attrs->data);
       g_strup(status_value);
       if ( strncmp(status_value, "ALLOCATED-BY-RIR", strlen("ALLOCATED-BY-RIR")) == 0 ||
             ( ! strcasecmp(rir, "AFRINIC") && strncmp(status_value, "SUB-ALLOCATED PA", strlen("SUB-ALLOCATED PA")) == 0) )
       {
         if (org_attrs == NULL)
         {
           retval = UP_FAIL; /* this object must have an "org:" attribute! */
         }
         else
         {
           retval = UP_OK;
         }
       }
       else
       {
         retval = UP_OK;
       }
       free(status_value);
    }
  }
  else
  {
    /* if the object is not an inet(6)num object, just return OK */
    retval = UP_OK;
  }

  if (retval != UP_OK)
  {
    RT_wrong_org_attr_optionality(rt_ctx);
  }

  LG_log(lg_ctx, LG_FUNC,"<UP_check_org_attr: exiting with value [%s]", UP_ret2str(retval));
  return retval;
}


/* checks that if the incoming object for creation is an organisation object, 
   it has AUTO-nic handle or not.  If not, the creation must be rejected.
   Receives RT context
            LG context
            parsed object
            operation type
   Returns  UP_FAIL if the object is an organisation object, 
                    and the operation is creation,
                    and the "organisation:" attribute is NOT "AUTO-".
            UP_OK otherwise
*/

int UP_check_organisation(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                          rpsl_object_t *preproc_obj, int operation)
{
  int retval = UP_OK; 
  const char *type = NULL;
  char *organisation_value = NULL;
  GList *organisation_attrs = NULL;

  LG_log(lg_ctx, LG_FUNC,">UP_check_organisation: entered\n");

  /* if the operation is not creation, then return OK */
  if (operation != UP_CREATE)
  {
    return retval;
  }

  type = rpsl_object_get_class(preproc_obj);
  if( strncmp(type,"organisation", strlen("organisation")) == 0 )
  {
    /* if the object is an organisation */
    organisation_attrs = rpsl_object_get_attr(preproc_obj, "organisation");
    if (organisation_attrs == NULL)
    {
      /* this should never happen */
      retval = UP_FAIL;
    }
    else
    {
      organisation_value = rpsl_attr_get_clean_value(organisation_attrs->data);
      g_strup(organisation_value);
      if ( strstr(organisation_value, "AUTO-") != organisation_value )
      {
        retval = UP_FAIL; /* "organisation:" attribute does not have AUTO- */
      }
      else
      {
        retval = UP_OK;
      }
      free(organisation_value);
    }
  }
  else
  {
    /* if the object is not an organisation object, just return OK */
    retval = UP_OK;
  }

  if(retval != UP_OK)
  {
    RT_organisation_creation_without_auto_id(rt_ctx);
  }

  LG_log(lg_ctx, LG_FUNC,"<UP_check_organisation: exiting with value [%s]\n", UP_ret2str(retval));
  return retval;
}


/* checks for a valid suffix at the end of a 'nic-hdl' attributes 
   Receives RT context
            LG context
            options structure
            parsed object
            array of country code strings
   Returns  UP_OK if the nic suffix is valid
            UP_FAIL otherwise
*/

int UP_check_nicsuffixes(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             options_struct_t *options, 
                             rpsl_object_t *preproc_obj, char **countries)
{
  int retval = UP_FAIL;
  int str_idx, src_idx, nic_idx, ctry_idx;
  char *name;
  char *nic_str = NULL;
  char *nic_suffixes[NIC_SUFFIX_LIST_SIZE];
  char **temp_vector;
  int num_sources = 0;
  ca_updDbSource_t **upd_source_hdl;
  GList *list;

  LG_log(lg_ctx, LG_FUNC,">UP_check_nicsuffixes: entered\n");

  /* get the nic-hdl from the object, there will either be one or none */
  list = rpsl_object_get_attr(preproc_obj, "nic-hdl");

  if ( list )
  {
    name = rpsl_attr_get_clean_value( (rpsl_attr_t *)(list->data) );
    LG_log(lg_ctx, LG_DEBUG,"UP_check_nicsuffixes: nic-hdl [%s]", name);

    if ( !strchr(name,'-') || strncasecmp(name,"AUTO-",strlen("AUTO-")) == 0 )
    {
      /* the nic-hdl does not have a suffix, or it is an AUTO- nic-hdl */
      retval = UP_OK;
    }
    else
    {
      /* first check against list of standard nic-suffixes */
      /* get nic suffix details from config file */
      LG_log(lg_ctx, LG_INFO,"UP_check_nicsuffixes: get nic suffixes from config file");
      nic_str = ca_get_nicsuffix;

      /* construct nic_suffixes array from nic string variable */
      temp_vector = ut_g_strsplit_v1(nic_str, "\n", 0);
      for (str_idx=0, nic_idx=0; temp_vector[str_idx] != NULL; str_idx++)
      {
        if ( nic_idx == NIC_SUFFIX_LIST_SIZE )
        {
          LG_log(lg_ctx, LG_FATAL, "UP_check_nicsuffixes: NIC_SUFFIX_LIST_SIZE exceeded");
          UP_internal_error(rt_ctx, lg_ctx, options, "UP_check_nicsuffixes: NIC_SUFFIX_LIST_SIZE exceeded\n", 0);
        }

        temp_vector[str_idx] = g_strstrip(temp_vector[str_idx]);
        if (strlen(temp_vector[str_idx]) > 0)
	    {
          nic_suffixes[nic_idx] = strdup(temp_vector[str_idx]);
          g_strup(nic_suffixes[nic_idx++]);
        }
      }
      nic_suffixes[nic_idx] = NULL; /* mark the end of array */
      g_strfreev(temp_vector);
      free(nic_str);
      LG_log(lg_ctx, LG_DEBUG,"UP_check_nicsuffixes: number of nic suffixes [%d]", nic_idx);

      /* compare nic suffix with list entries */
      nic_idx = 0 ;
      while (nic_suffixes[nic_idx])
      {
        if ( strcasecmp(nic_suffixes[nic_idx++],strchr(name,'-')+1) == 0 )
        {
          LG_log(lg_ctx, LG_DEBUG,"UP_check_nicsuffixes: nic-hdl [%s] matched nic-suffix [%s]", name, nic_suffixes[--nic_idx]);
          retval = UP_OK;
          break;
        }
      }

      if ( retval != UP_OK )
      {
        /* next check against source names */
        /* retrieve source variables for the multiple sources */
        /* upd_source_hdl is a pointer to an array of pointers to source data 
           held in the ca module */
        LG_log(lg_ctx, LG_INFO,"UP_check_nicsuffixes: get sources from config file");
        upd_source_hdl = ca_get_UpdSourceHandle(CA_UPDSOURCE);

        if (upd_source_hdl[0] == NULL)
        {
          LG_log(lg_ctx, LG_FATAL,"UP_check_nicsuffixes: There must be at least one updateable source in the config file");
          UP_internal_error(rt_ctx, lg_ctx, options,
                 "There must be at least one updateable source in the config file.", 0);
        }
        num_sources = ca_get_UpdSourceNum();
        LG_log(lg_ctx, LG_DEBUG,"UP_check_nicsuffixes: number of sources [%d]", num_sources);

        for ( src_idx=0; src_idx<num_sources; src_idx++ )
        {
          if ( ! strcasecmp( upd_source_hdl[src_idx]->name, strchr(name,'-')+1) )
          {
            LG_log(lg_ctx, LG_DEBUG,"UP_check_nicsuffixes: nic-hdl [%s] matched source [%s]", name, upd_source_hdl[src_idx]->name);
            retval = UP_OK;
            break;
          }
        }

        if ( retval != UP_OK )
        {
          /* finally check against country codes */
          ctry_idx = 0 ;
          while (countries[ctry_idx])
          {
            if ( strcasecmp(countries[ctry_idx++],strchr(name,'-')+1) == 0 )
            {
              LG_log(lg_ctx, LG_DEBUG,"UP_check_nicsuffixes: nic-hdl [%s] matched country [%s]", name, countries[--ctry_idx]);
              retval = UP_OK;
              break;
            }
          }
        }
      }
    }

    if ( retval != UP_OK )
    {
      /* the nicsuffix is not recognised */
      LG_log(lg_ctx, LG_DEBUG,"UP_check_nicsuffixes: nic-hdl suffix not recognised [%s]", name);
      RT_unknown_nic_suffix(rt_ctx);
    }

    free(name);
    rpsl_attr_delete_list(list);
  }
  else
  {
    LG_log(lg_ctx, LG_DEBUG,"UP_check_nicsuffixes: no nic-hdl found");
    retval = UP_OK;
  }
  
  LG_log(lg_ctx, LG_FUNC,"<UP_check_nicsuffixes: exiting with value [%s]\n", UP_ret2str(retval));
  return retval;
}



/* obtains a list of dates from the given list of attributes
   Receives LG context
            list of changed attributes
   Returns  list of date strings
*/

GList *up_get_dates(LG_context_t *lg_ctx, GList *changed_list)
{
  GList *item;
  char *str, *temp; 
  GList *list = NULL;

  LG_log(lg_ctx, LG_FUNC,">up_get_dates: entered\n");

  for ( item = changed_list; item != NULL ; item = g_list_next(item) )
  {
    str = rpsl_attr_get_clean_value((rpsl_attr_t *)(item->data));

    /* now, we have the 'changed' attribute's content in "normalized" form 
       We are sure it contains a date. So, it must be the second (and last)
       word in the attrib. */
    assert(index(str,' ') != NULL);  /* shouold never happen */
    /* extract the date from the attribute value */
    temp = (char *)malloc(strlen(str) - (index(str,' ') - str ));
    temp = strncpy(temp, index(str,' ') + 1, strlen(str) - (index(str,' ') - str ) - 1);
    temp[strlen(str) - (index(str,' ') - str ) - 1] = '\0'; /* NULL terminate it */
    list = g_list_append (list, temp);   
    LG_log(lg_ctx, LG_DEBUG,"up_get_dates: date [%s]", temp);
  }
  
  LG_log(lg_ctx, LG_FUNC,"<up_get_dates: exiting\n");
  return list;
}



/* Does this 'changed' attribute have a date already?
   Receives value part of changed attribute
   Returns 1 if it does
           0 if not.
*/

int up_changed_has_date(char *value)
{
  /* now, if there is still a white space, then we have a date in the string
     (it has to be something like "ripe-dbm@ripe.net 20001210") 
     a later check tests the second word for a valid date format */
  if (index(value, ' ') != NULL)
  {
    return 1; 
  }
  else
  {
    return 0;
  }
}



/* supplies the current date in YYYYMMDD format (for example 20011010) 
   Receives none
   Returns  date string
*/

char *UP_get_current_date()
{
  /* We will use Glib's functions here */

  char *date;
  struct tm *time_struct;
  time_t *time_loc;

  time_loc = (time_t *)malloc(sizeof(time_t));
  time(time_loc);
  
  time_struct = localtime(time_loc);

  date = (char *)malloc(9);
  sprintf(date, "%04i%02i%02i", 
          time_struct->tm_year + 1900, 
          time_struct->tm_mon + 1,
          time_struct->tm_mday);
  return date;
}



/* adds dates to 'changed' attributes which are missing one.
   Receives RT context
            LG context
            parsed object
   Returns  UP_FAIL if more than one changed line with no date found
            UP_OK otherwise
*/

int up_add_dates(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                      rpsl_object_t *preproc_obj)
{
  int retval = UP_OK;
  char *current_date = NULL;
  int count_no_date = 0; 
  char *value = NULL;
  char *temp = NULL;
  int pos;
  rpsl_attr_t *changed = NULL;
  GList *item;
  GList *changed_list;

  LG_log(lg_ctx, LG_FUNC,">up_add_dates: entered\n");

  changed_list = rpsl_object_get_attr(preproc_obj, "changed");

  /* get the current date in YYYYMMDD format (for example 20011010) */
  current_date = UP_get_current_date();
  LG_log(lg_ctx, LG_DEBUG,"up_add_dates: current date %s", current_date);
  
  for ( item = changed_list; item != NULL ; item = g_list_next(item) )
  {
    /* if this attribute does not have a date in it, add it. 
       Also add a warning message about this */
    /* although only one changed attr is allowed with no date, add a date
       to multiple attrs with no date so that later checks will not fail */
	value = rpsl_attr_get_clean_value((rpsl_attr_t *)(item->data));

    if ( !up_changed_has_date(value) )
	{
      LG_log(lg_ctx, LG_DEBUG,"up_add_dates: no date found [%s]", value);
      count_no_date++;

	  /* create a copy of this changed attribute and add the date to the value */
	  changed = rpsl_attr_copy((rpsl_attr_t *)(item->data));
      temp = (char *)malloc(strlen(value) + strlen(current_date) + 2 );
      sprintf(temp, "%s %s", value, current_date);
      rpsl_attr_replace_value(changed, temp);
      free(temp);

	  /* delete the original attribute from the object */
	  pos = rpsl_attr_get_ofs(changed);
	  rpsl_object_remove_attr(preproc_obj, pos, NULL);
	  /* add the new changed attribute in the same position */
	  rpsl_object_add_attr(preproc_obj, changed, pos, NULL);

      /* report a warning message */
      LG_log(lg_ctx, LG_DEBUG,"up_add_dates: current date added");
      RT_changed_date_missing(rt_ctx, current_date, value);
    }
	free(value);
  }
  free(current_date);

  rpsl_attr_delete_list(changed_list);
  
  if (count_no_date > 1)
  { 
    LG_log(lg_ctx, LG_DEBUG,"up_add_dates: More than one 'changed' attributes without date");
    RT_multiple_changed_date_missing(rt_ctx);
    retval = UP_FAIL;
  }

  LG_log(lg_ctx, LG_FUNC,"<up_add_dates: exiting with value [%s]\n", UP_ret2str(retval));
  return retval;
}



/* Checks the order of dates in the given list. 
   The dates in the 'changed' attributes should be in accending order
   Receives RT context
            LG context
            list of date strings
   Returns  UP_OK if they are in order 
            UP_FAIL if not
*/

int up_check_date_order(RT_context_t *rt_ctx, LG_context_t *lg_ctx, GList *list)
{
  int retval = UP_OK;
  char *previous;
  char *value;
  GList *item;

  LG_log(lg_ctx, LG_FUNC,">up_check_date_order: entered\n");

  /* an empty list is UP_OK */
  /* but should never happen */
  if ( list != NULL )
  {
    /* initialize the 'previous' date */
    previous = strdup("00000000");

    for ( item = list; item != NULL ; item = g_list_next(item))
    {
      assert((item->data) != NULL);  /* should never happen */
      /* check if the new date is smaller (earlier) than the previous */
	  value = (char *)(item->data);
      if ( strcmp(value, previous) < 0 )
	  {
        /* date out of order */
        LG_log(lg_ctx, LG_DEBUG,"up_check_date_order: date [%s] is earlier than [%s]",
                            value, previous );
        RT_changed_date_order(rt_ctx, value, previous );
        retval = UP_FAIL;
        break;
      }
      free(previous);
      previous = strdup(value);
    }

    free(previous);
  }
  else
    LG_log(lg_ctx, LG_DEBUG,"up_check_date_order: no dates in list");

  LG_log(lg_ctx, LG_FUNC,"<up_check_date_order: exiting with value [%s}\n", UP_ret2str(retval));
  return retval; 
}



/* checks the syntax of the specified date. 
   The argument is checked if it is in YYYYMMDD format
   Receives LG context
            a date as a string
   Returns  UP_DATE_OK if no error found
            UP_DATE_**** value depending on the type of error.
*/

int up_check_date(LG_context_t *lg_ctx, const char *date)
{
  int date_int; /* integer representation of the date */
  char *current_date;
  int year, month, day; /* integers for the components of the date */

  LG_log(lg_ctx, LG_FUNC,">up_check_date: entered with date [%s]\n", date);

  errno = 0;
  date_int = atoi(date);
      
  if (errno != 0)
  { /* there was an error in the conversion, syntax error */
    LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with syntax error\n");
    return UP_DATE_SYNERR;
  }
    
  /* wrong format */
  if (date_int <= 10000000 )
  { /* the date is not in YYYYMMDD format */
    LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with wrong format\n");
    return UP_DATE_WRONGFORMAT;
  }

  /* check if it is too small */  
  if (date_int <= 19840000 )
  { /* the date is older than the DB itself! */
    LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with date too small\n");
    return UP_DATE_TOOSMALL;
  }

  /* check if it is too big */
  if (date_int >= 21000000 )
  {/* too big: syntax error */
    LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with date too big\n");
    return UP_DATE_SYNERR;
  }

  /* and now check year, month and day components */
  year = date_int / 10000;
  month = (date_int - (year * 10000) ) / 100;
  day = (date_int % 100);
  
  /* check year */
  if (year < 1984 )
  {
    LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with date too small\n");
    return UP_DATE_TOOSMALL;
  }

  /* check month */
  if (month < 1 || month > 12)
  {
    LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with invalid month\n");
    return UP_DATE_INVMONTH;
  }

  /* check day */
  if (day < 1 || day > 31)
  {
    LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with invalid day\n");
    return UP_DATE_INVDAY;
  }

  switch ( month )
  {
    case 1: case 3: case 5: case 7:
    case 8: case 10: case 12:
         /* already checked max day above */
         break;
    case 2: 
         if ( (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) && (day > 29 ))
		 { /* leap year */
           LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with invalid day\n");
           return UP_DATE_INVDAY;
         }
		 else if( (!(year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) && (day > 28) )
		 { /* non-leap year */
           LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with invalid day\n");
           return UP_DATE_INVDAY;
         };
         break;
    case 4: case 6: case 9: case 11:
         if (day > 30)
		 {
           LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with invalid day\n");
           return UP_DATE_INVDAY;
         };
         break;
    default: 
         LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with invalid month\n");
         return UP_DATE_INVMONTH;
  }

  /* check if the date is in the future or not */
  current_date = UP_get_current_date();
  if (strcmp(current_date, date) < 0 )
  { 
    /* date is in the future */
    free(current_date);
    LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with future date\n");
    return UP_DATE_FUTURE;
  }
  free(current_date);
    
  LG_log(lg_ctx, LG_FUNC,"<up_check_date: exiting with date ok\n");
  return UP_DATE_OK;
}



/* Checks the syntax of the dates in the list
   Receives RT context
            LG context
            list of date strings
   Returns  UP_OK if all correct
            UP_FAIL if any has incorrect syntax
*/

int up_check_dates_syntax(RT_context_t *rt_ctx, LG_context_t *lg_ctx, GList * list)
{
  int retval = UP_OK;
  int res;
  char *err_mess = NULL;
  GList *item;

  LG_log(lg_ctx, LG_FUNC,">up_check_dates_syntax: entered\n");

  /* an empty list is UP_OK */
  /* but should never happen */
  if (list != NULL)
  {
    /* loop through the members of the list, check each of them */
    for ( item = list; item != NULL ; item = g_list_next(item))
    {
      assert((item->data) != NULL);  /* should never happen */

      /* check the date */
      res = up_check_date(lg_ctx, (char *)(item->data));
      switch (res)
	  {
        case UP_DATE_OK: 
               break;

        case UP_DATE_FUTURE:
        case UP_DATE_TOOSMALL:
        case UP_DATE_INVDAY:
        case UP_DATE_INVMONTH:
        case UP_DATE_WRONGFORMAT:

               err_mess = malloc(strlen(up_date_errmsgs[res]) + strlen((char *)(item->data)) + 5);
               err_mess = strcpy(err_mess, up_date_errmsgs[res]);
               err_mess = strcat(err_mess, " '");
               err_mess = strcat(err_mess, (char *)(item->data));
               err_mess = strcat(err_mess, "'");

               LG_log(lg_ctx, LG_ERROR,"up_check_dates_syntax: %s", err_mess);
               RT_changed_date_syntax(rt_ctx, err_mess);
               retval = UP_FAIL;
               free(err_mess);
               break;


        case UP_DATE_SYNERR: /* syntax error in the date */
        default:

               err_mess = malloc(strlen(up_date_errmsgs[UP_DATE_SYNERR]) + strlen((char *)(item->data)) + 5);
               err_mess = strcpy(err_mess, up_date_errmsgs[UP_DATE_SYNERR]);
               err_mess = strcat(err_mess, " '");
               err_mess = strcat(err_mess, (char *)(item->data));
               err_mess = strcat(err_mess, "'");

               LG_log(lg_ctx, LG_ERROR,"up_check_dates_syntax: %s", err_mess);
               RT_changed_date_syntax(rt_ctx, err_mess);
               retval = UP_FAIL;
               free(err_mess);
               break;
      }
    }
  }
  else
    LG_log(lg_ctx, LG_DEBUG,"up_check_dates_syntax: no dates in list");
  
  LG_log(lg_ctx, LG_FUNC,"<up_check_dates_syntax: exiting with value [%s]\n", UP_ret2str(retval));
  return retval;
}



/* adds dates to any changed attributes missing one
   checks the order of dates in the 'changed' attributes 
   checks the syntax of the dates
   Receives RT context
            LG context
            parsed object
   Returns  UP_FAIL if any errors found
            UP_OK if all ok
*/

int UP_check_changed_attr(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj)
{
  int retval = UP_OK;
  GList *date_list;
  GList *date_item = NULL;
  GList *changed_list;

  LG_log(lg_ctx, LG_FUNC,">UP_check_changed_attr: entered");

  /* perform all checks to report any errors.
     all functions called return 0 (UP_OK) for success, non zero (UP_FAIL) for error
     all function calls must do an |= on return value */

  /* First, add dates to any "changed" attributes without one */
  retval |= up_add_dates(rt_ctx, lg_ctx, preproc_obj);

  /* get the list of dates from the changed attributes and check their order */
  /* we may have added a date to one of the changed attrs so get the list again */
  changed_list = rpsl_object_get_attr(preproc_obj, "changed");
  date_list = up_get_dates(lg_ctx, changed_list);
  rpsl_attr_delete_list(changed_list);

  /* and check the order */ 
  retval |= up_check_date_order(rt_ctx, lg_ctx, date_list);

  /* check the syntax of dates */
  retval |= up_check_dates_syntax(rt_ctx, lg_ctx, date_list);
  
  /* free the date_list */
  for ( date_item = date_list; date_item; date_item = g_list_next(date_item) )
  {
    if ( date_item->data )
      free (date_item->data);
  }
  g_list_free(date_list);

  LG_log(lg_ctx, LG_FUNC,"<UP_check_changed_attr: exiting with value [%s]", UP_ret2str(retval));
  return retval;
}


/* checks if any mntner attributes contain a disallowed mntner
   Receives RT context
            LG context
            parsed object
   Returns  UP_FAIL if any disallowed mntners found
            UP_OK if all ok
*/

int UP_check_disallowmnt(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj)
{
  int retval = UP_OK;
  char *disallow_mntner_str = NULL;
  char **disallow_mntner_list = NULL;
  int disallow_idx;
  char *value;
  GList *p;
  GList *mntners = NULL;
  GList *mntby;
  GList *mntlower;
  GList *mntroutes;
  GList *mntdomains;
  GList *mntirt;
  GList *mntattr;
  GList *refby;

  LG_log(lg_ctx, LG_FUNC,">UP_check_disallowmnt: entered");

  /* get the list of DISALLOWMNT mntner names */
  disallow_mntner_str = ca_get_disallowmnt;
LG_log(lg_ctx, LG_DEBUG, "UP_check_disallowmnt: disallow_mntner_str %s",
                   disallow_mntner_str ? disallow_mntner_str : "None" );

  if ( disallow_mntner_str )
  {
    /* there are some disallowed mntners */
    /* split the disallow_mntner_str on comma */
    disallow_mntner_list = ut_g_strsplit_v1(disallow_mntner_str, ",", 0);

    /* get the list of all mntner names in the object */
    mntby = rpsl_object_get_attr(preproc_obj, "mnt-by");
    mntners = g_list_concat(mntners, mntby);

    mntlower = rpsl_object_get_attr(preproc_obj, "mnt-lower");
    mntners = g_list_concat(mntners, mntlower);

    mntroutes = rpsl_object_get_attr(preproc_obj, "mnt-routes");
    mntners = g_list_concat(mntners, mntroutes);

    mntdomains = rpsl_object_get_attr(preproc_obj, "mnt-domains");
    mntners = g_list_concat(mntners, mntdomains);

    mntirt = rpsl_object_get_attr(preproc_obj, "mnt-irt");
    mntners = g_list_concat(mntners, mntirt);

    mntattr = rpsl_object_get_attr(preproc_obj, "mntner");
    mntners = g_list_concat(mntners, mntattr);

    refby = rpsl_object_get_attr(preproc_obj, "referral-by");
    mntners = g_list_concat(mntners, refby);
    rpsl_attr_split_multiple(&mntners);

    /* compare the two lists and look for a match */
    for (disallow_idx = 0; disallow_mntner_list[disallow_idx] != NULL && retval==UP_OK; disallow_idx++)
    {
      g_strstrip(disallow_mntner_list[disallow_idx]);
      for (p=mntners; (p != NULL) && retval==UP_OK; p = g_list_next(p))
      {
        value = (char *)rpsl_attr_get_clean_value(p->data);
        LG_log(lg_ctx, LG_DEBUG, "UP_check_disallowmnt: comparing mntner: %s with DISALLOWMNT %s",
                     value, disallow_mntner_list[disallow_idx]);
        if (strcasecmp(value, disallow_mntner_list[disallow_idx]) == 0)
        {
          /* a disallowed mntner has been used */
          retval = UP_FAIL;
          RT_disallowed_mnt(rt_ctx, value);
        }
        free(value);
      }
    }
    rpsl_attr_delete_list(mntners);
    g_strfreev(disallow_mntner_list);
  }

  LG_log(lg_ctx, LG_FUNC,"<UP_check_disallowmnt: exiting with value [%s]", UP_ret2str(retval));
  return retval;
}

/*
    Determines if an inetnum: attribute is in the CIDR notation or not.
    At the moment all this function is doing is it checks if the attribute
    contains a '/' -> is yes it's a CIDR prefix.

    This test might look a bit sloppy but together with the front-end syntax 
    checks it should be enough (and all I could do to improve this is to 
    copy/paste the front-end regex here anyway).
*/   
 
int up_is_inetnum_cidr(rpsl_object_t *object)
{
  int retval = 0;
  GList *attr_list = NULL;

  attr_list = rpsl_object_get_attr(object, "inetnum");

  if (attr_list == NULL)
  {
    return retval;
  }

  if (strchr(rpsl_attr_get_clean_value((rpsl_attr_t *)attr_list->data), '/') != NULL)
  {
    retval = 1;
  }
  rpsl_attr_delete_list(attr_list);
  return retval;
}


/*
 
   Converts a prefix in an inetnum: attribute into range notation.

   The prefix will be converted in the original object passed as argument.

   Receives RT context
            LG context
            the object
            a pointer to a flag that gets set if the conversion took place
            in the function
   Returns  UP_FAIL if any errors found
            UP_OK if all ok

*/

int up_convert_inetnum_prefix(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                               rpsl_object_t *object, 
                               int *inetnum_key_converted)
{
  int ip_retval = IP_OK;
  int is_converted = 0;

  gchar ascii_range[IP_RANGSTR_MAX];

  gchar *prefix;
  ip_range_t range;
  ip_keytype_t ipk;
  GList *attr_list = NULL;
  rpsl_attr_t *attr;

  LG_log(lg_ctx, LG_FUNC,">up_convert_inetnum_prefix: entered");

  attr_list = rpsl_object_get_attr(object, "inetnum");

  if (attr_list == NULL)
  {
    LG_log(lg_ctx, LG_ERROR,"<up_convert_inetnum_prefix: cannot fetch %s inetnum: attribute");
    return UP_FAIL;
  }

  /*
   * Check if the key is in CIDR notation
   * Return without doing anything if it is.
   */
  if ( ! up_is_inetnum_cidr(object))
  {
    LG_log(lg_ctx, LG_ERROR,"<up_convert_inetnum_prefix: not CIDR");
    rpsl_attr_delete_list(attr_list);
    return UP_OK;
  }
      
  prefix = g_strdup(rpsl_attr_get_clean_value((rpsl_attr_t *)attr_list->data));

  if (prefix == NULL)
  {
    LG_log(lg_ctx, LG_ERROR,"<up_convert_inetnum_prefix: g_strdup() returned NULL");
    rpsl_attr_delete_list(attr_list);
    return UP_FAIL;
  }

  /* convert ASCII prefix into binary range representation */
  ip_retval = IP_smart_range(prefix, &range, IP_EXPN, &ipk);

  /* handle error condition when the range could not be parsed */
  if (ip_retval != IP_OK)
  {
    LG_log(lg_ctx, LG_ERROR,"<up_convert_inetnum_prefix: IP_smart_range() could not parse [%s]", prefix);
    RT_inetnum_prefix_convert_failed(rt_ctx, prefix);
    rpsl_attr_delete_list(attr_list);
    g_free(prefix);
    return UP_FAIL;
  }
 
  /* convert the binary range structure into ASCII range representation */
  ip_retval = IP_rang_b2a(&range, ascii_range, IP_RANGSTR_MAX);
  if (ip_retval != IP_OK)
  {
    LG_log(lg_ctx, LG_ERROR,"<up_convert_inetnum_prefix: IP_rang_b2a() failed on [%s]", prefix);
    rpsl_attr_delete_list(attr_list);
    g_free(prefix);
    return UP_FAIL;
  }

  /* the key attribute we'll rewrite */
  attr = (rpsl_attr_t *)rpsl_object_get_attr_by_ofs(object, 0);

  if (attr == NULL)
  {
    LG_log(lg_ctx, LG_FUNC,"<up_convert_inetnum_prefix: cannot fetch attribute at offset 0");
    g_free(prefix);
    return UP_FAIL;
  }

  rpsl_attr_replace_value(attr, ascii_range);

  is_converted = 1;

  if (inetnum_key_converted != NULL)
  {
    *inetnum_key_converted = is_converted;
  }

  /* shall I free() attr at this point? attr was obtained from rpsl_object_get_attr_by_ofs()
   * which I'm not sure allocates memory for the return value -- the doc doesn't say anything
   * about that. */

  LG_log(lg_ctx, LG_FUNC,"<up_convert_inetnum_prefix: exiting with value [%s]", 
      UP_ret2str(UP_OK));

  rpsl_attr_delete_list(attr_list);

  RT_inetnum_prefix_converted(rt_ctx, prefix, ascii_range);
  g_free(prefix);
  return UP_OK;
}

/* checks the filter-set objects.
   makes sure that the object has only one of the "mp-filter:" and "filter:"
    attributes. It cannot have both at the same time. "mp-filter:" attribute
    and the above restriction was added during RPSLng effort. 
   Receives RT context
            LG context
            parsed object
   Returns  UP_FAIL if any errors found
            UP_OK if all ok
*/

int UP_check_filter_set_object(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj)
{
  int retval = UP_OK;
  GList *mp_filter_list = NULL;
  GList *filter_list = NULL;
  int filter_count, mp_filter_count;
  const char *type = NULL;

  LG_log(lg_ctx, LG_FUNC,">UP_check_filter_set_object: entered");

  type = rpsl_object_get_class(preproc_obj);
  /* if the object is not a filter-set object, return OK */
  if ( strcasecmp(type, "filter-set") != 0)
  {
    LG_log(lg_ctx, LG_FUNC,"<UP_check_filter_set_object: exiting with value [%s]",
                            UP_ret2str(UP_OK));
    return UP_OK;
  }

  /* Get the count of mp-filter attributes */
  mp_filter_list = rpsl_object_get_attr(preproc_obj, "mp-filter");
  mp_filter_count = g_list_length(mp_filter_list);

  /* Get the count of filter attributes */
  filter_list = rpsl_object_get_attr(preproc_obj, "filter");
  filter_count = g_list_length(filter_list);

  if ( (filter_count > 0) && (mp_filter_count > 0))
  {
    /* if we have both filter and mp-filter attributes, we have an error */
    retval = UP_FAIL;  
    LG_log(lg_ctx, LG_ERROR,"UP_check_filter_set_object: %s", 
           "a filter-set object cannot contain both mp-filter and filter attributes");
    RT_filter_set_syntax(rt_ctx);
  }
  else
  {
    retval = UP_OK;
  }

  rpsl_attr_delete_list(filter_list);
  rpsl_attr_delete_list(mp_filter_list);

  LG_log(lg_ctx, LG_FUNC,"<UP_check_filter_set_object: exiting with value [%s]",
             UP_ret2str(retval));
  return retval;
}


/* checks the peering-set objects.
   makes sure that the object contain at least one of the "peering:" and
    "mp-peering:" attributes. They are both optional attributes, but at
    least one of them must appear. "mp-peering:" attribute
    and the above restriction was added during RPSLng effort. 
   Receives RT context
            LG context
            parsed object
   Returns  UP_FAIL if any errors found
            UP_OK if all ok
*/

int UP_check_peering_set_object(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                             rpsl_object_t *preproc_obj)
{
  int retval = UP_OK;
  GList *mp_peering_list = NULL;
  GList *peering_list = NULL;
  int peering_count, mp_peering_count;
  const char *type = NULL;

  LG_log(lg_ctx, LG_FUNC,">UP_check_peering_set_object: entered");

  type = rpsl_object_get_class(preproc_obj);
  /* if the object is not a peering-set object, return OK */
  if ( strcasecmp(type, "peering-set") != 0)
  {
    LG_log(lg_ctx, LG_FUNC,"<UP_check_peering_set_object: exiting with value [%s]",
                            UP_ret2str(UP_OK));
    return UP_OK;
  }

  /* Get the count of mp-peering attributes */
  mp_peering_list = rpsl_object_get_attr(preproc_obj, "mp-peering");
  mp_peering_count = g_list_length(mp_peering_list);

  /* Get the count of peering attributes */
  peering_list = rpsl_object_get_attr(preproc_obj, "peering");
  peering_count = g_list_length(peering_list);

  if ( (peering_count == 0) && (mp_peering_count == 0))
  {
    /* if we have both peering and mp-peering attributes, we have an error */
    retval = UP_FAIL;  
    LG_log(lg_ctx, LG_ERROR,"UP_check_peering_set_object: %s", 
           "a peering-set object must contain at least one mp-peering or peering attribute");
    RT_peering_set_syntax(rt_ctx);

  }
  else
  {
    retval = UP_OK;
  }

  rpsl_attr_delete_list(peering_list);
  rpsl_attr_delete_list(mp_peering_list);

  LG_log(lg_ctx, LG_FUNC,"<UP_check_peering_set_object: exiting with value [%s]", 
                   UP_ret2str(retval));
  return retval;
}


/* Gets the Sub Allocation Window size from a lookup file,
   indexed by LIR organisation name.
   Receives RT context
            LG context
            options structure
            organisation
   Returns  saw size if successful
            -1 otherwise
*/

int up_get_saw(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                 options_struct_t *options, char *organisation)
{
  int saw;
  int size;
  int found;
  char *saw_file_name = NULL;
  char buf[256];
  char *line = NULL;
  char *saw_str = NULL;
  char **list = NULL;
  FILE *saw_fh;

  LG_log(lg_ctx, LG_FUNC,">up_get_saw: entered with org reference [%s]", organisation);
  
  saw_file_name = ca_get_sawfile;
  assert(saw_file_name != NULL); /* should never fail */
  if (( saw_fh = fopen(saw_file_name, "r")) == NULL)
  {
    LG_log(lg_ctx, LG_FATAL, "up_get_saw: Can't open SAW lookup file [%s]", saw_file_name);
    UP_internal_error(rt_ctx, lg_ctx, options, "Can't open SAW lookup file\n", 0);
  }

  found = 0;
  /* find the organisation in the file */
  while ( fgets(buf, 255, saw_fh) != NULL ) 
  {
    line = strdup(buf);
    line = UP_remove_EOLs(line);
    list = ut_g_strsplit_v1(line, ",", 0);
    if ( list[0] == NULL )
    {
      /* ignore any blank lines */
      free(line);
      continue; 
    }
    if ( ! strcasecmp(organisation, list[0]) )
    {
      /* found it, now get the SAW */
      found = 1;
      if ( list[1] == NULL )
      {
        LG_log(lg_ctx, LG_DEBUG,"up_get_saw: organisation found, but no SAW value");
      }
      else
      {
        saw_str = strdup(list[1]);
        LG_log(lg_ctx, LG_DEBUG,"up_get_saw: organisation found, SAW string [%s]", saw_str);
      }
      free(line);
      break;
    }
    free(line);
  }
  fclose(saw_fh);
  
  if ( found && saw_str != NULL )
  {
    saw = atoi((const char *)saw_str);
    LG_log(lg_ctx, LG_DEBUG,"up_get_saw: organisation found, SAW slash size [%d]", saw);
    free(saw_str);
    if ( saw > 32 )
    {
      LG_log(lg_ctx, LG_DEBUG,"<up_get_saw: SAW value > 32");
      return -1;
    }
  }
  /* This is the /<size> value,
     now find the size of this */
  size = 2<<(32-saw-1);

  LG_log(lg_ctx, LG_FUNC,"<up_get_saw: exiting with saw [%d]", size);
  return size;
}


/* Gets the size of a range,
   Receives LG context
            value - range string
   Returns  size if successful
            -1 otherwise
*/

int up_get_size_from_range(LG_context_t *lg_ctx, char *value)
{
  int retval;
  char **list = NULL;
  ip_range_t range;
  ip_rangesize_t  span;

  LG_log(lg_ctx, LG_FUNC,">up_get_size_from_range: entered with range [%s]", value);
  retval = IP_rang_t2b(&range, (const char *)value, IP_PLAIN);
  if ( retval != IP_OK )
  {
    /* invalid range */
    LG_log(lg_ctx, LG_DEBUG,"<up_get_size_from_range: IP_rang_t2b returned [%d]", retval);
    return -1;
  }
  LG_log(lg_ctx, LG_DEBUG,"up_get_size_from_range: range.begin [%d]", range.begin.words[0]);
  LG_log(lg_ctx, LG_DEBUG,"up_get_size_from_range: range.begin [%d]", range.end.words[0]);
  span = IP_rang_span(&range) +1;

  LG_log(lg_ctx, LG_FUNC,"<up_get_size_from_range: exiting with size [%d]", (int)span);
  return (int)span;
}


/* Check if override password was supplied.
   Receives LG context
            credentials list
   Returns  UP_OK if supplied
            UP_FAIL otherwise
*/

int up_override(LG_context_t *lg_ctx, GList *credentials)
{
  int retval = UP_FAIL;
  char *override_pwd;
  size_t len;

  LG_log(lg_ctx, LG_FUNC, ">up_override: entered");

  override_pwd = ca_get_overridecryptedpw;
  /* chop off any trailing newline */
  len = strlen(override_pwd);
  if ( (len > 0) && (override_pwd[len-1] == '\n') )
  {
    override_pwd[len-1] = '\0';
  }
  
  if ( CR_credential_list_check(credentials, CR_OVERRIDE, override_pwd, FALSE) )
  {
    /* override password was supplied */
    retval = UP_OK;
  }
  
  return retval;
}


/* Applies policy checks to an object.
   Receives RT context
            LG context
            options structure
            parsed object
            operation
            reason pointer to policy check fail string
   Returns  UP_OK if successful
            UP_FWD otherwise
*/

int UP_check_policy(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                     options_struct_t *options, int operation, 
                     rpsl_object_t *preproc_obj, char **reason,
                     GList *credentials)
{
  int retval = UP_OK;
  int size;
  int saw;
  const char *type = NULL;
  char *value;
  GList *attr = NULL;

  LG_log(lg_ctx, LG_FUNC,">UP_check_policy: entered");

  if ( up_override(lg_ctx, credentials) == UP_OK )
  {
    /* approved by hostmaster */
    LG_log(lg_ctx, LG_DEBUG,"UP_check_policy: override used");
    retval = UP_OK;
  }
  else
  {
    /* policy checks */
    /* Check size against the sub allocation window when creating an 
       inetnum object with a status of SUB-ALLOCATED PA */
    type = rpsl_object_get_class(preproc_obj);
    assert(type != NULL); /* should never happen */
    if ( ( ! strcasecmp(type, "inetnum")) && operation==UP_CREATE  ) 
    {
      attr = rpsl_object_get_attr(preproc_obj, "status"); 
      value = rpsl_attr_get_clean_value((rpsl_attr_t *)(attr->data));
      if ( ! strcasecmp(value, "SUB-ALLOCATED PA") )
      {
        LG_log(lg_ctx, LG_DEBUG,"UP_check_policy: SUB-ALLOCATED PA inetnum creation");
        attr = rpsl_object_get_attr(preproc_obj, type); 
        value = rpsl_attr_get_clean_value((rpsl_attr_t *)(attr->data));
        size = up_get_size_from_range(lg_ctx, value);

        attr = rpsl_object_get_attr(preproc_obj, "org"); 
        value = rpsl_attr_get_clean_value((rpsl_attr_t *)(attr->data));
        saw = up_get_saw(rt_ctx, lg_ctx, options, value);

        if ( size > saw )
        {
          LG_log(lg_ctx, LG_DEBUG,"UP_check_policy: size > saw");
          *reason = "Sub allocation size exceeds SAW";
          retval = UP_FWD;
        }
      }
    }
  }
  
  LG_log(lg_ctx, LG_FUNC,"<UP_check_policy: exiting with return status [%s]",
                            UP_ret2str(retval));
  return retval;
}


/* Gets the certif attributes from a key-cert object and packs the data into
   a string with newlines appended to each line.
   Use rpsl_attr_get_clean_lines to make sure blank certif values are transposed
   into blank lines in the output string.
   Receives LG context
            parsed key-cert object
   Returns  key certif data string
*/

char *UP_get_certif_data(LG_context_t *lg_ctx, rpsl_object_t *preproc_obj)
{
  char *value;
  char *key;
  GList *certiflist, *item;
  GString *gkey;

  LG_log(lg_ctx, LG_FUNC,">up_get_certif_data: entered");

  /* get the certif data first */
  certiflist = rpsl_object_get_attr(preproc_obj, "certif");

  gkey = g_string_sized_new(1024);
  for( item = certiflist; item != NULL; item = g_list_next(item) )
  {
    value = rpsl_attr_get_clean_lines( (rpsl_attr_t *)(item->data) );
	if (value == NULL)
	{ /* if this was an empty attribute, just print an empty line */
      g_string_sprintfa(gkey, "\n"); 
    }
	else
	{
	  g_string_sprintfa(gkey, "%s\n", value);
	  free(value);
	}
  }

  rpsl_attr_delete_list(certiflist);
  key = gkey->str;
  g_string_free(gkey, FALSE);
  LG_log(lg_ctx, LG_FUNC,"<up_get_certif_data: exiting with key [\n%s]\n", key);
  return key;
}


/* gets data from a key-cert object
   Receives RT context
            LG context
            key_data pointer
            pointer to internal key data structure
            parsed object
            key-cert type
   Returns  KM_OK if successful
            anything else is a failure.
*/

int UP_get_key_data(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                         key_info_t *key_info, KM_key_return_t **key_data,
                         rpsl_object_t *preproc_obj,
                         KM_context_t key_cert_type)
{
  KM_status_t key_status;
  char *key = NULL;

  LG_log(lg_ctx, LG_FUNC,">UP_get_key_data: entered with key cert type [%s]",
                        KM_context_string((int)key_cert_type));

  /* get the key data and extract some info from it */
  key = UP_get_certif_data(lg_ctx, preproc_obj);
  key_info->key = key;
  key_status = KM_key_get_info(key_cert_type, key, key_data);
  /* key_data is a KM internal structure and should only be accessed 
     with KM supplied accessing functions */
  /* report any errors */
  RT_report_key_info(rt_ctx, *key_data);

  if ( key_cert_type == KM_PGP )
  {
    if ( key_status == KM_OK )
    {
      key_info->key_id = strdup(KM_key_return_get_key_id(*key_data));
      LG_log(lg_ctx, LG_DEBUG,"UP_get_key_data: key_id [%s]", key_info->key_id);
    }
    else
    {
      LG_log(lg_ctx, LG_DEBUG,"UP_get_key_data: no key_id set");
    }
  }

  LG_log(lg_ctx, LG_FUNC,"<UP_get_key_data: exiting with KM return status %d",key_status);
  return key_status;
}


/* Processes a generated attribute for a key-cert object.
   The object may already contain this attribute. 
   If present and it does not match the generated value
   a warning message is included in the ack and the
   supplied value is replaced by the new generated value.
   Receives LG context
            parsed object
            attribute type (method, owner, fingerpr)
            generated attribute
            generated attribute value
            pointer to warnings list
            preset position of generated attribute in object
   Returns  none
*/

void up_process_attr(LG_context_t *lg_ctx, rpsl_object_t *preproc_obj,
                       char *attr_type, rpsl_attr_t *gen_attr, const char *gen_value,
                       GList **warn_list, int preset_pos)
{
  char *sup_value;
  int pos;
  char *gen;
  GList *attr_list = NULL;

  LG_log(lg_ctx, LG_FUNC,">up_process_attr: entered with generated %s attribute [%s]",
                                attr_type, gen_value);
  gen = strdup(gen_value);
  g_strstrip(gen);

  /* check if supplied object already has this attribute */
  if ( (attr_list = rpsl_object_get_attr(preproc_obj, attr_type)) )
  {
    /* List with one item only, get the supplied value */
    /* Don't use rpsl_attr_get_clean_value here as it reducez the two
       spaces in the middle of the fingerpr: atr to one space */
    sup_value = (char *)rpsl_attr_get_value((rpsl_attr_t *)(attr_list->data));
    g_strstrip(sup_value);
    LG_log(lg_ctx, LG_DEBUG, "up_process_attr: supplied %s attribute [%s]", attr_type, sup_value );

    /* compare supplied value with generated one */
    if ( strcasecmp(sup_value, gen) )
    {
      /* values are different, add warning */
      *warn_list = g_list_append (*warn_list, attr_type); 
      LG_log(lg_ctx, LG_DEBUG, "up_process_attr: WARNING %s attribute different", attr_type);

      /* replace supplied attribute with generated one */
      pos = rpsl_attr_get_ofs((rpsl_attr_t *)(attr_list->data));

      /* remove the supplied attribute */
      rpsl_object_remove_attr(preproc_obj, pos, NULL);

      /* insert new generated attribute in same position */
      rpsl_object_add_attr(preproc_obj, gen_attr, pos, NULL);
      LG_log(lg_ctx, LG_DEBUG, 
         "up_process_attr: supplied %s attribute replaced with generated %s attribute",
                            attr_type, attr_type );
    }
    else 
    {
      /* values are the same, no need to change */
      LG_log(lg_ctx, LG_DEBUG, "up_process_attr: supplied %s attribute used", attr_type);
    }

    rpsl_attr_delete_list(attr_list);
  }
  else
  {
    /* attribute not supplied, so add generated attribute */
    rpsl_object_add_attr(preproc_obj, gen_attr, preset_pos, NULL);
    LG_log(lg_ctx, LG_DEBUG, "up_process_attr: generated %s attribute used", attr_type);
  }

  free(gen);
  LG_log(lg_ctx, LG_FUNC,"<up_process_attr: exiting");
}


/* Adds the generated attrs to a key-cert object.
   The object may already contain these attributes. 
   If present and they do not match the generated values
   a warning message is included in the ack and the
   supplied values are replaced by the new generated values.
   Receives RT context
            LG context
            key_data pointer
            parsed object
            key-cert type
   Returns  UP_OK if successful
            UP_FAIL otherwise
*/

int UP_generate_keycert_attrs(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                           key_info_t *key_info, rpsl_object_t *preproc_obj, 
                           KM_context_t key_cert_type)
{
  int retval = UP_OK;
  const char *type;
  char *attr_str;
  char *method = "PGP";
  const char *keyowner = NULL;
  const char *fingerprint = NULL;
  int key_status;
  rpsl_attr_t *method_attr;
  rpsl_attr_t *owner_attr;
  rpsl_attr_t *fingerpr_attr;
  KM_key_return_t *key_data = NULL;
  GList *warn_list = NULL;

  LG_log(lg_ctx, LG_FUNC,">UP_generate_keycert_attrs: entered for key-cert type [%s]", 
                          KM_context_string((int)key_cert_type));
  
  type = rpsl_object_get_class(preproc_obj);

  /* get the key data and extract some info from it */
  key_status = UP_get_key_data(rt_ctx, lg_ctx, key_info, &key_data, preproc_obj, key_cert_type);

  if ( key_status == KM_OK )
  {
    /* method: attribute, generate value */
    method = strdup(KM_context_string((int)key_cert_type));
    attr_str = (char *)malloc( strlen("method: ") +5 );
    strcpy(attr_str, "method: ");
    if ( key_cert_type == KM_PGP )
    {
      strcat(attr_str, "PGP");
    }
    else
    {
      strcat(attr_str, "X509");
    }

    /* process the attribute */
    if ( (method_attr = rpsl_attr_init(attr_str, type)) )
    {
      up_process_attr(lg_ctx, preproc_obj, "method", method_attr, method, &warn_list, 1);
    }
    free(attr_str);
    free(method);

    /* owner: attribute, generate value */
    keyowner = KM_key_return_get_parameter(key_data, "key_owner");
    assert(keyowner != NULL);  /* should never happen */
    attr_str = (char *)malloc( strlen("owner: ") + strlen(keyowner) +1 );
    strcpy(attr_str, "owner: ");
    strcat(attr_str, keyowner);

    /* process the attribute */
    if ( (owner_attr = rpsl_attr_init(attr_str, type)) )
    {
      up_process_attr(lg_ctx, preproc_obj, "owner", owner_attr, keyowner, &warn_list, 2);
    }
    free(attr_str);

    /* fingerprint: attribute, generate value */
    fingerprint = KM_key_return_get_parameter(key_data, "finger_print");
    assert(fingerprint != NULL);  /* should never happen */
    attr_str = (char *)malloc( strlen("fingerpr: ") + strlen(fingerprint) +1 );
    strcpy(attr_str, "fingerpr: ");
    strcat(attr_str, fingerprint);

    /* process the attribute */
    if ( (fingerpr_attr = rpsl_attr_init(attr_str, type)) )
    {
      up_process_attr(lg_ctx, preproc_obj, "fingerpr", fingerpr_attr, fingerprint, &warn_list, 3);
    }
    free(attr_str);
    
    /* report warnings, if any */
    if ( warn_list )
    {
      RT_kc_gen_diff(rt_ctx, warn_list);
      g_list_free(warn_list);
    }
  }
  else
  {
    retval = UP_FAIL;
  }
  
  KM_key_return_free(key_data);
  LG_log(lg_ctx, LG_FUNC,"<UP_generate_keycert_attrs: exiting with value [%s]", UP_ret2str(retval));
  return retval;
}
