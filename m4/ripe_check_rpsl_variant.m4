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
   echo '--------------------------------'
   echo $RPSL_VARIANT > ./rpsl.variant])
AC_MSG_CHECKING([setting default rpsl variant])
AC_CONFIG_COMMANDS([variant],[
  RPSL_VARIANT=`cat ./rpsl.variant 2>/dev/null `
  export RPSL_VARIANT
  cd src/defs
  echo variants/$RPSL_VARIANT/*.xml
  ln -sf variants/$RPSL_VARIANT/*.xml .
  echo variants/$RPSL_VARIANT/*.h
  ln -sf variants/$RPSL_VARIANT/*.h .
  cd ../include
  ln -sf ../defs/variants/$RPSL_VARIANT/*.def .
  cd ../..])
variant=`cat ./rpsl.variant 2>/dev/null`
AC_MSG_RESULT([$variant])
])
