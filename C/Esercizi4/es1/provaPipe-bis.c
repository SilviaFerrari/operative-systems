#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */

int main(int argc, char** argv){
    int fd1;        /* per la prima open */
    int fd2;        /* per la seconda open */
    int retval;     /* per la pipe */
    int piped[2];   /* per la pipe */

    /* controllo  dei parametri */
    if (argc != 3){
        printf("Errore: necessari  argomenti [nome file] [nome file] per %s.\n", argv[0]);
        exit(1);
    }
    
    /* controllo che il primo parametro sia il nome di un file esistente*/
    if ((fd1 = open(argv[1], O_RDONLY)) < 0){
        printf("Errore: il file %s non esiste.\n", argv[1]);
        exit(2);
    }

    /* controllo che il secondo parametro sia il nome di un file esistente*/
    if ((fd2 = open(argv[2], O_RDONLY)) < 0){
        printf("Errore: il file %s non esiste.\n", argv[2]);
        exit(3);
    }

    close(fd1); /* chiudiamo e quindi eliminiamo le informazioni relative all'elemento della TFA di fd1 */

    retval = pipe (piped);  /* creo la pipe */
    if(retval < 0){
        printf("Errore nella creazione pipe\n");
		exit(4); 
    }

    printf("Creata pipe con piped[0]= %d \n", piped[0]);
	printf("Creata pipe con piped[1]= %d \n", piped[1]);

    exit(0);
}