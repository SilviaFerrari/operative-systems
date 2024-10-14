#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <string.h>    /* funzioni stringhe */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <sys/wait.h>  /* per la wait e le macro */
#include <ctype.h>     /* per usare isdigit() isalpha() islower() */

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int fd;             /* per la open */
    int i, k, N;        /* indice dei figli e delle pipe*/
    char ccur;          /* per scorrere e leggere il file */
    char nc;            /* per il nuovo carattere alfabetico maiuscolo */
    long int trasf;     /* per il numero di trasformazioni effettuate */
    pipe_t *fp;         /* array di pipe comunicazione figlio-padre */
    pipe_t *np;         /* array di pipe comunicazione nipote-padre */

    /* controllo lasco dei parametri */
    if (argc < 3){
        printf("Errore: necessari almeno 2 argomenti [nomi file] per %s.\n", argv[0]);
        exit(1);
    }
    
    N = argc - 1;   /* calcolo il numero di file passati e di conseguenza il numero di figli da creare */

    printf("DEBUG: creazione delle pipe...\n");

    /* alloco la memoria necessaria per l'array di pipe_t figlio-padre sulla base del valore di N */
    fp = (pipe_t *)(malloc(N * sizeof(pipe_t)));
    if (fp == NULL){    /* controllo il valore di ritorno della malloc */
        printf("Errore nella creazione dell'array di pipe_t figlio-padre.\n");
        exit(2);
    }

    /* alloco la memoria necessaria per l'array di pipe_t nipote-padre sulla base del valore di N */
    np = (pipe_t *)(malloc(N * sizeof(pipe_t)));
    if (np == NULL){    /* controllo il valore di ritorno della malloc */
        printf("Errore nella creazione dell'array di pipe_t nipote-padre.\n");
        exit(3);
    }

    /* creazione delle N pipe figlio-padre e nipote-padre */
    for(i = 0; i < N; i++){
        /* creo la pipe e controllo che sia avvenuto con successo */
        if (pipe(fp[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe filgio-padre di indice %d.\n", i);
            exit(4);
        }

        if (pipe(np[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe nipote-padre di indice %d.\n", i);
            exit(5);
        }
    }

    printf("DEBUG: Il processo padre %d sta per creare %d processi figli.\n", getpid(), N);

    /* creo gli N figli */
    for(i = 0; i < N; i++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork.\n");
            exit(6);
        }   
        
        /* se pid == 0 la fork() ha avuto successo */
        if (pid == 0){
            
            /* codice processo figlio */
            
            /* chiudo solo le pipe figlio-padre non utilizzate, le altre le dovrò chiudere dopo */
            for(k = 0; k < N; k++){
                close(fp[k][0]); /* lato di lettura figlio-padre */
                if(k != i){
                    close(fp[k][1]); /* lato di scrittura figlio-padre */
                }
            }  
            
            printf("DEBUG: Il processo figlio %d creato dal processo padre %d sta per creare un processo nipote.\n", getpid(), getppid());
            
            /* creo il processo nipote e controllo che la fork sia andata a buon fine */
            if((pid = fork()) < 0){
                /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
                printf("Errore nella fork.\n");
                exit(-1);
            }
            
            /* se pid == 0 la fork() ha avuto successo */
            if (pid == 0){
            /* codice processo nipote */
                printf("DEBUG: Il processo nipote %d è stato creato dal processo figlio %d.\n", getpid(), getppid());
                
                /* ogni nipote (produttore) chiude tutte le pipe che non usa */
                close(fp[i][1]);    /* lato di scrittura figlio-padre rimasta aperta */

                for(k = 0; k < N; k++){
                    close(np[k][0]); /* lato di lettura nipote-padre */
                    if(k != i){
                        close(np[k][1]); /* lato di scrittura nipote-padre */
                    }
                }

                /* controllo che il parametro passato sia il nome di un file esistente */
                /* sia processo figlio che padre dovranno aprire il file separatamente per operare le trasformazioni */
                /* apro il file in lettura/scrittura */
                if ((fd = open(argv[i+1], O_RDWR)) < 0){
                    printf("Errore: il file %s non esiste.\n", argv[i+1]);
                    exit(-1);
                }

                /* cerco i caratteri alfabetici minuscoli */
                while((read(fd, &ccur, 1)) > 0){
                    if(islower(ccur)){
                        lseek(fd, -1L, SEEK_CUR);   /* per retrocedere di una posizione nel file */
                        nc = toupper(ccur);     /* trasformo il carattere da minuscolo a maiuscolo */
                        write(fd, &nc, 1);      /* sostituico il carattere */
                        trasf++;                /* incremento il numero di trasformazioni */
                    }
                }

                /* scrivo sulla pipe associata il numero di trasformazioni effettuate */
                write(np[i][1], &trasf, sizeof(trasf));
            
                /* calcolo il valore di ritorno del nipote */
                returnValue = trasf / 256;
                exit(returnValue);
            }

            /* codice del figlio */

            /* ora posso chiudere anche le pipe nipote-padre non usate dal figlio */
            /* DA FARE SOLO DOPO LA CREAZIONE DEL NIPOTE */
            for(k = 0; k < N; k++){
                close(np[k][0]);    /* lato di lettura nipote-padre */
                close(np[k][1]);    /* lato di scrittura nipote-padre */
            }  

            /* controllo che il parametro passato sia il nome di un file esistente */
            /* apro il file in lettura/scrittura */
            if ((fd = open(argv[i+1], O_RDWR)) < 0){
                printf("Errore: il file %s non esiste.\n", argv[i+1]);
                exit(-1);
            }
            
            /* cerco i caratteri numerici */
            while((read(fd, &ccur, 1)) > 0){
                if(isdigit(ccur)){                  
                    lseek(fd, -1L, SEEK_CUR);   /* per retrocedere di una posizione nel file */
                    nc = ' ';                   /* carattere da sostituire */
                    write(fd, &nc, 1);          /* faccio la sostituzione */
                    trasf++;                    /* incremento le trasfromazioni */
                }
            }
            
            /* scrivo sulla pipe associata il numero di trasformazioni effettuate */
            write(fp[i][1], &trasf, sizeof(trasf));

            /* il processo figlio aspetta il processo nipote */
            pidFiglio = wait(&status);
            if (pidFiglio < 0){
                printf("Errore in wait.\n");
                exit(-1);
            }
            
            /* controllo il valore di uscita del processo filgio */
            /* se status & 0xFF == 0 allora il figlio è terminato correttamente */
            if ((status & 0xFF) != 0){
                printf("Processo figlio con pid %d terminato in modo anomalo.\n", pidFiglio);
                /* non facciamo exit con errore, il problema è solo del nipote, non del figlio */
            }
            else {
                returnValue = (int)((status >> 8) & 0xFF);
                printf("Il processo nipote di pid %d ha ritornato %d.\n", pidFiglio, returnValue);
            }

            /* calcolo il valore di ritorno del figlio */
            returnValue = trasf / 256;
            exit(returnValue);
        }
    }
    
    /* codice processo padre */
    /* ogni padre (consumatore) chiude tutte le pipe che non usa */
    for(k = 0; k < N; k++){
        /* TUTTI I LATI DI LETTURA VANNO LASCIATI APERTI O IL PADRE NON POTRÀ LEGGERE */
        close(fp[k][1]); /* lato di scrittura figlio */
        close(np[k][1]); /* lato di scrittura nipote */
    }       

    for(i = 0; i < N; i++){
        printf("Nel file %s sono stati trovati: ", argv[i+1]);
        
        read(fp[i][0], &trasf, sizeof(long int));     /* leggo dalla pipe figlio padre */
        printf("%ld caratteri numerici e ", trasf);

        read(np[i][0], &trasf, sizeof(long int));     /* leggo dalla pipe nipote padre */
        printf("%ld caratteri alfabetici.\n", trasf);
    }

    /* il processo padre aspetta gli N processi figli */
    for(i = 0; i < N; i++){
        pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore in wait.\n");
            exit(7);
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