#!/bin/bash -e

. $DATADIR/nrtm/filtering/functions.sh

# insert entry to aaa table
mysql -h$RIPADMIN_DBHOST -P$RIPADMIN_DBPORT -u$RIPADMIN_DBUSER -p$RIPADMIN_DBPASS $RIPADMIN_DBNAME <<EOM
UPDATE aaa set mirror=1;
EOM

# have server re-read aaa cache
issue_command set aaa_cache
