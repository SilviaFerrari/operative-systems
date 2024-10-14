#include <stdio.h>  	/*standard input output*/
#include <unistd.h> 	/*unix standard*/
#include <stdlib.h> 	/*standard library*/
#include <sys/wait.h>	/*per la wait e le macro*/
#include <time.h>		/*per il random*/

int myRandom(int n){
    int randNum;
    randNum = rand() % n;
    return randNum;
}

int main(int argc, char **argv){
	int n;				/*per salvare il parametro*/
	int rnum;			/*per il valore del random*/
	int i, j;			/*indice per il ciclo for dei figli*/
	int *pid;        	/*array dinamico per il valore di ritorno della fork*/
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
	if (n <= 0 || n >= 155){
        	printf("Errore: Il secondo parametro non è un numero strettamente positivo.\n");
        	exit(2);
	}

	/*stampo il pid del processo corrente e il parametro passato al programma*/
	printf("Il pid del processo corrente è: %d. Il parametro passato è: %d.\n", getpid(), n);

	srand(time(NULL));  /*inizializzo il seme del random*/

	/*alloco la memoria necessaria per i pid sulla base del valore di n*/
	pid = (int *)(malloc(n * sizeof(int)));
	if (pid == NULL){	/*controllo il valore di ritorno della malloc*/
        printf("Errore: problemi nella malloc.\n"); 
		exit(3);
	}

	/*genero n processi figli*/
	for(i = 0; i < n; i++){

		/*eseguo la creazione del processo figlio e controllo che la fork sia andata a buon fine*/
    	/*fork() restituisce al figlio 0 e al padre il pid del processo figlio*/
    	if((pid[i] = fork()) < 0){
    	    printf("Errore nella fork.");
    	    exit(4);    
    	}

		/*ora sono nel processo figlio*/
		if(pid[i] == 0){
			printf("Il pid del processo di indice %d è %d.\n", i, getpid());
			rnum = myRandom(101+i); /*l'estrazione random avviene tra 0 e 99*/
			exit(rnum);
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
			for(j = 0; j < n; j++){
				if(pidFiglio == pid[j]){
					printf("Il figlio di indice %d con pid = %d ha ritornato %d.\n", j, pidFiglio, returnValue);
				}
			}
		}
	}

	exit(0);
}