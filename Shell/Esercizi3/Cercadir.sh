#!/bin/sh
#file comandi Cercadir.sh
#ricerca in breadth-first

#ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
cd $1

#verifico se la cartella corrente è quella che stiamo cercando
case $1 in
*/$2) echo La directory $2 si trova in `pwd`/$1;;  #trovata directory
esac

#passiamo ora alla parte ricorsiva
for i in *
do
	if test -d $i -a -x $i 
	then
		#stampa di debugging
		echo "\nDEBUG-Stiamo per esplorare la directory" `pwd`/$i
		#invocazione ricorsiva: ATTENZIONE NOME ASSOLUTO dir!!!
		Cercadir.sh `pwd`/$i $2
	fi
done
