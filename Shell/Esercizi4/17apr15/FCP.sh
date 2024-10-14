#!/bin/sh
#FCP.sh
#file comandi principale

#controllo del numero di parametri
case $# in
0|1|2) echo ERRORE: parametri insufficienti.
       exit 1;;
*);;
esac

#controllo sul primo parametro
case $1 in
*[!0-9]*) echo ERRORE: bisogna inserire un numero intero strettamente positivo.
          exit 2;;
*) if test $1 -eq 0
   then
          echo ERRORE: $1 non può essere 0.
   fi
   exit 3;;
esac

#salvo il primo parametro in una variabile e poi lo elimino
X=$1
shift

for i in $*
do
	case $i in 
	/*)if test ! -d $i -o ! -x $i
	   then 
		   echo ERRORE: $i deve essere una directory attraversabile.
                   exit 4
	   fi;;
	*) echo ERRORE: $i deve essere un nome assoluto.
	   exit 5;;
	esac
done

#definisco ed esporto la variabile PATH
PATH=`pwd`:$PATH
export PATH

#creo e azzero il file temporaneo con la ridirezione a vuoto
> /tmp/conta$$

for N in $*
do
	FCR.sh $N $X /tmp/conta$$
done

#conto i file che hanno rispettato le specifiche
echo Il numero di file che rispettano le specifiche è: `wc -l > /tmp/conta$$`

#stampo tutti i nomi assoluti dei file e chiedo all'utente il numero K
for i in `cat /tmp/conta$$`
do
	echo Nome del file corrente: $i.
	echo Inserire un numero strettamente positivo e minore di $2:
	read K

	case $K in
	*[!0-9]*) echo ERRORE: bisogna inserire un numero intero strettamente positivo.
        	  exit 2;;
	*) if test $K -eq 0
   	   then
          	 echo ERRORE: $K non può essere 0.
   	   fi
	   exit 3

	   #controllo rispetto a X
   	   if test $K -ge $X
   	   then
	   	 echo ERRORE: il numero non può maggiore o uguale a $X
           fi
           exit 6;;
	esac


	#stampo la k-esima riga del file corrente	
	echo La righa numero $K del file $i è: `head -$K $i | tail -1`
done

#elimino il file temporaneo
rm /tmp/conta$$
