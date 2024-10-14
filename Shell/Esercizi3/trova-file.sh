#!/bin/sh
#controllo del numero dei paramentri
if test $# -ne 1
then echo Error 1: Numero di parametri sbagliato.
     exit 1
fi
#controllo del nome relativo semplice
case $1 in
/*) echo Error 2: nome assoluto
    exit 2;;
*/*) echo Error 2: nome relativo
     exit 2;;
esac
#controllo dell'esistenza del file
if test -f $1
then echo Il percorso assoluto del file $1 è: 
     readlink -f $1
     echo In alternativa posso usare il comando
     `pwd`/$1
else echo Error 3: Il file passato non esiste.
     exit 3
fi
