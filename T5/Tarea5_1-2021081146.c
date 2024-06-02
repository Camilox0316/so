#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COUNT 10 // Número de números pares e impares a imprimir

pid_t child_even, child_odd; // IDs de los procesos hijos

void even_handler(int sig) {
    static int even_num = 0;
    printf("Par: %d\n", even_num);
    even_num += 2;
    kill(child_odd, SIGUSR2); // Señal al proceso impar
}

void odd_handler(int sig) {
    static int odd_num = 1;
    printf("Impar: %d\n", odd_num);
    odd_num += 2;
    if (odd_num / 2 < MAX_COUNT) {
        kill(child_even, SIGUSR1); // Señal al proceso par
    } else {
        // Ambos procesos terminan al alcanzar el máximo conteo
        kill(child_even, SIGTERM);
        kill(getpid(), SIGTERM);
    }
}

int main() {
    struct sigaction sa_even, sa_odd;

    // Configurar manejadores de señales
    sa_even.sa_handler = even_handler;
    sa_even.sa_flags = 0;
    sigemptyset(&sa_even.sa_mask);
    sigaction(SIGUSR1, &sa_even, NULL);

    sa_odd.sa_handler = odd_handler;
    sa_odd.sa_flags = 0;
    sigemptyset(&sa_odd.sa_mask);
    sigaction(SIGUSR2, &sa_odd, NULL);

    // Crear el primer proceso hijo para números pares
    if ((child_even = fork()) == 0) {
        while (1) pause(); // Esperar señales
        exit(0);
    }

    // Crear el segundo proceso hijo para números impares
    if ((child_odd = fork()) == 0) {
        while (1) pause(); // Esperar señales
        exit(0);
    }

    // El proceso padre inicia la señalización
    kill(child_even, SIGUSR1);

    // Esperar a que los procesos hijos terminen
    wait(NULL);
    wait(NULL);

    return 0;
}


