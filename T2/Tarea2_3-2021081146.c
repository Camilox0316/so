#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct alarm_tag {
    int seconds;
    char *message;
    pthread_t thread;
    struct alarm_tag *siguiente;
} alarm_t;

void *alarm_thread(void *arg) {
    alarm_t *alarm = (alarm_t *)arg;
    sleep(alarm->seconds);
    printf("Me desperte luego de (%d)s y soy el Hilo: %s\n", alarm->seconds, alarm->message);

    if (alarm->siguiente != NULL) {
        pthread_join(alarm->siguiente->thread, NULL);
        printf("Soy %s y Terminó la espera por el hilo: %s\n", alarm->message,alarm->siguiente->message);
    }

    // Liberar memoria aquí podría causar problemas si se accede después en main
    return NULL;
}

alarm_t *create_alarm(int seconds, char *message, alarm_t *siguiente) {
    alarm_t *alarm = (alarm_t *)malloc(sizeof(alarm_t));
    if (alarm == NULL) {
        perror("Error al asignar memoria para la alarma");
        exit(EXIT_FAILURE);
    }
    alarm->seconds = seconds;
    alarm->message = strdup(message);
    if (alarm->message == NULL) {
        perror("Error al asignar memoria para el mensaje de la alarma");
        free(alarm);
        exit(EXIT_FAILURE);
    }
    alarm->siguiente = siguiente;

    int status = pthread_create(&alarm->thread, NULL, alarm_thread, alarm);
    if (status != 0) {
        perror("Error al crear hilo");
        free(alarm->message);
        free(alarm);
        exit(EXIT_FAILURE);
    }

    return alarm;
}

int main() {
    alarm_t *c = create_alarm(30, "C", NULL); // C no espera a nadie
    alarm_t *b = create_alarm(20, "B", c); // B espera a C
    alarm_t *a = create_alarm(10, "A", b); // A espera a B

    // Esperar por los hilos en el orden correcto para asegurar que todos terminen.
    pthread_join(a->thread, NULL);
    pthread_join(b->thread, NULL);
    pthread_join(c->thread, NULL);

    // Ahora es seguro liberar la memoria
    free(a->message); free(a);
    free(b->message); free(b);
    free(c->message); free(c);

    return 0;
}
