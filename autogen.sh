#!/bin/sh

myFail() {
  echo "$* failed"
  exit 1
}

# I don't know a better way for this. Please replace as appropriate.
automake_version=`automake --version \
  | grep ^automake \
  | sed -e 's/^.* \([^ ][^ ]*\)$/\1/'`
if [ "$automake_version" != "1.5" ]
then
  echo ===============================
  echo Automake v1.5 is used for building.
  echo Your version is ${automake_version}.
  echo If configuration fails, keep this in mind :\)
  echo ===============================
fi

echo "Running aclocal"
aclocal -I m4 || myFail aclocal

echo "Running autoheader"
autoheader || myFail autoheader

echo "Running automake"
#automake -a -c  || myFail automake
automake -a -c || myFail automake

echo "Running autoconf"
autoconf || myFail autoconf
