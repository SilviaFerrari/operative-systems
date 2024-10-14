#!/bin/sh
#FCP.sh
#file comandi principale

#controllo del numero di parametri
case $# in
1);;
*) echo ERRORE: bisogna inserire esattamente un parametro.
   exit 1;;
esac

#controllo che il parametro sia un nome di gerarchia
case $1 in
/*) if test ! -d $1 -o ! -x $1
    then
	    echo ERRORE: $1 non è una directory traversabile.
	    exit 2
    fi;;
*) echo ERRORE: $1 deve essere un percorso assoluto.
   exit 3;;
esac

#superato i controlli setto il PATH
PATH=`pwd`:$PATH
export PATH

#creo/azzero il file temporaneo
echo 0 > /tmp/levels$$

cont=0  #varaibile per contare e confrontare il numero di livelli
step=1  #indica a che fase del progetto siamo

#fase A: conteggio ricorsivo dei livelli della gerarchia
#chiamo il file ricorsivo
FCR.sh $1 $cont $step /tmp/levels$$

nl=`cat /tmp/levels$$`  #leggo il numero di livelli dal file temporaneo

echo Il numero di livelli della gerarchia è: $nl.
echo DEBUG: fine fase A. Inizio fase B.

#richiedo all'utente un valore numerico
val=0
while test $val -eq 0
do
	echo Inserire un valore numerico pari compreso fra 1 e $nl inclusi:
	read val

	#controllo se il valore inserito è valido
	case val in
	*[!0-9]*) echo ERRORE: bisogna inserire un intero positivo!
		  val=0;;
	*) if test $val -gt $nl
	   then 
		   echo ERRORE: inserire un valore <= $NL!
		   val=0
  	   fi;;
	esac
done

cont=0; step=2

#invocazione del file comandi ricorsivo
FCR.sh $1 $cont $step $val

#finito lo script elimito il file temporaneo
rm /tmp/levels$$
