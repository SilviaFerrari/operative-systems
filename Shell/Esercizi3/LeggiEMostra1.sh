#!/bin/sh
 
#CONTROLLO SU PRESENZA DI ALMENO UN PARAMETRO FATTO IN MODO DIVERSO DA PRIMA
if test $# -lt 1
then 	echo Error 1: inserire almeno un parametro 
	exit 1
fi

for var in $*
do
	#controlliamo che i parametri siano un file e sia leggibile!
	if test ! -f $1 -o ! -r $1
  	then echo Errore: $1 non file oppure non leggibile
             exit 2
	fi
	
	#chiediamo all'utente se vuole visualizzare il contenuto del file: 
	#N.B. forziamo l'output sul terminale corrente per non avere problemi se si usasse 
	#la ridirezione dello standard output
	echo "vuoi visualizzare il file $1 (si/no)?" > /dev/tty
	
	#leggiamo la risposta dell'utente
	read risposta

	case $risposta in
	S* | s* | Y* | y*) echo info del file $1   
       			   ls -la $1
                           echo contenuto del file $1
                           cat $1;;
	*) echo Niente stampa di $var;;
	esac

done
