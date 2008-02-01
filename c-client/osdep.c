/*
 * Program:	Operating-system dependent routines -- Secure Linux version
 *
 * Author:	Mark Crispin
 *		Networks and Distributed Computing
 *		Computing & Communications
 *		University of Washington
 *		Administration Building, AG-44
 *		Seattle, WA  98195
 *		Internet: MRC@CAC.Washington.EDU
 *
 * Date:	1 August 1993
 * Last Edited:	24 October 2000
 * 
 * The IMAP toolkit provided in this Distribution is
 * Copyright 2000 University of Washington.
 * The full text of our legal notices is contained in the file called
 * CPYRIGHT, included with this Distribution.
 */
 
#include "tcp_unix.h"		/* must be before osdep includes tcp.h */
#include "mail.h"
#include "osdep.h"
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <errno.h>
extern int errno;		/* just in case */
#include <pwd.h>
#include <shadow.h>
#include "misc.h"


#include "fs_unix.c"
#include "ftl_unix.c"
#include "nl_unix.c"
#include "env_unix.c"
#define fork vfork
#include "tcp_unix.c"
#include "gr_waitp.c"
#include "tz_sv4.c"
/*
 * Program:	POSIX check password
 *
 * Author:	Mark Crispin
 *		Networks and Distributed Computing
 *		Computing & Communications
 *		University of Washington
 *		Administration Building, AG-44
 *		Seattle, WA  98195
 *		Internet: MRC@CAC.Washington.EDU
 *
 * Date:	1 August 1988
 * Last Edited:	4 April 2001
 * 
 * The IMAP toolkit provided in this Distribution is
 * Copyright 2001 University of Washington.
 * The full text of our legal notices is contained in the file called
 * CPYRIGHT, included with this Distribution.
 */

/* Check password
 * Accepts: login passwd struct
 *	    password string
 *	    argument count
 *	    argument vector
 * Returns: passwd struct if password validated, NIL otherwise
 */

struct passwd *checkpw (struct passwd *pw,char *pass,int argc,char *argv[])
{
  char tmp[MAILTMPLEN];
  struct spwd *sp = NIL;
  time_t left;
  time_t now = time (0);
  struct tm *t = gmtime (&now);
  int zone = t->tm_hour * 60 + t->tm_min;
  int julian = t->tm_yday;
  t = localtime (&now);		/* get local time now */
				/* minus UTC minutes since midnight */
  zone = t->tm_hour * 60 + t->tm_min - zone;
  /* julian can be one of:
   *  36x  local time is December 31, UTC is January 1, offset -24 hours
   *    1  local time is 1 day ahead of UTC, offset +24 hours
   *    0  local time is same day as UTC, no offset
   *   -1  local time is 1 day behind UTC, offset -24 hours
   * -36x  local time is January 1, UTC is December 31, offset +24 hours
   */
  if (julian = t->tm_yday -julian)
    zone += ((julian < 0) == (abs (julian) == 1)) ? -24*60 : 24*60;
				/* days since 1/1/1970 local time */
  now = ((now /60) + zone) / (60*24);
				/* non-shadow authentication */
  if (!pw->pw_passwd || !pw->pw_passwd[0] || !pw->pw_passwd[1] ||
      strcmp (pw->pw_passwd,(char *) crypt (pass,pw->pw_passwd))) {
    /* As far as I've been able to determine, here is how the expiration
     * fields in the shadow authentication data work:
     *  lstchg	last password change date if non-negative.  If zero, the
     *		user can not log in without changing password.
     *  max	number of days a password is valid if positive
     *  warn	number of days of password expiration warning
     *  expire	date account expires if positive
     *  inact	number of days an accout can be inactive (not checked!)
     * The expiration day is the *last* day that the password or account
     * is valid.
     */
				/* shadow authentication */
    if ((sp = getspnam (pw->pw_name)) && sp->sp_lstchg &&
	((sp->sp_lstchg < 0) || (sp->sp_max <= 0) ||
	 ((sp->sp_lstchg + sp->sp_max) >= now)) &&
	((sp->sp_expire <= 0) || (sp->sp_expire >= now)) &&
	sp->sp_pwdp && sp->sp_pwdp[0] && sp->sp_pwdp[1] &&
	!strcmp (sp->sp_pwdp,(char *) crypt (pass,sp->sp_pwdp))) {
      if ((sp->sp_lstchg > 0) && (sp->sp_max > 0) &&
	  ((left = (sp->sp_lstchg + sp->sp_max) - now) <= sp->sp_warn)) {
	if (left) {
	  sprintf (tmp,"[ALERT] Password expires in %ld day(s)",(long) left);
	  mm_notify (NIL,tmp,NIL);
	}
	else mm_notify (NIL,"[ALERT] Password expires today!",WARN);
      }
      if ((sp->sp_expire > 0) && ((left = sp->sp_expire - now) < 28)) {
	if (left) {
	  sprintf (tmp,"[ALERT] Account expires in %ld day(s)",(long) left);
	  mm_notify (NIL,tmp,NIL);
	}
	else mm_notify (NIL,"[ALERT] Account expires today!",WARN);
      }
      endspent ();		/* don't need shadow password data any more */
    }
    else pw = NIL;		/* password failed */
  }
  return pw;
}
/*
 * Program:	Standard login
 *
 * Author:	Mark Crispin
 *		Networks and Distributed Computing
 *		Computing & Communications
 *		University of Washington
 *		Administration Building, AG-44
 *		Seattle, WA  98195
 *		Internet: MRC@CAC.Washington.EDU
 *
 * Date:	1 August 1988
 * Last Edited:	24 October 2000
 * 
 * The IMAP toolkit provided in this Distribution is
 * Copyright 2000 University of Washington.
 * The full text of our legal notices is contained in the file called
 * CPYRIGHT, included with this Distribution.
 */

/* Log in
 * Accepts: login passwd struct
 *	    argument count
 *	    argument vector
 * Returns: T if success, NIL otherwise
 */

long loginpw (struct passwd *pw,int argc,char *argv[])
{
  uid_t uid = pw->pw_uid;
  char *name = cpystr (pw->pw_name);
  long ret = !(setgid (pw->pw_gid) || initgroups (name,pw->pw_gid) ||
	       setuid (uid));
  fs_give ((void **) &name);
  return ret;
}
/*
 * Program:	Dummy (no SSL) authentication/encryption module
 *
 * Author:	Mark Crispin
 *		Networks and Distributed Computing
 *		Computing & Communications
 *		University of Washington
 *		Administration Building, AG-44
 *		Seattle, WA  98195
 *		Internet: MRC@CAC.Washington.EDU
 *
 * Date:	7 February 2001
 * Last Edited:	6 April 2001
 *
 * The IMAP toolkit provided in this Distribution is
 * Copyright 2001 University of Washington.
 * The full text of our legal notices is contained in the file called
 * CPYRIGHT, included with this Distribution.
 */

/* Init server for SSL
 * Accepts: server name
 */

void ssl_server_init (char *server)
{
  syslog (LOG_ERR,"This server does not support SSL");
  exit (1);			/* punt this program too */
}


/* Start TLS
 * Accepts: /etc/services service name
 * Returns: cpystr'd error string if TLS failed, else NIL for success
 */

char *ssl_start_tls (char *server)
{
  return cpystr ("This server does not support TLS");
}

/* Get character
 * Returns: character or EOF
 */

int PBIN (void)
{
  return getchar ();
}


/* Get string
 * Accepts: destination string pointer
 *	    number of bytes available
 * Returns: destination string pointer or NIL if EOF
 */

char *PSIN (char *s,int n)
{
  return fgets (s,n,stdin);
}


/* Get record
 * Accepts: destination string pointer
 *	    number of bytes to read
 * Returns: T if success, NIL otherwise
 */

long PSINR (char *s,unsigned long n)
{
  unsigned long i;
  while (n && ((i = fread (s,1,n,stdin)) || (errno == EINTR))) s += i,n -= i;
  return n ? NIL : LONGT;
}


/* Wait for input
 * Accepts: timeout in seconds
 * Returns: T if have input on stdin, else NIL
 */

long INWAIT (long seconds)
{
  return server_input_wait (seconds);
}

/* Put character
 * Accepts: character
 * Returns: character written or EOF
 */

int PBOUT (int c)
{
  return putchar (c);
}


/* Put string
 * Accepts: source string pointer
 * Returns: 0 or EOF if error
 */

int PSOUT (char *s)
{
  return fputs (s,stdout);
}


/* Put record
 * Accepts: source sized text
 * Returns: 0 or EOF if error
 */

int PSOUTR (SIZEDTEXT *s)
{
  unsigned char *t;
  unsigned long i,j;
  for (t = s->data,i = s->size;
       (i && ((j = fwrite (t,1,i,stdout)) || (errno == EINTR)));
       t += j,i -= j);
  return i ? EOF : NIL;
}


/* Flush output
 * Returns: 0 or EOF if error
 */

int PFLUSH (void)
{
  return fflush (stdout);
}
