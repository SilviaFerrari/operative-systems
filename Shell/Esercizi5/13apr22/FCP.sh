#!/bin/sh
#FCP.sh
#file comandi principale

#controllo del numero di parametri
case $# in
0|1|2) echo ERRORE: devono essere inseriti almeno 3 parametri.
       exit 1;;
*);;
esac

#controllo del primo parametro
case $1 in
*[!0-9]*) echo ERRORE: $1 non è un numero intero strettamente positivo.
	  exit 2;;
*) if test $1 -eq 0
   then
	   echo ERRORE: il numero non può essere 0.
	   exit 3
   fi;;
esac

#salvo il primo parametro nella variabile X e lo elimino
X=$1
shift

#controllo i nomi delle gerarchie che devono essere dei nomi assoluti
for G in $*
do
	case $G in
	/*) if test ! -d $G -o ! -x $G
	    then
		    	echo ERRORE: $G deve essere una directory traversabile.
			exit 4 
	    fi;;
	 *) echo ERRORE: $G deve essere un percorso assoluto.
            exit 5;;
	 esac
done

#definisco ed esporto il PATH
PATH=`pwd`:$PATH
export PATH

#variabile contatore per dare un nome a tutti i file temporanei
cont=1

#scorro tutte le gerarchie e prima di chimare il file ricorsivo creo il file temporaneo
for G in $*
do
	>/tmp/nomiAssoluti$cont
	#chiamo il file ricorsivo
	FCR.sh $G $X /tmp/nomiAssoluti$cont
	#incremento il contatore
	cont=`expr $cont + 1`
done

for file1 in `cat /tmp/nomiAssolutii1`
do
	cont=2
	for file2 in `cat /tmp/nomiAssoluti$cont`
	do
		if diff $file1 $file2 > /dev/null 2>&1
		then 
			echo Il file $F e $L sono uguali.
		fi
	done

	#incremento il contatore
	cont=`expr $cont + 1`
done

cont=1
#finito lo script elimino tutti i file temporanei
for i in $*
do
	rm /tmp/nomiAssoluti$cont
	cont=`expr $cont + 1`
done
