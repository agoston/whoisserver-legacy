#!/bin/bash -e

. $DATADIR/query/proxy/functions.sh

# save value of denials count
NEWPASS_PROXY=`issue_command show access | awk '/^::\/[1-9]/ {print $3}'`
NEWQRY_HOST=`issue_command show access | awk '/^1234:5678:/ {print $5}'`

# load the old denials count
read OLDPASS_PROXY <$DATADIR/query/proxy/005/pass_count_proxy
read OLDQRY_HOST <$DATADIR/query/proxy/005/qry_count_host

# remove the surplus file now
rm $DATADIR/query/proxy/005/*_count_*

if [ '!' $OLDPASS_PROXY ]; then
    echo "Old proxy pass count wrong: '$OLDPASS_PROXY'"
    exit 1
fi

if [ '!' $OLDQRY_HOST ]; then
    echo "Old host query count wrong: '$OLDQRY_HOST'"
    exit 1
fi

if [ '!' $NEWPASS_PROXY ]; then
    echo "New proxy pass count wrong: '$NEWPASS_PROXY'"
    exit 1
fi

if [ '!' $NEWQRY_HOST ]; then
    echo "New host query count wrong: '$NEWQRY_HOST'"
    exit 1
fi

if [ $[OLDQRY_HOST+1] '!=' $NEWQRY_HOST ]; then
	echo "ERROR: host query count increase mismatch; OLD: $OLDQRY_HOST, NEW: $NEWQRY_HOST (should be: $[OLDQRY_HOST+1])"
	exit 1
fi

if [ $[OLDPASS_PROXY+1] '!=' $NEWPASS_PROXY ]; then
	echo "ERROR: proxy pass count mismatch; OLD: $OLDPASS_PROXY, NEW: $NEWPASS_PROXY (should be: $[OLDPASS_PROXY+1])"
	exit 1
fi
