#!/bin/bash
if [ "`uname -m`" = x86_64 ]; then
	cclientinc=`pwd`/third-party/imap-2004g-64bit/c-client
	cclientlib=`pwd`/third-party/imap-2004g-64bit/c-client
else
	cclientinc=`pwd`/third-party/c-client
	cclientlib=`pwd`/third-party/c-client
fi

./configure --with-rdns --prefix=$HOME/local --with-cclientinc=$cclientinc --with-cclientlib=$cclientlib
