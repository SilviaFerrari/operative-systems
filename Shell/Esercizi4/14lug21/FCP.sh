#!/bin/sh
#FCP.sh
#file comandi principale

#controllo del numero dei parametri
case $# in 
0|1|2|3) echo ERRORE: $# parametri insufficieni, devono essere almeno 4.
	 exit 1;;
*);;
esac

echo DEBUG: parametri totali $#.

#isoliamo gli ultimi 2 parametri 
num=1     #variabile per capire quando abbiamo trovato l'ultimo e il penultimo parametro
qparam=   #variabile per accumulare le gerarchie


for i
do
	#non consideriamo gli ultimi 2 parametri e facciamo i controlli sui percorsi assoluti
	if test $num -lt `expr $# - 1`
	then
		case $i in
	        #se è un percorso assoluto controllo che sia una directory traversabile
        	/*) if test ! -d $i -o ! -x $i
                    then
                          echo ERRORE: $i deve essere una directory traversabile.
                  	  exit 2
            	    fi;;
        	#dò errore se non è un percorso assoluto
        	*) echo ERRORE: $i deve essere un nome assoluto.
           	   exit 3;;
        	esac
		
		#se ha superato i controlli, memorizzo la gerarchia in qpar
		qpar="$qpar $i"
	else
		#abbiamo trovato l'ultimo o il penultimo parametro e controlliamo se sono numeri strettamente positivi
                case $i in
       		*[!0-9]*) echo ERRORE: $i non è un numero intero positivo.
                          exit 4;;
       		*) if test $i -eq 0
                   then
                	echo ERRORE: $i non può essere 0.
                        exit 5
        	   fi;;
	        esac

		#se i controlli sono andati bene salviamo i parametri
		if test $num -eq `expr $# - 1`
		then 
			H=$i  #penultimo parametro
		elif test $num -eq $#
		then
			M=$i  #ultimo parametro
		fi
	fi

	#incremento il contatore
	num=`expr $num + 1`
done

#echo DEBUG: le gerarchie da esplorare sono $qpar.
#echo DEBUG: penultimo parametro $H
#echo "DEBUG: ultimo parametro $M \n"

#creo ed esporto la variabile $PATH
PATH=`pwd`:$PATH
export PATH

#inizio delle Q fasi per esplorare le gerarchie
for G in $qpar
do
	#invoco il file comandi ricorsivo FCR.sh
	echo DEBUG: esploro la directory $G...
	FCR.sh $G $H $M
	echo "DEBUG: esplorazione di $G finita.\n"
done
