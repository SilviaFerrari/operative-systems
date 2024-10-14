#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <time.h>      /* per il random */
#include <sys/wait.h>  /* per la wait e le macro */
#include <signal.h>    /* per i segnali */
#include <stdbool.h>   /* per i valori booleani */

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

/* creo la struct formata da un char e un long int e la definisco come nuovo TIPO*/
typedef struct {
    int c1;             /* per salvare il pid del nipote */
    int c2;             /* per il numero della linea */
    char c3[250];       /* linea corrente mandata dal nipote */
} cstruct;

int myRandom(int n){
    int randNum;
    randNum = rand() % n;
    return randNum;
}

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int N;              /* file passati e dei figli */
    int X;              /* numeri linee dei file */
    int i, k;           /* indice dei figli e delle pipe */
    int rnum;           /* numero random generato dal nipote */
    char opt[5];		/* stringa per passare ad head l'opzione con il numero delle linee da stampare */
	/* supponiamo che bastino 3 cifre per il numero, oltre al simbolo di opzione '-' e al terminatore di stringa */
    pipe_t *piped;      /* per creare un numero variabile di pipe figlio-padre */
    pipe_t p;           /* una sola pipe per la coppia nipote-figlio */
    cstruct data;       /* struct per le linee */
    bool end;
    
    /* controllo  dei parametri */
    if(argc < 3 || (argc-1)%2 != 0 ){
        printf("Errore: necessari almeno 2 argomenti in numero pari [nome file] [numero intero > 0] per %s.\n", argv[0]);
        exit(1);
    }

    N = (argc-1)/2;     /* calcolo il numero di file passati (e di numeri) */
    /* indice file --> 2*i+1 */
    /* indice linee --> 2*i+2 */

    for(i = 2; i < N; i+=2){
        X = atoi(argv[i]); /* converto il parametro */
        
        /* controllo parametro intero positivo */
        if(X <= 0){
            printf("Errore: Il parametro non è un numero strettamente maggiore di 0.\n");
            exit(2);
        }
    }

    /* alloco la memoria necessaria per l'array di pipe_t sulla base del valore di n */
    piped = (pipe_t *)(malloc(N * sizeof(pipe_t)));
    if (piped == NULL){    /* controllo il valore di ritorno della malloc */
        printf("Errore nella creazione dell'array di pipe_t figlio-padre.\n");
        exit(3);
    }

    /* creazione delle n pipe */
    for(i = 0; i < N; i++){
        /* creo la pipe e controllo che sia avvenuto con successo */
        if (pipe(piped[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe di indice %d.\n", i);
            exit(4);
        }
    }

    printf("DEBUG: il processo padre %d sta per creare %d processi figli.\n", getpid(), N);

    for(i = 0; i < N; i++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork del figlio.\n");
            exit(5);
        }
        
        /* se pid == 0 la fork() ha avuto successo */
        if (pid == 0){
        /* codice processo figlio */
            printf("DEBUG: Il processo figlio %d sta per creare un processo nipote.\n", getpid());

            /* ogni figlio (produttore) chiude tutte le pipe che non usa */
            for(k = 0; k < N; k++){
                close(piped[k][0]); /* lato di lettura */
                if(k != i){
                    close(piped[k][1]); /* lato di scrittura */
                }
            }     

            /* creazione della pipe di comunicazione nipote-filgio */
            if (pipe(p) < 0 ){  /* controllo che la creazione abbia successo */
            /* la pipe ha fallito, riporto errore */
                printf("Errore creazione pipe nipote-figlio.\n");
                exit(-1);
            }
            
            /*creazione del processo nipote */
            if((pid = fork()) < 0){
                /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
                printf("Errore nella fork del nipote.\n");
                exit(-1);
            }
            
            /* se pid == 0 la fork() ha avuto successo */
            if(pid == 0){
            /* codice processo nipote */
                /* chiusura della pipe figlio-padre rimasta aperta che il nipote però non usa */
                close(piped[i][1]);

                printf("DEBUG: Il processo nipote %d sta per generare un numero casuale ed eseguire il comando head.\n", getpid());

                srand(time(NULL));      /* inizializzo il seme del random */
                X = atoi(argv[2*i+2]);  /* numero di linee del file */ 
                rnum = myRandom(X+1);   /* l'estrazione random avviene tra 0 e X */
                printf("rnum = %d\n", rnum);

                sprintf(opt, "-%d", rnum);  /* creo il parametro opzionale per la head*/

                /* simulo il piping dei comandi verso il figlio */
                
                /* ridirezione dello standard output e simulazione del piping  */
                close(1);
                dup(p[1]);  /* duplico il lato di scrittura */
                
                /* ogni nipote ora può chiudere entrambi i lati della pipe */
                close(p[0]); /* il lato di lettura 0 non viene usato */
                close(p[1]); /* il lato di scrittura 1 viene usato dallo standard output */
                
                /* eseguo il comando head in modo da stampare le prime rnum righe del file */
                execlp("head", "head", opt, argv[2*i+1], (char *)0);

                printf("Errore: problemi di esecuzione della head.\n");  /* sarà eseguita solo se execlp fallisce */
                exit(-1);   /* ritorniamo al padre -1 = 255 che sarà interpretato come errore */
            }

            /* codice processo figlio */

            close(p[1]);    /* chiudo il lato di scrittura della pipe nipote-figlio */

            k = 0;              /* riutilizzo k come indice dei caratteri delle linee */
            data.c1 = pid;      /* salvo nella struct il pid del nipote */
            returnValue = 0;    /* numero delle linee lette */

            while((read(p[0], &(data.c3[k]), 1)) > 0){    /* leggo la linea carattere per carattere */
                printf("carattere corrente = %c\n", data.c3[k]);
                k++;        /* aumento l'indice di carattere della linea */

                if(data.c3[k] == '\n'){ /* se arrivo a fine linea */

                    returnValue++;  /* incremento il contatore di linee lette */            
                    data.c2 = returnValue;      
                    data.c3[k] = '\0';  /* converto in stringa per facilitare il processo padre */

                    write(piped[i][1], &data, sizeof(data)); /* scrito la struct sulla pipe figlio-padre */
                    k = 0;  /* azzero l'indice per la prossima linea */
                }
            }

            /* il processo figlio aspetta il processo nipote */
            /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
            pidFiglio = wait(&status);
            if(pidFiglio < 0){
                printf("Errore in wait.\n");
                exit(-1);
            }
            
            /* controllo il valore di uscita del processo figlio */
            /* se status & 0xFF == 0 allora il figlio è terminato correttamente */
            if ((status & 0xFF) != 0){
                printf("Processo nipote con pid %d terminato in modo anomalo.\n", pidFiglio);
                exit(-1);   /* in modo che venga interpretato come errore dal padre */
            }
            else {
                printf("Il processo nipote di pid %d ha ritornato %d.\n", pidFiglio, returnValue);
            }
            
            exit(returnValue); /* esco ritornando il numero di linee lette */
        }
        
    }

    /* codice processo padre */

    /* ogni padre (consumatore) chiude tutte le pipe che non usa */
    for(k = 0; k < N; k++){
        /* TUTTI I LATI DI LETTURA VANNO LASCIATI APERTI O IL PADRE NON POTRÀ LEGGERE */
        close(piped[k][1]); /* lato di scrittura */
    }

    end = false; /* per iniziare il ciclo */
    while(!end){
        end = true; /* se il valore non cambia abbiamo finito */
        for(i = 0; i < N; i++){ /* itero su tutti i figli */
            if((read(piped[i][0], &data, sizeof(data))) != 0){ /* provo a leggere da pipe */
                end = false;    /* se leggo qualcosa dalla pipe, non ho ancora finito */
                /* stampo su standard output come da specifica */
                printf("Il processo nipote %d ha letto dal file %s la linea %d:\n%s", data.c1, argv[i*2+1], data.c2, data.c3);
            }
        }
    }
    
    /* il processo padre aspetta i figli */
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
            printf("Il processo figlio di pid %d ha ritornato %d linee lette dal processo nipote.\n", pidFiglio, returnValue);
        }
    }

    exit(0);
}