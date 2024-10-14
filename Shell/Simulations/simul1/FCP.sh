#!/bin/sh
#file comandi 

#controllo lasco del numero di parametri
case $# in
0|1|2) echo ERRORE: parametri insufficienti: N+1 con N>=2
   exit 1;;
*);;
esac

#controllo che il primo parametro sia un nome relatico semplice
case $1 in
*/*) echo ERRORE: il primo parametro deve essere un nome relativo semplice
    exit 2;;
*);;
esac

#salvo il primo parametro in una variabile e la elimino dalla lista
v1=$1
shift

#controllo che gli altri parametri siano nomi assoluti
for i in $* #potremmo anche solo scrivere for i e basta
do
	case $i in
        /*)if test ! -d $i-o ! -x $i
	   then echo ERRORE: $i parametro non valido, deve essere un percorso assoluto.
	        exit 3
	   fi;;
        *);;
        esac 	
done

#controlli finiti, settiamo il PATH e lo esportiamo
PATH=`pwd`:$PATH
exoprt PATH

#azzeriamo il file temporaneo e lo creiamo se non esiste. QUI SOLO 1 FILE TMP
> /tmp/tmpfile$$

#invocazione del file comando ricorsivo
for i in $*
do
	echo Fase per $i
	FCR.sh $i $v1 /tmp/tmpfile$$
done

#terminare tutte le N fasi contiamo le linee del file tmpfile$$ 
echo il nuermo dei file totali che soddisfano la specifica è: `wc -l < /tmp/tmpfile$$`

for i in /tmp/tmpfile$$
do
	#stampiamo i nomi assoluti dei file trovati
	echo Creato il file $i
	echo La sua prima linea:
	head -1 $i
	echo La sua ultima linea:
	tail -1 $1
done

