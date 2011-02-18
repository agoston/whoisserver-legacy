#!/bin/bash
# builds third-party libraries and runs configure.
# prerequisites: run autogen.sh
# postrequisites: run make
#
# FIXME: this should be done by configure, not a custom shellscript
#
# agoston, 2008-07-04

# bail out if anything went wrong
set -e

# determine machine bits
if [ "`uname -m`" = x86_64 ]; then
	BITS=64
else
	BITS=32
fi

###
### Compile c-client
###
CCCOMP=1
CCDIR=`pwd`/third-party/imap-2007e/

if [ -s ${CCDIR}/c-client/mail.o ]; then
	CCBITS=`file ${CCDIR}/c-client/mail.o | sed 's/.*\([0-9][0-9]\)-bit.*/\1/'`
	
	if [ $CCBITS -eq $BITS ]; then
		CCCOMP=0
	fi
fi

if [ $CCCOMP -eq 1 ]; then
	echo ' ***'
	echo ' *** Compiling c-client'
	echo ' ***'
	pushd $CCDIR
	rm -rf imap2007e
	tar xjf imap-2007e.tar.bz2
	cd imap-2007e
	make slx EXTRACFLAGS="-I/usr/include/openssl"
	# clean up
	cd ..
	cp -Lr imap-2007e/c-client .
	rm -rf imap-2007e
	popd
fi

###
### Compile libxml
###

LIBXMLCOMP=1
LIBXMLDIR=`pwd`/third-party/libxml2_2.6.16-7sarge1

if [ -s ${LIBXMLDIR}/lib/libxml2.so.2.6.16 ]; then
	LIBXMLBITS=`file ${LIBXMLDIR}/lib/libxml2.so.2.6.16 | sed 's/.*\([0-9][0-9]\)-bit.*/\1/'`
	
	if [ $LIBXMLBITS -eq $BITS ]; then
		LIBXMLCOMP=0
	fi
fi

if [ $LIBXMLCOMP -eq 1 ]; then
	echo ' ***'
	echo ' *** Compiling libxml'
	echo ' ***'
	pushd $LIBXMLDIR
	rm -rf bin include lib libxml2-2.6.16 man share
	tar xzf libxml2_2.6.16.orig.tar.gz
	cd libxml2-2.6.16
	zcat ../libxml2_2.6.16-7sarge1.diff.gz | patch -p1
	# eliminate non-working elfgcchack.h
	>elfgcchack.h
	./configure --prefix=$LIBXMLDIR
	make
	make install
	# clean up
	cd $LIBXMLDIR
	rm -rf libxml2-2.6.16
	popd
fi

###
### Compile libxslt
###

LIBXSLTCOMP=1
LIBXSLTDIR=`pwd`/third-party/libxslt_1.1.12-8

if [ -s ${LIBXSLTDIR}/lib/libxslt.so.1.1.12 ]; then
	LIBXSLTBITS=`file ${LIBXSLTDIR}/lib/libxslt.so.1.1.12 | sed 's/.*\([0-9][0-9]\)-bit.*/\1/'`
	
	if [ $LIBXSLTBITS -eq $BITS ]; then
		LIBXSLTCOMP=0
	fi
fi

if [ $LIBXSLTCOMP -eq 1 ]; then
	echo ' ***'
	echo ' *** Compiling libxslt'
	echo ' ***'
	pushd $LIBXSLTDIR
	rm -rf bin include lib libxslt-1.1.12 man share
	tar xzf libxslt_1.1.12.orig.tar.gz
	cd libxslt-1.1.12
	zcat ../libxslt_1.1.12-8.diff.gz | patch -p1
	./configure --prefix=$LIBXSLTDIR --with-libxml-prefix=$LIBXMLDIR
	make
	make install
	# clean up
	cd ..
	rm -rf libxslt-1.1.12
	popd
fi

###
### Compile whois-server
###

echo ' ***'
echo ' *** Extensions built. You can now compile the whois-server'
echo ' ***'

#./configure --with-rdns --prefix=$HOME/local
# default options omitted:
#--with-cclientinc=`pwd`/third-party/imap-2007e/c-client --with-cclientlib=`pwd`/third-party/imap-2007e/c-client
#--with-xml-prefix=`pwd`/third-party/libxml2_2.6.16-7sarge1 --with-xslt-prefix=`pwd`/third-party/libxslt_1.1.12-8
