dnl
dnl Check for java compiler
dnl
AC_DEFUN([AC_PROG_JAVAC],[
AC_REQUIRE([AC_EXEEXT])dnl
if test x"$OPLONLY" != xyes; then
  if test "x$JAVAPREFIX" = x; then
        test "x$JAVAC" = x && AC_CHECK_PROGS(JAVAC, javac$EXEEXT)
  else
        test "x$JAVAC" = x && AC_CHECK_PROGS(JAVAC, javac$EXEEXT, $JAVAPREFIX)
  fi
  test "x$JAVAC" = x && AC_MSG_ERROR([no acceptable Java compiler found in \$PATH])
AC_PROVIDE([$0])dnl
fi
])
