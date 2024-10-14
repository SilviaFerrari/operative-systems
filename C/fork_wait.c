#include <sys/wait.h>

/*Sappiamo che la fork crea nuovi processi figli. Quando termineranno con la exit, i processi figli ritornano
un exit status che viene passato al padre. Il processo padre usa la primitiva wait con le macro per capire
in che modo il figlio è finito. I valori di ritorno della wait sono:
less than -1 --> sta aspettando qualsiasi processo figlio con groupID == |pid|
-1 --> sta aspettando un qualsiasi processo figlio
0 --> sta aspettando un qualsiasi processo figlio con groupID == 
greater than 0 -->*/

int main(){
    int pid;            /*per il valore di ritorno della fork*/
    int pidFiglio;      /*per il valore di ritorno della wait*/
    int returnValue;    /*per filtrare il valore d'uscita del processo figlio*/
    int status;         /*da usare nella wait*/

    /*stampo il pid del processo padre*/
    printf("Il pid del processo padre è: %d.\n", getpid());

    /* eseguo la creazione del processo figlio e controllo che la fork sia andata a buon fine */
    /* fork() restituisce al figlio 0 e al padre il pid del processo figlio */
    if((pid = fork()) < 0){
        printf("Errore nella fork.");
        exit(1);    
    }

    /*ora sono nel processo figlio*/
    if(pid == 0){
        printf("Il pid del processo figlio è %d, creato dal processo padre con pid %d.\n", getpid(), getppid());
        /*do something*/
        exit(/*valore da ritornare*/);  /*termino il processo figlio*/
    }



    /*se non ci sono processi da attendere la wait ritorna -1
    altrimenti ritorna il pid del processo da attendere*/
    pidFiglio = wait(&status);
    printf("DEBUG: pidFiglio dato dalla wait = %d.\n", pidFiglio);

    if(pidFiglio < 0){
        printf("Errore nella wait %d.\n", pidFiglio);
        exit(2);
    }

    /*se la wait ha successo salva in status il valore restituito dal figlio e lo passa al padre*/
    /*controllo che il pid restitutio dalla wait sia lo stesso che la fork aveva dato al padre*/
    if(pidFiglio == pid){
        printf("Terminato il processo figlio con pid = %d.\n", pidFiglio);
        printf("DEBUG: pidFiglio dato dalla fork = %d.\n", pid);
    } 
    else{
        printf("Il pid della wait %d non corrisponde col pid della fork %d.\n", pidFiglio, pid);
        exit(3);
    }

    /*controllo il valore di uscita del processo filgio */
    /*se status & 0xFF == 0 allora il figlio è terminato correttamente %d*/
    if((status & 0xFF) != 0){   /*0xFF = 11111111*/
        printf("Figlio terminato in modo involontario.\n");
        /*non facciamo exit con errore, il problema è solo del figlio, non del padre*/
    } 
    else{
        returnValue = (int)((status >> 8) & 0xFF);    /*salvo il valore di ritorno del figlio*/
        printf("Il figlio con pid = %d ha ritornato %d.\n", pidFiglio, returnValue);
    }

    /*il padre fa una seconda wait anche se non ci sono più figli da aspettare*/
    if(wait((int *)0) < 0){
        printf("Errore nella wait.\n");
        exit(4);
    }

    exit(0);
}