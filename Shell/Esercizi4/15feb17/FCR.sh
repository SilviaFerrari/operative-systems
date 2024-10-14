#!/bin/sh
#FCR.sh
#file comandi ricorsivo

cd $1 #accedo alla directory passata come parametro

#numero righe file
rf=

#numero righe file con un carattere numerico
rfn=

#nome del file
filename=

for i in *
do
	if test -f $i -a -r $i
	then
		rf=`wc -l < $i` #contro le righe del file
		rfn=`grep '[0-9]' $i | wc -l`  #conto le righe del file che contengono numeri
		if rf -eq $2 -a rfn -eq rf
		then 
			filename="$filename $i"
			bool=1
		fi
	fi 
done

if test "$filename"
then 
	La directory `pwd` contiene i files: $filename.
	echo Devo chiamare la parte C?
	read answ

	case $answ in 
	Yes|yes|Si|si) 15feb17 $filename $2;;
	*);;
        esac	
fi

for i in *
do
	if -d $1 -a -x $1
        then
                $0 `pwd`/$i $2
        fi
done
