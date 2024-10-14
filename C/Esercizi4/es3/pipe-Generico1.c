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
    int i, j;           /* contatori delle linee passate e dei singoli caratteri*/
    int fd;             /* per la open */
    char mess[512];    /* per contenere la linea letta */

    /* controllo  dei parametri */
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
        close(piped[0]); /* chiudo il lato di lettura, dato che il figlio ha ruolo di produttore */

        /* controllo che il parametro passato sia il nome di un file esistente*/
        if ((fd = open(argv[1], O_RDONLY)) < 0){
            printf("Errore: il file %s non esiste.\n", argv[1]);
            exit(-1);
        }

        printf("DEBUG: il figlio sta per leggere le linee del file e le scriverà sulla pipe.\n");
        i = 0;
        j = 0; /* inizializzo il contatore a 0 */

        /* il figlio legge dal file e scrive le linee sulla pipe*/
        while((read(fd, &(mess[i]), 1)) != 0){
            if(mess[i] == '\n'){
                /* ho raggiunto la fine della linea */
                mess[i] = '\0';                 /* sostituisco il terminatore di linea in terminatore di stringa */
                i++;                            /* incremento i per tenere conto del terminatore */
                write(piped[1], &i, sizeof(i)); /* comunico al pocesso padre la lunghezza della stringa*/
                write(piped[1], mess, i);       /* scrivo sulla pipe */
                j++;                            /* incremento il contatore delle linee */
                i = 0;                          /* azzero l'indice dei caratteri */
            }
            else{
                i++;    /* incremento l'indice dei caratteri letti */
            }
        }
        printf("DEBUG: il processo figlio %d ha scritto %d messaggi sulla pipe.\n", getpid(), j);
        exit(j);
    }

    /* processo padre */
    close(piped[1]);    /* chiudo il lato di scrittura */
    printf("DEBUG: il processo padre %d sta per leggere dalla pipe.\n", getpid());
    j = 0;  /* inizializzo j a 0 per vrificare il numero di messaggi letti */

    /* il processo padre legge dalla pipe */
    while(read(piped[0], &i, sizeof(i))){
        /* ricevuta la lunghezza della stringa, posso procedere a leggerle */
        read(piped[0], mess, i);
        /* stampo la stringa letta dalla pipe con numerazione crescente */
        printf("%d: %s\n", j, mess);
        j++;    /* incremento il numero di messaggi letti */
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