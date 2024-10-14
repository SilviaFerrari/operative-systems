#!/bin/sh
#file comandi Beginc-dir.sh

d=	#memorizza il primo parametro oppure la directory corrente
dr=	#memorizza il parametro che rappresenta il nome relativo semplice
        #del file (primo o secondo parametro a seconda dei casi)

case $# in
0) echo "POCHI PARAMETRI - Usage is: $0 [directory assoluta] [directory semplice]" 
   exit 1;;
1) d=`pwd`; dr=$1;;
2) d=$1; dr=$2;;
*) echo "TROPPI PARAMETRI - Usage is: $0 [directory assoluta] [directory semplice]"
   exit 2;;
esac

#controllo che d sia dato in forma assoluta
case $d in
/*) echo OK: nome assoluto.;;
*) echo Errore: $d non in forma assoluta.
   exit 3;;
esac

#controllo che f sia dato in forma relativa semplice 
case $dr in
*/*) echo Errore: $dr non in forma relativa semplice 
     exit 4;;
*) echo OK: nome relativo semplice;;
esac

#controllo che d sia una directory e che sia traversabile 
if test ! -d $d -o ! -x $d
then
	echo Errore: $d non directory o non attraversabie
	exit 5
fi

#controllo che f sia una directory
if test ! -d $dr
then
        echo Errore: $dr non directory
        exit 5
fi


PATH=`pwd`:$PATH
export PATH
echo "Beginc.sh: \nStiamo per esplorare la directory $d"
#invocazione del secondo file comandi RICORSIVO e fa in questo caso tutto il lavoro
Cercadir.sh $d $dr
echo HO FINITO TUTTO
