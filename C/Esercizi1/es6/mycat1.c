#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv){       
    int nread;                      //valore ritorno read
	char buffer[BUFSIZ];            //usato per i caratteri
	int fd = 0;			            //per la open
    int i, j;                          //per iterare tutti parametri

    //scorro tutti i parametri con un ciclo for
    for(j = 0; i < argc; j++){

        //se abbiamo almeno 2 paramtri, allora controlliamo i file con un seconod for
        if(argc >= 2){
            for(i = 1; i < argc; i++){
                if((fd = open(argv[i], O_RDONLY)) < 0){
                    printf("Errore in apertura file %s dato che fd = %d\n", argv[i], fd);
                    exit(1);
                }
            }

        }

        //se abbiamo solo un parametro fd sarà sempre 0
        
        //ora leggiamo dal file o dallo stndard input fino a che ci sono caratteri
        while((nread = read(fd, buffer, BUFSIZ)) > 0){
            //scrivo sullo standard output i caratteri letti
            write(1, buffer, nread);
        }
    }
	exit(0);
}