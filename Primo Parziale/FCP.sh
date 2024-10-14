#!/bin/sh
#FCP.sh
#file comandi principale

#controllo del numero di parametri
case $# in
0|1|2) echo ERRORE: devono essere passati almeno 3 parametri.
     exit 1;;
*);; # echo DEBUG-OK: $# parametri;;
esac

#controllo del primo parametro che deve essere un nome relativo semplice
case $1 in
/*) echo ERRORE: $1 non deve essere un nome assoluto.
    exit 2;;
*/*) echo ERRORE: $1 non deve essere un nome relativo.
     exit 3;;
*);; # echo DEBUG-OK: $1 è un nome relativo semplice.;;
esac

#salvo il primo parametro in una variabile e poi lo cancello dalla lista dei parametri in modo
#che rimangano solo i nomi assoluti di directory
J=$1
shift

#controllo degli altri M parametri scorrendoli con il for
for M in $*
do
	case $M in
	/*) if test ! -d $M -o ! -x $M  #controllo che sia una directory e che sia traversabile
	    then
		echo ERRORE: $M deve essere una directory traversabile.
		exit 4
	    fi;;
	 *) echo ERRORE: $M deve essere un percorso assoluto.
	    exit 5;;
	 esac
done

#setto ed esporto la variabile PATH
PATH=`pwd`:$PATH
export PATH

#creo/azzero il file temporaneo con la ridirezione a vuoto
> /tmp/Temporaneo$$

#eseguo le M fasi del programma, una per ogni gerarchia passata come parametro
for dir in $*
do
	#chiamo il file comandi ricorsivo
	FCR.sh $dir /tmp/Temporaneo$$ 
done


#leggo il numero di righe del file temporaneo e salvo il numero nella variabile Totale
Totale=`wc -l < /tmp/Temporaneo$$`

#controllo se il numero globale di file è uguale a 0
if test $Totale -eq 0
then
	#non sono stati trovati file in terza posizione, quindi diamo errore
	echo ERRORE: in tutte le gerarchie passate, non esistono file in terza posizione.
	exit 6
else
	#è stato trovato almeno un file in terza posizione, riportiamo il numero preciso
	echo Il numero globale di file che rispetta le specifiche è: $Totale.

	#creo nella directory corrente una nuova sotto directory 
	#la nomino con la stringa passata come primo parametro al programma
	mkdir $J
	echo DEBUG la cartella creata si chiama $J

	cd `pwd`/$J  #accedo alla directory appena creata

	#creo per ogni file trovato in terza posizione un link hardware 
	#con lo stesso nome relativo semplice del file originale
	for K in `cat /tmp/Temporaneo$$`
	do
		echo "\nDEBUG: creo il link del file con percorso $K"
		`ln $K .`
	done
fi

#finito il programma, elimino il file temporaneo
rm /tmp/Temporaneo$$

#fine del file comandi principale
