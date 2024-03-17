#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#ifdef __unix__
#include <unistd.h>
#elif defined _WIN32
#include "windows.h"
#define sleep(x) Sleep(1000 * x)
#endif

#define NUM_THREADS 10

pthread_t threads[NUM_THREADS];

void handle_error_en(int en,const char *msg) {
    errno = en;                  
    perror(msg);                 
    exit(EXIT_FAILURE);          

}
void *thread_func(void *arg) {
    int myIndex = *(int*)arg;
    free(arg);
    int s;
    // Nota: La semilla para rand() se inicializa en main.

    s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (s != 0)
        handle_error_en(s, "pthread_setcancelstate");

    // Dar tiempo a todos los hilos para que se inicien antes de permitir cancelaciones.
    sleep(1);
    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (s != 0)
        handle_error_en(s, "pthread_setcancelstate");
    /*AL momento de estarse cancelando, aunquetenga los sleeps, parece hacerse muy rapido y no da chance de avisar
    cuales han sido los hilos cancelados, lo que hace que haya 1 hilo al ultimo y se queda en el ciclo*/
    s = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (s != 0)
        handle_error_en(s, "pthread_setcanceltype");

    while (true) {
        printf("\nSoy %lu y sigo vivo.", (unsigned long)pthread_self());
        int r = rand() % NUM_THREADS;
        if (pthread_self() == threads[r]) { // Evita auto-cancelación.
            continue;
        } else { 
            s = pthread_cancel(threads[r]);
            if (s != 0)
                handle_error_en(s, "pthread_cancel");
            printf("\nSoy %lu y acabo de intentar cancelar a %lu", (unsigned long)pthread_self(), (unsigned long)threads[r]);
        }
        sleep(1); // Reduce el uso intensivo de la CPU y hace que la salida sea más manejable.
    }

    return NULL;
}

int main(void) {
    /*
    Estoy teniendo problemas de segmentation fault y que los hilos no se estan cancelando.
    Entonces voy a tener que pasar por parametro algun indice para el arreglo de hilos que tengo 
    */
    srand((unsigned)time(NULL)); // Inicializar la semilla para rand() una sola vez.

    int s;
    void *res;

    for (int i = 0; i < NUM_THREADS; i++) { // Crear hilos.
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        s = pthread_create(&threads[i], NULL, thread_func, arg);
        if (s != 0)
            handle_error_en(s, "pthread_create");
    }

    for (int i = 0; i < NUM_THREADS; i++) { // Esperar por la terminación de los hilos.
        s = pthread_join(threads[i], &res);
        if (s != 0)
            handle_error_en(s, "pthread_join");
        if (res == PTHREAD_CANCELED)
            printf("\nEl hilo %lu fue cancelado.", (unsigned long)threads[i]);
        else
            printf("\nEl hilo %lu terminó normalmente.", (unsigned long)threads[i]);
    }

    return 0;
}
