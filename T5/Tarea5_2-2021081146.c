#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_CHILDREN 10

pid_t children[NUM_CHILDREN]; // Almacenar PIDs de los procesos hijos

void handler(int sig) {
    printf("Proceso %d: bye\n", getpid());
    exit(0);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTERM, &sa, NULL);

    // Crear procesos hijos
    for (int i = 0; i < NUM_CHILDREN; i++) {
        if ((children[i] = fork()) == 0) {
            // Proceso hijo
            while (1) {
                printf("Proceso %d: corriendo\n", getpid());
                sleep(1);
            }
        }
    }

    // Esperar un momento antes de enviar señales
    sleep(5);

    // Inicializar generador de números aleatorios
    srand(time(NULL));

    // Enviar señales de terminación en orden aleatorio
    for (int i = 0; i < NUM_CHILDREN; i++) {
        int index = rand() % NUM_CHILDREN;
        while (children[index] == 0) {
            index = rand() % NUM_CHILDREN;
        }
        kill(children[index], SIGTERM);
        waitpid(children[index], NULL, 0);
        children[index] = 0; // Marcar como terminado
    }

    // Esperar a que todos los procesos hijos terminen
    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    return 0;
}

