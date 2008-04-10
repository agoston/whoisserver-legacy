#!/bin/bash
cclientinc=`pwd`/third-party/c-client
cclientlib=`pwd`/third-party/c-client

./configure --with-rdns --prefix=$HOME/local --with-cclientinc=$cclientinc --with-cclientlib=$cclientlib
