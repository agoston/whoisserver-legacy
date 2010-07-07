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

# log execution times (used in CMM reports ONLY)
TIMELOG=$BASEDIR/var/log/time/timelog.$(date +%Y%m%d)
TIME="/usr/bin/time --append --quiet --format %e --output ${TIMELOG}"

# check for STOPUPDATES
if [ -f $STOPUPDATES ]; then
	echo "Updates are down for maintenance at the moment. Please try again later."
	echo "For further information, please visit the RIPE NCC Service Announcements at"
	echo "http://ripe.net/news/status.html"
	exit 1
fi

# wrap dbupdate call - note that this is delibaretly using $*, and not "$@", as some arguments passed
# from perl over ssh "might" get concatenated, and this is the only change to separate them.
# We never have spaces in any of the arguments here, so it's fine - agoston, 2010-06-07
INFILE=`tempfile`
OUTFILE=`tempfile`
ERRFILE=`tempfile`
cat >$INFILE
${TIME} ${DBUPDATE} $* <$INFILE >$OUTFILE 2>$ERRFILE
RET=$?
cat $OUTFILE
cat $ERRFILE 1>&2

# send email on non-zero return code
if [ $RET -gt 0 ]; then
	echo "${DBUPDATE} exited with code ${RET} on ${HOSTNAME}
Command line: ${DBUPDATE} $*

STDIN: >>>`cat $INFILE`<<<

STDOUT: >>>`cat $OUTFILE`<<<

STDERR: >>>`cat $ERRFILE`<<<
" | mail -s "${DBUPDATE} execution problem" $ERRORS_TO
fi

rm -f $INFILE $OUTFILE $ERRFILE
exit $RET
