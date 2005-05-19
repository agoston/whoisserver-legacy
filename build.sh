#!/bin/sh
# $Id$

# Path to the installation of c-client libraries.
# It must contain c-client.a. This file may be renamed as "lib-cclient.a"
# In this case, it's necessary to rename or copy the file to c-client.a
cclientlib=/usr/local/lib

# Path to the installation of c-client headers.
cclientinc=/usr/local/include

# Path to the installation of libxml2 and libxslt.
# Whoisserver currently requires an old version of these software,
# therefore system-provided versions of these libraries may not work.
XML2_CONFIG=/usr/local/bin/xml2-config
XSLT_CONFIG=/usr/local/bin/xslt-config

# Set the PATH variable to point to all programs needed by configure.
# These include mysql_config, glib-config, pkg-config, javac, perl, gpg...
PATH=/usr/local/bin:/usr/local/mysql/bin:/usr/local/j2sdk/bin:$PATH

# LD_LIBRARY_PATH is a list of paths that list unstandard library
# installations. Remember to include the library paths to the
# non-standard installations of libxml2 and libxslt.
LD_LIBRARY_PATH=/usr/local/lib:/usr/local/mysql/lib

export cclientinc cclientlib XML2_CONFIG XSLT_CONFIG PATH LD_LIBRARY_PATH

# Compiler fine-tuning
CC=gcc
export CC
#CFLAGS="-I/usr/local/bind/include -L/usr/local/bind/lib"
#export CFLAGS

./configure \
  --without-rdns \
  "$@"
