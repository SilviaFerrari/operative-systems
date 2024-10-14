#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv){
    int n;  //per salvare il numero di righe da leggere
    int i = 1; //per tenere traccia delle righe mostrate
    char c; //per salcare il carattere corrente

    //controllo del numero di parametri
    if(argc > 2){
        printf("Errore: i parametri devono essere al massimo 1 per %s e non %d.\n", argv[0], argc);
        exit(1);
    }

    //controlliamo se c'è o meno il parametro
    if(argc == 2){
        //cotrollo del parametro
        if(argv[1][0] != '-'){
            printf("Errore: il parametro non è stato inserito correttamente, deve iniziare con '-'.\n");
            exit(1);
        }

        n = atoi(&(argv[1][1]));  //convertiamo il numero che parte dal secondo carattere
        if(n <= 0){
            printf("Errore: l'opzione non è corretta.\n");
            exit(2);
        }
    } else{
        n = 10;
    }
    
    while(i <= n && read(0, &c, 1)){
        if(c == '\n'){
            i++;
        }
        write(1, &c, 1);
    }

    exit(0);
}