#ifndef GETOPT_H_
#define GETOPT_H_

typedef struct {
  int optind;            /* index into parent argv vector */
  int optopt;            /* character checked for validity */
  int optreset;          /* reset getopt */
  char *optarg;          /* argument associated with option */
  char *place;           /* option letter processing */
} getopt_state_t;

#define BADCH   (int)'?'
#define BADARG  (int)':'
#define EMSG    ""


getopt_state_t *mg_new(int optind);
int mg_getopt( int            nargc,
	   char * const   *nargv,
	   const char     *ostr,
	   getopt_state_t *state);

#endif /* GETOPT_H_ */
