#!/bin/bash
# strips the queries from a qry log file (read from STDIN), strips -k flags, and emits queries on STDOUT
# agoston, 2008-05-14

sed 's/.*--  //g' | sed 's/^-k //g; s/ -k / /g' | sed 's/-\([a-zA-Z]*\)k\([a-zA-Z]*\)/-\1\2/g' | grep -vx ''
