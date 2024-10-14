#!/bin/sh
for i in * #funziona per tutti i file/directory per la cartella corrente
do 
	echo ELEMENTO CORRENTE $i
	if test -f $i
	then echo Questo elemento è un file di contenuto:
	     cat < $i
             echo ======================
        elif test -d $i
	then echo Questo elemento è una directory.
	fi	
done
#ciao questa è una modifica
