#!/bin/bash -e

. $DATADIR/query/acl/functions.sh

# save value of denials count
NEWDENIALS=`issue_command show access | awk '/^127.0.0.1\/32 / {print $4}'`

# load the old denials count
read OLDDENIALS <$DATADIR/query/acl/001/denials_count

# remove the surplus file now
rm $DATADIR/query/acl/001/denials_count

if [ '!' $OLDDENIALS ]; then
    echo "Old denial count wrong: '$OLDDENIALS'"
    exit 1
fi


if [ '!' $NEWDENIALS ]; then
    echo "New denial count wrong: '$NEWDENIALS'"
    exit 1
fi

if [ $[OLDDENIALS+1] '!=' $NEWDENIALS ]; then
	echo "ERROR: deny count increase mismatch; OLD: $OLDDENIALS, NEW: $NEWDENIALS (should be: $[OLDDENIALS+1])"
	exit 1
fi
