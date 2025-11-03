#!/bin/bash
if [ $# != "1" ]
then
   echo usage: grappe.grp
   exit
fi
grappe=$1
LOCAL=$( basename `pwd` )
DIR=$BIGLDIST
warning=0
echo target $DIR
while read plage log max mode
do
  echo $plage
  if [ ${plage:0:1} != "#" ]
  then
    succes=0
    host=${plage%%-*}
    last=${plage##*-}
    echo "$host $log $max "
    if ping -c1 -w2 $host >/dev/null
     then
         rsync  -av  *.c            --delete $log@$host:$DIR/
         rsync  -av  *.h            --delete $log@$host:$DIR/ 
         rsync  -av  scripts        --delete $log@$host:$DIR 
	 rsync  -av  Makefile       --delete $log@$host:$DIR/ 
	 ssh    $log@$host "make -C $DIR"  < /dev/null
         succes=1
     fi
   if [ $succes == "0" ]
     then
         echo net $plage is absent
	 echo $plage $log $max $mode >> update
         warning=1
    fi
  fi
done < $grappe
if [ $warning == "1"  ]
then
  echo  WARNING SOME HOSTS WERE UNREACHABLE
  echo "LATER touch hosts.grp; make"
fi
