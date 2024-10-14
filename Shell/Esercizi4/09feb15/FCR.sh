#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1 #accedo alla directory passata

#variabile per il numero righe del file
nr=

#variabile per vedere se abbiamo o meno trovato i file richiesti
find=false 

for i in *
do
	if test -f $i -a -r $i
	then
		nr=`wc -l < $i`
		if test nr -eq $2
		then
		       echo `pwd`/$i >> /tmp/conta$$  #aggiungiamo il file trovato alla lista
	               find=true	       
		fi
	fi
done

if test $find = true
then
	echo Trovata la directory `pwd` 
fi

for i in *
do
	if test -d $i -a -x $i
	then
		$0 `pwd`/$i $2 $3
	fi
done
