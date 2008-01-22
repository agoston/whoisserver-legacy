#!/bin/bash -e

. $DATADIR/nrtm/filtering/functions.sh

# insert complicated model into aaa table
# ripupdate should stay at 1, otherwise dbupdate will fail ;)
mysql -h$RIPADMIN_DBHOST -P$RIPADMIN_DBPORT -u$RIPADMIN_DBUSER -p$RIPADMIN_DBPASS $RIPADMIN_DBNAME <<EOM
DELETE FROM aaa;
INSERT INTO aaa VALUES (0, 0, 'DB-TEST', 1, 0, 'Root');
INSERT INTO aaa VALUES (inet_aton('127.0.0.0'), 4, 'DB-TEST', 1, 2, 'Root');
INSERT INTO aaa VALUES (inet_aton('127.0.0.0'), 9, 'DB-TEST', 1, 1, 'Root');
INSERT INTO aaa VALUES (inet_aton('127.0.0.0'), 16, 'DB-TEST', 1, 0, 'Root');
INSERT INTO aaa VALUES (inet_aton('127.0.0.0'), 27, 'DB-TEST', 1, 2, 'Root');
INSERT INTO aaa VALUES (inet_aton('127.0.0.1'), 32, 'DB-TEST', 1, 0, 'Root');
EOM

# have server re-read aaa cache
issue_command set aaa_cache
