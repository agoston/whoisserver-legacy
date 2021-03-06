M4PATH=m4

AC_PREREQ(2.59)
AC_INIT([Whois Server], [3.16.2],
        [RIPE Database Management <ripe-dbm@ripe.net>],
        [whoisserver])
AM_INIT_AUTOMAKE([whoisserver],[3.16.2])

PRODUCT=$PACKAGE_NAME
VERSION=$PACKAGE_VERSION
RELEASE_TAG=$PACKAGE_VERSION
RIP=`pwd`

AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Package version])

AC_SUBST(PRODUCT)
AC_SUBST(VERSION)
AC_SUBST(RELEASE_TAG)
AC_SUBST(RIP)

AC_PREFIX_DEFAULT([/usr/local/whoisd])

RIPE_OPL_ONLY

AM_CONFIG_HEADER(config.h)
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
LF_SET_WARNINGS
AC_PATH_PROGS(BISON, bison)
AC_CANONICAL_HOST
AC_PATH_PROGS(PERL5, perl perl5)
AC_PATH_PROGS(GPGCMD, gpg gnupg)
AC_PATH_PROGS(M4, gm4 gnum4 m4, m4)

dnl
dnl installation directory layout
dnl
topdir=$prefix
bindir=${topdir}/bin
confdir=${topdir}/conf
scriptdir=${topdir}/scripts
utildir=${topdir}/utils
docdir=${topdir}/doc
vardir=${topdir}/var
tmpdir=${vardir}/tmp
lockdir=${vardir}/lock

libdir=${topdir}/lib
mandir=${topdir}/man/man1

logdir=${topdir}/log

dnl Let the user set his own paths
AC_ARG_WITH(confdir,
    AC_HELP_STRING([--with-confdir=DIR], [Directory, where all the configuration files will be stored(default: $confdir)]),
    if test "x$withval" != xyes; then
        confdir=$withval
    fi
)

AC_ARG_WITH(tmpdir,
    AC_HELP_STRING([--with-tmpdir=DIR], [Directory, where all the temporary files will be stored(default: $tmpdir)]),
    if test "x$withval" != xyes; then
        tmpdir=$withval
    fi
)

AC_ARG_WITH(logdir,
    AC_HELP_STRING([--with-logdir=DIR], [Directory, where all the logs will be stored(default: $logdir)]),
    if test "x$withval" != xyes; then
        logdir=$withval
    fi
)

AC_ARG_WITH(perl,
    AC_HELP_STRING([--with-perl=/path/to/perl], [Full path to perl. Defaults to path lookup)]),
    if test "x$withval" != xyes; then
        PERL5=$withval
        echo "Using perl located in $PERL5"
    fi
)

dnl Log infrastructure
qrylog=${logdir}/qry
dbuperr=${logdir}/dbuperr
allriperr=${logdir}/riperr
ripaudit=${logdir}/audit
ripmirlog=${logdir}/mirror
ripsvrlog=${logdir}/svr
sqlog=${logdir}/sql
ripupdlog=${logdir}/ripupd
updlog=${logdir}/upd
acklog=${logdir}/ack
notiflog=${logdir}/notif
forwlog=${logdir}/forw
crosslog=${logdir}/cross

dnl
AC_SUBST(topdir)
AC_SUBST(bindir)
AC_SUBST(confdir)
AC_SUBST(scriptdir)
AC_SUBST(utildir)
AC_SUBST(docdir)
AC_SUBST(vardir)
AC_SUBST(tmpdir)
AC_SUBST(lockdir)

AC_SUBST(logdir)
AC_SUBST(qrylog)
AC_SUBST(dbuperr)
AC_SUBST(allriperr)
AC_SUBST(ripaudit)
AC_SUBST(ripmirlog)
AC_SUBST(ripsvrlog)
AC_SUBST(sqlog)
AC_SUBST(ripupdlog)
AC_SUBST(updlog)
AC_SUBST(acklog)
AC_SUBST(notiflog)
AC_SUBST(forwlog)
AC_SUBST(crosslog)

AC_SUBST(docdir)
AC_SUBST(libdir)
AC_SUBST(mandir)

dnl check for c-client
RIPE_CHECK_CCLIENT

dnl Link xml files depending on the variant
RIPE_SET_RPSL_VARIANT

dnl check rdns extensions
RIPE_CHECK_RDNS

dnl check for ctime
AC_CHECK_CTIME_R

dnl check for various functions
AC_CHECK_FUNCS([atexit bzero dup2 floor getcwd gethostbyname gethostname \
                gettimeofday inet_ntoa isascii localeconv localtime_r \
                mblen memchr memmove memset mkdir modf pow regcomp rmdir \
                select socket strcasecmp strchr strdup strerror \
                strncasecmp strrchr strstr strtol strtoul strtoull \
                strsep strtok_r])

dnl checks for header files
AC_FUNC_ALLOCA
AC_HEADER_STDC
AC_HEADER_SYS_WAIT

AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h inttypes.h limits.h locale.h])
AC_CHECK_HEADERS([malloc.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h])
AC_CHECK_HEADERS([string.h strings.h sys/file.h sys/ioctl.h sys/param.h])
AC_CHECK_HEADERS([sys/socket.h sys/time.h sys/timeb.h unistd.h wchar.h wctype.h])
AC_CHECK_HEADERS([pthread.h typeinfo.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STAT
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_CHECK_TYPES([ptrdiff_t])

dnl Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MKTIME
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_SETPGRP
AC_FUNC_STRCOLL
AC_FUNC_STRFTIME
AC_FUNC_STRTOD
AC_FUNC_VPRINTF

AC_LIBRARY_NET

dnl
dnl GnuPG executable
dnl

AC_ARG_WITH(gpgcmd,
    [  --with-gpgcmd=CMD       CMD=pathname of gpg executable],
    if test "x$withval" != "x" -a -x "$withval"; then
        GPGCMD=$withval
    else
        AC_MSG_ERROR([Please specify a suitable gpg executable: --with-gpgcmd=CMD], 1)
    fi
)

AC_CHECK_FILE($GPGCMD, [], [
    AC_MSG_ERROR([Cannot find gpg executable
    Please specify a suitable gpg executable: --with-gpgcmd=CMD], 1)
])
AC_SUBST(GPGCMD)

dnl Fancy check for uint32_t and uint64_t, which are defined in <stdint.h>
dnl or <netinet/in.h> in Linux, but not in a header searched by AC_CHECK_TYPE
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <inttypes.h>
]], [[
uint32_t xyzzy;
]])],[],[AC_CHECK_TYPE(uint32_t, u_int32_t)
])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <inttypes.h>
]], [[
uint64_t xyzzy;
]])],[],[AC_CHECK_TYPE(uint64_t, u_int64_t)
])

dnl
dnl decipher pthread compilation model
dnl

dnl the old way doesn't work on BSD systems
dnl AC_CHECK_LIB(pthread, pthread_create)

dnl first, try linking a threaded application with "-pthread"
dnl which will work on FreeBSD (and OpenBSD) systems
dnl LDFLAGS="-lpthread $LDFLAGS"

dnl then, try looking in various libraries, which will work on other systems
AC_SEARCH_LIBS(pthread_create, [ pthread pthreads thread threads ])

dnl check for pthread_attr_getstacksize, which doesn't exist on Linux
AC_SEARCH_LIBS(pthread_attr_getstacksize,[pthread pthreads thread threads])
AC_CHECK_FUNCS(pthread_attr_getstacksize pthread_attr_setstacksize)

dnl Checks for libraries (must occur after Pthread checks).
AC_CHECK_LIB(bind_r, gethostbyname_r)
AC_CHECK_LIB(pam, pam_start)

# some libraries we use
AC_SEARCH_LIBS(crypt, crypt)
AC_SEARCH_LIBS(compress, z)

dnl Adjust libtool
AC_PROG_LIBTOOL

dnl Check for glib
AC_CHECK_PROG(PKGCONFIG,[pkg-config],[yes])
if test "x${PKGCONFIG}" = "xyes"
then
    AM_PATH_GLIB_2_0(2.0.0,,AC_MSG_ERROR([glib-2.x.x not found]),gthread)
else
    AC_MSG_ERROR([pkg-config not found])
fi
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

dnl Java related checks
AC_PROG_JAVAC
AC_PROG_JAVA
AM_PROG_GCJ

dnl Check for mysql
AC_CHECK_MYSQLR
AC_PATH_PROGS(MYSQLADMIN,mysqladmin)
AC_PATH_PROGS(MYSQLCLIENT,mysql)
AC_PATH_PROGS(MYSQLCONVERT,mysql_convert_table_format)
AC_SUBST(MYSQL_LIBS)
AC_SUBST(MYSQL_CFLAGS)

dnl Check for proper versions of libxml and libxslt
dnl disabled by agoston, 2008-07-04, as we depend on an older version of
dnl libxml & libxslt, and we will link the older version statically
dnl to avoid incorrect dynamic linking
dnl
dnl AM_PATH_XML2([2.4.0], [], if test x"$OPLONLY" != xyes
dnl                           then
dnl                            AC_MSG_ERROR([libxml2 is required!])
dnl                          fi)
dnl AM_PATH_XSLT([1.0.0], [], if test x"$OPLONLY" != xyes
dnl                          then
dnl                            AC_MSG_ERROR([libxslt is required!])
dnl                          fi)
RIPE_CHECK_LIBXML
RIPE_CHECK_LIBXSLT

dnl Check for md5 headers
AC_SEARCH_LIBS(MD5Init, md5)    # Solaris
AC_CHECK_FUNCS(MD5Init MD5Update MD5Final)
AC_SEARCH_LIBS(MD5_Init, ssl)   # Linux, FreeBSD
AC_CHECK_FUNCS(MD5_Init MD5_Update MD5_Final)
# this is quite an assumption, in need of exceptions
AC_CHECK_HEADER(openssl/md5.h,[ CFLAGS="$CFLAGS -I/usr/include/openssl -I/usr/local/include/openssl" ])

dnl add -Wall to CFLAGS
dnl also add the flags that were, almost accidentally, provided by
dnl '$PERL5 -MExtUtils::Embed -e ccopts', which included pthread. Meaning builds
dnl would fail with a non-threaded perl used for rdns... scary! --jib
dnl The flags after -Wall were provided by a threaded perl(5.8), but not by an
dnl unthreaded perl(5.10)
CFLAGS="$CFLAGS -Wall -D_REENTRANT -D_GNU_SOURCE -DTHREADS_HAVE_PIDS"

dnl Create links for header files
AC_MSG_CHECKING([Creating Links])
AC_CONFIG_COMMANDS([links],[
  OPWD=`pwd`
  mkdir -p $ac_abs_top_builddir/src/include
  cd $ac_abs_top_builddir/src/include
  chmod u+rwx $ac_abs_top_builddir/src/include
  OIFS=$IFS
  IFS='
'
  AC_MSG_NOTICE([Linking in ${PWD}])
  for file in $ac_abs_top_srcdir/src/modules/??/*.h $ac_abs_top_srcdir/src/modules/rpsl/syntax_api.h
  do
    if ! test $file -ef `basename $file`
    then
      ln -sf $file . 2>/dev/null
    fi
  done
  mkdir -p $ac_abs_top_builddir/src/modules/rpsl
  cd $ac_abs_top_builddir/src/modules/rpsl
  chmod u+rwx $ac_abs_top_builddir/src/include
  AC_MSG_NOTICE([Linking in ${PWD}])
  for file in $ac_abs_top_srcdir/src/modules/rpsl/*.h \
    $ac_abs_top_srcdir/src/modules/rpsl/*.c \
    $ac_abs_top_srcdir/src/modules/rpsl/*.l  \
    $ac_abs_top_srcdir/src/modules/rpsl/*.y  
  do
    if ! test $file -ef `basename $file`
    then
      ln -sf $file . 2>/dev/null
    fi
  done
  IFS=$OIFS
  cd $OPWD
  ])
AC_MSG_RESULT([Done])

dnl Output & finalization
AC_CONFIG_FILES([
  conf/rip.config.sample
  conf/sources.config.sample
  conf/ack_template
  conf/ack_text
  conf/forw_create_template
  conf/frwd_template
  conf/frwd_text
  conf/notif_template
  conf/notif_text
  conf/SAMPLE.CURRENTSERIAL
  doc/WHOIS-HELP
  doc/LOOKUP-HELP
  src/defs/makedefsall
  src/defs/Makefile
  src/opl-perl/Makefile.PL
  Makefile
  src/Makefile
  src/utils/make_mysql_user
  src/utils/remadmin.pl
  src/utils/ripe2rpsl
  src/utils/enableupdates
  src/utils/extract-keycerts-from-text-dump.pl
  src/utils/extract-keycerts-sql.pl
  src/utils/ip6arpa-fix.pl
  src/utils/radb_filter.pl
  src/utils/runqueue
  src/utils/resync-gnupg-ring-from-text-dump.sh
  src/utils/resync-gnupg-ring-from-sql.sh
  src/utils/restore_dbase.sh
  src/utils/ftp_export.sh
  src/utils/make_db
  src/utils/load_all.sh
  src/utils/whoisd.server
  src/utils/whoisd_start
  tests/dbupdate/testing/conf/dataset.config])
AC_CONFIG_COMMANDS([permissions],[
  chmod u+x src/defs/makedefsall; \
  chmod u+x src/utils/remadmin.pl; \
  chmod u+x src/utils/ftp_export.sh])
AC_CONFIG_COMMANDS([sample database],[
  gzip -9c conf/sample.db > ${ac_abs_builddir}/conf/sample.db.gz])
AC_OUTPUT
