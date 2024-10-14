#!/bin/sh
#File prova-comandi.sh

echo Il percorso assoluto del file è `pwd`/p.txt

#-n serve per non andare a capo
#gli apici singoli servono solo per mettere lo spazio
echo -n 'Direcotry corrente '
pwd

#verifichiamo che funzioni anche con gli apici doppi
echo -n "DEBUG-Directory corrente "
pwd
