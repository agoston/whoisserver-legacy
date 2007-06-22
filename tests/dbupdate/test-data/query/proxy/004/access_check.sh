#!/bin/bash -e

. $DATADIR/query/proxy/functions.sh

# save value of denials count
NEWDENIALS_PROXY=`issue_command show access | awk '/^::\/[1-9]/ {print $4}'`
NEWDENIALS_HOST=`issue_command show access | awk '/^1234:5678:/ {print $4}'`

# load the old denials count
read OLDDENIALS_PROXY <$DATADIR/query/proxy/004/denials_count_proxy
read OLDDENIALS_HOST <$DATADIR/query/proxy/004/denials_count_host

# remove the surplus file now
rm $DATADIR/query/proxy/004/denials_count_*

if [ '!' $OLDDENIALS_PROXY ]; then
    echo "Old proxy denial count wrong: '$OLDDENIALS_PROXY'"
    exit 1
fi

if [ '!' $OLDDENIALS_HOST ]; then
    echo "Old host denial count wrong: '$OLDDENIALS_HOST'"
    exit 1
fi


if [ '!' $NEWDENIALS_PROXY ]; then
    echo "New proxy denial count wrong: '$NEWDENIALS_PROXY'"
    exit 1
fi

if [ '!' $NEWDENIALS_HOST ]; then
    echo "New host denial count wrong: '$NEWDENIALS_HOST'"
    exit 1
fi

if [ $[OLDDENIALS_HOST+1] '!=' $NEWDENIALS_HOST ]; then
	echo "ERROR: host deny count increase mismatch; OLD: $OLDDENIALS_HOST, NEW: $NEWDENIALS_HOST (should be: $[OLDDENIALS_HOST+1])"
	exit 1
fi

if [ $OLDDENIALS_PROXY '!=' $NEWDENIALS_PROXY ]; then
	echo "ERROR: proxy deny count mismatch; OLD: $OLDDENIALS_PROXY, NEW: $NEWDENIALS_PROXY (should be: $OLDDENIALS_PROXY)"
	exit 1
fi
