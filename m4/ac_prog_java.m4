dnl
dnl Check for java compiler
dnl
AC_DEFUN([AC_PROG_JAVA],[
AC_REQUIRE([AC_EXEEXT])dnl
if test "x$JAVAPREFIX" = x; then
        test "x$JAVA" = x && AC_CHECK_PROGS(JAVA, java$EXEEXT)
else
        test "x$JAVA" = x && AC_CHECK_PROGS(JAVA, java$EXEEXT, $JAVAPREFIX)
fi
test "x$JAVA" = x && AC_MSG_ERROR([no acceptable Java interpreter found in \$PATH])
AC_PROVIDE([$0])dnl
])
