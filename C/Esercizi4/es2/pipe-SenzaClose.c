/* FILE: pipe.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MSGSIZE 5 

int main (int argc, char **argv){   
   int pid, j, piped[2]; 		/* pid per fork, j per indice, piped per pipe */
   char mess[MSGSIZE];			/* array usato dal figlio per inviare la stringa al padre */
   int fd;
   int pidFiglio, status, ritorno;      
   /* ogni processo (padre e figlio) avrà la propria AREA DATI e il contenuto di 
   questo array sarà PRIVATO di ogni processo, quindi NON serve creare due array distinti */

	if (argc != 2){	
        printf("Numero dei parametri errato %d: ci vuole un parametro [nome file].\n", argc);
    	exit(1);
	}

	/* si crea una pipe: si DEVE sempre controllare che la creazione abbia successo!  */
	if (pipe(piped) < 0 ){   
        printf("Errore creazione pipe.\n");
    	exit(2); 
	}

	/* si crea un figlio (controllando sempre!) */
	if ((pid = fork()) < 0){   
        printf("Errore creazione figlio.\n");
    	exit(3); 
	}

	if (pid == 0){ 
		close(piped[0]); 	/* il figlio CHIUDE il lato di lettura: sarà quindi lo SCRITTORE della pipe */

		if ((fd = open(argv[1], O_RDONLY)) < 0){		
			printf("Errore in apertura file %s\n", argv[1]);
            exit(-1); /* torniamo al padre un -1 interpretato come 255 e quindi errore */
		}

		printf("DEBUG: Il figlio %d scriverà messaggi di lunghezza %d sulla pipe, dopo averli letti dal file.\n", getpid(), MSGSIZE);
		j = 0; /* variabile j per contare i messaggi che manderà al padre */
		
        /* il figlio legge tutto il file passato come parametro */
		while (read(fd, mess, MSGSIZE)){    

			/* il contenuto del file è tale che in mess ci saranno 4 caratteri e il terminatore di linea */
            /* il padre ha concordato con il figlio che gli manderà solo stringhe e quindi dobbiamo */
            mess[MSGSIZE-1]='\0'; /* sostituisco il terminatore di linea con il terminatore di stringa */

			/* il figlio (produttore) comunica al padre la linea letta dal file dopo averla trasformata in stringa */
            write(piped[1], mess, MSGSIZE);	
			
            j++;	/* incrementiamo il contatore */
		}
		printf("DEBUG: Il figlio %d ha scritto %d messaggi sulla pipe.\n", getpid(), j);
		exit(j);	/* figlio deve tornare al padre il numero di linee lette che corrisponde al numero di stringhe mandate al padre, supposto < 255! */
	}

	/* codice del padre */
	close(piped[1]); /* il padre CHIUDE il lato di scrittura: sara' quindi il LETTORE della pipe */		
	printf("DEBUG: Il padre %d sta per iniziare a leggere i messaggi dalla pipe\n", getpid());
	j = 0; /* il padre inizializza la sua variabile j per verificare quanti messaggi ha mandato il figlio */
	
	/* il ciclo finirà non appena il figlio terminerà, dato che la read ritorna 0 se non ci sono scrittori*/
	while (read(piped[0], mess, MSGSIZE)){ 
		/* dato che il figlio gli ha inviato delle stringhe, 
		il padre le puo' scrivere direttamente con una printf */
        printf("%d: %s\n", j, mess);
        j++;
	}
	printf("DEBUG: Il padre %d letto %d messaggi dalla pipe\n", getpid(), j);
	
    /* padre aspetta il figlio */
	if ((pidFiglio=wait(&status)) < 0){
      	printf("Errore wait\n");
      	exit(4);
	}

	if ((status & 0xFF) != 0){
        printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
    }
	else {
       	ritorno=(int)((status >> 8) & 0xFF);
       	printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
	}

	exit(0);
}