#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <string.h>    /* funzioni stringhe */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <time.h>      /* per il random */
#include <sys/wait.h>  /* per la wait e le macro */
#include <sys/stat.h>  /*  */
#include <ctype.h>     /* per usare isdigit() e isalpha() */

/* creo la struct formata da due int e un array di char e la definisco come nuovo TIPO*/
typedef struct {
    int c1;             /* per salvare pid del processo nipote */
    int c2;             /* lunghezza linea compreso il terminatore */
    char c3[250];       /* linea ricevuta dal nipote */
} data;

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int i, k;              /* indice dei figli e delle pipe */
    int n;              /* parametri passati */
    int fd;
    data d;             /* struct */
    pipe_t *piped;     /* per creare un numero variabile di pipe */
    pipe_t fn;          /* per la pipe tra nipote e figlio */
    
    /* controllo lasco dei parametri */
    if (argc < 4){
        printf("Errore: necessari almeno 3 argomenti [nomi file] per %s.\n", argv[0]);
        exit(1);
    }
    
    n = argc - 1;   /* calcolo il numero di parametri passati */

    printf("DEBUG: In esecuzione il processo padre di pid %d.\n", getpid());

    /* alloco la memoria necessaria per l'array di pipe_t sulla base del valore di n */
    piped = (pipe_t *)(malloc(n * sizeof(pipe_t)));
    if (piped == NULL){    /* controllo il valore di ritorno della malloc */
        printf("Errore nella creazione dell'array di pipe_t.\n");
        exit(3);
    }

    /* ciclo per creare le n pipe */
    for(i = 0; i < n; i++){
        /* creo la pipe e controllo che sia avvenuto con successo */
        if (pipe(piped[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe di indice %d.\n", i);
            exit(4);
        }
    }

    /* genero n processi figli */
    for(i = 0; i < n; i++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork del figlio di indice %d.\n", i);
            exit(2);
        }
        
        /* se pid == 0 la fork() ha avuto successo */
        if (pid == 0){

        /* codice processo figlio */

            printf("DEBUG: Il processo figlio di pid %d è stato creato dal processo padre %d.\n", getpid(), getppid());
            
            /* ogni figlio (produttore) chiude tutte le pipe che non usa */
            for(k = 0; k < n; k++){
                close(piped[k][0]); /* lato di lettura */
                if(k != i){
                    close(piped[k][1]); /* lato di scrittura */
                }
            }            

            /* creo la pipe tra nipote e figlio e controllo che la creazione abbia successo */
            if (pipe(fn) < 0 ){
            /* la pipe ha fallito, riporto errore */
                printf("Errore creazione pipe.\n");
                exit(-1);
            }
                           
            /* creo il processo nipote e controllo che la fork sia andata a buon fine */
            if((pid = fork()) < 0){
                /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
                printf("Errore nella fork del nipote di indice %d.\n", i);
                exit(-1);
            }
            
            /* se pid == 0 la fork() ha avuto successo */
            if (pid == 0){

            /* codice processo nipote */

                printf("DEBUG: Il processo nipote di pid %d è stato creato dal processo figlio %d.\n", getpid(), getppid());
                
                /* chiusura della pipe figlio-padre rimasta aperta che il nipote però non usa */
                close(piped[i][1]);
                                
                /* ridirezione dello standard output e simulazione del piping  */
                close(1);
                dup(fn[1]);  /* ora il nipote può chiudere entrambi i lati della pipe */
                
                /* ogni nipote ora può chiudere entrambi i lati della pipe */
                close(fn[0]); /* il lato di lettura 0 non viene usato */
                close(fn[1]); /* il lato di scrittura 1 viene usato dallo standard output */
                  
                execlp("sort", "sort", "-f", argv[i+1], (char *)0); /* per eseguire il comando sort -f */

                perror("Errore: execl fallita.\n");  /* sarà eseguita solo se execl fallisce */
                exit(-1);   /* ritorniamo al padre -1 = 255 che sarà interpretato come errore */
            }

            /* codice processo figlio */

            close(fn[1]);   /* chiudo il lato di scrittura figlio-nipote non usato */

            d.c1 = pid;     /* salvo il pid del processo nipote */
            d.c2 = 0;
            
            /* il figlio legge la prima linea dalla pipe */
            k = 0;
            while(read(fn[0], &(d.c3[k]), 1)){
                if(d.c3[k] == '\n'){
                    d.c2++;  
                    break;      /* il resto delle righe non ci interessano, usciamo dal ciclo */
                }
                k++;
                d.c2++;
            }

            if(d.c2 != 0){
                write(piped[i][1], &d, sizeof(data));   /* comunico al padre la struct */
            }
            else{
                printf("Errore: il nipote non ha inviato nulla\n");
            }

            

            /* il processo figlio aspetta il processo  nipote */

            returnValue = -1;   /* assegnamo di default -1 in modo che si possa comunicare un errore */

            pid = wait(&status);
            if (pid < 0){
                printf("Errore in wait.\n");
                //exit(-1); il figlio deve ritornare qualcosa in ogni modo al padre
            }
            
            /* controllo il valore di uscita del processo filgio */
            /* se status & 0xFF == 0 allora il figlio è terminato correttamente */
            if ((status & 0xFF) != 0){
                printf("Processo figlio con pid %d terminato in modo anomalo.\n", pid);
                /* non facciamo exit con errore, il problema è solo del figlio, non del padre */
            }
            else {
                returnValue = d.c2 - 1; /* non includiamo il terminatore */
                printf("Il nipote di pid %d ha ritornato il valore %d.\n", pid, (int)((status >> 8) & 0xFF));
            }
            exit(returnValue);  /* ritorno la lunghezza della prima linea */
        }
    }


    /* codice processo padre */

    
    /* ogni padre (consumatore) chiude tutte le pipe che non usa */
    for(k = 0; k < n; k++){
        /* TUTTI I LATI DI LETTURA VANNO LASCIATI APERTI O IL PADRE NON POTRÀ LEGGERE */
        close(piped[k][1]); /* lato di scrittura */
    }

    for(i = 0; i < n; i++){
        fd = read(piped[i][0], &d, sizeof(data));
        if(fd != 0){
            d.c3[d.c2-1] = '\0';
            printf("Il processo nipote di pid %d ha letto dal file %s la linea '%s' di lunghezza %d (compreso terminatore).\n", d.c1, argv[i+1], d.c3, d.c2);
        }
    }

    /* il padre aspetta gli n figli */
    for(i = 0; i < n; i++){
        /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
        pid = wait(&status);
        if (pid < 0){
            printf("Errore in wait.\n");
            exit(5);
        }
        
        /* controllo il valore di uscita del processo filgio */
        /* se status & 0xFF == 0 allora il figlio è terminato correttamente */
        if ((status & 0xFF) != 0){
            printf("Processo figlio con pid %d terminato in modo anomalo.\n", pid);
            /* non facciamo exit con errore, il problema è solo del figlio, non del padre */
        }
        else {
            returnValue = (int)((status >> 8) & 0xFF);
            printf("Il processo figlio di pid %d ha ritornato %d.\n", pid, returnValue);
        }
    }

    exit(0);
}