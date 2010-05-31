#!/bin/bash
#    ________ .___ ___________
#   /  _____/ |   |\__    ___/
#  /   \  ___ |   |  |    |   
#  \    \_\  \|   |  |    |   
#   \______  /|___|  |____|   
#          \/                 
# git clone git:whoisserver
# src/utils/wrapper.sh
# ****************************************
#
# wrapper around dbupdate to:
# - process STOPUPDATES (mailupdates has its own; syncupdates, override, ... is relying on this one)
# - handle dbupdate crash
#
# agoston, 2010-05-31

# import default settings
. /etc/default/ripe

BASEDIR=/home/dbase/services/whois-update
STOPUPDATES=${BASEDIR}/var/STOPUPDATES
DBUPDATE=${BASEDIR}/bin/dbupdate.real
ERRORS_TO="dbint@ripe.net"

# check for STOPUPDATES
if [ -f $STOPUPDATES ]; then
	echo "Updates are down for maintenance at the moment. Please try again later."
	echo "Please also check out the RIPE NCC Service Announcements page for further information:"
	echo "http://ripe.net/news/status.html"
	exit 1
fi

# wrap dbupdate call
${DBUPDATE} "$@"
RET=$?

# send email on non-zero return code
if [ $RET -gt 0 ]; then
	echo "${DBUPDATE} exited with code ${RET} on ${HOSTNAME}
Command line: ${DBUPDATE} $@" | mail -s "${DBUPDATE} execution problem" $ERRORS_TO
	exit $RET
fi
