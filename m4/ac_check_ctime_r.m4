dnl @sypnosis AC_CHECK_CTIME_R
dnl
dnl If ctime_r has three arguments, we 're probably on Solaris
dnl so we need to add -DPOSIX_PTHREAD_SEMANTICS to have a ctime_r
dnl with two arguments
dnl
dnl @version $Id: ac_check_ctime_r.m4,v 1.2 2005/02/01 16:12:03 can Exp $
dnl @author Can Bican <can@ripe.net>
dnl
AC_DEFUN([AC_CHECK_CTIME_R],[
  AC_MSG_CHECKING([if ctime_r has two arguments])
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#ifndef _REENTRANT
# define _REENTRANT
#endif

#include <time.h>

#if defined(ACE_LACKS_SYSTIME_H)
# include <sys/time.h>
#endif
       ]], [[
        const time_t *t = 0;
        char *buf;
        ctime_r(t, buf);
       ]])],[
        AC_MSG_RESULT([yes])
        ac_lib_posix_ctime_r_2_params=yes
       ],[
        AC_MSG_RESULT([no])
        ac_lib_posix_ctime_r_2_params=no
       ])
    ], [AC_DEFINE([ACE_HAS_2_PARAM_ASCTIME_R_AND_CTIME_R])],)
])

if test x$ac_lib_posix_ctime_r_2_params = xno
then
  AC_DEFINE_UNQUOTED([POSIX_PTHREAD_SEMANTICS],[],[required for proper ctime_r])
fi
