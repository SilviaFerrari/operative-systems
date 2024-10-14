#include <stdio.h>  	/*standard input output*/
#include <unistd.h> 	/*unix standard*/
#include <stdlib.h> 	/*standard library*/
#include <sys/wait.h>	/*per la wait e le macro*/

int main(){
	printf("Il pid del processo corrente è %d.\n", getpid());

	if(wait((int *)0) < 0){
        printf("Errore nella wait.\n");
        exit(4);
    }

	exit(0);
}