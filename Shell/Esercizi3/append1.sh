#!/bin/sh

if $# -ne 2
then echo Error 1: bisogna inserire esattamente 2 parametri.
     exit 1
fi

#controllo del nome relativo semplice di $1 e $2
for var in $*
do
	case $var in
        /*) echo Error 2: $var è un nome assoluto
            exit 2;;
        */*) echo Error 3: $var è un nome relativo
             exit 3;;
	*) echo Giusto, $var è un nome relativo semplice;;
        esac
done

#controllo che il file esista e sia leggibile
if test ! -f $1 -o ! -r $1
then echo Error 4: Il file non esiste o non è leggibile
     exit 4
fi

#controllo che il file esista e sia scrivibile
if test -f $2 -a ! -w $2
then echo Error 5: Il file esiste ma non è scrivibile
     exit 5
elif test ! -f $2 -a ! -w pwd
then echo Error 6: Il file non esiste e la cartella corrente non è scrivibile
     exit 6
fi

#unisco i contenuti dei file e li mostro
cat $1 >> $2
cat $2
#questa è una modifica
