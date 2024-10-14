#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1  #accedo alla gerarchia passata come primo parametro

find=false #variabile per sapere se abbiamo trovato almeno un file che termina con $S

for F in *
do
	if test -f $F
	then
		case $F in
                *.$S) echo DEBUG: file trovato!
                      find=true;;
                *);;
                esac
	fi
done

if test find = true
then
	echo DEBUG: directory trovata!
	echo `pwd` >> $3  #aggiungo il percroso assoluto al file temporaneo
fi

#cerco ed esloro eventuali sotto-directory
for i in *
do
	if test -d $i -a -x $i  #controllo che sia una directory traversabile
	then
		#chiamo il file ricorsivo
		$0 `pwd`/$i $2 $3
	fi
done
