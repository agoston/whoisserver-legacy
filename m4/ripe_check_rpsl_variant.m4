dnl
dnl Link xml files depending on the variant
dnl

dnl begin defun
AC_DEFUN(RIPE_SET_RPSL_VARIANT,[
AC_ARG_WITH(rpslvariant,
  AC_HELP_STRING([--with-rpslvariant=VARIANT],[RPSL variant to use, either RIPE, APNIC, AFRINIC or IETF]),
  [  if test "x$withval" != xyes; then
       rpslvariant=$withval
     else
       rpslvariant=RIPE
     fi
  ],
  [
     rpslvariant=RIPE
  ])
  AC_MSG_NOTICE([setting rpsl variant $rpslvariant])
  case $rpslvariant in
    RIPE )
      AC_DEFINE_UNQUOTED(RIPE,, [RPSL variant])
      ;;
    AFRINIC )
      AC_DEFINE_UNQUOTED(AFRINIC,, [RPSL variant])
      ;;
    IETF )
      AC_DEFINE_UNQUOTED(IETF,, [RPSL variant])
      ;;
    APNIC )
      AC_DEFINE_UNQUOTED(APNIC,, [RPSL variant])
      ;;
    * )
      AC_MSG_ERROR([$rpslvariant is invalid])
      ;;
  esac
  AC_CONFIG_COMMANDS(rpslvariant,
  [
      if test x$rpslvariant = x
      then
        rpslvariant=RIPE
      fi
      mkdir -p $ac_abs_builddir/src/defs
      OPWD=`pwd`
      cd $ac_abs_builddir/src/defs
      for file in $ac_abs_srcdir/src/defs/variants/$rpslvariant/*.xml \
                  $ac_abs_srcdir/src/defs/variants/$rpslvariant/*.h \
                  $ac_abs_srcdir/src/defs/*.dtd
      do
        if test -f $file
        then
          ln -sf $file 2>/dev/null
        fi
      done
      mkdir -p $ac_abs_builddir/include
      cd $ac_abs_builddir/include
      for file in $ac_abs_srcdir/src/defs/variants/$rpslvariant/*.def
      do
        if test -f $file
        then
          ln -sf $file 2>/dev/null
        fi
      done
      cd $OPWD
  ])
dnl end defun
])
