#!/bin/sh
#File prova-inibizioni.sh

a=ciao	#assegnamo la stringa ciao alla variabile a

#visualizzo a, l'esecuzione di pwd e del metacarattere *
echo Il valore della variabile a è $a e directory corrente `pwd` e valore di asterisco *

#procedo ad inibire tutte le sostituzioni
echo 'Il valore della variabile a è $a e directory corrente `pwd` e valore di asterisco *'

#inibizione solo dell'ultima sostituzione
echo "Il valore della variabile a è $a e directory corrente `pwd` e valore di asterisco *"
