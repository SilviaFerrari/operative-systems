#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

int main(int argc, char **argv){

    int fd; //per la open
    int n;  //numero della linea che ci interessa
    bool find = false;  //per sapere se abbiamo o meno trovato la linea
    int i = 0; //contatore delle righe del file
    char buffer[256];  //per leggere i caratteri dal file
    int j = 0;  //indice per la lunghezza del buffer

    //controllo dei parametri
    if(argc != 3){
        printf("Errore: i parametri devono essere 2 per %s e non %d.\n", argv[0], argc);
        exit(1);
    }

    //controllo del primo parametro che deve essere un nome di file
    if((fd = open(argv[1], O_RDONLY)) < 0){
        printf("Errore: il parametro %s non è un file esistente.\n", argv[1]);
        exit(2);
    }

    //controllo del secondo parametro
	n = atoi(argv[2]);  //salvo e converto a numero il secondo parametro
    if(n <= 0){
        printf("Errore: il parametro %d non è un numero strettamente positivo.\n", n);
        exit(3);
    }

    //scorro le linee del file per trovare l'n-esima
    while(read(fd, &(buffer[j]), 1) != 0){
        if(buffer[j] == '\n'){
            if(j+1 == n){
                 //rendiamo la linea una stringa
                buffer[j+1] = '\0';

                //stampiamo su standard output
                printf("La linea numero %d del file %s ha lunghezza %d: \n%s", i, argv[1], n, buffer);
                
                find = true;
            }
            else{
                j = 0;
                i++;
            }
        }
        else{
            j++;
        }
    }

    if(!find){
        printf("Non è stata trovata nessuna riga di lunghezza %d.\n", n);
    }

    exit(0);
}

