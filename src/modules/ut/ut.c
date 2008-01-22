#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "rip.h"

LG_context_t *ut_context;

void UT_init(LG_context_t *ctx) {
	ut_context = ctx;
}

/* Sends a mail using executing /usr/bin/mail -s "$SUBJECT" "$TO", feeding body into the stdin.
 * No return value as:
 * - we don't want the possibility of handling error in an error branch of a C code
 * - mail system should handle errors
 * 
 * It can be argued how the errors inside this function should be handled. We can die or just silently
 * fail. The best solution would be checking the log files periodically for errors, and alarm from there.
 * This function will come handy anyway.
 * 
 * agoston, 2007-11-14 
 */
void UT_sendmail(const char *to, const char *subject, const char *body) {
	int pfd[2];	/* 0 for read, 1 for write */
	pid_t cpid;

	if (pipe(pfd) < 0) {
		LG_log(ut_context, LG_ERROR, "UT_sendmail::pipe: %s", strerror(errno));
		return;
	}

	cpid = fork();
	if (cpid == 0) { /* child */
		close(pfd[1]);
		close(0);	/* stdin */
		if (dup(pfd[0]) < 0) {
			LG_log(ut_context, LG_ERROR, "UT_sendmail::dup: %s", strerror(errno));
			exit(0);
		}
		execl("/usr/bin/mail", "mail", "-s", subject, to, NULL);
		LG_log(ut_context, LG_ERROR, "UT_sendmail::execl: %s", strerror(errno));
		exit(0);

	} else if (cpid > 0) { /* parent */
		close(pfd[0]);
		write(pfd[1], body, strlen(body));
		close(pfd[1]);
		waitpid(cpid, NULL, 0);
		
	} else { /* error */
		LG_log(ut_context, LG_ERROR, "UT_sendmail::fork: %s", strerror(errno));
		close(pfd[0]);
		close(pfd[1]);
		return;
	}
}

/* Current implementation sends the title & description in an email to the address(es) defined under OPEREMAIL 
 * in rip.config.
 * 
 * agoston, 2007-11-15 */
void UT_alarm_operator(const char *title, const char *description) {
	char *operemail = ca_get_operemail;
	if (operemail) {
		*(strchr(operemail, '\n')) = 0;
		UT_sendmail(operemail, title, description);
		free(operemail);
	}
}
