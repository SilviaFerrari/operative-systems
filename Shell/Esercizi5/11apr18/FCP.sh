#!/bin/sh
#FCP.sh
#file comandi principale

#controllo del numero di parametri
case $# in
0|1|2) echo ERRORE: $# parametri insufficienti, richiesti 3 [num] [2+ dir]
       exit 1;;
*);;
esac

#controllo del primo parametro
case $1 in
*[!0-9]*) echo ERRORE: $1 non è un numero strettamente positivo.
	  exit 2;;
*) if test $1 -eq 0
   then
	   echo ERRORE: $1 non può essere 0.
	   exit 3
   fi;;
esac

#salvo il primo parametro nella variabile Y e lo cancello
Y=$1
shift

#controllo dei percorsi delle gerarchie
for i in $*
do
	case $i in
	/*) if test ! -d $i -o ! -x $i
	    then
		    echo ERRORE: $i non è una directory attraversabile.
		    exit 4
            fi;;
	*) echo ERRORE: $i non è un percorso assoluto.
	   exit 5;;
	esac
done

#creo ed esporto la variabile PATH
PATH=`pwd`:$PATH
export PATH

#creo/azzero il file temporaneo con la ridirezione a vuoto
> /tmp/conta$$

#inizio le N fasi per tutte le gerarchie
for N 
do
	echo DEBUG: sto per esplorare la directory $N...
	FCR.sh $N $Y /tmp/conta$$
	echo "DEBUG: esplorazione finita.\n"
done

#conto tutte le righe del file temporaneo
echo I file totali creati sono: `wc -l < /tmp/conta$$`

#stampo prima tutti i file che contengono una quinta riga
echo "\nFile creati in cui è presente la quinta riga:"
for q in `cat /tmp/conta$$`
do
	case $q in
	*.quinta) echo "\nPercorso assoluto del file: $q"
		  echo La riga contenuta è: `cat $q`;;
	esac
done

#stampo i restanti file vuoti
echo "\nFile creati vuoti:"
for nq in `cat /tmp/conta$$`
do
        case $nq in
        *.NOquinta) echo Percorso assoluto del file: $nq
                  echo "...\n";;
        esac
done


#il programma è finito, cancello il file temporaneo
rm /tmp/conta$$
