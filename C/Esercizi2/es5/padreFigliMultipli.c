#include <stdio.h>  	/*standard input output*/
#include <unistd.h> 	/*unix standard*/
#include <stdlib.h> 	/*standard library*/
#include <time.h>		/*per il random*/
#include <sys/wait.h>	/*per la wait e le macro*/

int main(int argc, char **argv){
	int n;				/*per salvare il parametro*/
	int i;				/*indice per il ciclo for dei figli*/
	int pid;        	/*per il valore di ritorno della fork*/
    int pidFiglio;  	/*per il valore di ritorno della wait*/
    int returnValue;    /*per filtrare il valore d'uscita del processo figlio*/
    int status;    		/*da usare nella wait*/

	/*controllo stretto del numero di parametri*/
    if (argc != 2){ 	
		printf("Errore: necessario 1 argomento, numero intero > 0 e < 255, per %s.\n", argv[0]); 
		exit(1); 
	}

	/*controllo parametro intero positivo*/
	n = atoi(argv[1]); /*converto il secondo parametro*/
	if (n <= 0 || n >= 255){
        	printf("Errore: Il secondo parametro non è un numero strettamente positivo.\n");
        	exit(2);
	}

	/*stampo il pid del processo corrente e il parametro passato al programma*/
	printf("Il pid del processo corrente è: %d. Il parametro passato è: %d.\n", getpid(), n);

	/*genero n processi figli*/
	for(i = 0; i < n; i++){

		/*eseguo la creazione del processo figlio e controllo che la fork sia andata a buon fine*/
    	/*fork() restituisce al figlio 0 e al padre il pid del processo figlio*/
    	if((pid = fork()) < 0){
    	    printf("Errore nella fork.");
    	    exit(3);    
    	}

		/*ora sono nel processo figlio*/
		if(pid == 0){
			printf("Il pid del processo di indice %d è %d.\n", i, getpid());
			exit(i);
		}
	}

	for(i = 0; i < n; i++){
		/*se non ci sono processi da attendere la wait ritorna -1
		altrimenti ritorna il pid del processo da attendere*/
		pidFiglio = wait(&status);
		if(pidFiglio < 0){
			printf("Errore nella wait %d.\n", pidFiglio);
			exit(4);
		}

		/*controllo il valore di uscita del processo filgio */
		/*se status & 0xFF == 0 allora il figlio è terminato correttamente %d*/
		if((status & 0xFF) != 0){   
			printf("Figlio terminato in modo involontario.\n");
			/*non facciamo exit con errore, il problema è solo del figlio, non del padre*/
		} 
		else{
			returnValue = (int)((status >> 8) & 0xFF);    /*salvo il valore di ritorno del figlio*/
			printf("Il figlio con pid = %d ha ritornato %d.\n", pidFiglio, returnValue);
		}
	}

	exit(0);
}