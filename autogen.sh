#!/bin/sh

myFail() {
  echo "$* failed"
  exit 1
}

echo "Running aclocal"
aclocal -I m4 || myFail aclocal

echo "Running autoheader"
autoheader || myFail autoheader

echo "Running automake"
#automake -a -c  || myFail automake
automake || myFail automake

echo "Running autoconf"
autoconf || myFail autoconf
