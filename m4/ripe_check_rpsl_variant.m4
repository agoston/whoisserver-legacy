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
  RPSL_VARIANT=`cat $ac_abs_top_srcdir/rpsl.variant 2>/dev/null `
  export RPSL_VARIANT
  mkdir -p $ac_abs_top_builddir/src/defs
  for file in $ac_abs_top_srcdir/src/defs/variants/$RPSL_VARIANT/*.xml \
              $ac_abs_top_srcdir/src/defs/variants/$RPSL_VARIANT/*.h \
              $ac_abs_top_srcdir/src/defs/*.dtd
  do
    if test -f $file
    then
      ln -sf $file $ac_abs_top_builddir/src/defs 2>/dev/null
    fi
  done
  mkdir -p $ac_abs_top_builddir/include
  for file in $ac_abs_top_srcdir/src/defs/variants/$RPSL_VARIANT/*.def
  do
    if test -f $file
    then
      ln -sf $file $ac_abs_top_builddir/include 2>/dev/null
    fi
  done
  ])
variant=`cat ./rpsl.variant 2>/dev/null`
AC_MSG_RESULT([$variant])
])
