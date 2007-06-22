/*
 * $Id: ns_perl.c,v 1.4 2006/08/07 11:20:28 katie Exp $
 */

#include <EXTERN.h>             /* from the Perl distribution     */
#include <perl.h>               /* from the Perl distribution     */
#include "ns_perl.h"
#include "ns_rir.h"

/*
 * Stuff that makes dynamic loading in Perl tick (or so I'm told)
 */
static void xs_init(pTHX);
EXTERN_C void boot_DynaLoader(pTHX_ CV * cv);
EXTERN_C void boot_Socket(pTHX_ CV * cv);

static PerlInterpreter *my_perl = NULL; /* The Perl interpreter */

/*
 * This is needed to enable dynamic loading of modules in Perl
 */
EXTERN_C void xs_init(pTHX)
{
  char *file = __FILE__;

  /* DynaLoader is a special case */
  newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
}


/*
 * Invoke delchecker, and communicate the result
 * result and errors must be g_freed, if not NULL
 */
void rdns_perl_delcheck(gchar * conf, gchar * domain, gchar ** nservers, gchar ** ds_rdata,
                        gchar ** result, gchar ** errors)
{
  STRLEN n_a;                   /* perl embedding related */
  char *embedding[] = { "", "-e", "0" };        /* embedded perl must start
                                                   with shortest
                                                   possible code */
  gchar *command;               /* Perl code to execute */
  gchar *errorPerl;             /* Error string from Perl */
  gchar *resultPerl;            /* Result from delegations checking */
  gchar *ds_rdata_str = NULL;   /* string for array of ds-rdata records */

  /* check if nservers are filled */
  if ((nservers == NULL) || (nservers[0] == NULL)) {
    *result = NULL;
    *errors = strdup("No nameservers are given.");
  }

  /* Embedded Perl init */
  if (my_perl == NULL) {
    my_perl = perl_alloc();
    perl_construct(my_perl);
    perl_parse(my_perl, xs_init, 3, embedding, (char **) NULL);
  }

  /* Prepare the Perl code, placing configuration file, domain and nserver
   * strings
   */
  if (ds_rdata != NULL) {
    ds_rdata_str = g_strdup_printf ("'%s'", g_strjoinv("','", (gchar **) ds_rdata));
  } else {
    ds_rdata_str = g_strdup("");
  }
  command = g_strdup_printf("\
      use warnings; \
      $error=''; \
      $result=''; \
      eval { \
        require Net::DelCheck; \
        $checker = new Net::DelCheck('%s'); \
        if ( !defined $checker ) { \
          die ('Error making DelCheck object: '. \
            join ( ' ', &Net::DelCheck::errors )); \
        } else { \
          my @errors =  $checker->get_data( '%s', '%s' ); \
          if (@errors) { \
            die  (join (' ', @errors )); \
          }  else { \
            $result = $checker->check(%s); \
          } \
        } \
      }; \
      if ($@) { \
        $error=$@; \
        $result=\"\"; \
      } else { \
        $error=\"\"; \
      } \
        ", conf, domain, g_strjoinv("','", (gchar **) nservers), ds_rdata_str);

  { char buf[2000];
     sprintf (buf, "DEBUG: delcheck string [%s]\n", command);
     fprintf (stderr, buf);
  }


  /* Execute the Perl code, get result and error */
  eval_pv(command, TRUE);
  errorPerl = SvPV(get_sv("error", FALSE), n_a);
  resultPerl = SvPV(get_sv("result", FALSE), n_a);

  /* Fill in errors and results, or assign them NULL if they aren't
   * available
   */
  if (strcmp(errorPerl, "") != 0) {
    *errors = g_strdup(SvPV(get_sv("error", FALSE), n_a));
  } else {
    *errors = NULL;
  }
  if (strcmp(resultPerl, "") != 0) {
    *result = g_strdup(SvPV(get_sv("result", FALSE), n_a));
  } else {
    *result = NULL;
  }

  /* Cleanup checker */
  // eval_pv("$checker->cleanup;", TRUE);

  /* Embedded perl cleanup */
  /* removed - we call stuff numerous times
     perl_destruct(my_perl);
     perl_free(my_perl);
   */

  /* Function cleanup */
  g_free(command);
  if (ds_rdata_str != NULL) {
    g_free(ds_rdata_str);
  }
}

/*
 * find the configuration file to use
 */
gchar *ns_find_delcheck_conf(LG_context_t * lg_context, gchar * domain)
{
  gchar *result = NULL;         /* allocated result to return */
  gint dot_count = 0;           /* number of dots in the domain */
  gchar *p;                     /* generic pointer */
  gchar *prefix;                /* prefix, from configuration */

  prefix = ca_get_ns_delcheckconf;
  g_strchomp(prefix);

  /* count dots in the domain */
  p = domain;
  while ((p != NULL) && (*p != 0)) {
    if (*p == '.') {
      dot_count++;
    }
    p++;
  }
  g_strdown(domain);
  p = strstr(domain, "in-addr.arpa");

  /* decide on the conf file name */
  if ((p != NULL) && (g_strcasecmp(p, "in-addr.arpa") == 0)) {  /* ipv4 */
    if (dot_count == 3) {       /* slash 16 */
      if (ns_is_erx(domain)) {
        result = g_strdup_printf("%serx-ipv4-16", prefix);
      } else {
        result = g_strdup_printf("%sipv4-16", prefix);
      }
    } else if (dot_count == 4) {        /* slash 24 */
      result = g_strdup_printf("%sipv4-24", prefix);
    } else if (dot_count == 2) { /* slash 8 */
      result = g_strdup_printf("%sipv4-8", prefix);
    }
  } else {                      /* ipv6 */
    if ((dot_count == 9) ||  // /32
        (dot_count == 8) ||  // /28
        (dot_count == 7) ||  // /24
        (dot_count == 6) ||  // /20
        (dot_count == 5) ||  // /16
        (dot_count == 4) ) { // /12
      result = g_strdup_printf("%sipv6-32", prefix);
    } else if ( (dot_count == 13) ||
                (dot_count == 17) ) { // /48 or /64
      result = g_strdup_printf("%sipv6-48", prefix);
    }
  }

  if (ns_has_suffix(domain,"e164.arpa")) {
    result = g_strdup_printf("%senum", prefix);
  }
  
  if (result == NULL) {
    LG_log(lg_context, LG_DEBUG, "no delchecker configuration found.");
  } else {
    LG_log(lg_context, LG_DEBUG, "delchecker configuration found: %s",
           result);
  }

  // give nice error if this is classless delegation

  return result;
}
