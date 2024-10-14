#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1  #accedo alla directory passata come parametro

ln=  #variabile per salvare il numero delle linee di un file

#cerco tutti i file leggibili
for f in *
do
	#controllo se è un file leggibile
	if test -f $f -a -r $f
	then 
		ln=`wc -l < $f`  #leggo il numero di linee
		if test $ln -ge $2  #controllo se verifica la specifica
		then
			if test $ln -ge 5  #controllo se il file ha almeno 5 linee
			then
				echo DEBUG FCR: linea numero 5 del file $f: `head -5 $f | tail -1`
				#salvo la quinta riga nel file appena creato
				echo `head -5 $f | tail -1` > $f.quinta
				echo `pwd`/$f.quinta >> $3
			elif test $ln -lt 5
			then
				#creo il file vuoto
				> $f.NOquinta
				echo `pwd`/$f.NOquinta >> $3
			fi
		fi
	fi
done

#finito di controllare i file, passo alle sotto-directory
for i in *
do
	if test -d $i -a -x $i
	then
		#chiamo il file ricorsivo
		$0 `pwd`/$i $2 $3
	fi
done
