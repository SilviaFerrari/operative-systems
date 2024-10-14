/* file parametri2.c */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main  (int argc, char **argv)
{ 	
   int fd;	//per la open
   int N;	//per convertire il secondo parametro
   char C; 	//per selezionare il terzo parametro

	//controllo stretto sul numero di parametri
	if (argc != 4)	
	{ 	
		printf("Errore: Necessari esattamente 3 parametri per %s e invece argc=%d\n", argv[0], argc); 
		exit(1); 
	}

	//controllo primo parametro che deve essere il nome di un file
	if ((fd = open(argv[1], O_RDONLY)) < 0)	//proviamo ad aprire il file in sola lettura
	{
        	printf("Errore: FILE %s NON ESISTE\n", argv[1]); 
		exit(2); 	//se il ritorno della open è <0 il file non esiste
	}

	//controllo secondo parametro
	N=atoi(argv[2]);	//converto in un numero strettamente positivo
	if (N <= 0) 
	{ 	
		printf("Errore: Il secondo parametro %s non è un numero strettamente maggiore di 0\n", argv[2]); 
		exit(3); 
	}

	//controllo terzo parametro
	if (strlen(argv[3]) != 1)
	{ 	
		printf("Errore: Il terzo parametro %s non è un singolo carattere\n", argv[3]); 
		exit(4); 
	}

	//isolo il singolo carattere
	C=argv[3][0];
	printf("Eseguo il programma %s con %d parametri\n", argv[0], argc - 1); 

	printf ("Il primo parametro è il nome di un file %s.\n", argv[1]);
	printf ("Il secondo parametro e' il numero strettamente positivo %d.\n", N);
	printf ("Il terzo parametro e' il singolo carattere %c\n", C);

	exit(0);	
}
