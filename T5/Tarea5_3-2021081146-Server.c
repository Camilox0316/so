#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501 /* Windows XP. */
#endif
#include <Ws2tcpip.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#endif

#if defined _WIN32
#define close(x) closesocket(x)
#endif

#define PORT 8888
#define MAX_CLIENTS 100

void *connection_handler(void *);

int client_sockets[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {
    int socket_desc, client_sock, c, *new_sock;
    struct sockaddr_in server, client;
    
#if defined _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(1, 1), &wsa_data);
#endif

    // Crear socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("No se pudo crear el socket");
        return 1;
    }
    puts("Socket creado");

    // Preparar la estructura sockaddr_in
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Vincular
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Fallo en bind. Error");
        return 1;
    }
    puts("Bind hecho");

    // Escuchar
    listen(socket_desc, 3);
    puts("Esperando conexiones entrantes...");
    c = sizeof(struct sockaddr_in);

    // Aceptar conexiones entrantes
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))) {
        puts("Conexión aceptada");

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = client_sock;
                break;
            }
        }
        pthread_mutex_unlock(&clients_mutex);

        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0) {
            perror("No se pudo crear el hilo");
            return 1;
        }

        puts("Handler asignado");
    }

    if (client_sock < 0) {
        perror("Fallo en accept");
        return 1;
    }

#if defined _WIN32
    WSACleanup();
#endif

    return 0;
}

void send_message_to_all_clients(char *message, int exclude_sock) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0 && client_sockets[i] != exclude_sock) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *connection_handler(void *socket_desc) {
    int sock = *(int *)socket_desc;
    int read_size;
    char client_message[2000];

    while ((read_size = recv(sock, client_message, 2000, 0)) > 0) {
        client_message[read_size] = '\0';
        send_message_to_all_clients(client_message, sock);
    }

    if (read_size == 0) {
        puts("Cliente desconectado");
        fflush(stdout);
    } else if (read_size == -1) {
        perror("Fallo en recv");
    }

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == sock) {
            client_sockets[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    free(socket_desc);
    return 0;
}


