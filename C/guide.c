/* FILE: .c */
#include <stdio.h>  	/*standard input output*/
#include <unistd.h> 	/*unix standard*/
#include <stdlib.h> 	/*standard library*/
#include <fcntl.h>  	/*file control*/
#include <string.h> 	/*funzioni stringhe*/
#include <time.h>		/*per il random*/
#include <sys/wait.h>	/*per la wait e le macro*/

#define PERM 0644
#define MSGSIZE 5 

int myRandom(int n){
    int randNum;
    randNum = rand() % n;
    return randNum;
}

int main(){
	int pid;        	/*per il valore di ritorno della fork*/
    int pidFiglio;  	/*per il valore di ritorno della wait*/
    int num;        	/*per il valore random*/
    int returnValue;    /*per filtrare il valore d'uscita del processo figlio*/
    int status;     	/*da usare nella wait*/
	int fd;				/*per la open del file*/


	srand(time(NULL));  /*inizializzo il seme del random(da fare nel processo padre)*/
	num = myRandom(100); /*l'estrazione random avviene tra 0 e 99*/ 


    /*stampo il valore di BUFSIZ*/
	printf("Il valore di BUFSIZ is %d\n", BUFSIZ);


    /* controllo del numero di parametri */
    if (argc != 3){ 	
		printf("Errore: necessari 2 argomenti [] [] per %s.\n", argv[0]); 
		exit(1); 
	}


	/*controllo parametro tipo nome file*/
	if ((fd = open(argv[1], O_RDONLY)) < 0){
		printf("Errore: il file %s non esiste.\n", argv[1]);
		exit(2);
	}


	/*controllo parametro intero positivo*/
	n = atoi(argv[2]); /*converto il secondo parametro*/
	if (n <= 0){
        	printf("Errore: Il secondo parametro non è un numero strettamente maggiore di 0.\n");
        	exit(3);
	}


    /*controllo parametro singolo carattere*/
    if (strlen(argv[2]) != 1){ 	
        printf("Errore: %s non è un singolo carattere.\n", argv[2]);
		exit(3); 
	}

	
    /*buffer dinamico per leggere i caratteri dal file*/
    char *buffer;
    /*alloco la memoria necessaria per il buffer sulla base del valore di n*/
	buffer = (char *)(malloc(n * sizeof(char)));
	if (buffer == NULL){	/*controllo il valore di ritorno della malloc*/
        printf("Errore: problemi nella malloc.\n"); 
		exit(4);
	}


    /*per calcolare la lunghezza del file*/
    length = lseek(fd, 0L, SEEK_END);   /*SEEK_END --> fine file*/

    /*per spostarsi alla posizione desiderata nel file*/
    lseek(fd, position, SEEK_SET);      /*SEEK_SET --> inizio file*/

    /*per retrcedere di una posizione nel file*/
    lseek(fd, -1L, SEEK_CUR);           /*SEEK_CUR --> posizione corrente*/


	/*execl(percorso assoluto del comando da invocare, argomenti da passare, terminatore)*/
	/*se non dobbiamo passare nessun parametro dobbiamo comunque mettere un argomento vuoto*/
	/*il terminatore deve essere un putatore a NULL, non va bene mettere 0*/
	execl("prova", "", (char *)0);


	exit(0);
}