#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <string.h>    /* per le funzioni delle stringhe */
#include <sys/wait.h>  /* per la wait e le macro */
#include <ctype.h>     /* per usare isdigit() e isalpha() */

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int n;              /* per calcolare quanti caratteri sono stati passati*/
    int i, k;           /* indice degli n figli e delle pipe da chiudere */
    int fd;             /* per la open */
    char cx;            /* per salvare il carattere letto corrente */
    long int count;     /* per contare le occorrenze dei caratteri */
    pipe_t *piped;             /* per creare un numero variabile di pipe*/       

    /* controllo  dei parametri */
    if (argc < 4){
        printf("Errore: necessari almeno 3 argomenti [un nome file] [2+ caratteri] per %s.\n", argv[0]);
        exit(1);
    }

    n = argc - 2;   /* calcolo il numero di caratteri passati */

    /* controllo che i parametri siano un singolo carattere */
    for(i = 0; i < n; i++){
        if (strlen(argv[i+2]) != 1){
            printf("Errore: %s non è un singolo carattere.\n", argv[i+2]);
            exit(2);
        }
    }

    /* alloco la memoria necessaria per il pipe_t sulla base del valore di n */
    piped = (pipe_t *)(malloc(n * sizeof(pipe_t)));
    if (piped == NULL){    /* controllo il valore di ritorno della malloc */
        printf("Errore: problemi nella malloc.\n");
        exit(3);
    }

    /* creo le n pipe */    
    for(i = 0; i < n; i++){
        /* creo la pipe corrispondente a questo processo figlio */
        if (pipe(piped[i]) < 0 ){
            printf("Errore creazione pipe di indice %d.\n", i);
            exit(4);
        }
    }  
    
    /* creo gli n processi figli */
    for(i = 0; i < n; i++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork.\n");
            exit(5);
        }
        
        /* se pid == 0 la fork() ha avuto successo e possiamo eseguire il codice del processo figlio */
        if (pid == 0){
            /* codice del processo figlio*/
            printf("DEBUG: Il pid del processo figlio è %d, creato dal processo padre con pid %d.\n", getpid(), getppid());
            
            /* ogni figlio (produttore) chiude tutte le pipe che non usa */
            for(k = 0; k < n; k++){
                close(piped[k][0]); /* lato di lettura */
                if(k != i){
                    close(piped[k][1]); /* lato di scrittura */
                }
            }

            /* DEVO APRIRE IL FILE PER OGNI PROCESSO FIGLIO */
            /* controllo che il parametro passato sia il nome di un file esistente*/
            if ((fd = open(argv[1], O_RDONLY)) < 0){
                printf("Errore: il file %s non esiste.\n", argv[1]);
                exit(-1);
            }

            count = 0;  /* azzero il contatore delle occorrenze */
            //printf("contatore: %ld  parametro: %d\n", count, argv[i+2][0]);
            while((read(fd, &cx, 1)) > 0){
                //printf("carattere corrente: %d\n", cx);
                if(cx == argv[i+2][0]){
                    count++;    /* ho trovato il carattere, quindi incremento il contatore*/
                    //printf("contatore: %ld\n", count);
                }
            }
            write(piped[i][1], &count, sizeof(count));    /* scrivo sulla pipe il numero di occorrenze trovate */
            exit(argv[i+2][0]);    /* esco restituendo il carattere cercato */
        }
    }

    /* ogni padre (consumatore) chiude tutte le pipe che non usa */
    for(k = 0; k < n; k++){
        /* TUTTI I LATI DI LETTURA VANNO LASCIATI APERTI O IL PADRE NON POTRÀ LEGGERE */
        close(piped[k][1]); /* lato di scrittura */
    }

    /* codice del padre */
    for(i = 0; i < n; i++){
        if(read(piped[i][0], &count, sizeof(count)) > 0){   /* leggo dalla il numero di occorrenze */
            printf("Sono state trovate %ld occorrenze del carattere %s.\n", count, argv[i+2]);
        }
    }
    
    /* il processo padre aspetta il processo figlio */
    /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
    for(i = 0; i < n; i++){
        pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore in wait.\n");
            exit(6);
        }
        
        /* controllo il valore di uscita del processo filgio */
        /* se status & 0xFF == 0 allora il figlio è terminato correttamente */
        if ((status & 0xFF) != 0){
            printf("Processo figlio con pid %d terminato in modo anomalo.\n", pidFiglio);
            /* non facciamo exit con errore, il problema è solo del figlio, non del padre */
        }
        else {
            returnValue = (int)((status >> 8) & 0xFF);
            printf("Il processo figlio di pid %d ha cercato il carattere %d.\n", pidFiglio, returnValue);
        }
    }
    
    exit(0);
}
