/*file contaOccorrenze.c*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int fd;     //per la open
	char Cx;    //per salvare il singolo carattere
	char c;     //per scorrere il file
	int total;	//per contare il numero totale di occorrenze
	
	//controllo dei parametri
	if(argc != 3)
	{
		printf("Errore: i parametri devono essere 2 per %s e non %d.\n", argv[0], argc);
		exit(1);
	}

	//controllo del primo parametro
	if((fd=open(argv[1], O_RDONLY)) < 0)
	{
		printf("Errore: il file %s non esiste.\n", argv[1]);
		exit(2);
	}

	//controllo del secondo parametro
	if(strlen(argv[2]) != 1)
	{
		printf("Errore: il parametro %s deve essere un singolo carattere.\n", argv[2]);
		exit(3);
	}

	//assegno il carattere ad una varibile per comodità
	Cx=argv[2][0];

	while(read(fd, &c, 1) != 0)
	{
		if(c == Cx){
			total++;
		}
	}

	printf("Il numero totale di occorrenze del carattere %c nel file %s è %d.\n", Cx, argv[1], total);
	exit(0);
}
