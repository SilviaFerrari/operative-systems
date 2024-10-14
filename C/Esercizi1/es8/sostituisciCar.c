#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv){

    int fd; //per la open
    char ch; //per salvare il carattere che ci interessa
    char cc; //carattere corrente letto

    //controllo dei parametri
    if(argc != 3){
        printf("Errore: i parametri devono essere 2 per %s e non %d.\n", argv[0], argc);
        exit(1);
    }

    //controllo del primo parametro che deve essere un nome di file
    if((fd = open(argv[1], O_RDWR)) < 0){
        printf("Errore: il parametro %s non è un file esistente.\n", argv[1]);
        exit(2);
    }

    //controllo del secondo parametro che deve essere un singolo carattere
    if(strlen(argv[2]) != 1){
        printf("Errore: il parametro %s deve essere un singolo carattere.\n", argv[2]);
		exit(3);
    }

    char *blanck = " ";  //definisco la variabile che contiene il carattere spazio
    ch = argv[2][0];    //assegno il carattere che dobbiamo trovare

    //leggo il file
    while(read(fd, &cc, 1)){
        if(cc == ch){
            lseek(fd, -1L, 1); //riporto indietro di uno il file pointer
            //posso anche usare SEEK_CUR come terzo parametro al posto dell'1
            write(fd, &blanck, 1); //scrivo il carattere spazio al posto del precedente
        }
    }
    exit(0);
}
