#!/bin/sh
#file comandi prova2.sh
echo Sto eseguendo lo script...
echo Cerco $2 in $1...
#verifico se la cartella corrente è quella che stiamo cercando
case $2 in
*/$1) echo Ho trovato $2 in $1!;;  #trovata directory
esac
echo Fatto.
#Questa è una modifica che si vede solo qua
