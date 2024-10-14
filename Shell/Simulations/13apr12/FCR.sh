#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1  #accedo alla directory passata come primo parametro
echo DEBUG: esploro la cartella $1.

#le rinomino per non impazzire
cont=$2
step=$3

currlv=`expr $cont + 1`  #calcolo il livello corrrente e lo salvo nella variabile currlv
echo DEBUG: livello corrente: $currlv.

#fase A
if test $step -eq 1  #controllo se sono al primo step
then
	#scorro ciascun elemento della directory
	for i in *
	do
		#controllo se l'elemento corrente è una directory traversabile
		if test -d $i -a -x $i
		then
			#chiamo il file ricorsivo
			$0 `pwd`/$i $currlv $step $4
		fi
	done

	#leggo il valore nel file temporaneo
	read previous < $4
	
	#se il valore di ritorno è maggiore del valore precedente aggiorno il numero totale di livelli
	if test $returnValue -gt $previous
	then
		echo $returnValue > $4
	fi
fi

fase B
if test $step -eq 2  #controllo se sono al secondo step
then
	if test
fi
