/*
Cree un proceso padre que cree a su vez 5 procesos hijos los cuáles deben generar un número aleatorio que imprimirán 
y luego retornarán al proceso padre como su estado de terminación (debe ser un número entero pequeño). 
El padre también debe imprimir los números aleatorios que le retornaron sus procesos hijos al finalizar.
*/
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
void print_num_random(int random_generado[], int n){
    printf("Mis hijos generaron los siguientes numeros: \n");
    for (int i = 0; i < n; i++){
        printf("%d \n", random_generado[i]);
    }
    printf("\n");
}

void e3(){
    int random_generado[5];
    char nombre_proceso[] = {'A', 'B', 'C', 'D', 'E'};

    for (int i = 0; i < 5; i++){
        int pid = fork();
        if (pid == 0){
            srand(getpid()); 
            int my_random = rand() % 10; 
            printf("Soy %c y mi numero generado es: %d \n", nombre_proceso[i] ,  my_random);
            exit(my_random); 
        } else {
            
            int status;
            waitpid(pid, &status, 0); 
            if (WIFEXITED(status)){
                random_generado[i] = WEXITSTATUS(status); 
            }
        }
    }
    print_num_random(random_generado, 5);

    exit(0);
}
int main() {
    e3();
       
}