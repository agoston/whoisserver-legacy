dnl
dnl enable rdns features(requires Perl5)
dnl
AC_DEFUN(RIPE_CHECK_RDNS,[
dnl DO NOT reprobe perl5 here! it's set from the configure.ac! --jib
dnl AC_PATH_PROGS(PERL5, perl perl5)
AC_ARG_WITH(rdns,
    [  --with-rdns             use this to enable rdns extensions],
    [
    AC_MSG_CHECKING([for RDNS extensions])
    if test "x$PERL5" = "x"; then
      AC_MSG_RESULT([disabled! No working Perl5!])
      buildrdns=false;
    else
      if test "x$withval" != "xno"; then
        AC_MSG_RESULT([enabled])
        RDNSCFLAGS="`${PERL5} -MExtUtils::Embed -e ccopts` -DRDNSYES"
        RDNSLDFLAGS="`${PERL5} -MExtUtils::Embed -e ldopts`"
        buildrdns=true;
      else
        AC_MSG_RESULT([disabled by configure])
        buildrdns=false;
      fi
    fi
    ],[buildrdns=false])
AC_SUBST(RDNSCFLAGS)
AC_SUBST(RDNSLDFLAGS)
AM_CONDITIONAL(BUILDRDNS, test x$buildrdns = xtrue)
])
