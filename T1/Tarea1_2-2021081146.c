/*
Cree tres procesos A, B y C asíncronos de forma que el proceso A duerma 5 segundos y luego espere que termine el proceso B 
que dormirá 10 segundos y luego esperará a que termine el proceso C que dormirá 15 segundos. 
Los procesos deben imprimir cuando terminen de dormir y cuando terminen la espera del otro proceso.
*/
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void e2(){
    char nombre_proceso[] = {'A', 'B', 'C'};
    int tiempo_espera[] = {5, 10, 15};
    int i = 0;
    for (i = 0 ; i < 3; i++){
        int pid = fork();
        //printf("Esperando... \n");
        if (pid != 0){
            //printf("Esperando... \n");
            sleep(tiempo_espera[i]);
            printf("Yo soy %c: Y ya termine de dormir %d  \n", nombre_proceso[i], tiempo_espera[i]);
            wait(NULL);
            //printf("Esperando... \n");
            if (i+1 < 3) 
                {
                    printf("Yo soy A %c: Y ya termine de esperar a mi hijo que es: %c \n", nombre_proceso[i], nombre_proceso[i+1]);
                }
            break;
        }
    }
}

int main(){
    e2();
}