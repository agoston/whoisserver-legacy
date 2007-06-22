#!/bin/bash -e

. $DATADIR/query/acl/functions.sh

# insert entry to acl table
mysql -h$RIPADMIN_DBHOST -P$RIPADMIN_DBPORT -u$RIPADMIN_DBUSER -p$RIPADMIN_DBPASS $RIPADMIN_DBNAME <<EOM
INSERT INTO acl VALUES (inet_aton('127.0.0.1'),32,2000,-1,10,1,0,4,6,'Stupid test');
EOM

# have server re-read acl table
issue_command set acl_tree

# save value of denials count
issue_command show access | awk '/^127.0.0.1\/32 / {print $4}' >$DATADIR/query/acl/001/denials_count
[ -s $DATADIR/query/acl/001/denials_count ] || echo 0 >$DATADIR/query/acl/001/denials_count