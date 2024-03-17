#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define MSGSZ 128
struct message {
  long type;
  char text[MSGSZ];
} msg;

int main() {
    srand(time(NULL));
    int status;
    key_t msqkey = 999;
    int msqid = msgget(msqkey, IPC_CREAT | S_IRUSR | S_IWUSR);
    pid_t hijo_1;
    pid_t hijo_2;
    int n_mensajes = 5;
    int aleatorio = rand() % (n_mensajes - 1) + 1;
    int resto = n_mensajes - aleatorio;

    for (int i = 0; i < 2; i++) {
        if (i == 0) {
            hijo_1 = fork();
            if (hijo_1 == 0) { // Primer hijo
                for (int j = 0; j < aleatorio; j++) {
                    //sprintf(msg.text, "Soy hijo %d y recibí el mensaje: %d", getpid(), j);
                    msgrcv(msqid, &msg, MSGSZ, j + 1, 0); // Ajuste para tipos de mensaje
                    printf("Soy hijo %d y recibi el mensaje: %s\n",getpid(),  msg.text);
                }
                exit(0); // Termina el proceso hijo después de recibir mensajes
            }
        } else if (i == 1) {
            hijo_2 = fork();
            if (hijo_2 == 0) { // Segundo hijo
                for (int j = aleatorio; j < n_mensajes; j++) {
                    //sprintf(msg.text, "Soy hijo %d y recibí el mensaje: %d", getpid(), j);
                    msgrcv(msqid, &msg, MSGSZ, j + 1, 0); // Ajuste para tipos de mensaje
                    printf("Soy hijo %d y recibi el mensaje: %s\n",getpid(),  msg.text);
                }
                exit(0); // Termina el proceso hijo después de recibir mensajes
            }
        }
    }

    // Asegurarse de que este bloque se ejecute solo en el proceso padre
    if (hijo_1 > 0 && hijo_2 > 0) {
        for (int i = 0; i < n_mensajes; i++) {
            msg.type = i + 1; // Ajuste para enviar diferentes tipos de mensajes
            sprintf(msg.text, "Mensaje %d", i);
            msgsnd(msqid, &msg, sizeof(msg.text), IPC_NOWAIT);
            printf("\nSoy papa y envio el mensaje: %s", msg.text);
        }

        // Espera a que terminen los procesos hijo
        for (int i = 0; i < 2; i++) {
            wait(&status);
        }
        msgctl(msqid, IPC_RMID, NULL); // Corrección del uso de msgctl
    }
    return 0;
}
