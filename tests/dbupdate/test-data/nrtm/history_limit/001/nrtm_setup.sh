#!/bin/bash -e

. $DATADIR/nrtm/history_limit/functions.sh

# date to update timestamps to
TIMESTAMP=`date +%s`
TIMESTAMP=$[TIMESTAMP-5184000]

# update timestamps to be older than 'now'
mysql -h$RIPE_DBHOST -P$RIPE_DBPORT -u$RIPE_DBUSER -p$RIPE_DBPASS $RIPE_DBNAME <<EOM
UPDATE last SET timestamp = $TIMESTAMP;
UPDATE history SET timestamp = $TIMESTAMP;
EOM
