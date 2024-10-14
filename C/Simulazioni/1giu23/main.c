#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <sys/wait.h>  /* per la wait e le macro */
#include <signal.h>    /* per i segnali */

/* creo la struct formata da due int e un array di char e la definisco come nuovo TIPO*/
typedef struct {
    int c1;             /* per salvare il pid del processo nipote */
    char c2[250];       /* per l'ultima linea letta dal nipote */
    int c3;             /* lunghezza della linea contenuta in c2 compreso il terminatore */
} Strut;

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

int i;     /* variabili globali */
int flag = 0;

void handler(int sig){
    printf("DEBUG: il padre %d ha ricevuto il segnale %d e il valore di i è %d.\n", getpid(), sig, i);
    flag = 1;
}

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int N;              /* numero di file e di figli */
    int i, k;           /* indice figli e pipe */
    pipe_t *piped;      /* per creare un numero variabile di pipe */
    pipe_t p;           /* pipe di comunicazione nipote-figlio */
    Strut data;         /* struct in cui salvare i dati richiesti */
    char buffer[250];   /* si suppone che le linee non superino i 250 caratteri */
    
    /* controllo lasco dei parametri */
    if(argc < 3){
        printf("Errore: necessari almeno 2 argomenti [nomi di file] per %s.\n", argv[0]);
        exit(1);
    }

    N = argc-1; /* calcolo il numero di file passati e quindi di figli da creare */

    /* installo il gestore handler per il segnale SIGPIPE */
    signal(SIGPIPE, handler);

    /* alloco la memoria necessaria per l'array di pipe_t sulla base del valore di n */
    piped = (pipe_t *)(malloc(N * sizeof(pipe_t)));
    if (piped == NULL){    /* controllo il valore di ritorno della malloc */
        printf("Errore nella creazione dell'array di pipe_t.\n");
        exit(2);
    }

    /* creazione delle n pipe */
    for(i = 0; i < N; i++){
        /* creo la pipe e controllo che sia avvenuto con successo */
        if (pipe(piped[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe di indice %d.\n", i);
            exit(3);
        }
    }

    /* creazione degli N processi figli */
    for(i = 0; i < N; i++){
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork del figlio.\n");
            exit(4);
        }
        
        /* se pid == 0 la fork() ha avuto successo */
        if(pid == 0){
        /* codice processo figlio */
            printf("DEBUG: Il processo figlio di pid %d deve creare un processo nipote.\n", getpid());

            /* ogni figlio (produttore) chiude tutte le pipe che non usa */
            for(k = 0; k < N; k++){
                close(piped[k][0]); /* lato di lettura */
                if(k != i){
                    close(piped[k][1]); /* lato di scrittura */
                }
            }           

            /* creo una pipe e controllo che la creazione abbia successo */
            if (pipe(p) < 0 ){
            /* la pipe ha fallito, riporto errore */
                printf("Errore creazione pipe nipote-figlio.\n");
                exit(-1);
            }
            
            /* il processo figlio crea il processo nipote e controllo che sia andato a buon fine */
            /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
            if((pid = fork()) < 0){
                /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
                printf("Errore nella fork del nipote.\n");
                exit(-1);
            }
            
            /* se pid == 0 la fork() ha avuto successo */
            if(pid == 0){
            /* codice processo nipote */
                printf("DEBUG: Il processo nipote di pid %d è stato creato dal processo figlio %d.\n", getpid(), getppid());
                
                /* chiusura della pipe figlio-padre rimasta aperta che il nipote però non usa */
                close(piped[i][1]);
                
                /* ridirezione dello standard output e simulazione del piping  */
                close(1);
                dup(p[1]);  /* duplico il lato di scrittura */
                
                /* ogni nipote ora può chiudere entrambi i lati della pipe nipote-figlio */
                close(p[0]); /* il lato di lettura 0 non viene usato */
                close(p[1]); /* il lato di scrittura 1 viene usato dallo standard output */
                
                execlp("rev", "rev", argv[i+1], (char *)0); /* per eseguire un comando esistente*/

                perror("Errore: execl fallita.\n");  /* sarà eseguita solo se execlp fallisce */
                exit(-1);   /* ritorniamo al padre -1 = 255 che sarà interpretato come errore */
            }
            
            /* codice processo figlio */

            close(p[1]); /* chiudo il lato di scrittura nipote-filgio e lascio solo la lettura */

            data.c1 = pid;  /* assegno il pid del nipote alla struct */
            k = 0;          /* azzero l'indice dei caratteri */

            while((read(p[0], &(buffer[k]), 1)) != 0){

                if(buffer[k] == '\n'){     /* se arrivo a fine linea */
                    returnValue = k;    /* assegno all struct la lunghezza di tale linea */
                    k = 0;  /* azzero l'indice per poter partire con la linea successiva */
                }
                else{
                    k++;    /* incremento l'indice */
                }
            }

            /* copio il contenuto del buffer nella struct */
            for(k = 0; k <= returnValue; k++){
                data.c2[k] = buffer[k];
            }

            write(piped[i][1], &data, sizeof(data));    /* scrivo la struct sulla pipe */
            returnValue = k-1;

            /* il processo filgio aspetta il processo nipote */
            /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
            pidFiglio = wait(&status);
            if(pidFiglio < 0){
                printf("Errore in wait del nipote.\n");
                exit(-1);
            }
            
            /* controllo il valore di uscita del processo figlio */
            /* se status & 0xFF == 0 allora il figlio è terminato correttamente */
            if ((status & 0xFF) != 0){
                printf("Processo figlio con pid %d terminato in modo anomalo.\n", pidFiglio);
                exit(-1); /* in modo che venga segnalato al padre come 255 */
            }
            else if((int)((status >> 8) & 0xFF) != 0){   /* qui c'è stato un errore nel nipote */
                exit(-1);
            }
            
            exit(returnValue);
        }
    }
    
    /* codice processo padre */

    /* ogni padre (consumatore) chiude tutte le pipe che non usa */
    for(k = 0; k < N; k++){
        /* lascio aperti tutti i lati di lettura o il padre non potrà leggere */
        close(piped[k][1]); /* lato di scrittura */
    }

    for(i = 0; i < N; i++){
        while((read(piped[i][0], &data, sizeof(data))) != 0){
            data.c2[data.c3+1] = '\0';      /* trasformo in stringa */
            printf("Il processo nipote %d ha riportato l'ultima linea del file %s lunga %d caratteri:\n%s", data.c1, argv[i+1], data.c3, data.c2);
        }
    }
    
    /* il padre aspetta i processi figli */
    for(i = 0; i < N; i++){
        /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
        pidFiglio = wait(&status);
        if(pidFiglio < 0){
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