#!/bin/bash
GRAPPE=grappe.grp
LOCAL=$( basename `pwd` )
DIR=$BIGLDIST/$LOCAL

function hint
{
 echo usage of bigloop-0.2
 echo "-g grappe  : default grappe.grp"
 exit 1
}

while getopts "g:h" opt; do
  case $opt in
    g) GRAPPE=$OPTARG
       ;;
    h) hint;
       ;;
    \?)
      echo "Invalid option: -$OPTARG" 
      hint;
      ;;
  esac
done
warning=0
echo target $DIR
rm -f ko.grp
touch ko.grp
while read plage log max mode
do
  echo $plage $log $max $mode
  if [ ${plage:0:1} != "#" ]
  then
    succes=0
    ipno=${plage%-*}
    last=${plage#*-}
    first=${ipno##*.}
    pref=${ipno%.*}
    if [ $last == $plage ]
    then
          last=$first
    fi
    while [ $first -le  $last ]
      do
    echo "$host $log $max "
    host=$pref.$first
    if ping -c1 -w2 $host > /dev/null
     then
         ssh    $log@$host "mkdir -p $DIR/data"  < /dev/null
         rsync  -av  makefile      $log@$host:$DIR
         rsync  -av  bigloop.conf  $log@$host:$DIR
         #rsync  -av  *.sh          $log@$host:$DIR
         rsync  -av  src  --delete $log@$host:$DIR
	 ssh    $log@$host "make -C $DIR BIGLROOT=$BIGLDIST"  < /dev/null
         succes=1
         break;
     fi
    let first++;
    done 
    if [ $succes == "0" ]
     then
         echo $plage is ko
	 echo $plage $log $max $mode >> ko.grp
         warning=1
    fi
  fi
done < $GRAPPE
if [ $warning == "1"  ]
then
  echo "ko are:"
  cat ko.grp
fi
