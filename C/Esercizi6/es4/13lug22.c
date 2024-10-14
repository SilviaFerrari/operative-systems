#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <string.h>    /* funzioni stringhe */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <time.h>      /* per il random */
#include <sys/wait.h>  /* per la wait e le macro */
#include <sys/stat.h>  
#include <ctype.h>     /* per usare isdigit() e isalpha() */
#include <signal.h>    /* per i segnali */

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

int main(int argc, char** argv){
    int *pid;           /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int Q;              /* per il numero di caratteri e di processi figli */
    int L;              /* per il numero di linee del file */
    int count;    /* per calcolare le occorrenze dei caratteri e le linee*/
    int i, k;           /* indice dei figli e delle pipe */
    int F;              /* per la open */
    char cx, cc;        /* carattere da cercare e carattere corrente letto dal file */
    pipe_t *piped;             /* per creare un numero variabile di pipe */
    
    /* controllo lasco dei parametri */
    if (argc < 5){
        printf("Errore: necessari almeno 4 argomenti [nome file] [lunghezza in linee > 0] [2+ caratteri] per %s.\n", argv[0]);
        exit(1);
    }

    L = atoi(argv[2]); /* converto il parametro */
    
    /* controllo parametro intero positivo */
    if (L <= 0){
        printf("Errore: Il parametro non è un numero strettamente maggiore di 0.\n");
        exit(3);
    }
    
    Q = argc - 3;   /* calcolo il numero di caratteri passati */

    for(i = 0; i < Q; i++){
        /* controllo parametro singolo carattere */
        if (argv[i+3][1] != '\0'){
            printf("Errore: %s non è un singolo carattere.\n", argv[i+3]);
            exit(4);
        }
    }

    /* alloco la memoria necessaria per i pid sulla base del valore di Q */
    pid = (int *)(malloc(Q * sizeof(int)));

    /* alloco la memoria necessaria per l'array di pipe_t sulla base del valore di Q */
    piped = (pipe_t *)(malloc(Q * sizeof(pipe_t)));
    
    if (piped == NULL || pid == NULL){    /* controllo il valore di ritorno della malloc */
        printf("Errore nella malloc.\n");
        exit(5);
    }

    /* creazione delle n pipe */
    for(i = 0; i < Q; i++){
        /* creo la pipe e controllo che sia avvenuto con successo */
        if (pipe(piped[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe di indice %d.\n", i);
            exit(6);
        }
    } 
    
    printf("DEBUG: il processo padre %d sta per creare %d processi figli.\n", getpid(), Q);

    for(i = 0; i < Q; i++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid[i] = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork.\n");
            exit(7);
        }
        
        /* se pid == 0 la fork() ha avuto successo */
        if (pid[i] == 0){
        /* codice processo figlio */
            //printf("DEBUG: Il processo figlio di pid %d è stato creato dal processo padre %d e calcolerà le occorrenze del carattere %c.\n", getpid(), getppid(), argv[i+3][0]);
            
            /* ogni figlio (produttore) chiude tutte le pipe che non usa */
            for(k = 0; k < Q; k++){
                close(piped[k][0]);     /* lato di lettura */
                if(k != i){
                    close(piped[k][1]); /* lato di scrittura */
                }
            }

            /* controllo che il parametro passato sia il nome di un file esistente */
            if ((F = open(argv[1], O_RDONLY)) < 0){
                printf("Errore: il file %s non esiste.\n", argv[1]);
                exit(-1);
            }

            cx = argv[i+3][0];  /* salvo il carattere da leggere */
            count = 0;          /* azzero il contatore delle occorrenze */

            while((read(F, &cc, 1)) > 0){   /* scorro i caratteri del file */

                if(cc == '\n'){     /* se arrivo a fine linea */
                    write(piped[i][1], &count, sizeof(int));    /* comunico al padre count */
                    count = 0;      /* azzero il contatore delle occorrenze */
                }

                if(cc == cx){   /* se trovo il carattere cercato */
                    count++;    /* incremento il contatore delle occorrenze */
                }
            }

            exit(count);    /* restituisco il numero di occorrenze del carattere nell'ultima linea */
        }
    }

    /* codice processo padre */

    /* il padre (consumatore) chiude tutte le pipe che non usa */
    for(k = 0; k < Q; k++){
        /* TUTTI I LATI DI LETTURA VANNO LASCIATI APERTI O IL PADRE NON POTRÀ LEGGERE */
        close(piped[k][1]); /* lato di scrittura */
    }

    for(i = 1; i <= L; i++){
        printf("\nLinea %d del file %s\n", i, argv[1]);
        for(k = 0; k < Q; k++){
            read(piped[k][0], &count, sizeof(int));
            printf("Figlio con indice %d e pid %d ha letto %d caratteri %c nella linea corrente.\n", k, pid[k], count, argv[k+3][0]);
        }
    }

    /* il processo padre aspetta i figli */
    for(i = 0; i < Q; i++){
        /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
        pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore in wait.\n");
            exit(8);
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