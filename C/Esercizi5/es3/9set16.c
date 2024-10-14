#include <stdlib.h>    /* standard library */
#include <stdio.h>     /* standard input output */
#include <string.h>    /* funzioni stringhe */
#include <unistd.h>    /* unix standard */
#include <fcntl.h>     /* file control */
#include <sys/wait.h>  /* per la wait e le macro */
#include <stdbool.h>   /* per i valori booleani */

#define N 26    /* valore costante delle lettere dell'alfabeto da cercare */

/* creo la struct formata da un char e un long int e la definisco come nuovo TIPO*/
typedef struct {
    char v1;            /* per salvare il carattere cercato */
    long int v2;        /* contatore delle occorrenze */
} cstruct;

typedef int pipe_t[2];      /* definizione del TIPO pipe_t come array di 2 interi */

/* funzione per riodirnare in ordine crescente un array */
void bubbleSort(cstruct *v, int dim){
    int j;
    cstruct tmp;
    bool tidy = false;

    while(dim > 1 && !tidy){
        tidy = true;

        for(j = 0; j < dim; j++){
            if(v[j].v2 > v[j+1].v2){

                /* scambio le variabili */
                tmp = v[j];
                v[j] = v[j+1];
                v[j+1] = tmp;
                
                tidy = false;   /* segnalo che l'array non è ancora ordinato */
            }
        }
        dim--;
    }
}

int main(int argc, char** argv){
    int pid[N];         /* per il valore di ritorno della fork */
    int pidFiglio;      /* per il valore di ritorno della wait */
    int returnValue;    /* per filtrare il valore d'uscita del processo figlio */
    int status;         /* da usare nella wait */
    int i, k;           /* indice dei processi figli */
    int fd;             /* per la open */
    int rn;             /* per la write e la read */
    char cc;            /* per il carattere corrente letto dal file */
    cstruct cx[N];      /* array dinamico di struct */
    pipe_t piped[N];    /* per creare un numero variabile di pipe */

    /* controllo stretto dei parametri */
    if (argc != 2){
        printf("Errore: necessario un solo argomento [nome assoluto di file] per %s.\n", argv[0]);
        exit(1);
    }

    /* In questo caso non c'è bisogno di usare la malloc, perchè sappiamo la lunghezza degli array */
    /* creazione delle 26 pipe */
    for(i = 0; i < N; i++){
    /* creo la pipe e controllo che sia avvenuto con successo */
        if (pipe(piped[i]) < 0 ){
            /* la creazione è fallita, restituisco errore */
            printf("Errore creazione pipe di indice %d.\n", i);
            exit(2);
        }
    }
    
    printf("DEBUG: Il processo padre %d sta per generare i %d figli.\n", getpid(), N);
    
    for(i = 0; i < N; i++){
        /* creo il processo figlio e controllo che la fork sia andata a buon fine */
        /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
        if((pid[i] = fork()) < 0){
            /* la fork() ha fallito, stampo un messaggio d'errore e ritorno un valore intero d'errore */
            printf("Errore nella fork.\n");
            exit(3);
        }
        
        /* se pid == 0 la fork() ha avuto successo */
        if (pid[i] == 0){
        /* codice processo figlio */
            printf("DEBUG: Il processo figlio di pid %d è stato creato dal processo padre %d.\n", getpid(), getppid());

            /* i figli in questo caso sono sia produttori che consumatori, devo distinguere vari casi */
            for(k = 0; k < N; k++){
                /* il figlio di indice 0 chiude tutte le pipe di lettura */
                /* gli altri figli non devono chiudere il lato di lettura di indice precedente */
                if(i == 0 || k != (i-1)){
                    close(piped[k][0]); /* chiudo il lato di lettura */
                }
              
                if(k != i){
                    /* chiudo i lati di scrittura diversi da quello di indice corrente */
                    close(piped[k][1]); /* lato di scrittura */
                }
            }

            /* controllo che il parametro passato sia il nome di un file esistente */
            if ((fd = open(argv[1], O_RDONLY)) < 0){
                printf("Errore: il file %s non esiste.\n", argv[1]);
                exit(-1);
            }

            /* dal secondo processo figlio in poi, devo leggere dalla pipe l'array precedentemente ottenuto */
            if(i > 0){
                rn = read(piped[i-1][0], cx, sizeof(cx));   /* leggo dalla pipe precedente */
                if (rn != sizeof(cx)){                      /* controllo che sia riuscita */
                    printf("Errore durante la lettura dalla pipe[%d].\n", i);
                    exit(-1);
                }
            }
            

            cx[i].v1 = 'a' + i;    /* calcolo la lettere dell'alfabeto inglese da cercare */
            cx[i].v2 = 0;          /* azzero il contatore delle occorrenze */

            while((read(fd, &cc, 1)) > 0){    /* cerco le occorrenzze di cx[i].v1 */
                if(cx[i].v1 == cc){
                    cx[i].v2++; /* se trovo il carattere incremento v2 */
                }
            }

            /* scrivo sulla pipe attuale il nuovo array di struct */
            rn = write(piped[i][1], cx, sizeof(cx));   /* scrivo sulla pipe precedente */
            if (rn != sizeof(cx)){                     /* controllo che sia riuscita */
                printf("Errore durante la scrittura sulla pipe[%d].\n", i);
                exit(-1);
            }

            exit(cc);   /* esco ritornando l'ultimo carattere letto dal file (sarà sempre lo stesso) */
        }
    }

    /* codice del padre */

    /* ogni padre (consumatore) chiude tutte le pipe che non usa */
    for(k = 0; k < N; k++){
        close(piped[k][1]); /* chiudo tutti i lati di scrittura, il padre è consumatore */
        if(k != (N-1)){
            /* devo chiudere tutti i lati di lettura tranne quello dell'ultima pipe che contiene l'array definitivo */
            close(piped[k][0]);
        }
    }

    /* leggo dalla 25esima pipe il vettore finale di struct */
    rn = read(piped[N-1][0], cx, sizeof(cx));   /* leggo dalla pipe precedente */
    if (rn != sizeof(cx)){                      /* controllo che sia riuscita */
        printf("Errore durante la lettura dalla 25esima pipe dal padre.\n");
    }
    else{
        /* ordino in ordine crescente le occorrenze trovate con il bubble sort */
        bubbleSort(cx, N);
        /* stampo come da speicifca le occorrenze di tutti i 26 caratteri */
        for(i = 0; i < N; i++){
            //k = cx[i].v1 - 'a';
            printf("%c\n", cx[i].v1);
            printf("Il processo di indice %d e pid %d ha trovato il carattere %c %ld volte nel file %s.\n", k, pid[k], cx[i].v1, cx[i].v2, argv[1]);
        }
        
    }  
    
    /* il processo padre aspetta i 26 processi figli */
    /* se non ci sono processi da attendere, la wait ritorna -1, altrimenti il pid del processo da aspettare*/
    for(i = 0; i < N; i++){
        pidFiglio = wait(&status);
        if (pidFiglio < 0){
            printf("Errore in wait.\n");
            exit(4);
        }
        
        /* controllo il valore di uscita del processo filgio */
        /* se status & 0xFF == 0 allora il figlio è terminato correttamente */
        if ((status & 0xFF) != 0){
            printf("Processo figlio con pid %d terminato in modo anomalo.\n", pidFiglio);
            /* non facciamo exit con errore, il problema è solo del figlio, non del padre */
        }
        else {
            returnValue = (int)((status >> 8) & 0xFF);
            printf("Il processo figlio di pid %d ha ritornato il carattere %c.\n", pidFiglio, returnValue);
        }
    }
    
    exit(0);
}