#!/bin/sh
#FCP.sh
#file comandi principale

#controllo del numero di parametri
case $# in
0|1|2) echo ERRORE: pochi parametri, devono essere almeno 3
       exit 1;;
*);;
esac

#definisco il contatore
cont=1

#definisco la variabile in cui salvo tutte le directory 
params=

for i in $*
do
	if test $cont -ne $#
	then
		case $i in
		/*) if test ! -d $i -o ! -x $i
		    then
			    echo ERRORE: $i deve essere una directory traversabile.
			    exit 2
	            fi;;
		*) echo ERRORE: $i deve essere un percorso assoluto.
	           exit 3;;
		esac
                
                #aggiungo la directory trovata alle altre
		params="$params $i"
	else
		#salvo la stringa nella variabile S
                S=$i
	
		#controllo che sia una stringa senza il carattere /
		case $S in
		*/*) echo ERRORE: la stringa $S non può contenere il carattere /.
		     exit 4;;
		*) echo DEBUG: il nome da cercare è $S.txt;;
		esac
	fi

	#incremento il contatore
	cont=`expr $cont + 1`
done

#definisco ed esporto la variabile PATH
PATH=`pwd`:$PATH
export PATH

#creo/azzero il file temporaneo con la ridirezione a vuoto
> /tmp/conta$$

for G in $params
do
	#invoco il file ricorsivo
	echo DEBUG: esplorazione di $G...
	FCR.sh $G $S /tmp/conta$$
	echo "DEBUG: esplorazione ultimata.\n"
done

#conto e stampo le righe di /tmp/conta$$
echo I file totali che soddisfano le specifiche sono: `wc -l < /tmp/conta$$`

line=1  #variabile che mi servirà per capire se la linea contiene un nome o un numero di linee

#stampo i nomi e le lunghezze in caratteri di tutti i file
for f in `cat /tmp/conta$$`
do
	if test `expr $line % 2` -ne 0
	then
                #elemento dispari --> numero di linee del file
                echo "\nNumero totale di caratteri: $f."

	else
		#elemento pari --> nome del file
		echo Nome assoluto del file: $f.
	       
		#chiedo all'utente se vuole riordinare il file
		echo Si desidera riordinare il file?
		read answ

		case $answ in
		s*|y*|S*|Y*) echo File ordinato non key sensitive:
			     sort -f $f;;
			     #sort -r $f ordinato con modalità inversa
                *) echo No grazie.;;
	        esac
	fi

	#incremeneto il contatore
	line=`expr $line + 1`
done

#finito lo script elimino anche il file temporaneo
rm /tmp/conta$$
