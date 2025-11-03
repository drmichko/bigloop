#!/bin/bash
echo making pictures

awk -F: '{print $1,$3}' log/info-$1.log | sort -g > data-$1.txt


echo "set terminal png giant size 1600,1200;" \
     "set title  'time big loop report';"\
     "set output 'img/data-$1.png';"  \
     "set xlabel 'integer'; "     \
     "set ylabel 'cpu time'; "    \
     "plot 'data-$1.txt' title 'runing time' with line;" \
 | gnuplot

awk -F: '{print $1,$3}' log/info-$1.log | sort -g > wkf-$1.txt


echo "set terminal png giant size 1600,1200;" \
     "set title  'wkf big loop report';"      \
     "set output 'img/wkf-$1.png';"  \
     "set xlabel 'integer'; "     \
     "set ylabel 'cpu time'; "    \
     "plot 'wkf-$1.txt'  using 1:(log(\$2)) title 'work factor' with line;" \
 | gnuplot

rm wkf-$1.txt
rm data-$1.txt
