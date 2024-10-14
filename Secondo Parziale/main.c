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
    int pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    pipe_t *piped;      /* per creare un numero variabile di pipe */
    pipe_t p;           /* pipe nipote-figlio */
    int j;              /* indice dei processi figli */
    int k;              /* indice delle pipe */
    int J;              /* per salvare il numero passato che sarà anche il numero di figli da creare */
    int createdf;       /* per il file descriptor del file da creare */
    char pattern[5];       /* per creare la stringa con il pid da cercare */
    /* ogni pid è di 4 cifre + terminatore di stringa */
    char ST[250];       /* array di char per memorizzare le linee */
    /* si suppone che le linee non superino i 250 caratteri */

    /* controllo stretto dei parametri */
    if(argc != 3){
        printf("Errore: necessari SOLO 2 argomenti [numero intero > 0] [nome di file] per %s.\n", argv[0]);
        exit(1);
    }

    J = atoi(argv[1]); /* converto il parametro in un int */
    
    /* controllo parametro intero strettamente positivo */
    if(J <= 0){
        printf("Errore: Il secondo parametro non è un numero strettamente maggiore di 0.\n");
        exit(2);
    }
    
    /* creo il file con il nome passato come secondo parametro */
    /* usaimo la open in versione estesa nel caso il file esista già */
    if((createdf = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0){
        printf("Errore nella creazione del file %s.\n", argv[2]);
        exit(3);
    }

    /* alloco la memoria necessaria per l'array di pipe_t sulla base del valore di n */
    piped = (pipe_t *)(malloc(J * sizeof(pipe_t)));
    if (piped == NULL){    /* controllo il valore di ritorno della malloc */
        printf("Errore nella creazione dell'array di pipe_t filgio-padre.\n");
        exit(4);
    }

    /* creazione delle J pipe figlio-padre */
    for(k = 0; k < J; k++){
        /* creo la pipe e controllo che sia avvenuto con successo */
        if (pipe(piped[k]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe figlio-padre di indice %d.\n", k);
            exit(5);
        }
    }

    printf("DEBUG: Il processo padre %d sta per creare %d processi figli.\n", getpid(), J);    
    
    /* creo J processi figli */
    for(j = 0; j < J; j++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork del figlio %d.\n", j);
            exit(6);
        }
        
        if(pid == 0){   /* se la fork ha avuto successo */
            /* codice processo figlio */
            printf("DEBUG: Il processo figlio di pid %d sta per creare un processo nipote.\n", getpid());

            /* ogni figlio (produttore) chiude tutte le pipe figlio-padre che non usa */
            /* in questo caso devo lasciare aperti entrambi i canali di comunicazione della pipe corrente per poi la fare la simulazione del piping e la ridirezione */
            for(k = 0; k < J; k++){
                if(k != j){
                    close(piped[k][0]); /* lato di lettura */
                    close(piped[k][1]); /* lato di scrittura */
                }
            }          

            /* creo la pipe nipote-figlio e controllo che sia avvenuto con successo */
            if (pipe(p) < 0 ){
                /* la creazione è fallita, restituisco errore */
                printf("Errore creazione pipe nipote-figlio.\n");
                exit(-1);
            }

            /* creazione del processo nipote */
            if((pid = fork()) < 0){
                /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
                printf("Errore nella fork del nipote %d.\n", j);
                exit(-1);
            }
            
            /* se pid == 0 la fork() ha avuto successo */
            if(pid == 0){
            /* codice processo figlio */
                printf("DEBUG: Il processo nipote %d è stato creato dal processo figlio %d e sta per eseguire il comando ps.\n", getpid(), getppid());
                
                /* chiusura della pipe figlio-padre rimasta aperta che il nipote però non usa */
                close(piped[j][0]);
                close(piped[j][1]);
                
                /* ridirezione dello standard output e simulazione del piping  */
                close(1);
                dup(p[1]);  /* duplico il lato di scrittura */
                
                /* ogni nipote ora può chiudere entrambi i lati della pipe nipote-figlio */
                close(p[0]); /* il lato di lettura 0 non viene usato */
                close(p[1]); /* il lato di scrittura 1 viene usato dallo standard output */

                execlp("ps", "ps", (char *)0); /* eseguo il comando ps */

                /* non si dovrebbe mai arrivare a questo punto */
                perror("Errore: execlp comando ps fallita.\n");  /* sarà eseguita solo se execl fallisce (non dovremmo mai arrivare qua) */
                exit(-1);   /* ritorniamo al padre -1 = 255 che sarà interpretato come errore */               
            }

            /* codice del processo figlio */
            
            /* ridirezione dello standard input */
            close(0);
            dup(p[0]);  /* duplico il lato di lettura */

            /* chiudo entrambi i lati della pipe nipote-figlio */
            close(p[0]); /* lato di lettura nipote figlio */
            close(p[1]); /* il lato di scrittura nipote figlio non serve */
            
            /* ridirezione dello standard output e simulazione del piping  */
            close(1);
            dup(piped[j][1]);  /* duplico il lato di scrittura */
            
            /* ogni figlio ora può chiudere i lati della pipe figlio-padre inutilizzati */
            close(piped[j][0]); /* lato di lettura figlio padre */
            close(piped[j][1]); /* il lato di scrittura figlio padre 1 viene usato dallo standard output */
            
            /* non faccio la ridirezione di standard error per sapere l'errore restituito da grep */
            //close(2);
            //open("/dev/null", O_WRONLY);    /* gli dico dove scrivere */
            
            sprintf(pattern, "%d", getpid());   /* creo il pattern da cercare per la grep*/
            printf("DEBUG: il patter da cercare è: %s\n", pattern);
            pattern[4] = '\0';
            execlp("grep", "grep", pattern, (char *)0); /* per eseguire un comando esistente*/
            
            /* non si dovrebbe mai arrivare a questo punto */
            perror("Errore: execlp comando grep fallita.\n");  /* sarà eseguita solo se execl fallisce (non dovremmo mai arrivare qua) */
            exit(-1);   /* ritorniamo al padre -1 = 255 che sarà interpretato come errore */
            /* non aspetto i nipoti perchè altrimenti non riesco a far eseguire i processi in concorrenza */
        }
    }

    /* codice processo padre */

    /* ogni padre (consumatore) chiude tutte le pipe che non usa */
    for(k = 0; k < J; k++){
        /* lascio aperti tutti i lati di lettura o il padre non potrà leggere */
        close(piped[k][1]); /* chiudo tutti i lati di scrittura */
    }

    printf("DEBUG: lettura del padre dalla pipe.\n");
    /* ricevo le linee selezionate dagli J figli */
    for(j = 0; j < J; j++){
        k = 0;  /* riutilizzo k come indice dei caratteri */
        
        while((read(piped[j][0], &(ST[k]), 1)) != 0){   /* leggo da pipe carattere per carattere la linea passata dal figlio */
            k++;    /* incremento l'indice di posizione */
        }

        ST[k] = '\n'; /* aggiungo in ultima posizione il terminatore di linea */
        write(createdf, &ST, sizeof(ST));  /* scrivo sul file precedentemente creato la linea */
    }

    /* il processo padre aspetta tutti i processi figli */
    for(j = 0; j < J; j++){
        /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
        pidFiglio = wait(&status);
        if(pidFiglio < 0){
            printf("Errore nella wait.\n");
            exit(7);
        }
        
        /* controllo il valore di uscita del processo figlio */
        if ((status & 0xFF) != 0){  
            /* il figlio NON è terminato correttamente */
            printf("Processo figlio con pid %d terminato in modo anomalo.\n", pidFiglio);
            /* non facciamo exit con errore, il problema è solo del figlio, non del padre */
        }
        else {
            /* figlio terminato correttamente */
            returnValue = (int)((status >> 8) & 0xFF);
            if(returnValue != 0){
                printf("Il processo figlio di pid %d ha riscontrato dei problemi nel comando grep: %d.\n", pidFiglio, returnValue);
            }
            else{
                printf("Il processo figlio di pid %d ha eseguito con successo il comando grep: %d.\n", pidFiglio, returnValue);
            }
        }
    }

    exit(0);
}