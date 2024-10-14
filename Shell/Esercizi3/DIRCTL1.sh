#!/bin/sh

echo Sono DIRCTL1.sh
if test $# -ne 1
then echo Errore: Serve inserire un solo parametro.
     exit 1
fi

echo 'Il valore di $0 ===>' $0
echo 'Il valore di $1 ===>' $1

if test -f $1
then echo $1 è un file.
     echo 'DEBUG - Eseguo ls -l $1'
     ls -l $1
else 
	if test -d $1 
	then echo $1 è una cartella
	     echo 'DEBUG - Eseguo ls -ld $1'
	     ls -ld $1
	fi
fi
