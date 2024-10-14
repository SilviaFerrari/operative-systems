#!/bin/sh

echo Sono DIRCTL1.sh
case $# in
0) echo Non sono stati passati parametri
   echo 'Eseguo ls -l paginato correttamente'
   ls -l | more
   exit 0;;
1) echo Passato un parametro $1;;
*) echo Errore: Serve inserire un solo paramentro.
   exit 1;;
esac 

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
