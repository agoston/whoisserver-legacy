dnl
dnl Link xml files depending on the variant
dnl
AC_DEFUN(RIPE_SET_RPSL_VARIANT,[
AC_ARG_WITH(rpslvariant,
  AC_HELP_STRING([--with-rpslvariant=VARIANT],[RPSL variant to use, either RIPE, APNIC or IETF]),
  [if test "x$withval" != xyes; then
     RPSL_VARIANT=$withval
   else
     RPSL_VARIANT=RIPE
   fi
   echo $RPSL_VARIANT > ./rpsl.variant])
AC_MSG_CHECKING([setting default rpsl variant])
AC_CONFIG_COMMANDS([variant],[
  RPSL_VARIANT=`cat ./rpsl.variant 2>/dev/null `
  export RPSL_VARIANT
  cd src/defs
  filestolink=variants/$RPSL_VARIANT/*.xml
  if test x$filestolink != x
  then
    ln -sf variants/$RPSL_VARIANT/*.xml .
  fi
  filestolink=variants/$RPSL_VARIANT/*.h
  if test x$filestolink != x
  then
    ln -sf variants/$RPSL_VARIANT/*.h .
  fi
  cd ../include
  filestolink=../defs/variants/$RPSL_VARIANT/*.def
  if test x$filestolink != x
  then
    ln -sf ../defs/variants/$RPSL_VARIANT/*.def .
  fi
  cd ../..])
variant=`cat ./rpsl.variant 2>/dev/null`
AC_MSG_RESULT([$variant])
])
