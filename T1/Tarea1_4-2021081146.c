/*
Cree un proceso padre que cree a su vez 5 procesos hijos donde cada uno de ellos entrará en un ciclo infinito esperando ser cancelado. 
El padre enviará mensajes de cancelación a cada uno de ellos apenas termine de crear todos los hijos. 
En el momento que el hijo sea cancelado debe imprimir un mensaje con su número de PID.
*/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void e4(){
    int i;
    int pid;
    int id_hijos[5];
    char nombre_proceso[] = {'A', 'B', 'C', 'D', 'E'};
    for (i = 0 ; i<5; i++){
        pid = fork();

        if (pid == 0){
            while(1){
                //printf ("Soy el hijo %c numero %d corriendo ...\n", nombre_proceso[i], id_hijos[i]);

                sleep(1);
            }
        }
        else{
            id_hijos[i] = pid;
        }
    }
    printf("Yo soy tu padre... \n");
    for (i = 0; i<5; i++){
        printf("Matando al hijo %c numero %d \n", nombre_proceso[i],id_hijos[i] );
        kill(id_hijos[i], SIGKILL);
    }
}

int main(){
    e4();
    return 0;
}