#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <string.h>    /* funzioni stringhe */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <time.h>      /* per il random */
#include <sys/wait.h>  /* per la wait e le macro */

int main(int argc, char** argv){
    int pid;           /* per il valore di ritorno della fork */
    int pidFiglio;     /* per il valore di ritorno della wait */
    int returnValue;   /* per filtrare il valore d'uscita del processo figlio */
    int status;        /* da usare nella wait */
    int fd;            /* per la open */
    int Cx;            /* per salvare il carattere da cercare */
    int ccur;          /* per scorrere e leggere il file */
    
    /* controllo stretto dei parametri */
    if (argc != 2){
        printf("Errore: necessario solo un argomento [nome file] per %s.\n", argv[0]);
        exit(1);
    }
    
    if((pid = fork()) < 0){
        /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
        printf("Errore nella fork.\n");
        exit(2);
    }
    
    /* se pid == 0 la fork() ha avuto successo e possiamo eseguire il codice del processo figlio */
    if (pid == 0){
        /* ridirezione dello standard input */
        close(0);
        /* controllo che il file passato come parametro esista */
        if ((fd = open(argv[1], O_RDONLY)) < 0){
            printf("Errore: il file %s non esiste.\n", argv[1]);
            exit(-1);   /* ritorniamo al padre -1 = 255 che sarà interpretato come errore */
        }

        execl("/home/silvia/C/Esercizi3/myCat", "myCat", (char *)0);
        printf("Errore: execl fallita.\n");  /* sarà eseguita solo se execl fallisce */
        exit(-1);   /* ritorniamo al padre -1 = 255 che sarà interpretato come errore */
    }
    
    /* il processo padre aspetta il processo figlio */
    /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
    pidFiglio = wait(&status);
    if (pidFiglio < 0){
        printf("Errore in wait.\n");
        exit(3);
    }
    
    /* controllo il valore di uscita del processo filgio */
    /* se status & 0xFF == 0 allora il figlio è terminato correttamente */
    if ((status & 0xFF) != 0){
        printf("Processo figlio con pid %d terminato in modo anomalo.\n", pidFiglio);
        /* non facciamo exit con errore, il problema è solo del figlio, non del padre */
    }
    else {
        returnValue = (int)((status >> 8) & 0xFF);
        if (returnValue != 0){
            printf("Il processo figlio con pid %d è incorso in un errore ed ha ritornato il valore %d.\n", pidFiglio, returnValue);
        }
        else {
            printf("Il processo figlio di pid %d ha ritornato %d.\n", pidFiglio, returnValue);
        }
    }
    
    exit(0);
}

