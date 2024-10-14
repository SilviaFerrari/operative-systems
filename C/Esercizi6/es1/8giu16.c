#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <string.h>    /* funzioni stringhe */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <time.h>      /* per il random */
#include <sys/wait.h>  /* per la wait e le macro */
#include <sys/stat.h>  /*  */
#include <ctype.h>     /* per usare isdigit() e isalpha() */
#include <signal.h>    /* per i segnali */

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

int i, randNum;     /* indice figli e numero random */
int flag = 0;

void handler(int sig){
    printf("DEBUG: il padre %d ha ricevuto il segnale %d e il valore di i è %d.\n", getpid(), sig, i);
    flag = 1;
}

int myRandom(int n){
    randNum = rand() % n;
    return randNum;
}

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    pipe_t *pipe1;      /* per l'array di pipe figlio-padre */
    pipe_t *pipe2;      /* per l'array di pipe padre-figlio */
    int H;              /* numero intero passato come parametro */
    int N;              /* numero di file passati come parametri */
    int k, val;         /* indice di pipe, valore generato dal random */
    int fd, fdout;      /* per la open e la creazione del file */
    char line[255];     /* array di caratteri per contenere la linea */
    int length;         /* per calcolare la lunghezza della linea */
    
    
    /* controllo lasco dei parametri */
    if (argc < 6){
        printf("Errore: necessari almeno 5 argomenti [4+ nomi file] [0 < numero intero < 255] per %s.\n", argv[0]);
        exit(1);
    }
    
    H = atoi(argv[argc]); /* converto il parametro (numero linee dei file garantita dalla parte shell) */
    N = argc - 2;         /* numero di file passati */  

    /* controllo parametro intero positivo */
    if (H <= 0 || H >= 255){
        printf("Errore: Il parametro non è 0 < numero intero < 255.\n");
        exit(2);
    }

    /* installo il gestore handler per il segnale SIGPIPE */
    signal(SIGPIPE, handler);
    
    srand(time(NULL));  /* inizializzo il seme del random */

    printf("DEBUG: il processo padre %d sta per creare il file e le %d pipe.\n", getpid(), N);

    /* creo il file 'creato' nella cartella tmp */
    /* usaimo la open in versione estesa nel caso il file esista già */
    if((fdout = open("/tmp/creato", O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0){
        printf("Errore nella creazione del file %s.\n", "/tmp/creato");
        exit(3);
    }
    /* tutti i figli potranno accedere a tale file, quindi non dovremo fare delle altre open */

    /* alloco la memoria necessaria per l'array di pipe_t figlio-padre */
    pipe1 = (pipe_t *)(malloc(N * sizeof(pipe_t)));

    /* alloco la memoria necessaria per l'array di pipe_t padre-figlio */
    pipe2 = (pipe_t *)(malloc(N * sizeof(pipe_t)));

    if (pipe1 == NULL || pipe2 == NULL){    /* controllo il valore di ritorno della malloc */
        printf("Errore nella creazione dell'array di pipe_t.\n");
        exit(4);
    }

    /* creazione delle n pipe */
    for(i = 0; i < N; i++){
        /* creo la pipe e controllo che sia avvenuto con successo */
        if (pipe(pipe1[i]) < 0){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe figlio-padre di indice %d.\n", i);
            exit(5);
        }

        if (pipe(pipe2[i]) < 0){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe padre-figlio di indice %d.\n", i);
            exit(6);
        }
    }

    printf("DEBUG: il processo padre %d sta per creare %d figli.\n", getpid(), H);

    /*creazione degli N figli */
    for(i = 0; i < N; i++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork.\n");
            exit(7);
        }
        
        /* se pid == 0 la fork() ha avuto successo */
        if (pid == 0){
        /* codice processo figlio */
            printf("DEBUG: Il processo figlio di pid %d è stato creato dal processo padre %d.\n", getpid(), getppid());

            /* in questo caso il figlio è sia produttore che consumatore, ma rispetto a pipe diverse! */

            /* chiudo tutte le pipe non usate */
            for(k = 0; k < N; k++){
                close(pipe1[k][0]); /* lato di lettura figlio-padre */
                close(pipe2[k][1]); /* lato di scrittura padre-figlio */

                if(k != i){
                    close(pipe1[k][1]); /* lato di scrittura filgio-padre */
                    close(pipe2[k][0]); /* lato di lettura padre-figlio */
                }
            }

            /* controllo che il parametro passato sia il nome di un file esistente */
            if ((fd = open(argv[i+1], O_RDONLY)) < 0){
                printf("Errore: il file %s non esiste.\n", argv[i+1]);
                exit(-1);
            }
            
            /* scorro il file lettera per lettera */
            length = 0;         /* azzero la lunghezza della linea */
            returnValue = 0;    /* azzero il numero di caratteri scritti */
            while((read(fd, &(line[length]), 1)) > 0){
                length++;           /* per ogni carattere letto incremento la lunghezza della linea */

                if(line[length] == '\n'){
                    write(pipe1[i][1], &length, sizeof(length));    /* scrivo la lunghezza sulla pipe al padre */
                    read(pipe2[i][0], &k, sizeof(k));               /* leggo la risposta del padre dalla pipe */

                    /* controllo che il valore ritornato sia ammissibile rispetto alla lunghezza attuale */
                    if(k < length){
                        printf("DEBUG: il processo di indice %d sta per scrivere il catarrete %c nel file.\n", i, line[k]);

                        /* ogni figlio per scrivere sul file creato usa il fdout passato dal padre */
                        write(fdout, &(line[k]), 1);
                        returnValue++;  /* incremento il valore di ritorno */
                    }
                    else{
                        printf("Il processo %d non scrive niente...", getpid());
                    }

                    length = 0; /* azzero la lunghezza per la nuova linea */
                }
            }

            exit(returnValue);
        }       
    }

    /* codice del padre */

    /* chiudo tutte le pipe non usate */
    for(k = 0; k < N; k++){
        /* TUTTI I LATI DI LETTURA DI PIPE1 VANNO LASCIATI APERTI O IL PADRE NON POTRÀ LEGGERE */
        close(pipe1[k][1]); /* lato di scrittura */

        /* TUTTI I LATI DI SCRITTURA DI PIPE2 VANNO LASCIATI APERTI O IL PADRE NON POTRÀ LEGGERE */
        close(pipe2[k][0]); /* lato di scrittura */
    }

    /* recupero tutte le lunghezze calcolate dai figli */
    for(k = 0; k < H; k++){
        /* calcolo in modo random l'indice del figlgio */
        randNum = myRandom(N);
        printf("DEBUG: numero di figlio gerato dal random: %d.\n", randNum);

        read(pipe1[randNum][0], &val, sizeof(val));    /* recupero il valore dal figlio calcolato */

        /* calcolo in modo random anche l'indice della riga */
        randNum = myRandom(val);
        printf("DEBUG: numero di linea gerato dal random: %d.\n", randNum);

        /* il padre comunica a tutti i figli tale indice */
        for(i = 0; i < N; i++){
            write(pipe2[i][1], &randNum, sizeof(randNum));
        }
    }

    /* il processo padre aspetta i processi figli */

    for(i = 0; i < N; i++){
        /* il processo padre aspetta il processo figlio */
        /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
        pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore in wait.\n");
            exit(8);
        }
        
        /* controllo il valore di uscita del processo filgio */
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