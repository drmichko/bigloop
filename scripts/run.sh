#!/bin/bash
dir=$1
max=$2
cd  $1
if [ -f bigloop.conf ]
then
  source bigloop.conf
else
  echo no bigloop.conf
  exit 1
fi

actif=$( ps ux | grep -c $CLIENT  )
let max-=$actif

nproc=$( grep -c processor /proc/cpuinfo )
echo $nproc cpu detected

avg=$( cat /proc/loadavg | cut -d ' ' -f3 )
av2=$( cat /proc/loadavg | cut -d ' ' -f2 )
av1=$( cat /proc/loadavg | cut -d ' ' -f1 )
echo charge $av1 $av2 $avg

if test $(echo "$av1 > $avg" | bc -l) == 1 ; then
        avg=$av1
fi
if test $(echo "$av2 > $avg" | bc -l) == 1 ; then
        avg=$av2
fi
echo $avg
nproc=$(echo "scale=0;(0.5+$nproc-$avg)/1;" | bc -l)

if [ $nproc -lt $max ]
then
  max=$nproc
fi
echo starting $max processus
while [ $max -gt 0 ]
do
  nohup nice ./$CLIENT 2>/dev/null 1>/dev/null 0</dev/null &
  echo " [ $max ] client started"
  let  max--
done

