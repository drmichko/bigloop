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

nproc=$max
max=$( qstat | grep -c $CLIENT )

echo $max are running
let  nproc-=$max
echo starting $nproc processus
while [ $nproc -gt 0 ]
do
  qsub -cwd -b y  ./$CLIENT 
  echo "[ $nproc ] "
  let  nproc--
done

