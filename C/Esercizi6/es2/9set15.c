#define _POSIX_SOURCE

#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <string.h>    /* funzioni stringhe */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <sys/wait.h>  /* per la wait e le macro */
#include <signal.h>    /* per i segnali */
#include <stdbool.h>   /* per le variabili booleane */

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

int main(int argc, char** argv){
    int *pid;           /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int N;              /* numero di file e di processi figli */
    int i, k;           /* indice processi figli */
    pipe_t *pipe1;      /* per creare un numero variabile di pipe figlio-padre */
    pipe_t *pipe2;      /* per creare un numero variabile di pipe padre-figlio */
    int fd, afd;        /* per le open del figlio e del padre */
    char cc, ca;        /* per scorrere e leggere il file */
    bool *equal;        /* per capire quali processi sono da terminare */
    bool go;            /* per i permessi dei processi */
    
    /* controllo lasco dei parametri */
    if (argc < 4){
        printf("Errore: necessari almeno 3 argomenti [nomi file] per %s.\n", argv[0]);
        exit(1);
    }

    N = argc - 2;
    printf("DEBUG: il processo padre %d sta per creare gli %d processi figli.\n", getpid(), N);

    /* alloco dinamicamente la memoria per l'array di bool */
    equal = (bool *)(malloc(N * sizeof(bool)));

    /* alloco dinamicamente la memoria per l'array di pid dei figli */
    pid = (int *)(malloc(N * sizeof(int)));

    /* alloco la memoria necessaria per gli array di pipe_t sulla base del valore di N */
    pipe1 = (pipe_t *)(malloc(N * sizeof(pipe_t))); /* figlio-padre */
    pipe2 = (pipe_t *)(malloc(N * sizeof(pipe_t))); /* padre-figlio */

    /* controllo il valore di ritorno della malloc */
    if (pipe1 == NULL || pipe2 == NULL || equal == NULL || pid == NULL){    
        printf("Errore in una delle malloc.\n");
        exit(2);
    }

    /* creazione delle n pipe e assegnazione dei booleani */
    for(i = 0; i < N; i++){
        /* creo la pipe e controllo che sia avvenuto con successo */
        if (pipe(pipe1[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe figlio-padre di indice %d.\n", i);
            exit(3);
        }

        if (pipe(pipe2[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe padre-figlio di indice %d.\n", i);
            exit(4);
        }

        /* inizialmente assegno a tutti i processi valore true */
        equal[i] = true;
    }
    
    /* genero N processi figli */
    for(i = 0; i < N; i++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid[i] = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork.\n");
            exit(6);
        }
        
        /* se pid == 0 la fork() ha avuto successo */
        if (pid[i] == 0){
        /* codice processo figlio */
            printf("DEBUG: Il processo figlio di pid %d è stato creato dal processo padre %d.\n", getpid(), getppid());

            /* chiudo tutte le pipe che non uso */
            for(k = 0; k < N; k++){
                close(pipe1[k][0]); /* lato di lettura figlio-padre */
                close(pipe2[k][1]); /* lato di scrittura padre-figlio */

                if(k != i){
                    close(pipe1[k][1]); /* lato di scrittura figlio-padre */
                    close(pipe2[k][0]); /* lato di lettura padre-figlio */
                }
            }       

            /* controllo che il parametro passato sia il nome di un file esistente */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0){
                printf("Errore: il file %s associato al figlio non esiste.\n", argv[i+1]);
                exit(-1);
            }

            /* controllo il permesso di lettura */
            while(read(pipe2[i][0], &go, 1)){
                if(go){
                    read(fd, &cc, 1);
                    write(pipe1[i][1], &cc, 1);  /* scrivo sulla pipe il carattere letto */
                }
                else{
                    break; /* esco dal ciclo */
                }
            }
            
            exit(EXIT_SUCCESS);
        }
    }
    
    /* codice del padre */

    /* chiudo tutte le pipe di cui non ho bisogno */
    for(i = 0; i < N; i++){
        close(pipe1[i][1]); /* lato di scrittura figlio-padre */
        close(pipe2[i][0]); /* lato di lettura padre-figlio*/
    }

    /* controllo che il parametro passato sia il nome di un file esistente */
    if ((afd = open(argv[argc-1], O_RDONLY)) < 0){
        printf("Errore: il file %s non esiste.\n", argv[argc-1]);
        exit(6);
    }

    while((read(afd, &ca, 1)) > 0){    /* leggo un carattere dal file */
        for(i = 0; i < N; i++){

            if(equal[i]){
                go = true;
                write(pipe2[i][1], &go, sizeof(bool));  /* scrivo sulla pipe padre-figlio il permesso */

                read(pipe1[i][0], &cc, 1);  /* leggo dalla pipe figlio-padre il carattere */

                if(ca != cc){ /* se i caratteri sono diversi, nego il permesso */
                    equal[i] = false;   /* segno che questo processo sarà da terminare in modo forzato */
                    go = false;         /* nego il permesso */
                }

                write(pipe2[i][1], &go, sizeof(bool));  /* scrivo sulla pipe padre-figlio il permesso */
            }
        }
    }

    /* una volta che ho finito di leggere il file afd, */
    /* il padre termina in modo forzato tutti i figli che non hanno ricevuto i consensi */
    for(i = 0; i < N; i++){
        if(!equal[i]){
            /* termino il processo in modo forzato */
            if((kill(pid[i], SIGKILL)) == -1){
                printf("Errore nella kill.\n");
                exit(8);
            }
        }
        else{   /* se non sono falliti, dobbiamo mandare il segnale d'uscita */
            go = false;
            write(pipe2[i][1], &go, sizeof(bool));
        }
    }
   

    /* il processo padre aspetta i processi figli restanti */
    for(i = 0; i < N; i++){
        /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare */
        pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore in wait.\n");
            exit(7);
        }
        
        /* controllo il valore di uscita del processo filgio */
        /* se status & 0xFF == 0 allora il figlio è terminato correttamente */
        if ((status & 0xFF) != 0){
            printf("Processo figlio con pid %d terminato in modo anormale.\n", pidFiglio);
            /* non facciamo exit con errore, il problema è solo del figlio, non del padre */
        }
        else {
            returnValue = (int)((status >> 8) & 0xFF);
            printf("Processo figlio di pid %d terminato in modo normale con valore.\n", pidFiglio, returnValue);
        }
    }

    exit(0);
}