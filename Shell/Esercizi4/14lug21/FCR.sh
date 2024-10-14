#!/bin/sh
#FCR.sh
#file comandi ricorsivo che esplora la gerarchia passatagli

cd $1  #accedo alla gerarchia passata come primo argomento

#variabile per contare i file che soddisfano la richiesta
cont=0

#variabile per memorizzare i file che esploriamo
files=

#scorro tutti gli elementi per cercare i file
for F in *
do
	if test -f $F
	then
	       #calcolo il numero di linee del file
	       nl=`wc -l < $F`

	       #guardo la lunghezza del nome del file
	       case $F in
	       ??) if test $nl -eq $3   #?? indicano 2 caratteri qualsiasi
	           then
			  cont=`expr $cont + 1`
                          files="$files $F"
		   fi;;
	       *) #echo DEBUG: il nome del file è troppo lungo o troppo corto;;
               esac
	fi
done

#controllo se la directory corrente soddisfa i criteri per essere stampata in output
if test $cont -lt $2 -a $cont -ge 2
then
	echo "Questa directory soddisfa i criteri richiesti: `pwd`\n"
	#invoco la parte C
	#14lug21 $files $3
fi

#scorro tutti gli elementi per cercare altre cartelle esplorabili
for i in *
do
	if test -d $i -a -x $i
	then
		#richiamo il file comandi ricorsivo
		echo Esploro la directory $i...
		$0 `pwd`/$i $2 $3
		echo "Esplorazizone di $i finita.\n"
	fi
done
