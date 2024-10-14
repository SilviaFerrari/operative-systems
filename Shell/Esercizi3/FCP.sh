#!/bin/sh
#file comandi FCP.sh

#controllo che sia stato passato esattamente un parametro
if test $# -ne 1
then
	echo ERRORE: bisogna inserire un solo parametro.
	exit 1
fi

#controllo che sia stato passato un percorso assoluto di una directory traversabile
case $1 in
/*)  if test ! -d $1 -o ! -x $1
     then
	     echo ERRORE: è stato passato un file o la directory non è traversabile.
	     exit 2
     else 
	     echo OK: è stato passato il nome assoluto di una directory traversabile.
     fi;;
*/*) echo ERRORE: passato percorso relativo.
     exit 3;;
*)   echo ERRORE: passato percorso relativo semplice.
     exit 4;;
esac

#invochiamo il secondo file comandi file-dir.sh
sh file-dir.sh $1
