dnl
dnl enable rdns features(requires Perl5)
dnl
AC_DEFUN(RIPE_CHECK_RDNS,[
AC_PATH_PROGS(PERL5, perl perl5)
AC_ARG_WITH(rdns,
    [  --with-rdns             use this to enable rdns extensions],
    AC_MSG_CHECKING([for RDNS extensions])
    if test "x$PERL5" != "x"; then
        AC_MSG_RESULT([enabled])
        RDNSCFLAGS="`$PERL5 -MExtUtils::Embed -e ccopts` -DRDNSYES"
        RDNSLDFLAGS="`$PERL5 -MExtUtils::Embed -e ldopts`"
    else
        AC_MSG_WARN([disabled! No working Perl5!])
    fi
)
AC_SUBST(RDNSCFLAGS)
AC_SUBST(RDNSLDFLAGS)
])
