#!/bin/sh
for i in p* #funzione per tutti i file/directory per la cartella corrente
do 
	echo ELEMENTO CORRENTE $i

	#controllo se l'elemento corrente è un file o una directory
	if test -f $i 
	#se è un file lo stampo 
	then echo Questo elemento è un file di contenuto:
	     cat $i
	     #salvo il suo percorso assoluto sul file /tmp/t
	     echo "Percorso assoluto di $i:\n" `pwd`/$i "\n" >> /tmp/t
        elif test -d $i
	#se l'elemento è una directory mi limito a segnalarlo
	then echo "\n$i è una directory."
	fi	
	echo "\n======================\n"
done

