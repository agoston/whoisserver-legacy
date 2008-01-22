#!/bin/bash

. $DATADIR/nrtm/history_limit/functions.sh

# date to update timestamps to
TIMESTAMP=`date +%s`
TIMESTAMP=$[TIMESTAMP-5184000]

# update timestamps to be older than 'now'
# this is a safe operation as timestamps are read from DB and are not cached
mysql -h$RIPE_DBHOST -P$RIPE_DBPORT -u$RIPE_DBUSER -p$RIPE_DBPASS $RIPE_DBNAME <<EOM
UPDATE last SET timestamp = $TIMESTAMP;
UPDATE history SET timestamp = $TIMESTAMP;
EOM

# check the access rights
ACCESS=`get_nrtm '-q sources' | grep DB-TEST | cut -d: -f3`
if [ $ACCESS != X ]; then
	echo "Error: '-q sources' returned access right '$ACCESS' instead of 'X'!"
	exit 1
fi

# check if the number of objects is right
SOURCES=`get_nrtm '-q sources' | grep DB-TEST | cut -d: -f4`

if [ $SOURCES != '1-6' ]; then
	echo "Error: '-q sources' returned interval '$SOURCES' instead of '1-6'!"
	exit 1
fi

# check if the objects look like they should
# if you changed ANY of the DUMMY_* options in the rip.config, this part will fail
FILE1=$DATADIR/nrtm/history_limit/001/expected_result
FILE2=$DATADIR/nrtm/history_limit/001/nrtm_result

get_nrtm '-g DB-TEST:0:1-6' >>$FILE2

diff $FILE1 $FILE2 &>/dev/null
RET=$?
rm -f $FILE2

if [ $RET -gt 0 ]; then
	echo 'Error: the objects returned by nrtm do not look like they should! This is likely to happen because you have changed the DUMMY_* options in rip.config.'
	exit 1
fi
