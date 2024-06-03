// servidor.c
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
#endif

#if defined _WIN32
#define close(x) closesocket(x)
#endif

#define PORT 8888
#define BUFFER_SIZE 1024

void handle_client(int client_sock);

int main(int argc, char *argv[]) {
    int socket_desc, client_sock, c;
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

    // Aceptar conexión entrante
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c))) {
        puts("Conexión aceptada");
        handle_client(client_sock);
        close(client_sock);
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

void handle_client(int client_sock) {
    char client_message[BUFFER_SIZE];
    int read_size;

    // Recibir el nombre del archivo del cliente
    if ((read_size = recv(client_sock, client_message, BUFFER_SIZE, 0)) > 0) {
        client_message[read_size] = '\0';
        printf("Cliente solicitó el archivo: %s\n", client_message);

        // Abrir el archivo
        FILE *file = fopen(client_message, "r");
        if (file == NULL) {
            perror("Error al abrir el archivo");
            return;
        }

        // Enviar el contenido del archivo al cliente
        char file_buffer[BUFFER_SIZE];
        size_t bytes_read;
        while ((bytes_read = fread(file_buffer, 1, BUFFER_SIZE, file)) > 0) {
            send(client_sock, file_buffer, bytes_read, 0);
        }

        fclose(file);
    } else {
        perror("Fallo en recv");
    }
}
