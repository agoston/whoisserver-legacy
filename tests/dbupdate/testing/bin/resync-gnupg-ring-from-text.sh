#!/bin/sh
#
#  resync-gnupg-ring-from-text.sh
#
# takes keycerts from text dump and makes resync of gnupg ring
# change of the original file by katie

# usage:
# resync-gnupg-ring-from-text.sh <gpg> <gpg_home> <file> <tmpfile>
# 1. extract keys from file into tmpfile
# 2. import keys from tmpfile into gpg pubring in <gpghome>
#

EXTRACT="$1"
GPG="$2"
GNUPGHOME="$3"; export GNUPGHOME
FILE="$4"
TMPFILE="$5"

$EXTRACT $FILE > $TMPFILE

$GPG --import --batch $TMPFILE

# eof

