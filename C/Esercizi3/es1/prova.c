#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char **argv){
    int answ;   /*per salvare la risposta data dall'utente*/

    printf("Digitare 0 per terminare la chiamata ricorsiva.\n");
    scanf("%d", &answ); 

    if(answ != 0){
        printf("Eseguo la chiamata ricorsiva.\n");

        /*execl(percorso assoluto del comando da invocare, argomenti da passare, terminatore)*/
        /*se non dobbiamo passare nessun parametro dobbiamo comunque mettere un argomento vuoto*/
        /*il terminatore deve essere un putatore a NULL, non va bene mettere 0*/
        execl("prova", "", NULL);

        printf("Errore in execl.\n");
        exit(-1);
    }
    
    exit(answ);
}