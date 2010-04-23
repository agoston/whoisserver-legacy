dnl
dnl Link xml files depending on the variant
dnl

dnl begin defun
AC_DEFUN(RIPE_SET_RPSL_VARIANT,[
AC_ARG_WITH(rpslvariant,
  AC_HELP_STRING([--with-rpslvariant=VARIANT],[RPSL variant to use: RIPE only]),
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
  AC_DEFINE_UNQUOTED(RPSL_VARIANT, "$rpslvariant", [RPSL variant])
dnl end defun
])
