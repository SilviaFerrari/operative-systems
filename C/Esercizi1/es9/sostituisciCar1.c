#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv){

    int fd; //per la open
    char ch; //per salvare il carattere che ci interessa trovare
    char cc; //carattere corrente letto
    char change; //per salvare il carattere da sostituire

    //controllo dei parametri
    if(argc != 4){
        printf("Errore: i parametri devono essere 3 per %s e non %d.\n", argv[0], argc);
        exit(1);
    }

    //controllo del primo parametro che deve essere un nome di file
    if((fd = open(argv[1], O_RDONLY)) < 0){
        printf("Errore: il parametro %s non è un file esistente.\n", argv[1]);
        exit(2);
    }

    //controllo del secondo parametro che deve essere un singolo carattere
    if(strlen(argv[2]) != 1){
        printf("Errore: il parametro %s deve essere un singolo carattere.\n", argv[2]);
		exit(3);
    }
    ch = argv[2][0];

    //controllo del terzo parametro che deve essere un singolo carattere
    if(strlen(argv[3]) != 1){
        printf("Errore: il parametro %s deve essere un singolo carattere.\n", argv[3]);
		exit(4);
    }
    change = argv[3][0];
    
    while(read(fd, &cc, 1) != 0){
        if(cc == ch){
            lseek(fd, -1L, 1); //riporto indietro di uno il file pointer
            //posso anche usare SEEK_CUR come terzo parametro al posto dell'1
            write(fd, &change, 1); //scrivo il carattere spazio al posto del precedente
        }
    }

    exit(0);
}