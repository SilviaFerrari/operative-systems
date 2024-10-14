#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1  #accedo alla directory passata come parametro

pos=0  #variabile usata per tenere tracca della posizione dei file della directory

#scorro il contenuto della directory corrente
for K in *
do
	if test -f $K  #controllo se l'elemento corrente è un file
	then
		pos=`expr $pos + 1`  #se è un file incremento la posizione, altrimento rimane invariata
	fi

	if test $pos -eq 3  #controllo se il file corrente occupa la terza posizione
	then
		#abbiamo trovato il file in terza posizione
		echo DEBUG: file trovato $K.
		echo `pwd`/$K >> $2  #aggiungo il nome assoluto del file trovato al file temporaneo
	fi
done

#controllo se nella directory corrente abbiamo trovato un file in terza posizione
if test $pos -lt 3
then
	#non abbiamo trovato un file in terza posizione
	echo "\n$1"  #stampo il nome assoluto di tale directory su terminale
	echo "Nella directory sopra specificata non è stato trovato nessun file in terza posizione.\n"
fi

#continuo l'esplorazione di eventuali sotto directory
for i in *
do
	if test -d $i -a -x $i  #controllo se l'elemento corrente è una directory traversabile
	then
		#chiamo di nuovo il file FCR.sh
		$0 `pwd`/$i $2
	fi
done
