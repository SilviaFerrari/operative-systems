#!/bin/sh
#FCP.sh
#file comandi principale

#controllo del numero di variabili
case $# in
0|1|2) echo ERRORE: $# parametri insufficienti, almeno 3 richiesti.
       exit 1;;
*);;
esac

#controllo del primo parametro che deve essere un singolo carattere
case $1 in
?);;
*) echo ERRORE: bisogna inserire un singolo carattere.
   exit 2;;
esac

#salvo il primo parametro nella variabile C e poi lo elimino
C=$1
shift

for i in $*
do
	case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
		    echo ERRORE: $i deve essere una directory traversabile.
		    exit 3
            fi;;
	*) echo ERRORE: $i deve essere un percorso assoluto.
           exit 4;;
	esac
done

#definsco ed esporto la variabile PATH
PATH=`pwd`:$PATH
export PATH

#creo o azzero il file temporaneo con la ridirezione a vuoto
> /tmp/conta$$

#eseguo l'esplorazione delle gerarchie
for G in $*
do
	#chiamo il file ricorsivo
	echo DEBUG: espoloro $G...
	FCR.sh $G $C /tmp/conta$$ 
	echo "DEBUG: fine esplorazione.\n"
done

#stampo il numero globale di directory trovate col nome richiesto
echo Il numero totale di directory che soddisfano le specifiche è: `wc -l < /tmp/conta$$`

for i in `cat /tmp/conta$$`
do
	echo "\nNome assoluto della directory: $i"
	echo Elena vuoi visualizzare tutti gli elementi della directory?
	read answ

	case $answ in
	s*|S*|y*|Y*) ls -a $i;;
	*) echo Ok niente.;;
        esac
done

#finito lo script elimino il file temporaneo
rm /tmp/conta$$
