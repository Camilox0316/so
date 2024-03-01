#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void e3(){
    int N_HIJOS = 5;
    int exit_codes[N_HIJOS];
    
    for (int i = 0; i < N_HIJOS; i++){
        int pid = fork();
        if (pid == 0){
            srand(getpid()); 
            int n_random = rand() % 100; 
            printf("Este es mi número random: %d \n", n_random);
            exit(n_random); 
        } else {
            
            int status;
            waitpid(pid, &status, 0); 
            if (WIFEXITED(status)){
                exit_codes[i] = WEXITSTATUS(status); 
            }
        }
    }
    printf("Los números random generados son: ");
    for (int i = 0; i < N_HIJOS; i++){
        printf("%d ", exit_codes[i]);
    }
    printf("\n");
    exit(0);
}
int main() {
    e3();
       
}