#!/bin/sh

#controllo i parametri passati
if test $# -eq 0
then echo Error 1: Bisogna inserire almeno un parametro
     exit 1
fi

#stampo su terminale i paramentri in forma aggregata
echo Stampo i paramentri in forma aggregata: $*

#stampo i paramentri singolarmente
echo Singoli parametri:
for var in $*
do
	echo $var
done
