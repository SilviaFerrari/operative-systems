#!/bin/sh
#file comandi Cercafile.sh
#ricerca in breadth-first

#ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
cd $1

#verifichiamo se esiste un file con il nome fornito dall'utente
if test -f $2
then
echo Il file $2 si trova in `pwd` 	#trovato file
fi

#passiamo ora alla parte ricorsiva
for i in *
do
if test -d $i -a -x $i 
then
	#stampa di debugging
	echo "\nDEBUG-Stiamo per esplorare la directory" `pwd`/$i
	#invocazione ricorsiva: ATTENZIONE NOME ASSOLUTO dir!!!
	Cercafile.sh `pwd`/$i $2
fi
done
