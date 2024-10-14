/* FILE: status1.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 

int main (){
     int pid; 				            /* pid per fork */
     int pidFiglio, status, returnValue;    /* per wait padre */
     int value;

	if ((pid = fork()) < 0){ 	        /* fork fallita */
		printf("Errore in fork\n");
		exit(1);
	}

	if (pid == 0){ 	/* figlio */
		printf("Esecuzione del figlio.\n");
        scanf("%d", &value);

		if(value > 255 || value < 0){
            printf("Il valore %d sarà troncato.\n", value);
        } 
        else{
            printf("Il valore %d NON sarà troncato.\n", value);
        }
		exit(value);	
	}

	/* padre */
	printf("Generato figlio con PID = %d.\n", pid);

	/* il padre aspetta il figlio in questo caso interessandosi del valore della exit del figlio */
	if ((pidFiglio=wait(&status)) < 0){
		printf("Errore in wait.\n");
		exit(2);
	}

	if (pid == pidFiglio){
        	printf("Terminato figlio con PID = %d.\n", pidFiglio);
	}
    else{   
        printf("Il pid della wait non corrisponde al pid della fork.\n");
        exit(3);
    }

	if((status & 0xFF) != 0){   /*0xFF = 11111111*/
        printf("Figlio terminato in modo involontario.\n");
        /*non facciamo exit con errore, il problema è solo del figlio, non del padre*/
    } 
    else{
        returnValue = (int)((status >> 8) & 0xFF);    /*salvo il valore di ritorno del figlio*/
        printf("Il figlio con pid = %d ha ritornato %d.\n", pidFiglio, returnValue);
    }

	exit(0);
}