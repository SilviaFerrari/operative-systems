#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <string.h>    /* funzioni stringhe */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <time.h>      /* per il random */
#include <sys/wait.h>  /* per la wait e le macro */
#include <sys/stat.h>  /*  */
#include <ctype.h>     /* per usare isdigit() e isalpha() */

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int i, k, n;           /* incide dei processi figli e delle pipe */
    pipe_t piped;       /* per la pipe tra figlio e nipote */
    pipe_t *pf;        /* per creare un numero variabile di pipe tra figlio e padre */
    long int length;         /* per salvare la lunghezza del file */
    int long total = 0;          /* per calcolare il totare di linee lette */
    char lines[11];       /* array di caratteri per memorizzare la stringa corrispondente al numero di righe */
    
    /* controllo lasco dei parametri */
    if (argc < 3){
        printf("Errore: necessari almeno 2 argomenti [nomi file] per %s.\n", argv[0]);
        exit(1);
    }
    
    n = argc - 1;   /* calcolo il numero di file passati */

    /* alloco la memoria necessaria per l'array di pipe_t sulla base del valore di n */
    pf = (pipe_t *)(malloc(n * sizeof(pipe_t)));
    if (pf == NULL){    /* controllo il valore di ritorno della malloc */
        printf("Errore nella creazione dell'array di pipe_t.\n");
        exit(2);
    }

    /* creazione delle n pipe figlio-padre */
    for(i = 0; i < n; i++){
        /* creo una pipe e controllo che la creazione abbia successo */
        if (pipe(pf[i]) < 0 ){
        /* la pipe ha fallito, riporto errore */
            printf("Errore creazione pipe di indice %d.\n", i);
            exit(3);
        }
    }
    
    /* creo gli n processi figli */
    for (i = 0; i < n; i++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork del figlio.\n");
            exit(4);
        }
        
        /* se pid == 0 la fork() ha avuto successo */
        if (pid == 0){
            /* codice processo figlio */
            printf("DEBUG: Il processo figlio di pid %d è stato creato dal processo padre %d.\n", getpid(), getppid());
                        
            /* ogni figlio (produttore) chiude tutte le pipe che non usa */
            for(k = 0; k < n; k++){
                close(pf[k][0]); /* lato di lettura */
                if(k != i){
                    close(pf[k][1]); /* lato di scrittura */
                }
            }

            /* creo la pipe per comunicare con il nipote */
            if (pipe(piped) < 0 ){
            /* la pipe ha fallito, riporto errore */
                printf("Errore creazione pipe.\n");
                exit(-1);
            }
            
            /* creo il processo nipote e controllo che la fork sia andata a buon fine */
            if((pid = fork()) < 0){
                /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
                printf("Errore nella fork del nipote.\n");
                exit(-1);
            }
            
            /* se pid == 0 la fork() ha avuto successo */
            if (pid == 0){
            /* codice processo nipote */
                printf("DEBUG: Il processo nipote di pid %d è stato creato dal processo figlio %d.\n", getpid(), getppid());
                
                /* chiusura della pipe figlio-padre rimasta aperta che il nipote però non usa */
				close(pf[i][1]);

                /* ridirezione dello standard input */
                close(0);

                /* controllo che il file passato come parametro esista */
                if (open(argv[i+1], O_RDONLY) < 0){
                    printf("Errore: il file %s non esiste.\n", argv[i+1]);
                    exit(-1);   /* ritorniamo al padre -1 = 255 che sarà interpretato come errore */
                }
                
                /* ridirezione dello standard output e simulazione del piping  */
                close(1);
                dup(piped[1]);  /* ora il nipote può chiudere entrambi i lati della pipe */
                                
                /* ogni nipote ora può chiudere entrambi i lati della pipe:
                il lato di lettura 0 non viene usato
                il lato di scrittura 1 viene usato dallo standard output */
                
                close(piped[0]);
                close(piped[1]);

                /* ridirezione di standard error su /dev/null */
                close(2);
                open("/dev/null", O_WRONLY);    

                /* eseguo il comando wc con l'opzione -l per contare le linee del file */
                /* al processo figlio verrà quindi ritornata una stringa */
                /* IN QUESTO CASO NON BIOGNA PASSARE IL NOME DEL FILE */
                execlp("wc", "wc", "-l", (char *)0); /* per eseguire il comando wc -l */
                
                printf("Errore: execlp fallita.\n");  /* sarà eseguita solo se execlp fallisce */
                exit(-1);   /* ritorniamo al padre -1 = 255 che sarà interpretato come errore */
            }

            /* codice processo figlio */
            close(piped[1]);    /* chiudo il lato di scrittura perchè il figlio è consumatore */

            /* leggo dalla pipe un carattere alla volta dalla pipe del nipote */
            k = 0;  /* per calcolare quanti caratteri vengono letti */
            while (read(piped[0], &(lines[k]), 1)){
				k++;
			}

            /* ora in lines ci sono i caratteri dei numeri e il terminatore di linea */
            if(k != 0){
                lines[k-1] = '\0';  /* sostituisco \n col terminatore di stringa \0 */
                length = atoi(lines);
            }
            else{
                length = 0; /* caso in cui il nipote sia incorso in un errore */
            }

            write(pf[i][1], &length, sizeof(length));   /* scrivo sulla pipe del padre la lunghezza calcolata */
            
            /* il processo figlio deve aspettare il processo nipote */
            returnValue = -1;   /* in caso di errore */
            /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
            pid = wait(&status);
            if (pid < 0){
                printf("Errore in wait.\n");
                exit(-1);
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
            
            exit(returnValue);
        }
    }

    /* codice processo padre */
    /* ogni padre (consumatore) chiude tutte le pipe che non usa */
    for(k = 0; k < n; k++){
        /* TUTTI I LATI DI LETTURA VANNO LASCIATI APERTI O IL PADRE NON POTRÀ LEGGERE */
        close(pf[k][1]); /* lato di scrittura */
    }

    /* recupero le informazioni dei figli in ordine di indice */
    for(i = 0; i < n; i++){
        read(pf[i][0], &length, sizeof(length));
        printf("DEBUG: Il processo figlio di indice %d ha convertito il valore %ld per il file %s.\n", i, length, argv[i+1]);
        total = total + (long int)length;
    }

    printf("La somma risultate di tutte le linee dei file è %ld.\n", total);

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
            if(returnValue == 255){
                printf("Il processo figlio %d ha ritornato %d: problemi nel figlio o nel nipote o nel comando wc.\n", pid, returnValue);
            }
            else{
                printf("Il processo figlio di pid %d ha ritornato %d.\n", pid, returnValue);
            }
        }
    }
    
    exit(0);
}