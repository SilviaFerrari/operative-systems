#!/bin/sh
#FCP.sh
#file comandi principale

#controllo del numero di parametri
case $# in
0|1|2|3) echo ERRORE: devono esserci almeno 4 parametri.
	 exit 1;;
*);;
esac

#controllo che il primo parametro sia un numero intero strettamente positivo
case $1 in
*[!0-9]*) echo ERRORE: $1 non è un numero intero positivo.
	  exit 2;;
*) if test $1 -eq 0
   then
	   echo ERRORE: $1 non può essere 0.
	   exit 3
   fi;;
esac

#salvo il primo parametro nella variabile W
W=$1

#controllo che il secondo parametro sia una stringa
case $2 in
*/*) echo ERRORE: il carattere / non è consentito nella stringa.
     exit 4;;
*);;
esac

#salvo la stringa nella variabule S
S=$2

#elimino il primo e il secondo parametro
shift
shift

#controllo dei percorsi delle gerarchie
for G in $*
do 
	case $G in
	/*) if test ! -d $G -o ! -x $G
	    then
		   echo ERRORE: $G deve essere una directory traversabile.
		   exit 5
            fi;;
	*) echo ERRORE: $G deve essere un percorso assoluto.
	   exit 6;;
        esac	   
done

#definisco ed esporto la variabile PATH
PATH=`pwd`:PATH
export PATH

#creo/azzero il file temporaneo
> /tmp/nomiAssoluti

#esploro tutte le gerarchie
for G in $*
do
	#invoco il file ricorsivo
	echo DEBUG: inizio esplorazione di $G...
	FCR.sh $G $S /tmp/nomiAssoluti
	echo "DEBUG: fine esplorazione.\n"
done

#stampo il numero totale di directory che soddisfano la condizione
nl=`wc -l < /tmp/nomiAssoluti`  #variabile per il numero di righe 
echo "\nNumero totale di directory che soddisfano le specifiche: $nl"

if test $nl -ge $W
then
	echo -n "Silvia scrivi un numero incluso tra 1 e $W: "
	read X  #variabile in cui salvo la risposta dell'utente

	#verifico che sia stato inserito un numero valido
	case $X in
        *[!0-9]*) echo ERRORE: $1 non è un numero intero positivo.
                  rm /tmp/nomiAssoluti
		  exit 7;;
        *) if test $X -gt $W -o $X -lt 1
           then
		   echo ERRORE: $X deve essere compreso tra 1 e $W.
		   rm /tmp/nomiAssoluti
                   exit 8
           fi;;
        esac

	#seleziono la directory corrispondente
	echo Percorso assoluto della directory alla posizione $X: `head -$X /tmp/nomiAssoluti | tail -1`
fi

#finito lo script rimuovo il file temporaneo
rm /tmp/nomiAssoluti
