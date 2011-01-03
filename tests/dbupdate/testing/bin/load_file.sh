#!/bin/sh

# execute debug (set by caller to 'set -x' if debug output required)
$DEBUG

check_error()
{
	if [ $2 -ne 0 ] 
	then
	echo "*** ERROR: $1 [$2]. Exiting\n"
	exit 1
fi

}

loadpass()
{
	    
  for object_file in ${LIST} 
  do
    echo `date` Loading ${object_file} 
    echo Loading by $LOADER -L $1 -p ${PROPERTIES} -s ${SOURCE} ${object_file}
    test -f ${object_file}
    check_error "${object_file} not found" $?
    
    # make a copy of the to-be-loaded file and attach the standard objects to it
    cp -f $object_file ${object_file}.tmp
    cat $CONFDIR/loader >>${object_file}.tmp
    $LOADER -L $1 -p ${PROPERTIES} -s ${SOURCE} ${object_file}.tmp
    check_error "Loader failure" $?
    rm ${object_file}.tmp
  done
}

# switch on debugging (set -x)
$DEBUG

# number of passes to load the db
# 0  means do not load data (empty_db)
# 1 means single pass, 2 means 2-pass loading
NPASSES=$1
NPASSES=${NPASSES:?"Usage $0 npasses"}
echo "*******************"`date`"******************" 
echo "Creating tables\n"

cd $SCRIPTDIR/SQL
# at the moment, we need the dummy object for nrtm v2 emulation.
# the whole dummy object caching can be removed completely as we
# deprecate nrtm v2 and support nrtm v3 only - agoston, 2010-09-27
cat main.create.sql main.data.sql main.dummy.sql| $MYSQL -h$HOST -P$PORT -u$USER -p$PASSWORD $DB
cd ..

# 2 - pass loading 

if [ $NPASSES -eq 2 ]
then 	
  echo `date`"\n"
  echo "Loading tables - 1 of 2 pass\n"

  loadpass 1 
  check_error "Pass I failed" $?
fi

if [ $NPASSES -eq 2 ]
then
  echo `date`"\n"
  echo "Loading tables - 2 of 2 pass\n"

  loadpass 2
  check_error "Pass I failed" $?
elif [ $NPASSES -eq 1 ]
then
  echo `date`"\n"
  echo "Loading tables - 1 of 1 pass\n"

# 0 indicates single pass loading 
  loadpass 0
  check_error "Pass I failed" $?
fi

echo "Finished "`date`

