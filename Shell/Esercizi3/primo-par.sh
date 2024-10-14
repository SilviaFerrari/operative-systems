#!/bin/sh

#controllo che ci sia almeno un parametro
case $# in
0) echo ERRORE : parametri insufficienti.
   exit 1;;
*) echo Sono stati passati $# parametri.;;
esac

#stampiamo il primo parametro e poi lo eliminiamo
echo Primo parametro: $1.
shift 
#stampo il resto dei paramentri se ci sono
echo Altri parametri: $*.
