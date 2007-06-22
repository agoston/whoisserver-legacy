#!/bin/bash -e

. $DATADIR/query/proxy/functions.sh

# insert entry to acl table
mysql -h$RIPADMIN_DBHOST -P$RIPADMIN_DBPORT -u$RIPADMIN_DBUSER -p$RIPADMIN_DBPASS $RIPADMIN_DBNAME <<EOM
INSERT INTO acl6 VALUES (0,0,0,0,94,2000,-1,10,0,1,4,6,'Proxy');
INSERT INTO acl VALUES (inet_aton("152.66.130.2"),32,2000,-1,10,1,0,4,6,'Denied host');
EOM

# have server re-read acl table
issue_command set acl_tree

# save value of denials count of the proxy
issue_command show access | awk '/^::\/[1-9]/ {print $4}' >$DATADIR/query/proxy/003/denials_count_proxy
[ -s $DATADIR/query/proxy/003/denials_count_proxy ] || echo 0 >$DATADIR/query/proxy/003/denials_count_proxy

# save value of denials count of the denied host
issue_command show access | awk '/^152.66.130.2\/32 / {print $4}' >$DATADIR/query/proxy/003/denials_count_host
[ -s $DATADIR/query/proxy/003/denials_count_host ] || echo 0 >$DATADIR/query/proxy/003/denials_count_host
