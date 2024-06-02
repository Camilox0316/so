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
#define SERVER_IP "127.0.0.1"

void *receive_handler(void *);

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char message[1000], server_reply[2000];
    
#if defined _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(1, 1), &wsa_data);
#endif

    // Crear socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("No se pudo crear el socket");
        return 1;
    }
    puts("Socket creado");

    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Conectar al servidor remoto
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Conexión fallida. Error");
        return 1;
    }
    puts("Conectado\n");

    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_handler, (void *)&sock) < 0) {
        perror("No se pudo crear el hilo de recepción");
        return 1;
    }

    // Continuar comunicándose con el servidor
    while (1) {
        printf("Escriba mensaje: ");
        fgets(message, 1000, stdin);
        message[strcspn(message, "\n")] = '\0';

        // Enviar datos
        if (send(sock, message, strlen(message), 0) < 0) {
            puts("Fallo en el envío");
            return 1;
        }
    }

    close(sock);
    
#if defined _WIN32
    WSACleanup();
#endif

    return 0;
}

void *receive_handler(void *sock_desc) {
    int sock = *(int *)sock_desc;
    char server_reply[2000];
    int read_size;

    while ((read_size = recv(sock, server_reply, 2000, 0)) > 0) {
        server_reply[read_size] = '\0';
        printf("Mensaje del servidor: %s\n", server_reply);
    }

    if (read_size == 0) {
        puts("Servidor desconectado");
    } else if (read_size == -1) {
        perror("Fallo en recv");
    }

    return 0;
}

