dnl
dnl libxslt settings
dnl agoston, 2008-07-04
dnl

AC_DEFUN(RIPE_CHECK_LIBXSLT,[
AC_ARG_WITH(libxslt,
    [  --with-libxsltlib       location of libxslt libraries (version 1.1.12)],
        if test [ x$withval != x ] -a [ -d $withval ]; then
          libxslt=$withval
        else
          if test x$libxslt = x
          then
            echo "Please specify the location of libxslt libraries: --with-libxsltlib"; exit 1
          fi
        fi ,
        if test x$libxslt = x
        then
          libxslt=$RIP/third-party/libxslt_1.1.12-8/lib
        fi)

AC_ARG_WITH(libxsltinc,
    [  --with-libxsltinc       location of libxslt include files (version 1.1.12)],
        if test [ x$withval != x ] -a [ -d $withval ]; then
          libxsltinc=$withval
        else
          if test x$libxsltinc = x
          then
            echo "Please specify the location of libxslt include files: --with-libxsltinc"; exit 1
          fi
        fi ,
        if test x$libxsltinc = x
        then
          libxsltinc=$RIP/third-party/libxslt_1.1.12-8/include
        fi)

XSLT_LIBS="$libxslt/libxslt.a"
XSLT_CFLAGS="-I$libxsltinc"

AC_SUBST(XSLT_LIBS)
AC_SUBST(XSLT_CFLAGS)
])
