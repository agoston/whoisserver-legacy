#!/bin/sh

# set the variables for the ripadmin database
for i in `echo $RIPADMIN | tr "," " "`; do
	if [ -z $RIPADMIN_DBHOST ]; then export RIPADMIN_DBHOST=$i; continue; fi
	if [ -z $RIPADMIN_DBPORT ]; then export RIPADMIN_DBPORT=$i; continue; fi
	if [ -z $RIPADMIN_DBUSER ]; then export RIPADMIN_DBUSER=$i; continue; fi
	if [ -z $RIPADMIN_DBPASS ]; then export RIPADMIN_DBPASS=$i; continue; fi
	if [ -z $RIPADMIN_DBNAME ]; then export RIPADMIN_DBNAME=$i; continue; fi
done

# set the variables for the RIPE database
for i in `echo $UPDSOURCE | cut -d" " -f2 | tr "," " "`; do
	if [ -z $RIPE_DBHOST ]; then export RIPE_DBHOST=$i; continue; fi
	if [ -z $RIPE_DBPORT ]; then export RIPE_DBPORT=$i; continue; fi
	if [ -z $RIPE_DBUSER ]; then export RIPE_DBUSER=$i; continue; fi
	if [ -z $RIPE_DBPASS ]; then export RIPE_DBPASS=$i; continue; fi
	if [ -z $RIPE_DBNAME ]; then export RIPE_DBNAME=$i; continue; fi
done

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
