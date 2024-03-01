#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
void  e1(){

int it_hijos, hijos;
    hijos = 5;
    int it_generacion = 1;
    for (int j = 0; j < 3; j++) {
        for (it_hijos = 0; it_hijos < hijos; it_hijos++) {
            int pid = fork();
            if (pid != 0) {
                wait(NULL); 
            } else {
                it_generacion++;
                break;  
            }
        }
        if (it_hijos == hijos) {
            break; 
            
        }
    }    

    printf("Generacion: %d \t Padre: %ld \t Proceso actual: %ld \n", it_generacion, (long)getppid(), (long)getpid());
    exit(0);     

}

int main () {
    e1();
}
