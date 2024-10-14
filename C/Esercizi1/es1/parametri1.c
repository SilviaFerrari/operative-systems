#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{	
	int i; /*indice per scorrere i parametri*/
       int N; /*per salvare il numero dei parametri*/	

	/*controllo del numero di parametri*/
	if (argc > 2)
	{
		printf("Errore nel numero di parametri: deve essere solo 1.");
		exit(1);
	}
	
	N = argc -1;  /*salvo il numero di parametri*/

	printf("Eseguo il programma %s con %d parametri\n", argv[0], N);

	for(i = 1; i <= N; i++)
	{
		printf("Il parametro di indice %d è %s\n", i, argv[i]);
	}

	exit(0);
}
