#!/bin/sh
if test $# -ne 1
then echo è richiesto un solo parametro
     exit 1
fi

case $1 in
/*) echo Nome assoluto
    if test -d $1 -a -x $1
    then echo Questa è una cartella traversabile.
    elif test -f $1
    then echo Questo è un file esistente.
    else echo Non è nè un file, nè una directory esistente.
    fi;;
*/*) echo Nome relativo;;
*) echo Nome relativo semplice;;
esac
