#!/bin/bash
# Extracts queries from a query log file, stripping -k flags (well, most of them)

if [ $# -eq 0 ]; then
	echo "usage: $0 qrylogfile"
	echo ""
	echo "$0 will read whoisserver's query log file, extract queries from it, strip the -k flag and put the result on the standard output."
	exit 1
fi

sed 's/.*--  //g' <"$1" | sed 's/^-k //; s/ -k / /; s/^-k\([a-zA-Z0-9]\)/-\1/' | grep -xv -- -k | grep -vx ''
