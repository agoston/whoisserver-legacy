# ----------------------------------------------------------------------
# Provide the configure script with an --with-warnings option that
# turns on warnings. Call this command AFTER you have configured ALL your
# compilers. 
# ----------------------------------------------------------------------

AC_DEFUN(LF_SET_WARNINGS,[
  dnl Check for --with-warnings
  AC_MSG_CHECKING([whether user wants warnings])
  AC_ARG_WITH(warnings,
              [  --with-warnings         Turn on warnings],
              [ lf_warnings=yes ], [ lf_warnings=no ])
  AC_MSG_RESULT($lf_warnings)
  
  dnl Warnings for the two main compilers
  cc_warning_flags="-Wall"
  cxx_warning_flags="-Wall -Woverloaded-virtual -Wtemplate-debugging"
  if test $lf_warnings = yes
  then
    if test -n "${CC}"
    then
      LF_CHECK_CC_FLAG($cc_warning_flags)
    fi
    if test -n "${CXX}" 
    then
      LF_CHECK_CXX_FLAG($cxx_warning_flags)
    fi
  fi
])

# --------------------------------------------------------------------------
# Check whether the C++ compiler accepts a certain flag
# If it does it adds the flag to CXXFLAGS
# If it does not then it returns an error to lf_ok
# Usage:
#   LF_CHECK_CXX_FLAG(-flag1 -flag2 -flag3 ...)
# -------------------------------------------------------------------------

AC_DEFUN(LF_CHECK_CXX_FLAG,[
  echo 'void f(){}' > conftest.cc
  for i in $1
  do
    AC_MSG_CHECKING([whether $CXX accepts $i])
    if test -z "`${CXX} $i -c conftest.cc 2>&1`"
    then
      CXXFLAGS="${CXXFLAGS} $i"
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  done
  rm -f conftest.cc conftest.o
])
