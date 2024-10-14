#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <string.h>    /* funzioni stringhe */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <sys/wait.h>  /* per la wait e le macro */
#include <signal.h>    /* per i segnali */
#include <stdbool.h>   /* per i booleani */

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int N;              /* numero di figli e di file passati */
    int i, k;           /* indice figli e pipe */
    pipe_t *pipe1;      /* per creare un numero variabile di pipe figlio-padre */
    pipe_t *pipe2;      /* per creare un numero variabile di pipe padre-figlio */
    int sost;           /* contatore occorrenze */
    long int pos;       /* posizione carattere corrente */
    int fd;             /* per la open */
    char cx;            /* per salvare il carattere da cercare */
    char ccur, s;          /* per scorrere e leggere il file e leggere da terminale */
    bool end;           /* per capire quando tutti i processi figli hanno finito */

    /* controllo lasco dei parametri */
    if (argc < 3){
        printf("Errore: necessari almeno 2 argomenti [1+ nomi file] [singolo carattere] per %s.\n", argv[0]);
        exit(1);
    }
    
    /* controllo parametro singolo carattere */
    if (argv[argc-1][1] != '\0'){
        printf("Errore: %s non è un singolo carattere.\n", argv[argc-1]);
        exit(2);
    }

    cx = argv[argc-1][0];
    N = argc - 2;

    /* alloco la memoria necessaria per gli array di pipe_t sulla base del valore di n */
    pipe1 = (pipe_t *)(malloc(N * sizeof(pipe_t))); /* figlio-padre */
    pipe2 = (pipe_t *)(malloc(N * sizeof(pipe_t))); /* padre-figlio */
    
    if (pipe1 == NULL || pipe2 == NULL){    /* controllo il valore di ritorno della malloc */
        printf("Errore nella creazione dell'array di pipe_t.\n");
        exit(3);
    }

    /* creazione delle n pipe */
    for(i = 0; i < N; i++){
        /* creo la pipe e controllo che sia avvenuto con successo */
        if (pipe(pipe1[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe di indice %d.\n", i);
            exit(4);
        }

        if (pipe(pipe2[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe di indice %d.\n", i);
            exit(5);
        }
    }
    
    printf("DEBUG: il processo padre %d sta per creare %d processi figli.\n", getpid(), N);

    for(i = 0; i < N; i++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork.\n");
            exit(3);
        }
        
        /* se pid == 0 la fork() ha avuto successo */
        if (pid == 0){
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
            if ((fd = open(argv[i+1], O_RDWR)) < 0){
                printf("Errore: il file %s non esiste.\n", argv[i+1]);
                exit(-1);
            }

            sost = 0;   /* azzero il contatore delle sostituzioni */
            pos = -1;   /* resetto la posizione (il primo carattere viene considerato in posizione 0) */
           
            while((read(fd, &ccur, 1)) > 0){
                pos++;  /* aumento la posizione del carattare corrente */

                if(cx == ccur){
                    write(pipe1[i][1], &pos, sizeof(long int)); /* comunico al padre la posizione trovata */
                    read(pipe2[i][0], &ccur, 1);                /* leggo la risposta del padre */
                    
                    if(ccur != '\n'){ /* controllo se devo sostituire il carattere, altrimenti vado avanti */
                        lseek(fd, -1L, SEEK_CUR);   /* retrocedo di una posizione nel file */
                        write(fd, &ccur, 1);        /* sovrascrivo il carattere */
                        sost++;                     /* incremento il contatore delle sostituzioni */
                    }                    
                }
            }
            
            exit(sost);
        }        
    }
    
    /* codice processo padre */

    /* chiudo tutte le pipe di cui non ho bisogno */
    for(i = 0; i < N; i++){
        close(pipe1[i][1]); /* lato di scrittura figlio-padre */
        close(pipe2[i][0]); /* lato di lettura padre-figlio*/
    }

    /* ricezione posizione e sostituzione carattare */
    end = false;   /* per poter entrare nel while */

    while(!end){
        end = true;
        for(i = 0; i < N; i++){
            if((read(pipe1[i][0], &pos, sizeof(long int))) != 0){
                end = false;
                printf("Il processo figlio di indice %d ha trovato il carattere %c in posizione %ld nel file %s.\n", i, cx, pos, argv[i+1]);
                
                printf("Con quale carattere si desidera sostituirlo? (premere invio per non cambiare)\n");
                scanf("%c\n", &ccur);   /* leggo il carattere scritto */
                
                if(ccur == '\n'){   /* se è stato scritto un carattere */
                    scanf("%c", &s); /* lettura a vuoto per eliminare il carattere \n */
                }      
                write(pipe2[i][1], &ccur, 1);   /* comunico al figlio il carattere letto */         
            }
        }
    }

    /* il processo padre aspetta gli N processi figli */
    for(i = 0; i < N; i++){
        /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
        pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore in wait.\n");
            exit(7);
        }
        
        /* controllo il valore di uscita del processo figlio */
        /* se status & 0xFF == 0 allora il figlio è terminato correttamente */
        if ((status & 0xFF) != 0){
            printf("Processo figlio con pid %d terminato in modo anomalo.\n", pidFiglio);
            /* non facciamo exit con errore, il problema è solo del figlio, non del padre */
        }
        else {
            returnValue = (int)((status >> 8) & 0xFF);
            printf("Il processo figlio di pid %d ha ritornato %d.\n", pidFiglio, returnValue);
        }
    }
    
    
    exit(0);
}
