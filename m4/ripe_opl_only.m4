dnl
dnl Set a flag for opl-only build
dnl
AC_DEFUN(RIPE_OPL_ONLY,[
AC_ARG_WITH(oplonly,
  AC_HELP_STRING([--with-oplonly],[skip checks for non-OPL related tests]),
  [if test "x$withval" != xyes
   then
     OPLONLY=$withval
   else
     OPLONLY=yes
   fi])])
