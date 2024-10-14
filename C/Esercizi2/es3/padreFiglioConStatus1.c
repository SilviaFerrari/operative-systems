/* FILE: padreFiglioConStatus.c */
#include <stdio.h>  
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

int myRandom(int n){
    int randNum;
    randNum = rand() % n;
    return randNum;
}

int main(){
    int pid;        /*per il valore di ritorno della fork*/
    int pidFiglio;  /*per il valore di ritorno della wait*/
    int num;        /*per il valore random*/
    int value;      /*per filtrare il valore d'uscita del processo figlio*/
    int status;     /*da usare nella wait*/

    /*stampo il pid del processo padre*/
    printf("Il pid del processo padre è: %d.\n", getpid());

    /*eseguo la creazione del processo figlio e controllo che la fork sia andata a buon fine, altrimenti esco*/
    if((pid = fork()) < 0){
        printf("Errore nella fork.");
        exit(1);    
    }

    srand(time(NULL));  /*inizializzo il seme*/

    /*se la creazione del processo figlio è avvenuta con successo*/
    if(pid == 0){
        printf("Il pid del processo figlio è %d, creato dal processo padre con pid %d.\n", getpid(), getppid());
        num = myRandom(100);
        exit(num);  /*il figlio termina e restituisce il valore random*/
    }

    /*se non ci sono processi da attendere la wait ritorna -1*/
    pidFiglio = wait(&status);
    printf("DEBUG: pidFiglio dell wait = %d.\n", pidFiglio);
    if(pidFiglio < 0){
        printf("Errore nella wait %d.\n", pidFiglio);
        exit(2);
    }

    /*se la wait ha successo salva in status il valore restituito dal figlio e lo passa al padre*/
    if(pidFiglio == pid){
        printf("Terminato il processo figlio con pid %d.\n", pidFiglio);
        printf("DEBUG: pidFiglio dato dalla fork = %d.\n", pid);
    } else{
        printf("Il pid della wait %d non corrisponde col pid della fork %d.\n", pidFiglio, pid);
        exit(3);
    }

    /*contrllo il valore di uscita del processo filgio */
    printf("DEBUG: status = %d.\n", status);
    printf("DEBUG: status & 0xFF = %d.\n", status & 0xFF);
    
    /*se status & 0xFF == 0 allora il figlio è terminato correttamente %d*/
    if((status & 0xFF) != 0){   /*0xFF = 11111111*/
        printf("Figlio terminato in modo involontario.\n");
    } else{
        value = (int)((status >> 8) & 0xFF);
        printf("Il figlio con pid = %d ha ritornato %d.\n", pidFiglio, value);
    }
  
    /*il padre fa una seconda wait ma non ci saranno più figli da aspettare*/
    if(wait((int *)0) < 0){
        printf("Errore nella wait.\n");
        exit(4);
    }  
}