#!/bin/sh
#FCR.sh
#file comandi ricorsivo

ln=  #variabile in cui salvo il numero di linee del file

for F in *
do
	if test -f $F -a -r $F
	then
		ln=`wc -l < $F`  #conto le linee del file
		if test $ln -eq $2
		then
			echo `pwd`/$F >> $3  #salvo il nome nel file temporaneo
		fi
	fi
done

for i in *
do
	if test -d $i -a -x $i
	then
		#chiamo il file ricorsivo
		$0 `pwd`/$i $2 $3
	fi
done
