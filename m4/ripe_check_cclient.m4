dnl
dnl c-client include directory (with c-client.a and various header files)
dnl

AC_DEFUN(RIPE_CHECK_CCLIENT,[
AC_ARG_WITH(cclientinc,
    [  --with-cclientinc=DIR      DIR=path of c-client include dir[$ac_default_prefix/build/imap/c-client]],
        if test [ x$withval != x ] -a [ -d $withval ]; then
          cclientinc=$withval
        else
          echo "Please specify a suitable c-client include directory: --with-cclientinc=DIR"; exit 1
        fi ,
        cclientinc=$ac_default_prefix/build/imap/c-client)

AC_ARG_WITH(cclientlib,
    [  --with-cclientlib=DIR      DIR=path of c-client library dir[$ac_default_prefix/build/imap/c-client]],
        if test [ x$withval != x ] -a [ -d $withval ]; then
          cclientlib=$withval
        else
          echo "Please specify a suitable c-client library directory: --with-cclientlib=DIR"; exit 1
        fi ,
        cclientlib=$ac_default_prefix/build/imap/c-client)

AC_CHECK_FILE($cclientinc/mail.h, ,
              echo "c-client include directory does not contain c-client.h!";
              echo "Please specify a suitable c-client include directory: --with-cclientinc=DIR"; exit 1)

AC_CHECK_FILE($cclientlib/c-client.a, ,
              echo "c-client lib directory does not contain c-client.a!";
              echo "Please specify a suitable c-client library directory: --with-cclientlib=DIR"; exit 1)

AC_SUBST(cclientinc)
AC_SUBST(cclientlib)
])
