#!/bin/sh
case $# in
0) echo Bisogna inserire almeno un paramentro.
   exit 1;;
1)# read filename --> questo sarebbe andato bene per prendere il contenuto di un file esistente
  # cat $filename >> $1 --> per poi aggiungerlo a quello di un altro
  cat >> $1
  cat $1;;
2) cat < $1 >> $2
   cat $2;;
*) echo Troppi paramentri.
   exit 2;;
esac
