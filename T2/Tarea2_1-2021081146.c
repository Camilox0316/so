/*
Cree un proceso padre que cree 10 procesos hijos a los cuáles les pasará mediante memoria compartida dos números aleatorios 
que ellos deberán sumar e imprimir. Puede usar r = rand() % n; para generar un número random entre 0 y n.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

int main() {
    int shmId;
    pid_t pid;
    int *memoria = NULL;
    int n = 10; // Número de procesos hijos
    int i;

    // Inicializar la semilla para generar números aleatorios
    srand(time(NULL));

    // Crear un segmento de memoria compartida
    shmId = shmget(IPC_PRIVATE, sizeof(int) * 2, IPC_CREAT | 0666);
    if (shmId < 0) {
        perror("shmget");
        exit(1);
    }

    // Adjuntar el segmento de memoria compartida
    memoria = (int *)shmat(shmId, NULL, 0);
    if (memoria == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    // Generar dos números aleatorios y guardarlos en la memoria compartida
    memoria[0] = rand() % 100; // Número entre 0 y 99
    memoria[1] = rand() % 100; // Número entre 0 y 99

    for (i = 0; i < n; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            // Proceso hijo
            printf("Hijo %d: %d + %d = %d\n", i+1, memoria[0], memoria[1], memoria[0] + memoria[1]);
            // Desconectar el segmento de memoria compartida
            shmdt((void *)memoria);
            exit(0);
        }
    }

    // Esperar a que todos los hijos terminen
    while (n > 0) {
        wait(NULL);
        n--;
    }

    // Desconectar el segmento de memoria compartida
    shmdt((void *)memoria);
    // Eliminar el segmento de memoria compartida
    shmctl(shmId, IPC_RMID, NULL);

    return 0;
}
