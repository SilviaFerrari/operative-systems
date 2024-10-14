#!/bin/sh
#file comandi ricorsivo FCR.sh per esplirare le gerarchie

cd $1 #entriamo nella directory passata dal file principale

#verifichiamo che il file passato sia nella directory corrente, che sia leggibile e con il nome richiesto
if test -f $2 -a -r $2
then 
