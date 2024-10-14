#!/bin/sh

echo Sono DIRCTL.sh

#cotrollo il numero di parametri passati, se meno di 1 restituisco errore
if test $# -ne 1
then echo Errore: Serve inserire un solo parametro.
     exit 1
fi

echo 'Il valore di $0 ===>' $0
echo 'Il valore di $1 ===>' $1
echo "DEBUG-Ora eseguo ls -l $1" 
ls -l $1
