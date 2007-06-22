#!/bin/bash -e

. $DATADIR/query/proxy/functions.sh

# insert entry to acl table
mysql -h$RIPADMIN_DBHOST -P$RIPADMIN_DBPORT -u$RIPADMIN_DBUSER -p$RIPADMIN_DBPASS $RIPADMIN_DBNAME <<EOM
INSERT INTO acl6 VALUES (0,0,0,0,94,2000,-1,10,0,0,4,6,'Stupid test');
EOM

# have server re-read acl table
issue_command set acl_tree

# save value of denials count
issue_command show access | awk '/^::\/[1-9]/ {print $4}' >$DATADIR/query/proxy/002/denials_count
[ -s $DATADIR/query/proxy/002/denials_count ] || echo 0 >$DATADIR/query/proxy/002/denials_count
