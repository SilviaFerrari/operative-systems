#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1 #accedo alla cartella passata come parametro

#definisco la variabile per il numero delle righe del file
rft=

#definisco la variabile per sapere se ho trovato almeno un file
find=false

for i in *
do
	if test -f $i -a -r $i
	then
		#conto le linee che finiscono con t
		rnt=`grep 't$' $i| wc -l`
		#controllo se tali linee sono almeno X
		if test $rnt -eq $2
		then
			#aggiungo il file al file temporaneo
			echo `pwd`/$i >> /tmp/conta$$
		fi
	fi
done

#controllo se ci sono delle altre cartelle da esplorare
for i in *
do
	if test -d $i -a -x $i
	then
		$0 `pwd`/$i $2 $3
	fi
done
