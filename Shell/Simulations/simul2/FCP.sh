#!/bin/sh
#file comandi FCP.sh

#controllo stretto del numero dei parametri (devono essere 2)
case $# in
2);;
*) echo ERRORE: bisogna inserire 2 parametri: [nome assoluto directory] [numero intero positivo]
   exit 1;;
esac

#contrllo che il primo parametro sia il nome assluto di una directory 
case $1 in
/*);;
*) echo ERRORE: deve essere un nome assoluto.
   exit 2;;
esac

#controllo che il secondo parametro sia un numero intero strettamente positivo
if test ! $2 -gt 0
then echo ERRORE: deve essere un numero intero strettamente positivo.
     exit 3
fi
