#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <string.h>    /* per le funzioni delle stringhe */
#include <sys/wait.h>  /* per la wait e le macro */
#include <ctype.h>     /* per usare isdigit() e isalpha() */

/* creo la struct formata da un char e un long int e la definisco come nuovo TIPO*/
typedef struct {
    char c;             /* per salvare il carattere cercato */         
    long int tot;       /* per il contatore delle occorrenze */
} strcurr;

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int n;              /* per calcolare quanti caratteri sono stati passati*/
    int i, k;           /* indice degli n figli e delle pipe da chiudere */
    int fd;             /* per la open */
    char cx;            /* per salvare il carattere letto */
    int piped[2];       /* per la pipe */       
    strcurr *occ;       /* array di struct */

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

    /* alloco la memoria necessaria per l'array di struct sulla base del valore di n */
    occ = (strcurr *)(malloc(n * sizeof(strcurr)));

    /* controllo il valore di ritorno della malloc */
    if (occ == NULL){    
        /* la malloc ha fallito, riporto errore */
        printf("Errore: problemi nella malloc.\n");
        exit(4);
    }

    /* creo una pipe e controllo che la creazione abbia successo */
    if (pipe(piped) < 0 ){
        /* la pipe ha fallito, riporto errore */
        printf("Errore creazione pipe.\n");
        exit(3);
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
            
            close(piped[0]); /* ogni figlio (produttore) chiude il lato di lettura*/

            /* DEVO APRIRE IL FILE PER OGNI PROCESSO FIGLIO */
            /* controllo che il parametro passato sia il nome di un file esistente*/
            if ((fd = open(argv[1], O_RDONLY)) < 0){
                printf("Errore: il file %s non esiste.\n", argv[1]);
                exit(-1);
            }

            occ[i].c = argv[i+2][0];    /* salvo nell'array di struct il carattere da cercare */
            occ[i].tot = 0;             /* azzero il contatore delle occorrenze */

            //printf("contatore: %ld  parametro: %d\n", count, argv[i+2][0]);
            while((read(fd, &cx, 1)) > 0){
                //printf("carattere corrente: %d\n", cx);
                if(cx == argv[i+2][0]){
                    occ[i].tot += 1;    /* ho trovato il carattere, quindi incremento il contatore*/
                    //printf("contatore: %ld\n", count);
                }
            }
            write(piped[1], &(occ[i]), sizeof(strcurr));      /* scrivo sulla pipe il numero di occorrenze trovate */
            exit(argv[i+2][0]);                         /* esco restituendo il carattere cercato */
        }
    }

    close(piped[1]); /* il padre (consumatore) chiude il lato di scrittura */

    /* codice del padre */
    for(i = 0; i < n; i++){
        if(read(piped[0], &(occ[i]), sizeof(strcurr)) > 0){   /* leggo dalla il numero di occorrenze */
            printf("Sono state trovate %ld occorrenze del carattere %c.\n", occ[i].tot, occ[i].c);
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