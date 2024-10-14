#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1  #accedo alla directory passata come primo parametro
empty=  #variabile in cui salviamo il numero di righe del file corrente per vedere se è vuoto o meno
line=  #variabile per sapere se il file contiene solo caratteri minuscoli

for F in *
do
	if test -f $F  #controllo che l'elemento corrente è un file 
	then 
		empty=`wc -c < $F`  #conto le linee del file per vedere se è vuoto o meno
		linelow=`grep -vc [[:lower:]] $F`  #conto le righe che contengono caratteri diversi da a-z
		lineblank=`grep -c [[:blank:]] $F`  #conto le righe che contengono spazi/tab
		line=`expr $linelow + $lineblank`  #sommo linelow e lineblank 		

		if test $empty -gt 0 -a $line -eq 0  #controllo se il file rispetta rispetta le specifiche
		then
			echo FILE TROVATO: `pwd`/$F  
			echo `pwd`/$F >> $2  #aggiungo il file F al file temporaneo passato come parametro
		fi
	fi
done

#cerco eventuali sotto directory da esplorare
for G in *
do
	if test -d $G -a -x $G  #controllo che l'elemento corrente sia una directory traversabile
	then
		#richiamo il file ricorsivo
		$0 `pwd`/$G $2
	fi
done
