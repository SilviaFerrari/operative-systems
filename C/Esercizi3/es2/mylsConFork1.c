/* FILE: myls1.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>  	/*file control*/
#include <sys/wait.h>	/*per la wait e le macro*/

int main (int argc, char **argv)
{
    int pid;
	int pidFiglio;
	int status;
	int returnValue;
    int fd;

	/*controllo stretto del numero di parametri*/
    if (argc != 2){ 	
		printf("Errore: necessario un solo agomento [nome file/nome directory] per %s.\n", argv[0]); 
		exit(1); 
	}

    /*controllo parametro che deve essere il nome di un file o di una cartella*/
	if ((fd = open(argv[1], O_RDONLY)) < 0)
	{
		printf("Errore: il file %s non esiste.\n", argv[1]);
		printf("Errore in execlp\n");
		exit(2);
	}

	/*eseguo la creazione del processo figlio e controllo che la fork sia andata a buon fine*/
    /*fork() restituisce al figlio 0 e al padre il pid del processo figlio*/
    if((pid = fork()) < 0){
        printf("Errore nella fork.");
        exit(3);    
    }

    /*ora sono nel processo figlio*/
    if(pid == 0){
        printf("Il pid del processo figlio è %d, creato dal processo padre con pid %d.\n", getpid(), getppid());
        printf("DEBUG: esecuzione del comando ls -l sul parametro %s.\n", argv[1]);

        /*IL PROCESSO SOTTO-SHELL USA SEMPRE LA VERSIONE CON P*/
		execlp("ls", "ls", "-l", argv[1], NULL);
        exit(-1);  /*termino il processo figlio*/
    }

	pidFiglio = wait(&status);
    if(pidFiglio < 0){
        printf("Errore nella wait %d.\n", pidFiglio);
        exit(4);
    }

    /*controllo il valore di uscita del processo filgio */
    /*se status & 0xFF == 0 allora il figlio è terminato correttamente %d*/
    if((status & 0xFF) != 0){   /*0xFF = 11111111*/
        printf("Figlio terminato in modo involontario.\n");
        /*non facciamo exit con errore, il problema è solo del figlio, non del padre*/
    } 
    else{
        returnValue = (int)((status >> 8) & 0xFF);    /*salvo il valore di ritorno del figlio*/
        printf("Il figlio con pid = %d ha ritornato %d.\n", pidFiglio, returnValue);
    }

	exit(0);	
}