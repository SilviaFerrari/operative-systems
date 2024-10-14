#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1  #accedo alla directory passata come parametro

nc= #variabile in cui salvo il numero di caratteri del file

#for i in *
#do
#	case $i in
#	$2.txt) echo File trovato!;;
#        esac
#done

if test -f $2.txt -a -r $2.txt -a -w $2.txt
then
       nc=`wc -c < $2.txt`  #conto tutti i caratteri:
       echo $nc `pwd`/$2.txt >> $3  #salvo il numero di caratteri e il percorso assoluto
fi


#cerco ed esploro eventuali sotto-directory
for i in *
do
	if test -d $i -a -x $i
	then
		#invoco ancora il file ricorsivo
		$0 `pwd`/$i $2 $3
	fi
done
