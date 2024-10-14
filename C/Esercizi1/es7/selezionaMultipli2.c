#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv){
    int fd; //per la open
    long int pos;
    long int len;
    int m = 0; //indicatore del multiplo
    char c; //per leggere i caratteri dal file

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

    //calcolo la lunghezza del file
    len = lseek(fd, 0L, SEEK_END);

    pos = 0L; //inizializzazione per entrare nel ciclo
    //0L --> inizio del file

    //leggo il file
    while(pos < len){
        m++;
        pos = m * n;

        if(pos < len){ //controlliamo che la posizione sia ancora valida
            lseek(fd, pos-1, SEEK_SET);
            read(fd, &c, 1);
            printf("Il carattere multiplo %d-esimo all'interno del file %s è %c.\n", m, argv[1], c);
        } else{
            printf("Il file ha una dimensione multipla di %d.\n", n);
        }
    }
    exit(0);
}