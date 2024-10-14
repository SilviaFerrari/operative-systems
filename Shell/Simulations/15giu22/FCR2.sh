#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1  #accedo alla directory passata come primo parametro
empty=  #variabile in cui salviamo il numero di righe del file corrente per vedere se è vuoto o meno
line=  #variabile per sapere se il file contiene solo caratteri minuscoli

for F in *
do
	if test -f $F
	then
		empty=`wc -l < $F`

		if test $empty -ne 0
		then
			case `cat $F` in
	                [!a-z]) echo FILE NON VALIDO;;
        	        *) echo FILE TROVATO: `pwd`/$F
                           echo `pwd`/$F >> $2;;
                	esac
		fi
	fi
done

#cerco eventuali sotto directory da esplorare
for G in *
do
	if test -d $G -a -x $G
	then
		#richiamo il file ricorsivo
		$0 `pwd`/$1 $2
	fi
done
