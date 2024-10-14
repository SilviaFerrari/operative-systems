#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv){
    int fd; //per la open
    int n;  //per salvare il secondo parametro
    int m = 0;  //per tenere conto dei multipli
    int nr; //per la read
    char *buffer; //buffer dinamico per leggere i caratteri dal file

    //controllo stretto dei parametri
    if(argc != 3){
        printf("Errore: i parametri devono essere esattamente 2 e non %d.\n", argc);
        exit(1);
    }

    //cotrollo del primo parametro
    if((fd=open(argv[1], O_RDONLY)) < 0){
        printf("Errore: il parametro %s non è un file esistente.\n", argv[1]);
        exit(2);
    }

    n = atoi(argv[2]);  //salvo e converto a numero il secondo parametro
    
    //controllo secondo parametro
    if(n <= 0){
        printf("Errore: il parametro %d non è un numero strettamente positivo.\n", n);
        exit(3);
    }

    buffer = malloc(n*sizeof(char));
    if(buffer == NULL){
        printf("Errore: problemi nella malloc.\n");
    }

    //leggo il file 
    while((nr = read(fd, buffer, n)) > 0){
        //incremento m per aggiornare il multiplo
        m++;
        //controllo se sono stati letti il numero di caratteri giusto
        if(nr == n){
            printf("Il carattere multiplo %d-esimo all'interno del file %s è %c.\n", m, argv[1], buffer[n-1]);

        }
    }
    exit(0);
}