#!/bin/sh

TMPFILE=`tempfile`

# set the variables for the ripadmin database
echo $RIPADMIN | tr "," " " >$TMPFILE
read RIPADMIN_DBHOST RIPADMIN_DBPORT RIPADMIN_DBUSER RIPADMIN_DBPASS RIPADMIN_DBNAME <$TMPFILE
rm -f $TMPFILE

# set the variables for the RIPE database
echo $UPDSOURCE | cut -d" " -f2 | tr "," " " >$TMPFILE
read RIPE_DBHOST RIPE_DBPORT RIPE_DBUSER RIPE_DBPASS RIPE_DBNAME <$TMPFILE
rm -f $TMPFILE

# issue a single command to the server
function issue_command() {
	echo "$@" | nc -q 1 $WHOIS_HOST $SVCONFIG_PORT
}

function issue_command_get_ret() {
        return `echo "$@" | nc -q 1 $WHOISHOST $SVCONFIG_PORT | sed 's/.*=\([0-9]*\)=.*/\1/g' | tail -1`
}

function get_nrtm() {
	echo "$@" | nc -q 1 $WHOIS_HOST $SVMIRROR_PORT
}
