dnl
dnl libxml settings
dnl agoston, 2008-07-04
dnl

AC_DEFUN(RIPE_CHECK_LIBXML,[
AC_ARG_WITH(libxml,
    [  --with-libxmllib        location of libxml libraries (version 2.6.16)],
        if test [ x$withval != x ] -a [ -d $withval ]; then
          libxml=$withval
        else
          if test x$libxml = x
          then
            echo "Please specify the location of libxml libraries: --with-libxmllib"; exit 1
          fi
        fi ,
        if test x$libxml = x
        then
          libxml=$RIP/third-party/libxml2_2.6.16-7sarge1/lib
        fi)

AC_ARG_WITH(libxmlinc,
    [  --with-libxmlinc        location of libxml include files (version 2.6.16)],
        if test [ x$withval != x ] -a [ -d $withval ]; then
          libxmlinc=$withval
        else
          if test x$libxmlinc = x
          then
            echo "Please specify the location of libxml include files: --with-libxmlinc"; exit 1
          fi
        fi ,
        if test x$libxmlinc = x
        then
          libxmlinc=$RIP/third-party/libxml2_2.6.16-7sarge1/include/libxml2
        fi)

XML_LIBS="$libxml/libxml2.a"
XML_CFLAGS="-I$libxmlinc"

AC_SUBST(XML_LIBS)
AC_SUBST(XML_CFLAGS)
])
