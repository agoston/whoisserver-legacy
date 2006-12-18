#!/bin/sh

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
    $LOADER -L $1 -p ${PROPERTIES} -s ${SOURCE} ${object_file}
    check_error "Loader failure" $?
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
echo "Creating tables\n"

cd $SCRIPTDIR/SQL; sh ./create.first-stage; cd ..

# 2 - pass loading 

if [ $NPASSES -eq 2 ]
then 	
  echo `date`"\n"
  echo "Loading tables - 1 of 2 pass\n"

  loadpass 1 
  check_error "Pass I failed" $?
fi

echo `date`"\n" 
echo "Making indexes\n"

cd $SCRIPTDIR/SQL; ./create.second-stage; cd ..



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



echo `date`"\n" 
echo "Making more indexes\n"

cd $SCRIPTDIR/SQL; ./create.third-stage; cd ..

echo "Finished "`date`

