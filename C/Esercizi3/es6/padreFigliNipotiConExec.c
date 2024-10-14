#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <string.h>    /* funzioni stringhe */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <time.h>      /* per il random */
#include <sys/wait.h>  /* per la wait e le macro */
#include <sys/stat.h>  /* per la pipe */

#define PERM 0644

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int n;              /* per salvare il numero di nomi di file */
    int i;              /* indice per i processi figli */
    int fout;           /* per la creat */
    char *nameOut;         /* array dinamico di char per poter dare il nome al nuovo file */
    
    /* controllo lasco dei parametri */
    if (argc < 4){
        printf("Errore: necessari almeno 4 argomenti [nomi di file] per %s.\n", argv[0]);
        exit(1);
    }
    
    n = argc - 1;   /* calcolo e salvo in n il numero di file passati */

    for(i = 0; i < n; i++){
        if((pid = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork.\n");
            exit(2);
        }
        
        /* se pid == 0 la fork() ha avuto successo e possiamo eseguire il codice del processo figlio */
        if (pid == 0){
            /* alloco la memoria necessaria per il nome sulla base del valore di n */
            nameOut = (char *)(malloc(strlen(argv[i+1]) + 6)); /* c'è il terminatore di stringa!!! */
            if (nameOut == NULL){    /* controllo il valore di ritorno della malloc */
                printf("Errore: problemi nella malloc.\n");
                exit(-1);
            }
            
            nameOut = strcat(argv[i+1], ".sort");  /* formo il nome */
            /* controllo che il parametro sia il nome di un file esistente*/
            if ((fout = creat(nameOut, PERM)) < 0){
                printf("Errore nella creazione del file del processo di indice %d.\n", i);
                exit(-1);
            }
			close(fout); /* chiudiamo il file creato dato che il figlio non lo usa */

            /* creazione del nipote */
            if((pid = fork()) < 0){
                /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
                printf("Errore nella fork.\n");
                exit(-1);
            }
            
            /* se pid == 0 la fork() ha avuto successo e possiamo eseguire il codice del processo nipote */
            if (pid == 0){
                close(0);   /* chiudiamo lo standard input per usare il comando filtro sort */
                /* controllo che il parametro sia il nome di un file esistente*/
                if ((open(argv[i+1], O_RDONLY)) < 0){
                    printf("Errore: il file %s non esiste.\n", argv[i+1]);
                    exit(-1);
                }

                /* ridirezione dello standard output su /dev/null */
                close(1);
                if ((open(nameOut, O_WRONLY)) < 0){  /* controllo che sia possibile scrivere */
                    printf("Errore: non si riesce ad aprire il file %s in scrittura.\n", nameOut);
                    exit(-1);
                } 
                
                execlp("sort", "sort", (char *)0); /* per eseguire un comando esistente*/
                printf("Errore: execlp fallita.\n");  /* sarà eseguita solo se execlp fallisce */
                exit(-1);   /* ritorniamo al padre -1 = 255 che sarà interpretato come errore */
            }
            
            /* riprende il codice del figlio che deve aspettare il nipote */
            /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
            pidFiglio = wait(&status);
            if (pidFiglio < 0){
                printf("Errore in wait.\n");
                exit(-1);
            }
            
            /* controllo il valore di uscita del processo filgio */
            /* se status & 0xFF == 0 allora il nipote è terminato correttamente */
            if ((status & 0xFF) != 0){
                printf("Processo nipote con pid %d terminato in modo anomalo.\n", pidFiglio);
                exit(-1);
            }
            else {
                returnValue = (int)((status >> 8) & 0xFF);
            }
            exit(returnValue);
            /* fine codice del figlio */
        }
        
    }

    for(i = 0; i < n; i++){
        /* il processo padre aspetta il processo figlio */
        /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
        pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore in wait.\n");
            exit(3);
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