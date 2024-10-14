#!/bin/sh
#FCP.sh
#file comandi principale

#controllo lasco del numero di parametri
case $# in
0|1|2) echo ERRORE: deveono essere inseriti 3 o più parametri.
       ;;
*);;
esac

#controllo che tutti i parametri inseriti siano nomi assoluti di directory traversabili
for i in $*
do
	case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
		    echo ERRORE: $i deve essere una directory traversabile.
		    exit 2
	    fi;;
	*) echo ERRORE: $i deve essere un percorso assoluto.
	   exit 3;;
	esac
done

#definisco ed esporto la variabile PATH
PATH=`pwd`:$PATH
export PATH

#creo/azzero il file temporaneo
> /tmp/nomiAssoluti$$

#esploro le gerarchie tramite il file ricorsivo
for G in $*
do
	#chiamo il file comandi ricorsivo
	FCR.sh $G /tmp/nomiAssoluti$$
done

#conto le righe del file temporaneo e salvo il numero nella variabile N
N=`wc -l < /tmp/nomiAssoluti$$`

echo Il numero globale di file che soddisfano le specifiche è: $N.

#finito lo script elimino il file temporaneo
#rm /tmp/nomiAssoluti$$
