dnl @sypnosis AC_CHECK_MYSQLR
dnl
dnl First check if mysqlconfig exists. It fails if mysqlconfig is not
dnl in path. Then it checks for the libraries and replaces
dnl -lmysqlclient statement with -lmysqlclient_r statement,
dnl to enable threaded client library.
dnl 
dnl The following are exported shell variables:
dnl 
dnl   MYSQL_LIBS
dnl   MYSQL_CFLAGS
dnl 
dnl @version $Id: ac_check_mysqlr.m4,v 1.2 2005/01/05 16:56:55 can Exp $
dnl @author Can Bican <bican@yahoo.com>
dnl
AC_DEFUN([AC_CHECK_MYSQLR],[
AC_PATH_PROG(mysqlconfig,mysql_config)
if test [ -z "${mysqlconfig}" ]
then
    AC_MSG_ERROR([mysql_config executable not found])
else
    AC_MSG_CHECKING([mysql libraries])
    MYSQL_LIBS=`${mysqlconfig} --libs | sed -e \
    's/-lmysqlclient /-lmysqlclient_r /' -e 's/-lmysqlclient$/-lmysqlclient_r/'`
    AC_MSG_RESULT([$MYSQL_LIBS])
    AC_MSG_CHECKING([mysql includes])
    MYSQL_CFLAGS=`${mysqlconfig} --cflags`
    AC_MSG_RESULT([$MYSQL_CFLAGS])
fi
])
