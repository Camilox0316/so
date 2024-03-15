#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>

int main() {
    int shmID;
    int *nums;
    int size = 2 * sizeof(int); // Espacio para dos enteros
    pid_t pid;
    key_t key = 123; // Clave privada

    // Inicializar generador de números aleatorios
    srand(time(NULL));

    // Crear segmento de memoria compartida
    shmID = shmget(key, size, IPC_CREAT | 0666);
    if (shmID < 0) {
        perror("shmget");
        exit(1);
    }

    // Adjuntar segmento de memoria compartida
    nums = (int *)shmat(shmID, NULL, 0);
    if (nums == (int *)(-1)) {
        perror("No se pudo crear la memoria compartida");
        exit(1);
    }

    for (int i = 0; i < 10; i++) {
        // Generar dos números aleatorios y guardarlos en memoria compartida
  
        pid = fork();
        nums[0] = rand() % 100; // Número entre 0 y 99
        nums[1] = rand() % 100; // Número entre 0 y 99  
        if (pid < 0) {
            // Error al crear proceso hijo
            perror("No se pudo crear el proceso hijo");
            exit(1);
        } else if (pid == 0) {
            // Proceso hijo
            printf("Hijo %d: %d + %d = %d\n", i, nums[0], nums[1], nums[0] + nums[1]);
            exit(0); // Terminar proceso hijo
        }
    }

    // Esperar a que todos los hijos terminen
    while (wait(NULL) > 0);

    // Desadjuntar y eliminar memoria compartida
    shmdt(nums);
    shmctl(shmID, IPC_RMID, NULL);

    return 0;
}
