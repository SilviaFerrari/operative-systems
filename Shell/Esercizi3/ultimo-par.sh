#!/bin/sh

#controllo che ci sia almeno un parametro
case $# in
0) echo ERRORE : parametri insufficienti.
   exit 1;;
*) echo Sono stati passati $# parametri.;;
esac

#stampiamo scorriamo tutti i parametri per trovare l'ultimo
count=0
for i
do
	count=`expr $count + 1`
	if test $count -eq $#
	then 
		echo Ultimo parametro: $i.
	else
		LIST="$LIST $i"
	fi
done
 
#stampo il resto dei paramentri
echo Altri parametri: $LIST
