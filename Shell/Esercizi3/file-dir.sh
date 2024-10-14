#!/bin/sh
#file comandi file-dir.sh

#non c'è bisogno di fare un controllo di parametri
#mi sposto nella directory passata come parametro
cd $1
countfile=0
countdir=0
for i in *
do
	if test -f $i
	then
		echo [F] `pwd`/$i "\n"
		countfile=`expr $countfile + 1`
	elif test -d $1
	then
		echo [D] `pwd`/$i "\n"
		countdir=`expr $countdir + 1`
	fi
done

#controllo che la directory non sia vuota
if test $countfile -eq 0 -a $countdir -eq 0
then
	echo La directory corrente è vuota!
	exit 5
fi

#stampo quanti file e quante directory ho trovato
echo Numero totale di file: $countfile.
echo Numero totale di directory: $countdir.
