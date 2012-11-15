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
# - handle dbupdate crash
# - log execution time
#
# agoston, 2011-04-20

# import default settings
. /export/services/.defaults

BASEDIR=$(readlink -e $(dirname $0))/..
DBUPDATE=${BASEDIR}/bin/dbupdate
ERRORS_TO="dbint@ripe.net"

# log execution times (used in CMM reports ONLY)
mkdir -p $BASEDIR/var/log/time &>/dev/null
TIMELOG=$BASEDIR/var/log/time/timelog.$(date +%Y%m%d)
TIME="/usr/bin/time --append --format %e --output ${TIMELOG}"

# wrap dbupdate call - note that this is delibaretly using $*, and not "$@", as some arguments passed
# from perl over ssh "might" get concatenated, and this is the only change to separate them.
# We never have spaces in any of the arguments here, so it's fine - agoston, 2010-06-07
INFILE=`mktemp`
OUTFILE=`mktemp`
ERRFILE=`mktemp`
cat >$INFILE
${TIME} ${DBUPDATE} $* <$INFILE >$OUTFILE 2>$ERRFILE
RET=$?
cat $OUTFILE
cat $ERRFILE 1>&2

# send email on non-zero return code
if [ $RET -gt 0 ]; then
	echo "To: $ERRORS_TO
Subject: ${DBUPDATE} execution problem

${DBUPDATE} exited with code ${RET} on ${HOSTNAME}
Command line: ${DBUPDATE} $*

STDIN: >>>`cat $INFILE`<<<

STDOUT: >>>`cat $OUTFILE`<<<

STDERR: >>>`cat $ERRFILE`<<<
" | /usr/sbin/sendmail -funread@ripe.net -t
fi

rm -f $INFILE $OUTFILE $ERRFILE
exit $RET
