#!/bin/sh
#FCP.sh
#file comandi principale

case $# in
2);;
*) echo ERRORE: devono essere inseriti 2 parametri [directory] [numero]
   exit 1;;
esac

#controllo del primo parametro
case $1 in 
/*) if test ! -d $1 -o ! -x $1
    then
          echo ERRORE: deve essere una directory attraversabile;;
	  exit 2
    fi;;
*) echo ERRORE: deve essere un percorso assoluto
   exit 3;;
esac

#controllo del secondo parametro
case $2 in
*[!0-9]*) echo ERRORE: deve essere un numero strettamente positivo	
	  exit 4;;
esac

if test $2 -eq 0
then 
	echo ERRORE: non può essere uguale a 0
	exit 5
fi

#esporto la variabile PATH
PATH=`pwd`:$PATH
export $PATH

#invocazione del file temporaneo
FCR.sh $*
