#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1  #accedo alla directory passata come parametro

#definisco la variabile per verificare la validità della directory
cond=true

#scorro tutti gli elementi e mi fermo se trovo una sotto-directory o un file con troppe righe
for i in *
do
	if test -d $i  #controllo se c'è una sotto-directory
	then
		cond=false

	elif test -f $i
	then	
		nl=`wc -l < $i`
		if test ! $nl -gt $2  #controllo le linee del file
		then
			cond=false
		fi
	fi
done

#verifico se la directory rispetta tutte le caratteristiche
if test $cond = true
then
	echo "La directory `pwd` soddisfa le specifiche!"
	echo `pwd` >> $3

#se non va bene vado avanti ad esplorare le sotto-directory	
elif test $cond = false
then
	for i in *
	do
		if test -d $i -a -x $i
		then
			#richiamo il file ricorsivo
			$0 `pwd`/$i $2 $3
		fi
	done
fi
