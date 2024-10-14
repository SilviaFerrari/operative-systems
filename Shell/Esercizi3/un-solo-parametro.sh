#!/bin/sh
if test $# -ne 1
then echo è richiesto un solo parametro
     exit 1
fi

case $1 in
/*) echo Nome assoluto;;
*/*) echo Nome relativo;;
*) echo Nome relativo semplice;;
esac
