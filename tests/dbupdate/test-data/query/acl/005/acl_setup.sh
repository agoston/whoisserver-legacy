#!/bin/bash -e

. $DATADIR/query/acl/functions.sh

# insert entry to acl table
mysql -h$RIPADMIN_DBHOST -P$RIPADMIN_DBPORT -u$RIPADMIN_DBUSER -p$RIPADMIN_DBPASS $RIPADMIN_DBNAME <<EOM
INSERT INTO acl6 VALUES (0,0,0,0,17,2000,-1,10,1,0,4,6,'Stupid test');
INSERT INTO acl6 VALUES (0,0,0,0,31,2000,-1,10,0,0,4,6,'Stupid test');
INSERT INTO acl6 VALUES (16383,0,0,0,35,2000,-1,10,1,0,4,6,'Stupid test');
INSERT INTO acl6 VALUES (0,0,0,0,32,2000,-1,10,1,0,4,6,'Stupid test');
INSERT INTO acl6 VALUES (0,0,0,0,41,2000,-1,10,0,0,4,6,'Stupid test');
INSERT INTO acl6 VALUES (0,16,0,0,61,2000,-1,10,1,0,4,6,'Stupid test');
EOM

# have server re-read acl table
issue_command set acl_tree
