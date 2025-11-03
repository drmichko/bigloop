#!/bin/bash
COMMANDE=uptime
GRAPPE=grappe.grp
LOCAL=$( basename `pwd` )
DIR=$BIGLDIST/$LOCAL

function hint
{
 echo usage of bigloop-0.2
 echo "-c commande: default uptime"
 echo "-d         : switch directory ."
 echo "-g grappe  : default grappe.grp"
 exit 1
}

while getopts ":c:dg:h" opt; do
  case $opt in
    c) COMMANDE=$OPTARG
       ;;
    d) DIR="."
       ;;
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
count=0
echo target $DIR
rm -f ko.grp
touch ko.grp
while read plage log max mode
do
  echo $plage $log $max $mode
  if [ ${plage:0:1} != "#" ]
  then
    ipno=${plage%-*}
    last=${plage#*-}
    first=${ipno##*.}
    pref=${ipno%.*}
    if [ $last == $plage ]
    then
          last=$first
    fi
    #echo $first $last
    while [ $first -le  $last ]
      do
      host=$pref.$first
      if ping -c1 -w2 $host >/dev/null
      then
         let  count++
         ssh  $log@$host "cd $DIR;$COMMANDE"  < /dev/null
      else
         echo $host is ko
	 echo $host >> ko.grp
         let warning++
     fi
     let first++;
   done
  fi
done < $GRAPPE
if [ $warning -gt 0  ]
then
  echo $warning  unreachable hosts
  cat ko.grp
fi

echo $count actions
