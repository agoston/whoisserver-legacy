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
   fi])
AC_MSG_CHECKING([setting default rpsl variant])
AC_CONFIG_COMMANDS([variant],[
  export RPSL_VARIANT
  if test x$RPSL_VARIANT = x
  then
    RPSL_VARIANT=RIPE
  fi
  mkdir -p $ac_builddir/src/defs
  for file in $ac_srcdir/src/defs/variants/$RPSL_VARIANT/*.xml \
              $ac_srcdir/src/defs/variants/$RPSL_VARIANT/*.h \
              $ac_srcdir/src/defs/*.dtd
  do
    if test -f $file
    then
      ln -sf $file $ac_builddir/src/defs 2>/dev/null
    fi
  done
  mkdir -p $ac_builddir/include
  for file in $ac_srcdir/src/defs/variants/$RPSL_VARIANT/*.def
  do
    if test -f $file
    then
      ln -sf $file $ac_builddir/include 2>/dev/null
    fi
  done
  ])
AC_MSG_RESULT([$variant])
if test x$RPSL_VARIANT = x
then
  RPSL_VARIANT=RIPE
fi
AC_DEFINE_UNQUOTED(RPSLVARIANT, "$RPSL_VARIANT", [RPSL variant])
])
