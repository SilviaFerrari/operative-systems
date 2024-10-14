#include <stdio.h>  	/*standard input output*/
#include <unistd.h> 	/*unix standard*/
#include <stdlib.h> 	/*standard library*/
#include <fcntl.h>  	/*file control*/
#include <string.h> 	/*funzioni stringhe*/
#include <time.h>		/*per il random*/
#include <sys/wait.h>	/*per la wait e le macro*/

int main(int argc, char **argv){
    int n;
    char cx, cc;
    int i;
    int curr;
    int pid;        	/*per il valore di ritorno della fork*/
    int pidFiglio;  	/*per il valore di ritorno della wait*/
    int returnValue;    /*per filtrare il valore d'uscita del processo figlio*/
    int status;    		/*da usare nella wait*/
	int fd;				/*per la open del file*/

    /*controllo lasco del numero di parametri*/
    if (argc < 4){ 	
		printf("Errore: necessari almeno 4 argomenti per %s:\n", argv[0]); 
        printf("N nome di file e un carattere.\n");
		exit(1); 
	}
    
    /*controllo parametro singolo carattere*/
    if (strlen(argv[argc-1]) != 1){ 	
        printf("Errore: %s non è un singolo carattere.\n", argv[argc-1]);
		exit(2); 
	}
    cx = argv[argc-1][0];  /*salvo il carattere nella variabile per comodità*/

    n = argc-2; /*calcolo il numero di nomi di file*/

    /*genero n processi figli*/
    for(i = 1; i <= n; i++){

		/*eseguo la creazione del processo figlio e controllo che la fork sia andata a buon fine*/
    	/*fork() restituisce al figlio 0 e al padre il pid del processo figlio*/
    	if((pid = fork()) < 0){
    	    printf("Errore nella fork.");
    	    exit(3);    
    	}

		/*ora sono nel processo figlio*/
		if(pid == 0){
			printf("Il pid del processo di indice %d è %d.\n", i, getpid());
			
            /*controllo parametro tipo nome file*/
            if ((fd = open(argv[i], O_RDONLY)) < 0)
            {
                printf("Errore: il file %s non esiste.\n", argv[1]);
                exit(4);
            }

            /*conto le occorrenze del carattere cx nel file appena aperto*/
            while(read(fd, &cc, 1) != 0){
                if(cc == cx){
                    curr++;
                }
            }
			exit(curr);
		}
	}

    for(i = 0; i < n; i++){
		/*se non ci sono processi da attendere la wait ritorna -1
		altrimenti ritorna il pid del processo da attendere*/
		pidFiglio = wait(&status);
		if(pidFiglio < 0){
			printf("Errore nella wait %d.\n", pidFiglio);
			exit(5);
		}

		/*controllo il valore di uscita del processo filgio */
		/*se status & 0xFF == 0 allora il figlio è terminato correttamente %d*/
		if((status & 0xFF) != 0){   
			printf("Figlio terminato in modo involontario.\n");
			/*non facciamo exit con errore, il problema è solo del figlio, non del padre*/
		} 
		else{
			returnValue = (int)((status >> 8) & 0xFF);    /*salvo il valore di ritorno del figlio*/
			printf("Il figlio con pid = %d ha ritornato %d occorrenze.\n", pidFiglio, returnValue);
		}
	}

    exit(0);
}