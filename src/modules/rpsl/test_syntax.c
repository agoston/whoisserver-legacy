/* 
  usage: test_syntax [-c|-f] [-n iterations] [-r] [-v] attribute [class]
  options: -c for core
           -f for front-end (default)
           -n iterations (default is 1)
           -r use totally random attributes, rather than guided
           -v be verbose
  arguments: attribute to check
             class if required (ambiguous)


  errors found:
    e000: API returns "attribute missing colon" if attribute is empty
    e001: assertion fails if ':' in attribute (Andrei already found)
    e002: core dump on rpsl_error_add() due to '%' in input string
    e003: core dump in rpsl_attr_init() when called with empty string
    e004: NULL from rpsl_attr_init() on "override" attribute due to 
          duplicate definition

  possible errors found:
    p000: bogus looking phone numbers allowed
    p001: missing matching inetnums, e.g. 8cd9::/19, bf92::/104
    p002: matching bogus inetnums, e.g.  :3c4f:360b:82e2:c99b:be58::/66,
              fb5:cd15:8ce4:11d6:c861:6d80:9feb::/4
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <glib.h>
#include <ctype.h>

#include "syntax_api.h"

/* need: list of variables that may be empty */

/* boundary lengths, based on values used by core parser */
int lengths[] = {
    0, 1, 29, 30, 31, 63, 64, 65, 79, 80, 81, 243, 244, 245 
};
#define NUM_LENGTHS (sizeof(lengths)/sizeof(lengths[0]))

/* global variable controlling verbosity */
int Verbose = 0;

/* global count of errors */
int Errors = 0;

/* function to use when generating a random attribute*/
GString* (*Random_Attr)();

/* use Xrand48() if available */
#define srand(seed) srand48(seed)
#define rand() (int)lrand48()

/* random characters */
int 
rnd_alpha()
{
    return (rand() % 26) + 'A';
}

int 
rnd_alnum()
{
    int n;

    n = rand() % 36;
    if (n < 10) {
        return n + '0';
    } else {
        return (n - 10) + 'A';
    }
}

int
rnd_rpslchar()
{
    int n;

    n = rand() % 38;
    if (n < 10) {
        return n + '0';
    } else if (n < 36) {
        return (n - 10) + 'A';
    } else if (n == 36) {
        return '-';
    } else {
        return '_';
    }
}

int
rnd_print()
{
    int ch;

    do {
        ch = rand() % 256;
    } while (!isprint(ch));
    return ch;
}

int
rnd_phone()
{
    int n;

    n = rand() % 14;
    switch (n) {
        case 0:   return '(';
        case 1:   return ')';
        case 2:   return '.';
        case 3:   return '-';
        default:  return (n - 4) + '0';
    }
}

/* generate a random RPSL name */
GString *
random_rpsl_name ()
{
    GString *ret;
    int i;
    int len;

    ret = g_string_new("");
    g_string_append_c(ret, rnd_alpha());
    len = rand() % 35;
    for (i=0; i<len; i++) {
        g_string_append_c(ret, rnd_rpslchar());
    }
    g_string_append_c(ret, rnd_alnum());
    return ret;
}

/* generate a random (possibly bogus) AS number */
GString *
random_aut_num ()
{
    GString *ret;

    ret = g_string_new("AS");
    g_string_sprintfa(ret, "%d", rand() % 200000);
    return ret;
}

/* generate a random (possibly bogus) e-mail address */
GString *
random_e_mail ()
{
    GString *ret;
    int len;
    int i;
    GString *name;

    ret = g_string_new("");
    len = rand() % 40;
    for (i=0; i<len; i++) {
        g_string_append_c(ret, rnd_print());
    }
    g_string_append_c(ret, '@');
    len = rand() % 3;
    for (i=0; i<len; i++) {
        name = random_rpsl_name();
        g_string_append(ret, name->str);
        g_string_free(name, TRUE);
        g_string_append_c(ret, '.');
    }
    name = random_rpsl_name();
    g_string_append(ret, name->str);
    g_string_free(name, TRUE);
    return ret;
}

/* generate random (possibly bogus) IP address */
GString *
random_ip_addr ()
{
    GString *ret;

    ret = g_string_new("");
    g_string_sprintf(ret, "%d.%d.%d.%d", 
                     rand() % 400, rand() % 400, rand() % 400, rand() % 400);
    return ret;
}

/* generate random (possibly bogus) IP range */
GString *
random_ip_range ()
{
    GString *ret;

    ret = g_string_new("");
    g_string_sprintf(ret, "%d.%d.%d.%d - %d.%d.%d.%d",
                     rand() % 400, rand() % 400, rand() % 400, rand() % 400,
                     rand() % 400, rand() % 400, rand() % 400, rand() % 400);
    return ret;
}

/* possibly bogus phone number */
GString *
random_phone_number ()
{
    GString *ret;
    int len;
    int i;

    ret = g_string_new("+");
    len = rand() % 40;
    for (i=0; i<len; i++) {
        g_string_append_c(ret, rnd_phone());
    }
    if ((rand() % 10) == 0) {
        g_string_append(ret, " ext. ");
        len = rand() % 10;
        for (i=0; i<len; i++) {
            g_string_append_c(ret, rnd_phone());
        }
    }
    return ret;
}

GString *
random_free_form ()
{
    GString *ret;
    int len;
    int i;

    ret = g_string_new("");
    len = rand() % 100;
    for (i=0; i<len; i++) {
        g_string_append_c(ret, rnd_print());
    }
    return ret;
}

/* possibly bogus inet6num */
GString *
random_inet6num ()
{
    GString *ret;
    int len;
    int i;

    ret = g_string_new("");
    len = rand() % 9;
    if ((rand() % 20) == 0) {
        g_string_append_c(ret, ':');
    }
    for (i=0; i<len; i++) {
        g_string_sprintfa(ret, "%x", rand() % 70000);
        g_string_append_c(ret, ':');
    }
    if ((rand() % 4) == 0) {
        g_string_append_c(ret, ':');
    }
    g_string_sprintfa(ret, "/%d", rand() % 200);
    return ret;
}

/* table of random generators */
typedef GString* (*rndfunc_t)();
rndfunc_t rnd_attrs [] = {
    random_rpsl_name,
    random_aut_num,
    random_e_mail,
    random_ip_addr,
    random_ip_range,
    random_phone_number,
    random_free_form,
    random_inet6num
};
#define NUM_RND_ATTRS (sizeof(rnd_attrs)/sizeof(rnd_attrs[0]))

/* generate a random attribute */
GString *
any_random_attr ()
{
    return rnd_attrs[rand() % NUM_RND_ATTRS]();
}

/* display errors */
void 
show_errors (const GList *errs)
{
    const rpsl_error_t *err;
    
    while (errs != NULL) {
        err = errs->data;
        printf("error: level=%d code=%d descr=[%s]\n",  
               err->level, err->code, err->descr);

        Errors++;
        errs = g_list_next(errs);
    }
}

/* perform the check of a given syntax */
void 
syntax_check (const char *attribute, const char *class, int iterations)
{
    int i, j;
    rpsl_attr_t *attr;
    GString *s;
    GString *rnd;
    long seed;

    seed = (long)time(NULL);

    if (Verbose) {
        if (class == NULL) {
            printf("# syntax_check(\"%s\", NULL, %d);\n", 
                   attribute, iterations);
        } else {
            printf("# syntax_check(\"%s\", \"%s\", %d);\n", 
                   attribute, class, iterations);
        }
        printf("#   seed=%ld\n", seed);
    }

    /* randomize */
    srand(seed);

    /* create a string */
    s = g_string_new("");

    /* check empty string */
    s = g_string_truncate(s, 0);
    attr = rpsl_attr_init(s->str, class);
    if (attr != NULL) {
        printf("error: rpsl_attr_init() works on empty string\n");
        show_errors(rpsl_attr_errors(attr));
        rpsl_attr_delete(attr);
    }

    /* check empty attribute */
    g_string_sprintf(s, "%s:", attribute);
    attr = rpsl_attr_init(s->str, class);
    /*XXX:show_errors(rpsl_attr_errors(attr));*/
    rpsl_attr_delete(attr);

    /* check various length that may break core */
    for (i=0; i<NUM_LENGTHS; i++) {
        g_string_sprintf(s, "%s: ", attribute);
        for (j=0; j<lengths[i]; j++) {
            g_string_append_c(s, 'X');
        }
        attr = rpsl_attr_init(s->str, class);
        /*show_errors(rpsl_attr_errors(attr));*/
        rpsl_attr_delete(attr);
    }

    /* random iterations */
    for (i=0; i<iterations; i++) {
        /* check semi-valid random attribute */
        rnd = Random_Attr();
        g_string_sprintf(s, "%s: %s", attribute, rnd->str);
        g_string_free(rnd, TRUE);
        attr = rpsl_attr_init(s->str, class);
        if (!rpsl_attr_has_error(attr, RPSL_ERRLVL_DEBUG)) {
            printf("match [%s]\n", s->str);
        }
        rpsl_attr_delete(attr);

        /* check totally random attribute */
        rnd = random_free_form();
        attr = rpsl_attr_init(rnd->str, class);
        if (attr != NULL) {
            rpsl_attr_delete(attr);
        }
        g_string_free(rnd, TRUE);
    }

    /* free our string memory, because we're polite */
    g_string_free(s, TRUE);
}

/* program entry point */
int 
main (int argc, char* const argv[])
{
    int opt;
    int dict_type;
    const char *attribute;
    const char *class;
    int iterations;

    dict_type = RPSL_DICT_FRONT_END;
    iterations = 1;
    Random_Attr = any_random_attr;
    for (;;) {
        opt = getopt(argc, argv, "cfrvn:");
        if (opt == EOF) {
            /* exit loop */
            break;
        }
        switch (opt) {
            case 'c':
                dict_type = RPSL_DICT_CORE;
                break;
            case 'f':
                dict_type = RPSL_DICT_FRONT_END;
                break;
            case 'r':
                Random_Attr = random_free_form;
                break;
            case 'v':
                Verbose++;
                break;
            case 'n':
                iterations = atoi(optarg);
                break;
            default:
                exit(1);
        }
    }

    attribute = NULL;
    class = NULL;
    if (optind >= argc) {
        fprintf(stderr, "Too few arguments; specify attribute.\n");
        exit(1);
    }
    attribute = argv[optind++];
    if (optind < argc) {
        class = argv[optind++];
        if (optind < argc) {
            fprintf(stderr,  "Too many arguments; "
                             "specify only attribute and class.\n");
            exit(1);
        }
    }

    rpsl_load_dictionary(dict_type);
    syntax_check(attribute, class, iterations);

    /* return code if error, else 0 */
    if (Errors) {
        return 1;
    } else {
        return 0;
    }
}


