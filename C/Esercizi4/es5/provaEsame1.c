#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <string.h>    /* funzioni stringhe */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <time.h>      /* per il random */
#include <sys/wait.h>  /* per la wait e le macro */
#include <ctype.h> 	   /* per usare isdigit() e isalpha() */

int main(int argc, char** argv){
    int pid;            /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int fd;             /* per la open */
    int i, n;           /* indice dei processi figli */
    int piped[2][2];    /* array statico di due pipe */
    int tot;            /* per calcolare il totale dei caratteri */
    char cx;
    char nr0, nr1;      /* numero caratteri letti dalla pipe pari e numero caratteri letti dalla pipe dispari */
    char ch0, ch1;      /* varibiali per salvare i caratteri letti dalle due pipe */

    /* controllo lasco dei parametri */
    if (argc < 3){
        printf("Errore: necessari almeno 2 argomenti [nomi file] per %s.\n", argv[0]);
        exit(1);
    }

    n = argc -1;    /* salvo il numero di file passati */

    /* creo due pipe e controllo che la creazione abbia successo */
    if (pipe(piped[0]) < 0 ){   /* prima pipe per dispari*/
        printf("Errore creazione prima pipe.\n");
        exit(2);
    }

    if (pipe(piped[1]) < 0 ){   /* seconda pipe per pari*/
        printf("Errore creazione seconda pipe.\n");
        exit(3);
    }

    /* creo i processi figli */
    for(i = 0; i < n; i++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork.\n");
            exit(2);
        }
        
        /* se pid == 0 la fork() ha avuto successo e possiamo eseguire il codice del processo figlio */
        if (pid == 0){            
            /* chiudo i lati di lettura di entrambe le pipe*/
            close(piped[0][0]);     
            close(piped[1][0]);

            /* chiudiamo anche il lato di scrittura della pipe non usata dal file (in base se è pari o dispari) */
            close(piped[i%2][1]);   /* se pari si chiude la 0, se dispari si chiude la 1*/

            /* controllo che il parametro passato sia il nome di un file esistente*/
            if ((fd = open(argv[i+1], O_RDONLY)) < 0){
                printf("Errore: il file %s non esiste.\n", argv[i+1]);
                exit(-1);
            }
            
            while((read(fd, &cx, 1)) != 0){
                if(((i%2) == 0 && isalpha(cx)) || ((i%2) == 1 && isdigit(cx))){
                    write(piped[(i+1)%2][1], &cx, 1);   /* scrivo sulla pipe il carattere */
                }
            }
            exit(cx);   /* si conclude il codice del figlio */
        }        
    }
    
    /* codice del padre */

    /* chiudo i lati di scrittura di entrambe le pipe*/
    close(piped[0][1]);     
    close(piped[1][1]);

    printf("I caratteri che sono stati inviati dai figli sono:\n"); 
  	/* si deve continuare a leggere dalle due pipe anche se non si riesce a garantire */
    /* l'alternanza in output a causa del termine dei processi pari o dei processi dispari */
    /* appena però finiscono entrambi i 'tipi' di processi il padre deve smettere di leggere dalle pipe */

  	nr0 = read(piped[0][0], &ch0, 1);   /* nr0 per capire se ho letto o no dei caratteri con la read*/
  	nr1 = read(piped[1][0], &ch1, 1);   /* nr1 per capire se ho letto o no dei caratteri con la read*/

  	while ((nr0 != 0) || (nr1 != 0))    /* controllo di aver letto qualcosa */
  	{
    		tot = tot + nr0 + nr1; 
            write(1, &ch1, nr1);   /* prima un alfabetico e poi un numerico (secondo la specifica) */
    		write(1, &ch0, nr0);
    		nr0 = read(piped[0][0], &ch0, 1);
    		nr1 = read(piped[1][0], &ch1, 1);
  	}
  	printf("\nNumero di caratteri scritti sullo standard output: %d\n", tot); /* stampa finale del numero di caratteri scritti; N.B. la iniziamo con uno \n per chiarezza */
	
    /* wait */
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