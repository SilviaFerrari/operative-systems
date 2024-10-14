#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <sys/wait.h>  /* per la wait e le macro */
#include <ctype.h>     /* per usare isdigit() e isalpha() */

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

/* creo la struct formata da un char e un long int e la definisco come nuovo TIPO*/
typedef struct {
    int c1;             /* indice d'ordine del processo */
    long int c2;        /* contatore delle occorrenze */
} cstruct;

int main(int argc, char** argv){
    int *pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int N;              /* numero di processi figli */
    int i, k;           /* indice dei processi figli e delle pipe */
    int fd;             /* per la open */
    char cx;            /* per salvare il carattere da cercare */
    char cc;            /* per scorrere e leggere il file */
    pipe_t *piped;      /* per creare un numero variabile di pipe */
    cstruct *data;      /* array di strutture dati */
    
    /* controllo lasco dei parametri */
    if (argc < 4){
        printf("Errore: necessari almeno 3 argomenti [2+ nomi file] [carattere alfabetico minuscolo] per %s.\n", argv[0]);
        exit(1);
    }

    /* controllo parametro singolo carattere */
    if (argv[argc][1] != '\0' || !islower(argv[argc][0])){
        printf("Errore: %s non è un singolo carattere alfabetico minuscolo.\n", argv[argc]);
        exit(2);
    }
    
    cx = argv[argc][0];     /* salvo il carattere per comodità */
    N = argc - 2;   /* calcolo il numero di file passati e i figli da creare */

    /* alloco la memoria necessaria per l'array di pipe_t sulla base del valore di N */
    piped = (pipe_t *)(malloc(N * sizeof(pipe_t)));

    if (piped == NULL){    /* controllo il valore di ritorno delle malloc */
        printf("Errore allocazione pipe_t.\n");
        exit(3);
    }

    /* alloco la memoria necessaria per l'array di pid sulla base del valore di N */
    pid = (int *)(malloc(N * sizeof(int)));

    if (pid == NULL){    /* controllo il valore di ritorno delle malloc */
        printf("Errore allocazine pid.\n");
        exit(4);
    }

    /* creazione delle n pipe */
    for(i = 0; i < N; i++){
        /* creo la pipe e controllo che sia avvenuto con successo */
        if (pipe(piped[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe di indice %d.\n", i);
            exit(5);
        }
    }

    printf("DEBUG: il processo padre %d sta per creare %d processi figli.\n", getpid(), N);

    /* creazione degli N figli */
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
            printf("DEBUG: Il processo figlio %d sta per cercare il carattere %c nel file %s.\n", getpid(), cx, argv[i+1]);
            
            /* ogni figlio chiude tutte le pipe che non usa */
            for(k = 0; k < N; k++){
                if(i == 0 || k != i-1)
                close(piped[k][0]);     /* lato di lettura */

                if(k != i){
                    close(piped[k][1]); /* lato di scrittura */
                }
            }   

            /* controllo che il parametro passato sia il nome di un file esistente */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0){
                printf("Errore: il file %s non esiste.\n", argv[i+1]);
                exit(-1);
            }

            if(i != 0){ /* solo con i figli superiori al primo */
                read(piped[i-1][0], &data, sizeof(data)); /* leggo dalla pipe l'array di struct */
            }

            data = realloc(data, (i+1) * sizeof(cstruct));

            data[i].c1 = i;     /* assegno il numero dell'indice */
            data[i].c2 = 0;     /* azzero il contatore delle occorrenze */
            
            while((read(fd, &cc, 1)) > 0){
                if(cc == cx){       /* se trovo il carattere cercato */
                    data[i].c2++;   /* incremento il contatore */
                }
            }

            write(piped[i][1], &data, sizeof(data));    /* scrivo sulla pipe la nuova struttura */
            
            exit(i);    /* ritorno l'indice del processo */
        }
    }

    /* codice del padre */

    /* ogni padre chiude tutte le pipe che non usa */
    for(k = 0; k < N; k++){
        /* chiudo tutte le pipe tranne il lato di lettura dell'ultima */
        close(piped[k][1]); /* lato di scrittura */

        if(k != (N-1)){
            close(piped[k][0]);
        }
    }

    read(piped[N-1][0], &data, sizeof(data));   /* leggo dalla pipe l'array finale di strutture */
    for(i = 0; i < N; i++){     /* ciclo per scorrere l'array */
        printf("Il processo %d ha trovato %ld occorrenze del carattere %c nel file %s.\n", pid[data[i].c1], data[i].c2, cx, argv[i+1]);
    }

    /* il padre aspetta i figli */
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