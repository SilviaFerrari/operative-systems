#!/bin/sh
#FCP.sh
#file comandi principale

#controllo numero dei parametri
case $# in
0|1|2) echo ERRORE: devono essere inseriti almeno 3 parametri.
       exit 1;;
*);;
esac

#definisco la variabile contatore
count=1

#definisco la variabile in cui vado a salvare tutte le gerarchie
ger=

for i in $*
do
	if test $count -eq $#
	then
		case $i in
		*[!0-9]*) echo ERRORE: $i non è un numero strettamente positivo.
			  exit 2;;
	        *) if test $i -eq 0
		   then
		   	   echo ERRORE: $i non può essere 0.
		           exit 3
		   fi;;
		esac

		#salvo l'ultimo parametro nella variabile X
		X=$i
	else
		case $i in
		/*) if test ! -d $i -o ! -x $i
		    then
			    echo ERRORE: $i non è una directory attraversabile.
			    exit 4
		    fi;;
		 *) echo ERRORE: $i non è un percorso assoluto.
			 exit 5;;
		 esac

		 #salvo la gerarchia nella variabile
		 ger="$ger $i"
	fi

	#incremento il contatore
	count=`expr $count + 1`
done

#settiamo la variabile path e la esportiamo
PATH=`pwd`:$PATH
export PATH

#creiamo e azzeriamo il file temporaneo
> /tmp/conta$$

for N in $ger
do
	echo DEBUG: esploro la directory $N...
	#invoco il file comandi principale
	FCR.sh $N $X /tmp/conta$$
	echo "DEBUG: finito di esplorare $N.\n"
done

#contiamo le righe del file temporaneo /tmp/conta$$
echo Il numero di directory che soddisfa le condizioni date è: `wc -l < /tmp/conta$$`

for i in `cat /tmp/conta$$`
do
	echo "\nNome assoluto della directory trovata: $i"
	cd $i  #accedo alla directory
	echo "I file in esse contenuti sono:\n"
	for f in *
	do
		echo `pwd`/$f 
		echo "La cui riga $X è: `head -$X $f | tail -1`\n"  
	done
done
