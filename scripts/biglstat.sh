#!/bin/bash
CLIENT=./bigl.exe
GRAPPE=grappe.grp
LOCAL=$( basename `pwd` )
DIR=$BIGLDIST/$LOCAL

function hint
{
 echo usage of bigloop-0.2
 echo "-c client : default bigl.exe"
 echo "-g grappe : default grappe.grp"
 exit 1
}

while getopts ":c:g:hjs" opt; do
  case $opt in
    c) CLIENT=$OPTARG
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
qte=0
echo target $DIR
rm -f ko.grp
touch ko.grp

echo $GRAPPE for status
while read plage log max mode
do  
  echo $plage
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
    while [ $first -le  $last ]
      do
    	host=$pref.$first
    	
    	if ping -c1 -w2 $host >/dev/null
     	  then
          ##echo "status on $host..."  
               case $mode in
                  sge)ssh  $log@$host qstat < /dev/null
                      ;;
                  std)ssh  $log@$host "ps aux | grep $CLIENT | grep -v grep" < /dev/null
                      ;;
                    *) echo $mode unknown mode 
               esac
           let qte++
         else
         echo $host is ko
	 echo $host >> ko.grp
         let warning++
        fi
      let first++
      done
  fi
done < $GRAPPE

if [ $warning -gt 0  ]
then
  echo $warning unreachable hosts
  cat ko.grp
fi
echo 
echo $qte hosts
