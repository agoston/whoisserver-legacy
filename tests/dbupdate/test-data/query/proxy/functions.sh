#!/bin/sh

# set the variables for the ripadmin database
for i in `echo $RIPADMIN | tr "," " "`; do
	if [ -z $RIPADMIN_DBHOST ]; then export RIPADMIN_DBHOST=$i; continue; fi
	if [ -z $RIPADMIN_DBPORT ]; then export RIPADMIN_DBPORT=$i; continue; fi
	if [ -z $RIPADMIN_DBUSER ]; then export RIPADMIN_DBUSER=$i; continue; fi
	if [ -z $RIPADMIN_DBPASS ]; then export RIPADMIN_DBPASS=$i; continue; fi
	if [ -z $RIPADMIN_DBNAME ]; then export RIPADMIN_DBNAME=$i; continue; fi
done

# issue a single command to the server
function issue_command() {
	echo "$@" | nc -q 1 $WHOIS_HOST $SVCONFIG_PORT
}

function issue_command_get_ret() {
        return `echo "$@" | nc -q 1 $WHOISHOST $SVCONFIG_PORT | sed 's/.*=\([0-9]*\)=.*/\1/g' | tail -1`
}
