/***************************************
  $Id $

  dbupdate 

  Status: NOT REVIEWED, NOT TESTED

  Author(s):       Denis Walker

  ******************/ /******************
  Modification History:
        denis (20021021) Created.
  ******************/ /******************
  Copyright (c) 2002                         RIPE NCC

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
#include <assert.h>
#include <sys/stat.h>

#include "rip.h"
#include "dbupdate.h"
#include "up_util.h"
#include "ep.h"
#include "ut.h"

#include <glib.h>


/* list of command line options */
/* short form, long form, argument 
   argument 0-none, 1-required, 2-optional, 3-certificate */
opt opt_list[] = { { 'm', "mail", 0},
                   { 'f', "file", 1},
                   { 'c', "config", 1},
                   { 'd', "debug", 2},
                   { 't', "test", 0},
                   { 'r', "redirect", 0},
                   { 'p', "print", 0},
                   { 'o', "origin", 1},
                   { 'k', "keywords", 1},
                   { 'x', "x509", 1},
                   { 'X', "cert", 3},
                   { 's', "save", 0}
                 };

/* list of keywords */
key_words keywords[] = { {"help", UP_HELP},
                         {"howto", UP_HOWTO},
                         {"new", UP_NEW},
                         {"diff", UP_NOTIF_DIFF}
                       };


/* Checks a file name to see if the file exists and can be accessed.
   Receives file name
   Returns  1 if the file is OK
            0 if there is a problem with the file
*/

int check_file(char * file_name)
{    
  int file_valid = 1;
  struct stat stat_buf;

  /* check for the existence of the file */
  if ( stat(file_name, &stat_buf) )
  {
    file_valid = 0;
  }
  else
  {
    if ( ! S_ISREG(stat_buf.st_mode) )
    {
      file_valid = 0;
    }
  }

  return file_valid;
}

/* Sets an option in the options structure for reference later.
   Receives options structure
            option string or character
            argument or null 
            form of option, long or short
   Returns  none
            ( sets values in options structure )
*/

void set_option(options_struct_t *options, char *arg_str, char *optarg, int form)
{
  /* set up the values in the options structure */
  if ( (form==0 && *arg_str=='m') || (form==1 && !strcmp(arg_str, "mail")) )
  {
    options->mail_input = 1;
  }
  if ( (form==0 && *arg_str=='s') || (form==1 && !strcmp(arg_str, "save")) )
  {
    options->save = 1;
  }
  else if ( (form==0 && *arg_str=='f') || (form==1 && !strcmp(arg_str, "file")) )
  {
    options->input_file_name = optarg;
  }
  else if ( (form==0 && *arg_str=='c') || (form==1 && !strcmp(arg_str, "config")) )
  {
    options->config_file_name = optarg;
  }
  else if ( (form==0 && *arg_str=='d') || (form==1 && !strcmp(arg_str, "debug")) )
  {
    options->debug = 1;
    if ( optarg )
    {
      options->debug_level = atoi(optarg);
      free(optarg);
    }
    else
    {
      options->debug_level = DEBUG_LEVEL;
    }
  }
  else if ( (form==0 && *arg_str=='t') || (form==1 && !strcmp(arg_str, "test")) )
  {
    options->test_mode = 1;
  }
  else if ( (form==0 && *arg_str=='r') || (form==1 && !strcmp(arg_str, "redirect")) )
  {
    options->redirect = 1;
  }
  else if ( (form==0 && *arg_str=='p') || (form==1 && !strcmp(arg_str, "print")) )
  {
    options->print = 1;
  }
  else if ( (form==0 && *arg_str=='o') || (form==1 && !strcmp(arg_str, "origin")) )
  {
    options->origin = optarg;
  }
  else if ( (form==0 && *arg_str=='k') || (form==1 && !strcmp(arg_str, "keywords")) )
  {
    options->keywords = optarg;
  }
  else if ( (form==0 && *arg_str=='x') || (form==1 && !strcmp(arg_str, "x509")) )
  {
    options->X509cert_file = optarg;
  }
  else if ( (form==0 && *arg_str=='X') || (form==1 && !strcmp(arg_str, "cert")) )
  {
    options->X509certificate = optarg;
  }
  /* no need for a final 'else invalid' as they have already been checked */
}


/* Determines if an option is valid and if it needs an argument.
   Receives option string or character
            form of option, long or short
            argument required flag pointer
   Returns  UP_OK or UP_FAIL
            sets arg_required flag
*/

int isoption(char *option, int form, int *arg_required)
{
  int opt_idx = 0;
  int found = 0;
  char opt_char[2];
  char *unrec_opt = NULL;
  
  /* find option in list */
  for ( opt_idx=0; opt_idx<sizeof(opt_list)/sizeof(opt); opt_idx++ )
  {
    if ( (form == 0 && *option == opt_list[opt_idx].option) ||
         (form == 1 && ! strcmp(option, opt_list[opt_idx].long_opt)) )
    {
      found = 1;
      *arg_required = opt_list[opt_idx].arg_required;
    }
  }
  
  if ( !found )
  {
    /* unrecognised option */
    if ( form == 0 )
    {
      unrec_opt = malloc(3);
      strcpy(unrec_opt, "-");
      opt_char[0] = *option;
      opt_char[1] = '\0';
      strcat(unrec_opt, opt_char);
    }
    else
    {
      unrec_opt = malloc(3+strlen(option));
      strcpy(unrec_opt, "--");
      strcat(unrec_opt, option);
    }

    fprintf(stderr, "isoption: invalid option %s\n", unrec_opt);
    free(unrec_opt);
  }
  
  return found;
}

/* Process the command line arguments.
   Receives options structure
            count of number of arguments
            array of argument strings
            pointer to argument string
   Returns  UP_OK or UP_FAIL
*/

int process_arguments(options_struct_t *options,
                        int argc, char **argv, char **args)
{
  int arg_cnt = 0;
  int arg_required = 0;
  int form = 0;
  int ret_val = UP_OK;
  char *arg_str = NULL;
  char *optarg = NULL;
  char *noarg_opt = NULL;
  char *arg_list = NULL;
  char opt_char[2];
  char end_cert;
  char end_cert_start;
  GString *cert;
  
  /* build a string of command line arguments to log later*/
  for ( arg_cnt = 0; arg_cnt < argc; arg_cnt++ )
  {
    if ( argv[arg_cnt] != NULL )
    {
      if ( arg_list == NULL )
        arg_list = calloc(1, strlen(argv[arg_cnt]) +1);
      else
      {
        arg_list = realloc(arg_list, strlen(arg_list) + strlen(argv[arg_cnt]) +2);
        strcat(arg_list, " ");
      }
      strcat(arg_list, argv[arg_cnt]);
    }
  }
  *args = arg_list;
//printf("arguments\n%s\n", arg_list);

  /* process each argument */
  for ( arg_cnt=1; arg_cnt < argc; arg_cnt++ )
  {
    arg_str = argv[arg_cnt];
    if ( *arg_str == '-' )
    {
      /* start of an option */
      if ( *(++arg_str) == '\0' )
      {
        /* invalid option, nothing follows the '-' */
        /* LG_log(lg_ctx, LG_ERROR,"process_arguments: invalid option -"); */
        fprintf(stderr, "process_arguments: invalid option -\n");
        continue;
      }
      if ( *arg_str == '-' && *(arg_str+1) == '\0' )
      {
        /* invalid option, nothing follows the '--' */
        /* LG_log(lg_ctx, LG_ERROR,"process_arguments: invalid option --"); */
        fprintf(stderr, "process_arguments: invalid option --\n");
        continue;
      }
      
      /* determine form of option - short 0, long 1 */
      form = ( *arg_str == '-' ) ? 1 : 0;

      if ( form == 1 )
      {
        /* long form */
        /* move past second '-' */
        ++arg_str;  
        
//printf("long form arg [%s]\n", arg_str);
        /* if this option is recognised, does it need an argument */
        if ( ! isoption(arg_str, form, &arg_required) )
        {
          ret_val = UP_FAIL;
          continue; /* option not recognised */
        }
        
        if ( arg_required )
        {
//printf("arg_required [%d]\n", arg_required);
          /* look for the next argument, 
             which must not start with a '-' unless it is a certificate */
          if ( arg_cnt+1==argc || *(argv[arg_cnt+1])=='\0' || 
                 ( *(argv[arg_cnt+1])=='-' && arg_required != 3 ) )
          {
            /* no following argument found for this option */
            if ( arg_required != 2 )  /* optional */
            {
              /* argument is mandatory */
              noarg_opt = malloc(3+strlen(arg_str));
              strcpy(noarg_opt, "--");
              strcat(noarg_opt, arg_str);
              /* LG_log(lg_ctx, LG_ERROR,"process_arguments: missing argument for option %s", noarg_opt); */
              fprintf(stderr, "process_arguments: missing argument for option %s\n", noarg_opt);
              free(noarg_opt);
              noarg_opt = NULL;
              ret_val = UP_FAIL;
              continue;
            }
          }
          else
          {
            /* process the argument for this option */
            if ( arg_required != 3 )
            {
              optarg = strdup(argv[++arg_cnt]);
            }
            else
            {
//printf("certificate follows\n");
              /* a certificate follows */
              end_cert = 0;
              end_cert_start = 0;
              cert = g_string_new(NULL);
              while ( arg_cnt+1 != argc && ! end_cert )
              {
                cert = g_string_append(cert, argv[++arg_cnt]);
//printf("[%s]\n", argv[arg_cnt]);
                if ( strstr(argv[arg_cnt], "-----END") )
                {
                  end_cert_start = 1;
                }
                if ( strstr(argv[arg_cnt], "CERTIFICATE-----") && end_cert_start )
                {
                  end_cert = 1;
                }
              }
//printf("certificate read [%s]\n", cert->str);

              if ( ! end_cert )
              {
//printf("certificate incomplete\n");
                /* the certificate is incomplete */
                noarg_opt = malloc(3+strlen(arg_str));
                strcpy(noarg_opt, "--");
                strcat(noarg_opt, arg_str);
                fprintf(stderr, "process_arguments: missing END CERTIFICATE for option %s\n", noarg_opt);
                free(noarg_opt);
                noarg_opt = NULL;
                ret_val = UP_FAIL;
                g_string_free(cert, 1);
                continue;
              }
              else
              {
                optarg = strdup(cert->str);
                g_string_free(cert, 1);
              }
            }
          }
        }

        set_option(options, arg_str, optarg, form);
        optarg = NULL;
      }
      else
      {
        /* short form */
        /* this could be a single option or string of options
           the single option or the last of a string of options 
           may require a following argument */
        
        /* loop through the string of options in this arg */
        do
        {
          /* if there is a single '-' in the middle of a string of options,
             then silently ignore it */
          if ( *arg_str=='-' && *(arg_str+1) != '\0' && *(arg_str+1) != '-')
            continue;
          /* If there is a "--" in the middle of a string of options,
             reject this argument as an invalid option. */
          if ( *arg_str=='-' && *(arg_str+1) != '\0' && *(arg_str+1) == '-')
          {
            /* LG_log(lg_ctx, LG_ERROR,"process_arguments: invalid option %s", argv[arg_cnt]); */
            fprintf(stderr, "process_arguments: invalid option %s\n", argv[arg_cnt]);
            ret_val = UP_FAIL;
            break;
          }
          
          /* if this option is recognised, does it need an argument */
          if ( ! isoption(arg_str, form, &arg_required) )
          {
             ret_val = UP_FAIL;
             continue; /* argument not recognised */
          }

          if ( arg_required )
          {
            /* this must be the last option if it is a string of options
               and there must be a following argument, 
               which must not start with a '-' unless it is a certificate */
            if ( *(arg_str+1) !='\0' || arg_cnt+1==argc || 
                    argv[arg_cnt+1]==NULL || 
                    ( *(argv[arg_cnt+1])=='-' && arg_required != 3 ) )
            {
              /* no following argument found for this option */
              if ( arg_required != 2 )  /* optional */
              {
                /* argument is mandatory */
                noarg_opt = malloc(3);
                strcpy(noarg_opt, "-");
                opt_char[0] = *arg_str;
                opt_char[1] = '\0';
                strcat(noarg_opt, opt_char);
                /* LG_log(lg_ctx, LG_ERROR,"process_arguments: missing argument for option %s", noarg_opt); */
                fprintf(stderr, "process_arguments: missing argument for option %s\n", noarg_opt);
                free(noarg_opt);
                noarg_opt = NULL;
                ret_val = UP_FAIL;
                continue;
              }
            }
            else
            {
              /* process the argument for this option */
              if ( arg_required != 3 )
              {
                optarg = strdup(argv[++arg_cnt]);
              }
              else
              {
                /* a certificate follows */
                end_cert = 0;
                cert = g_string_new(NULL);
                while ( arg_cnt+1 != argc && ! end_cert )
                {
                  cert = g_string_append(cert, argv[++arg_cnt]);
                  if (  strstr(argv[arg_cnt], "-----END CERTIFICATE-----") )
                  {
                    end_cert = 1;
                  }
                }

                if ( ! end_cert )
                {
                  /* the certificate is incomplete */
                  noarg_opt = malloc(3);
                  strcpy(noarg_opt, "-");
                  opt_char[0] = *arg_str;
                  opt_char[1] = '\0';
                  strcat(noarg_opt, opt_char);
                  fprintf(stderr, "process_arguments: missing END CERTIFICATE for option %s\n", noarg_opt);
                  free(noarg_opt);
                  noarg_opt = NULL;
                  ret_val = UP_FAIL;
                  g_string_free(cert, 1);
                  continue;
                }
                else
                {
                  optarg = strdup(cert->str);
                  g_string_free(cert, 1);
                }
              }
            }
          }
            
          set_option(options, arg_str, optarg, form);
          optarg = NULL;
        } while ( *(++arg_str) != '\0' );
      }
    }
    else
    {
      /* invalid option */
      /* LG_log(lg_ctx, LG_ERROR,"process_arguments: invalid option %s", arg_str); */
      fprintf(stderr, "process_arguments: invalid option %s\n", arg_str);
      ret_val = UP_FAIL;
      continue;
    }
  }

  /* report any invalid combination of arguments */
  /* -m and -k are mutually exclusive */
  if ( options->mail_input && options->keywords )
  {
    /* LG_log(lg_ctx, LG_ERROR,"process_arguments: invalid combination of options -m and -k"); */
    fprintf(stderr, "process_arguments: invalid combination of options -m and -k\n");
    ret_val = UP_FAIL;
  }
  
  /* -m and -x are mutually exclusive */
  if ( options->mail_input && options->X509cert_file )
  {
    /* LG_log(lg_ctx, LG_ERROR,"process_arguments: invalid combination of options -m and -x"); */
    fprintf(stderr, "process_arguments: invalid combination of options -m and -x\n");
    ret_val = UP_FAIL;
  }
  
  /* -c is mandatory */
  if ( options->config_file_name == NULL )
  {
    /* LG_log(lg_ctx, LG_ERROR,"process_arguments: missing mandatory option -c"); */
    fprintf(stderr, "process_arguments: missing mandatory option -c\n");
    ret_val = UP_FAIL;
  }
  
  /* LG_log(lg_ctx, LG_FUNC,"<process_arguments: exiting with value %s", UP_ret2str(ret_val)); */
  return ret_val;
}


/* Logs the options set from the command line.
   Receives LG context
            options structure
            command line string
   Returns  none
*/

void log_options(LG_context_t *lg_ctx, options_struct_t *options, char *args_str)
{
  LG_log(lg_ctx, LG_FUNC,">log_options: entered");

  /* log command line string supplied */
  LG_log(lg_ctx, LG_DEBUG,"log_options: command line = [%s]", args_str);

  /* log the options set in the options structure from parsing the command line */
  if ( options->mail_input )
    LG_log(lg_ctx, LG_DEBUG,"log_options: mail input set");
  if ( options->input_file_name )
    LG_log(lg_ctx, LG_DEBUG,"log_options: input file name = %s", options->input_file_name);
  if ( options->config_file_name )
    LG_log(lg_ctx, LG_DEBUG,"log_options: config file name = %s", options->config_file_name);
  if ( options->debug )
    LG_log(lg_ctx, LG_DEBUG,"log_options: debug set to level %d", options->debug_level);
  if ( options->test_mode )
    LG_log(lg_ctx, LG_DEBUG,"log_options: test mode set");
  if ( options->redirect )
    LG_log(lg_ctx, LG_DEBUG,"log_options: redirect set");
  if ( options->save )
    LG_log(lg_ctx, LG_DEBUG,"log_options: save set");
  if ( options->print )
    LG_log(lg_ctx, LG_DEBUG,"log_options: print set");
  if ( options->origin )
      LG_log(lg_ctx, LG_DEBUG,"log_options: origin = %s", options->origin);
  if ( options->keywords )
    LG_log(lg_ctx, LG_DEBUG,"log_options: keywords = %s", options->keywords);
  if ( options->X509cert_file )
    LG_log(lg_ctx, LG_DEBUG,"log_options: X509 certificate file name = %s", options->X509cert_file);
  if ( options->X509certificate )
    LG_log(lg_ctx, LG_DEBUG,"log_options: X509 certificate = %s", options->X509certificate);

  LG_log(lg_ctx, LG_FUNC,"<log_options: exiting");
}


/* Process the keywords, report any invalid keywords or an invalid combination.
   Receives RT context
            LG context
            options structure
            input data structure
   Returns  sets values in options structure
*/

void check_keywords(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                       options_struct_t *options, ep_input_structure_t *input)
{
  int kw_idx, kw_item;
  int invalid = 0;
  int found;
  char *keyword_str = NULL;
  char *ptrlc = NULL;
  char *ptruc = NULL;
  char **keywords_list = NULL;
  int keywords_found[NUMBER_OF_KEYWORDS];
  GString *invalid_keywords;

  LG_log(lg_ctx, LG_FUNC,">check_keywords: entered");

  invalid_keywords = g_string_new(NULL);

  /* if it is a mail message, get a list of the keywords from the subject line. 
     These are returned as a comma seperated list. */
  if ( options->mail_input )
  {
    keyword_str = (char *)EP_get_candidate_keywords(input);
//printf("keyword_str [%s]\n", keyword_str);
    /* if there is a keywords: tag on the subject line, 
       skip past everything up to and including it */
    if ( keyword_str && ( (ptrlc = strstr(keyword_str, "keywords:")) ||
                (ptruc = strstr(keyword_str, "KEYWORDS:")) ) )
    {
      keyword_str = ptrlc ? ptrlc + 10 : ptruc + 10;  /* yes it is 10, don't forget the comma */
//printf("keyword_str [%s]\n", keyword_str);
    }
  }
  else keyword_str = options->keywords;

  /* initialise the keywords_found array */
  for (kw_idx = 0; kw_idx < NUMBER_OF_KEYWORDS; kw_idx++)
  {
    keywords_found[kw_idx] = 0;
  }

  if ( keyword_str)
  {
    LG_log(lg_ctx, LG_DEBUG,"check_keywords: keywords [%s]", keyword_str);
    /* split the keyword string on comma */
    keywords_list = ut_g_strsplit_v1(keyword_str, ",", 0);
    
    /* check for valid and invalid keywords */
    for (kw_idx = 0; keywords_list[kw_idx] != NULL; kw_idx++)
    {
      found = 0;
      for ( kw_item=0; kw_item<sizeof(keywords)/sizeof(key_words); kw_item++ )
      {
        if ( ! strcasecmp(keywords_list[kw_idx], keywords[kw_item].word) )
        {
          /* found a valid keyword */
          LG_log(lg_ctx, LG_DEBUG,"check_keywords: valid keyword [%s]", keywords_list[kw_idx]);
          keywords_found[keywords[kw_item].index] = 1;
          found = 1;
          break;
        }
      }
      if ( ! found )
      {
        LG_log(lg_ctx, LG_DEBUG,"check_keywords: invalid keyword [%s]", keywords_list[kw_idx]);
        if ( invalid ) invalid_keywords = g_string_append(invalid_keywords, ", ");
        invalid_keywords = g_string_append(invalid_keywords, keywords_list[kw_idx]);
        invalid = 1;
      }
    }
    g_strfreev(keywords_list);
    
    if ( invalid )
    {
      /* at least one invalid keyword found, so ignoring all keywords */
      RT_invalid_keyword(rt_ctx, invalid_keywords->str);
      LG_log(lg_ctx, LG_WARN,
          "check_keywords: invalid keyword(s) found [%s]\nAll keywords will be ignored", 
          keyword_str);
    }
    else
    {
      /* check that the keywords combination is valid */
      if ( (keywords_found[UP_HELP] || keywords_found[UP_HOWTO]) && 
               keywords_found[UP_NEW] )
      {
        /* illegal combination */
        RT_invalid_keyword_combination(rt_ctx, "NEW and HELP/HOWTO");
        LG_log(lg_ctx, LG_WARN,
            "check_keywords: invalid combination of keywords NEW and HELP/HOWTO\nAll keywords will be ignored");
      }
      else if ( (keywords_found[UP_HELP] || keywords_found[UP_HOWTO]) && 
               keywords_found[UP_NOTIF_DIFF] )
      {
        /* illegal combination */
        RT_invalid_keyword_combination(rt_ctx, "DIFF and HELP/HOWTO");
        LG_log(lg_ctx, LG_WARN,
            "check_keywords: invalid combination of keywords DIFF and HELP/HOWTO\nAll keywords will be ignored");
      }
      else
      {
        /* set options values for keywords */
        if ( keywords_found[UP_HELP] || keywords_found[UP_HOWTO] )
        {
          options->help = 1;
          LG_log(lg_ctx, LG_DEBUG,"check_keywords: options->help= %d", options->help);
        }
        if ( keywords_found[UP_NEW] )
        {
          options->enforced_new = 1;
          LG_log(lg_ctx, LG_DEBUG,"check_keywords: options->enforced_new= %d", options->enforced_new);
        }
        if ( keywords_found[UP_NOTIF_DIFF] )
        {
          options->notif_diff = 1;
          LG_log(lg_ctx, LG_DEBUG,"check_keywords: options->notif_diff= %d", options->notif_diff);
        }
      }
    }
  }
  else
    LG_log(lg_ctx, LG_DEBUG,"check_keywords: no keywords found");
  
  g_string_free(invalid_keywords, 1);
  LG_log(lg_ctx, LG_FUNC,"<check_keywords: exiting");
}


/* Builds up a list of blobs (submissions with associated credentals).
   Receives an input blob
            list pointer
   Returns  adds blob to list
*/

void get_input_submission_list(ep_authenticated_data_t *blob, gpointer user_data)
{
   /* add blobs to list */
   GList **plist;

   plist = (GList**)user_data;

   *plist = g_list_append(*plist, blob);
}


/* Initialises the modules needed to get the input.
   Receives RT context
            LG context
            options structure
            pointer to server list
            pointer to source list
            pointer to unique server list
   Returns  none
*/

void get_input_init(RT_context_t *rt_ctx, LG_context_t *lg_ctx,
                                 options_struct_t *options,
                                 GList **server_list, GList **source_list,
                                 GList **unique_server_list)
{
  char *gpgcmd  = NULL;
  char *opensslcmd  = NULL;
  char *tmp_dir = NULL;

  LG_log(lg_ctx, LG_FUNC,">get_input_init: entered");

  /* initialise the lookup module */
  LU_init(lg_ctx);

  gpgcmd = ca_get_gpgcmd;
//  opensslcmd = ca_get_opensslcmd;
  opensslcmd = strdup(OPENSSL_CMD);
  tmp_dir = ca_get_tmpdir;
  LG_log(lg_ctx, LG_INFO,"get_input_init: gpgcmd %s", gpgcmd ? gpgcmd : "NULL");
  LG_log(lg_ctx, LG_INFO,"get_input_init: opensslcmd  %s", opensslcmd ? opensslcmd : "NULL");
  LG_log(lg_ctx, LG_INFO,"get_input_init: tmp_dir %s", tmp_dir ? tmp_dir : "NULL");
  
  if ( ! gpgcmd )
  {
    UP_internal_error(rt_ctx, lg_ctx, options, "No gpg command returned from CA\n", 0);
  }
  if ( ! opensslcmd )
  {
    UP_internal_error(rt_ctx, lg_ctx, options, "No opensslcmd command returned from CA\n", 0);
  }
  if ( ! tmp_dir )
  {
    tmp_dir = strdup("/tmp");
  }
  
  /* initialise key management library */
  LG_log(lg_ctx, LG_INFO,"get_input_init: initialise key management");
  UP_connect_all_servers(rt_ctx, lg_ctx, options, server_list, source_list, unique_server_list);
  KM_init(KM_PGP, rt_ctx, lg_ctx, *server_list, *source_list, tmp_dir, gpgcmd );
  KM_init(KM_X509, rt_ctx, lg_ctx, *server_list, *source_list, tmp_dir, opensslcmd );

  /* initialise the input processing */
  LG_log(lg_ctx, LG_INFO, "get_input_init: initialise the input processing");
  EP_init(lg_ctx, rt_ctx, tmp_dir);

  UT_free(tmp_dir);
  UT_free(gpgcmd);
  UT_free(opensslcmd);

  LG_log(lg_ctx, LG_FUNC,"<get_input_init: exiting");
}


/* Tidy up after getting the input.
   Receives LG context
            pointer to server list
            pointer to source list
            pointer to unique server list
   Returns  none
*/

void get_input_end(LG_context_t *lg_ctx,
                     GList **server_list, GList **source_list,
                     GList **unique_server_list)
{
  GList *svr_item = NULL;
  GList *source_item = NULL;

  LG_log(lg_ctx, LG_FUNC,">get_input_end: entered");

  /* close key management library and tidy up */
  KM_end(KM_PGP);
  KM_end(KM_X509);
  source_item = *source_list;
  while ( source_item )
  {
    free(source_item->data);
    source_item = source_item ->next;
  }
  g_list_free(*source_list);
  *source_list = NULL;
  svr_item = *unique_server_list;
  while( svr_item )
  {
    LU_cleanup(svr_item->data);
    svr_item = svr_item->next;
  }
  g_list_free(*unique_server_list);
  g_list_free(*server_list);
  *unique_server_list = NULL;
  *server_list = NULL;

  LG_log(lg_ctx, LG_FUNC,"<get_input_end: exiting");
}


/* Processes the input (each submission).
   Receives RT context
            LG context
            options structure
            input structure
   Returns  none
*/

void process_input(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                         options_struct_t *options,
                         ep_input_structure_t *input)
{
  int retval = UP_OK;
  int num_submissions = 0;
  char *field_ptr;
  GList *submission_list = NULL;
  GList *submission_item = NULL;

  LG_log(lg_ctx, LG_FUNC,">process_input: entered");
  
  /* check the keywords */
  check_keywords(rt_ctx, lg_ctx, options, input);

  /* if it is a mail mesage, get the mail hdr data */
  if ( options->mail_input )
  {
    LG_log(lg_ctx, LG_FUNC,"process_input: get the mail hdr data");
    options->mail_hdr_data.from    =
      (field_ptr = (char *)EP_get_mail_hdr_field(input, "From")) == NULL  ? NULL : strdup(field_ptr);
    LG_log(lg_ctx, LG_FUNC,"process_input: From: [%s]", options->mail_hdr_data.from ? options->mail_hdr_data.from : "NULL");

    options->mail_hdr_data.cc      =
      (field_ptr = (char *)EP_get_mail_hdr_field(input, "Cc")) == NULL  ? NULL : strdup(field_ptr);
    LG_log(lg_ctx, LG_FUNC,"process_input: Cc: [%s]", options->mail_hdr_data.cc ? options->mail_hdr_data.cc : "NULL");

    options->mail_hdr_data.subject =
      (field_ptr = (char *)EP_get_mail_hdr_field(input, "Subject")) == NULL  ? NULL : strdup(field_ptr);
    LG_log(lg_ctx, LG_FUNC,"process_input: Subject: [%s]", options->mail_hdr_data.subject ? options->mail_hdr_data.subject : "NULL");

    options->mail_hdr_data.date    =
      (field_ptr = (char *)EP_get_mail_hdr_field(input, "Date")) == NULL  ? NULL : strdup(field_ptr);
    LG_log(lg_ctx, LG_FUNC,"process_input: Date: [%s]", options->mail_hdr_data.date ? options->mail_hdr_data.date : "NULL");

    options->mail_hdr_data.replyto =
      (field_ptr = (char *)EP_get_mail_hdr_field(input, "ReplyTo")) == NULL  ? NULL : strdup(field_ptr);
    LG_log(lg_ctx, LG_FUNC,"process_input: ReplyTo: [%s]", options->mail_hdr_data.replyto ? options->mail_hdr_data.replyto : "NULL");

    options->mail_hdr_data.msgid   =
      (field_ptr = (char *)EP_get_mail_hdr_field(input, "Message-ID")) == NULL  ? NULL : strdup(field_ptr);
    LG_log(lg_ctx, LG_FUNC,"process_input: Message-ID: [%s]", options->mail_hdr_data.msgid  ? options->mail_hdr_data.msgid : "NULL");

    /* outgoing email header */
    RT_header_to(rt_ctx,
               options->mail_hdr_data.replyto ? options->mail_hdr_data.replyto : options->mail_hdr_data.from);
    RT_header_from(rt_ctx, g_strstrip(ca_get_humailbox));
    RT_header_subject(rt_ctx, options->mail_hdr_data.subject);

    /* incomming email header */
    RT_from_mail(rt_ctx, &options->mail_hdr_data);
  }

  if (options->origin)
  {
    /* write the details of the origin causing these changes to this ctx */
    RT_notif_origin(rt_ctx, options->origin);
  }
    
  if ( ! options->help )
  {
    LG_log(lg_ctx, LG_FUNC,"process_input: NOT a help request");
    /* get a list of submissions */
    EP_blobs_foreach(input, get_input_submission_list, &submission_list);
    num_submissions = g_list_length(submission_list);
    LG_log(lg_ctx, LG_DEBUG, "process_input: number of submissions= %d", num_submissions);

    /* process each submission in turn, 
       but stop processing if a fatal error is found in a submission */
    for ( submission_item = submission_list; submission_item != NULL; 
                       submission_item = g_list_next(submission_item) )
    {
      retval = UP_process_submission(rt_ctx, lg_ctx, options, (ep_authenticated_data_t*)(submission_item->data) );
      if ( retval & UP_FATAL)
      {
        UP_internal_error(rt_ctx, lg_ctx, options, "Fatal error returned from UP_process_submission\n", 0);
      }
    }
  }
  else
  {
    LG_log(lg_ctx, LG_FUNC,"process_input: help request");
    RT_help_request(rt_ctx);
    /* no more processing required, all done in the template */
  }

  /* tidy up */
  EP_input_structure_free(input);
  EP_end();
  g_list_free(submission_list);
  submission_list= NULL;
  if ( options->mail_hdr_data.from_email )
  {
    free(options->mail_hdr_data.from_email);
  }

  LG_log(lg_ctx, LG_FUNC,"<process_input: exiting");
}


/* Gets and logs the raw input data, 
   breaks it down into usable input (list of submissions with credentials),
   Receives RT context
            LG context
            options structure
            pointer to input structure
   Returns  UP_OK or UP_FAIL
*/

int get_input(RT_context_t *rt_ctx, LG_context_t *lg_ctx, 
                       options_struct_t *options,
                       ep_input_structure_t **input)
{
  int retval = UP_OK;
  char *temp_input_file_name = NULL;  /* used to hold input from stdin */
  char input_char;
  char *log_msg = NULL;
  FILE *input_file;
  GList *source_list = NULL;
  GList *server_list = NULL;
  GList *unique_server_list = NULL;

  LG_log(lg_ctx, LG_FUNC,">get_input: entered");

  /********************************************************************************/
  /* Get and log the raw input data.
     If it is stdin, then write it to a temp file. 
     Log the input to updlog */
  if ( ! options->input_file_name )
  {
    /* no file supplied so it must be from stdin */
    temp_input_file_name = UP_get_temp_filename(lg_ctx, "input");
    /* store temp filename for use in notifications forw_create message */
    options->input_file_name = temp_input_file_name;

    if (( input_file = fopen(temp_input_file_name, "a")) == NULL)
    {
      LG_log(lg_ctx, LG_FATAL, "get_input: Can't open temp input file [%s]", temp_input_file_name);
      UP_internal_error(rt_ctx, lg_ctx, options, "Can't open temp input file\n", 0);
    }

    /* read stdin */
    LG_log(lg_ctx, LG_INFO, "get_input: reading stdin and writing to temp file");
    while ((input_char = getchar()) != EOF)
    {
      fprintf(input_file, "%c",input_char);
    }
    fclose(input_file);
  }
 
  /* open input file */
  if ((input_file = fopen(options->input_file_name, "r")) == NULL)
  {
    LG_log(lg_ctx, LG_FATAL, "get_input: Can't open input file [%s]", options->input_file_name);
    UP_internal_error(rt_ctx, lg_ctx, options, "Can't open input file\n", 0);
  }

  /* log input to updlog and state file */
  UP_add_to_upd_log(rt_ctx, lg_ctx, options, input_file);
  /* rewind the file pointer */
  retval = fseek(input_file, 0, SEEK_SET);
  if ( retval )
  {
    LG_log(lg_ctx, LG_FATAL, "get_input: Can't rewind input file [%s]", options->input_file_name);
    UP_internal_error(rt_ctx, lg_ctx, options, "Can't rewind input file\n", 0);
  }

  /********************************************************************************/
  /* get the input in a usable state */
  /* initialise modules needed to get the input */
  get_input_init(rt_ctx, lg_ctx, options,
                    &server_list, &source_list, &unique_server_list);
  
  /* expand the input */
  LG_log(lg_ctx, LG_INFO, "get_input: expand the input");
  retval = EP_unfold(input_file, options->mail_input, options->X509cert_file,
                         options->X509certificate, input);
  /* close the input stream */
  fclose(input_file);
  if ( retval == EP_FATAL )
  {
    LG_log(lg_ctx, LG_FATAL, "get_input: Fatal Error returned from EP_unfold");
    UP_internal_error(rt_ctx, lg_ctx, options, "Fatal Error returned from EP_unfold\n", 0);
  }
  else if ( retval == EP_FAIL )
  {
    retval = UP_FAIL;
  }
  else retval = UP_OK;

  /* report any errors in the expanded input */
  log_msg = RT_EP_analyse(rt_ctx, *input);
  if ( ! log_msg )
  {
    /* should never happen */
    LG_log(lg_ctx, LG_FATAL, "get_input: No log message returned from RT_EP_analyse");
    UP_internal_error(rt_ctx, lg_ctx, options, "No log message returned from RT_EP_analyse\n", 0);
  }
  LG_log(lg_ctx, LG_DEBUG, "get_input: %s", log_msg);

  /* tidy up */
  get_input_end(lg_ctx, &server_list, &source_list, &unique_server_list);
  
  if ( temp_input_file_name )
  {
    unlink(temp_input_file_name);
    free(temp_input_file_name);
    temp_input_file_name = NULL;
    options->input_file_name = NULL;
  }

  LG_log(lg_ctx, LG_FUNC,"<get_input: exiting");
  return retval;
}

int main(int argc, char **argv)
{
  int retval = UP_OK;
  char *args_str = NULL;
  char *filename;
  char *state_file_name;
  time_t cur_time;
  int statefd;
  RT_context_t *rt_ctx;               /* context pointer used in all calls to RT module */
  LG_context_t *lg_ctx = NULL;        /* context pointer used by LG module */
  LG_appender_t *app;
  ep_input_structure_t *input = NULL; /* structure containing un-folded data */

  mail_hdr_t mail_hdr = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
  options_struct_t options = { NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, { NULL }, NULL, NULL, NULL};
  options.mail_hdr_data = mail_hdr;

  /********************* INITIALISE AND CONFIGURE  ***************************/

  /* get date and time for the start of dbupdate */
  cur_time = time(NULL);
  options.time_str = strdup(ctime(&cur_time));

  /* create hashes */
  options.ntfy_hash = g_hash_table_new(g_str_hash, g_str_equal);
  options.forw_hash = g_hash_table_new(g_str_hash, g_str_equal);
  options.AUTO_key_hash = g_hash_table_new(g_str_hash, g_str_equal);       

  /* process command line arguments */
  if ( process_arguments(&options, argc, argv, &args_str) != UP_OK )
  {
    /* LG_log(lg_ctx, LG_FATAL,"main: error processing command line arguments"); */
    fprintf(stderr, "main: error processing command line arguments\n");
    fprintf(stderr, "main: arguments: %s\n", args_str);
    free(args_str);
    free(options.time_str);
    exit(11);
  }

  /* configure */
  /* if -c flag is given, use the named file as config file */ 
  if ( options.config_file_name != NULL)
  {
    /* check the file */
    if ( ! check_file(options.config_file_name) )
    {
      /* config file does not exist or can't be accessed */
      fprintf(stderr, "main: config file %s does not exist\n", options.config_file_name);
      free(args_str);
      free(options.time_str);
      exit(12);
    }
    ca_init(options.config_file_name);
	free(options.config_file_name);
    options.config_file_name = NULL;
  }
  else
  {
    fprintf(stderr, "main: no config file specified\n");
    free(args_str);
    free(options.time_str);
    exit(13);
  }

//	die;

  /* set up a reporting context */
  rt_ctx = RT_start();

  /* create state log file */
  state_file_name = UP_get_temp_filename(NULL, "state");
  statefd = open(state_file_name, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH);		// rights can be revoked by setting umask
  if (statefd < 0) {
  	fprintf(stderr, "Could not open/create state file %s: %s\n", state_file_name, strerror(errno));
  	die;
  }
  options.statefd = statefd;

  /* initialise logging to state file */
  lg_ctx = LG_ctx_new();
  app = LG_app_get_filedes_dump(statefd);
  LG_app_set_level(app, LG_ALL);
  LG_app_set_formatter(app, LG_frm_dbupdate_prepared());
  LG_ctx_add_appender(lg_ctx, app);

  /* initialise logging to dbupdate error log file */
  filename = ca_get_dberrlog;
  app = LG_app_get_daily_info_dump(filename);
  free(filename);
  LG_app_set_level(app, LG_PERROR);
  LG_app_set_formatter(app, LG_frm_general_prepared("$TIMESTAMP dbupdate-$PID/ $SEVERITY $MESSAGE\n"));
  LG_ctx_add_appender(lg_ctx, app);

  /* if debug was selected, add it to the LG context */
  if ( options.debug )
  {
    app = LG_app_get_file_info_dump(stdout);
    LG_app_set_level(app, options.debug_level);
    LG_app_set_formatter(app, LG_frm_dbupdate_prepared());
    LG_ctx_add_appender(lg_ctx, app);
  }

  LG_log(lg_ctx, LG_INFO,"main:           dbupdate started at %s", options.time_str);
  RT_init(lg_ctx);
  RT_process_time(rt_ctx, options.time_str);

  /* log the command line options */
  log_options(lg_ctx, &options, args_str);
  free(args_str);
  
  /* check any supplied files */
  if ( options.input_file_name != NULL)
  {
    if ( ! check_file(options.input_file_name) )
    {
      /* input file does not exist or can't be accessed */
      LG_log(lg_ctx, LG_FATAL, "main: input file %s does not exist\n", options.input_file_name);
      UP_internal_error(rt_ctx, lg_ctx, &options, "main: input file does not exist\n", 0);
    }
  }
  
  if ( options.X509cert_file != NULL)
  {
    if ( ! check_file(options.X509cert_file) )
    {
      /* X.509 certificate file does not exist or can't be accessed */
      LG_log(lg_ctx, LG_FATAL, "main: X.509 certificate file %s does not exist\n", options.X509cert_file);
      UP_internal_error(rt_ctx, lg_ctx, &options, "main: X.509 certificate file does not exist\n", 0);
    }
  }

  /* are we in test mode */
  if ( ! options.test_mode)
  {
    /* if it is not already set to 1 (from command line), read from config */
    options.test_mode = ca_get_testmode;
    /* log test mode, if just set */
    if (options.test_mode)
      LG_log(lg_ctx, LG_DEBUG,"main: test mode option set from config file");
  }

  /* initialise the rpsl dictionary */
  LG_log(lg_ctx, LG_INFO,"main: initialise rpsl dictionary");
  rpsl_load_dictionary(RPSL_DICT_FRONT_END);

  /* initialise auth module */
  if ( ! AU_init(lg_ctx, options.test_mode) )
  {
    /* fatal error */
    LG_log(lg_ctx, LG_FATAL,"main:: AU module init failed");
    UP_internal_error(rt_ctx, lg_ctx, &options, "main: AU module init failed\n", 0);
  }

  /* initialise other modules */
  PG_init();
  SQ_init(lg_ctx);
  SK_init(lg_ctx);
  UT_init(lg_ctx);
  CR_init(lg_ctx);

  /***************************** UPDATE  **************************************/

  /* get the input and process it */
  retval = get_input(rt_ctx, lg_ctx, &options, &input);
  if ( retval == UP_OK )
  {
    process_input(rt_ctx, lg_ctx, &options, input);
  }
  /* removed CB 20041208 */
  /* printf("options.help [%d]\n", options.help); */
  
  if ( ! options.help )
  {
    /* check if we have processed any objects */
    if ( ! options.count_successful && ! options.count_unsuccessful &&
           ! options.count_noop )
    {
      RT_no_objects_processed(rt_ctx);
    }
    
    /* if any object failed the update result is FAILED or
       if there are no objects processed the update result is FAILED
       ( so blank update msgs are failures) */
    if ( options.mail_input )
    {
      if ( options.count_unsuccessful || 
           ( ! options.count_successful && ! options.count_unsuccessful &&
             ! options.count_noop ) )
      {
        RT_update_result(rt_ctx, "FAILED");
      }
      else RT_update_result(rt_ctx, "SUCCESS");
    }

    /* close input to the reporting context */
    RT_end(rt_ctx);
    /* log and send replies */
    NT_process_acknowledgement(rt_ctx, lg_ctx, &options);
    NT_process_notifications(rt_ctx, lg_ctx, &options, UP_NOTIF);
    NT_process_notifications(rt_ctx, lg_ctx, &options, UP_FRWD);
  }
  else
  {
    /* close input to the reporting context */
    RT_end(rt_ctx);
    /* for a help request just send the help info as an ack */
    NT_process_acknowledgement(rt_ctx, lg_ctx, &options);
  }

  /************************* TIDY UP AND SHUTDOWN  ***************************/

  /* close down modules as required */
  PG_end();
  AU_end();
  KM_end(KM_PGP);
  KM_end(KM_X509);
  
  /* close down the report */
  RT_destroy(rt_ctx);
  
  /* free any remaining memory */
//  UP_free_options(&options);
  
  /* close down the logging and delete the state log file */
  LG_ctx_free(lg_ctx);
  close(statefd);
  if ( state_file_name )
  {
    if (options.save == 0)
    {
      unlink(state_file_name);
    }
    free(state_file_name);
    state_file_name = NULL;
  }
  
  /* all tasks done successfully */
  /* report successful termination to the wrapper */
  exit(0); 
}
