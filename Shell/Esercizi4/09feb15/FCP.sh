#!/bin/sh
#FCP.sh
#file comandi principale

#contrllo del numero dei parametri
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

#invoco il file comandi ricorsivo
FCR.sh $*

#creo o azzero il file temporaneo
> /tmp/conta$$

#invocazione del file temporaneo
FCR.sh $1 $2 /tmp/conta$$

#variabile per memorizzare i numeri e i file rischiesti
params=

for i in `cat /tmp/conta$$`
do
	echo Insersci un numero strettamente positivo e maggiore di $2 per il file $1:
	read answ

	case $answ in
	*[!0-9]*) echo ERRORE: deve essere un numero strettamente positivo      
                  exit 4;;
	esac

	if test $answ -eq 0
	then
        	echo ERRORE: non può essere uguale a 0
        	exit 5
	fi
	if test $answ -gt $2
	then echo ERRORE: deve essere minore di $2
	     rm /tmp/conta$$
	     exit 6
	fi

	params="$params $answ"
done

#rimuovo il file temporaneo perchè non mi serve più
rm /tmp/conta$$

#invoco il file C
09feb15 $params
