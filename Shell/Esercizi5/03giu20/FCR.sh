#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1  #accedo alla gerarchia passata come primo parametro

case $1 in
*/$2?$2) echo DEBUG: directory trovata!
         echo `pwd` >> $3;;  #aggiungo il percorso assoluto al file temporaneo
esac


#esploro le sotto-directory
for i in *
do
	if test -d $i -a -x $i
	then
		#richiamo il file ricorsivo
		$0 `pwd`/$i $2 $3
	fi
done
