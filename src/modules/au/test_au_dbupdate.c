#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include "lg.h"
#include "sk.h"
#include "memwrap.h"
#include "au.h"
#include "lu_whois.h"
#include "cr.h"
#include "rt.h"



static GList *
read_credentials (const char *fname)
{
  FILE *fp;
  char type[64], val[128];

  CR_credential_t *cred;
  GList *ret;

  if (strcmp(fname, "-") == 0) 
  {
    fp = stdin;
  }
  else
  {
    fp = fopen(fname, "r");
    if (fp == NULL)
    {
      fprintf(stderr, "read_credentials() unable to open \"%s\" for reading\n",
              fname);
      exit(-1);
    }
  }

  ret = NULL;
  while (fscanf(fp, "%s %s", type, val) == 2)
  {
    if (strcasecmp(type, "MAIL-FROM") == 0)
    {
      cred = CR_credential_new(CR_FROM, val, TRUE);
    }
    else if (strcasecmp(type, "PASSWORD") == 0)
    {
      cred = CR_credential_new(CR_PASSWORD, val, TRUE);
    }
    else if (strcasecmp(type, "PGP") == 0)
    {
      cred = CR_credential_new(CR_PGP, val+7, TRUE);
    }
    else if (strcasecmp(type, "OVERRIDE") == 0)
    {
      cred = CR_credential_new(CR_OVERRIDE, val, TRUE);
    }
    else
    {
      fprintf(stderr, 
              "read_credentials() found unknown credential type \"%s\"\n",
              type);
      exit(-1);
    }
    ret = g_list_append(ret, cred);
  }
  fclose(fp);

  return ret;
}


rpsl_object_t *
read_object (const char *fname)
{
  FILE *fp;
  int c;
  GString *s;
  rpsl_object_t *ret_val;

  if (strcmp(fname, "-") == 0) 
  {
    fp = stdin;
  }
  else
  {
    fp = fopen(fname, "r");
    if (fp == NULL)
    {
      fprintf(stderr, "read_object() unable to open \"%s\" for reading\n",
              fname);
      exit(-1);
    }
  }

  s = g_string_new("");
  for (;;) 
  {
    c = getc(fp);
    if (c == EOF) break;
    g_string_append_c(s, c);
  }

  ret_val = rpsl_object_init(s->str);
  g_string_free(s, TRUE);
  return ret_val;
}


int 
main (int argc, char *argv[])
{
  int opt_ch;
  extern char *optarg;

  char *credentials_file = "credentials";
  char *input_file = "-";
  char *whois_server = "whois.ripe.net";
  int whois_port = 43;

  LG_appender_t *app;
  LG_context_t *ctx;
  LG_context_t *null_ctx;

  LU_server_t *whois;

  GList *cred_list;
  RT_context_t *rt;

  rpsl_object_t *obj;
  GList *mntner_used;

  AU_ret_t result;

  do
  {
    opt_ch = getopt(argc, argv, "c:i:h:p:");
    switch (opt_ch)
    {
      case -1:
        /* end of options */
        break;
      case 'c':
        credentials_file = optarg;
        break;
      case 'i':
        input_file = optarg;
        break;
      case 'h':
        whois_server = optarg;
        break;
      case 'p':
        whois_port = atoi(optarg);
        break;
      case '?':
        fprintf(stderr, "Unrecognised option, %c\n", optopt);
        exit(-1);
      case ':':
        fprintf(stderr, "Missing argument for option %c\n", optopt);
        exit(-1);
      default:
        fprintf(stderr, "Unrecognised return from getopt(), %d\n", opt_ch);
        exit(-1);
    }
  }
  while (opt_ch != -1);

  /* read credentials */
  cred_list = read_credentials(credentials_file);

  /* set up for logging */
  app = LG_app_get_file_info_dump(stderr);
  LG_app_set_level(app, LG_ALL);
  LG_app_set_formatter(app, LG_frm_dbupdate_prepared());
  ctx = LG_ctx_new();
  LG_ctx_add_appender(ctx, app);
  LU_init(ctx);

  /* we don't care about logging for these */
  null_ctx = LG_ctx_new();
  SK_init(null_ctx);
  UT_init(null_ctx);

  /* easy initialisation */
  PG_init();

  /* lookup server */
  whois = LU_whois_init(whois_server, whois_port, 30);

  /* initialise AU */
  AU_init(ctx, whois);

  /* read object */
  obj = read_object(input_file);

  /* check a person! */
  rt = RT_start();
  result = AU_check_auth(obj, cred_list, AU_CREATE, rt, &mntner_used);
  RT_end(rt);
  RT_destroy(rt);

  if (result == AU_AUTHORISED)
  {
    return 0;
  } 
  else
  {
    return 1;
  }
}


