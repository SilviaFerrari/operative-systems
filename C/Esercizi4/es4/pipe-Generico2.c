#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <sys/wait.h>  /* per la wait e le macro */

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int piped[2];       /* per la pipe */      
    int j, i;
    int fd; 
    char mess[512];
    
    /* controllo stretto dei parametri */
    if (argc != 2){
        printf("Errore: necessario un solo argomento [nome file] per %s.\n", argv[0]);
        exit(1);
    }
    
    /* creo una pipe e controllo che la creazione abbia successo! */
    if (pipe(piped) < 0 ){
        printf("Errore creazione pipe.\n");
        exit(2);
    }
    
    /* creo il processo figlio e controllo che la fork sia andata a buon fine */
    /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
    if((pid = fork()) < 0){
        /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
        printf("Errore nella fork.\n");
        exit(3);
    }
    
    /* se pid == 0 la fork() ha avuto successo e possiamo eseguire il codice del processo figlio */
    if (pid == 0){
        printf("DEBUG: Il pid del processo figlio è %d, creato dal processo padre con pid %d.\n", getpid(), getppid());
        
        close(piped[0]);    /* chiudo il lato di lettura, dato che il figlio ha ruolo di produttore */

        /* controllo che il parametro passato sia il nome di un file esistente*/
        if ((fd = open(argv[1], O_RDONLY)) < 0){
            printf("Errore: il file %s non esiste.\n", argv[1]);
            exit(-1);
        }

        i = 0;
        j = 0;
        printf("DEBUG: Il figlio sta per iniziare a leggere dal file.\n");

        /* leggo carattere per carattere dal file */
        while((read(fd, &(mess[i]), 1)) != 0){
            if(mess[i] == '\n'){    /* controllo se sono arrivata a fine linea */
                mess[i] = '\0';     /* ostituisco il terminatore di linea con quello di stringa*/
                i++;                /* aumento la dimensione della stringa*/
                write(piped[1], mess, i);   /*comunico direttamente la stringa alla pipe*/
                i = 0;      /* azzero la lunghezza */
                j++;        /* incremento il numero di linee lette*/
            }
            else{
                j++;
            }
        }
        printf("DEBUG: Il figlio %d ha scritto %d messaggi sulla pipe.\n", getpid(), j);
        exit(j);
    }
    
    /* codice del padre */
    close(piped[1]);    /* chiudo il lato di scrittura, dato che il padre è il consumatore */

    printf("DEBUG: Il processo padre sta per leggere dalla pipe.\n");
    while(read(piped[0], &(mess[i]), 1)){
        if(mess[i] == '\0'){ /* siamo arrivati alla fine di una stringa */
            /* dato che il figlio ha inviato delle stringhe, il padre può scrivere con una printf */
            printf("%d: %s\n", j, mess);
            j++;
 			i = 0;  /* azzeriamo l'indice per la prossima linea */
        }
        else{
            i++;
        }
    }
    
    /* il processo padre aspetta il processo figlio */
    /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
    pidFiglio = wait(&status);
    if (pidFiglio < 0){
        printf("Errore in wait.\n");
        exit(4);
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
    
    exit(0);
}