#!/bin/bash

. $DATADIR/nrtm/filtering/functions.sh

# check the access rights
ACCESS=`get_nrtm '-q sources' | grep DB-TEST | cut -d: -f3`
if [ $ACCESS != X ]; then
	echo "Error: '-q sources' returned access right '$ACCESS' instead of 'X'!"
	exit 1
fi

# check if the number of objects is right
SOURCES=`get_nrtm '-q sources' | grep DB-TEST | cut -d: -f4`

if [ $SOURCES != '1-5' ]; then
	echo "Error: '-q sources' returned interval '$SOURCES' instead of '1-5'!"
	exit 1
fi

# check if the objects look like they should
# if you changed ANY of the DUMMY_* options in the rip.config, this part will fail
FILE1=$DATADIR/nrtm/filtering/001/expected_result
FILE2=$DATADIR/nrtm/filtering/001/nrtm_result

get_nrtm '-g DB-TEST:0:1-5' >>$FILE2

diff $FILE1 $FILE2 &>/dev/null
RET=$?
rm -f $FILE2

if [ $RET -gt 0 ]; then
	echo 'Error: the objects returned by nrtm do not look like they should! Check expected_result and nrtm_check.sh!'
	exit 1
fi
