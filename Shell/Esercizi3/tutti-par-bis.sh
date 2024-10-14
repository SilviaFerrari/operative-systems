#!/bin/sh

#controllo i parametri passati
if test $# -eq 0
then echo Error 1: Bisogna inserire almeno un parametro
     exit 1
fi

#stampo su terminale i paramentri in forma aggregata
echo $*

#stampo i paramentri singolarmente
i=1
for var in $*
do
	echo Variabile numero $i: $var
	i=$((i+1))
done
