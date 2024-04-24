#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define MAX_EMPLOYEES 10
#define MAX_NAME_LENGTH 50

struct Employee {
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
};

struct Employee employees[MAX_EMPLOYEES] = {
    {1, "Juan Perez", 25},
    {2, "Maria Garcia", 32},
    {3, "Pedro Gomez", 41},
    {4, "Ana Rodriguez", 28},
    {5, "Carlos Sanchez", 35},
    {6, "Laura Fernandez", 27},
    {7, "Jorge Torres", 38},
    {8, "Lucia Ramirez", 44},
    {9, "Miguel Gonzalez", 31},
    {10, "Sofia Hernandez", 29}
};

int fd;

void *writerThread(void *arg) {
    struct flock lock;

    for (int i = 0; i < MAX_EMPLOYEES; i++) {
        // Adquirir el bloqueo de escritura
        memset(&lock, 0, sizeof(lock));
        lock.l_type = F_WRLCK;
        fcntl(fd, F_SETLKW, &lock);

        // Escribir los datos del empleado en el archivo
        lseek(fd, 0, SEEK_END);
        write(fd, &employees[i], sizeof(struct Employee));

        // Liberar el bloqueo de escritura
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &lock);

        printf("Empleado %d agregado al archivo.\n", employees[i].id);
        sleep(2); // Esperar 2 segundos antes de agregar el siguiente empleado
    }

    pthread_exit(NULL);
}

void *readerThread(void *arg) {
    struct flock lock;
    struct Employee employee;

    while (1) {
        // Adquirir el bloqueo de lectura
        memset(&lock, 0, sizeof(lock));
        lock.l_type = F_RDLCK;
        fcntl(fd, F_SETLKW, &lock);

        // Leer los datos de los empleados desde el archivo
        lseek(fd, 0, SEEK_SET);
        while (read(fd, &employee, sizeof(struct Employee)) > 0) {
            printf("ID: %d, Nombre: %s, Edad: %d\n", employee.id, employee.name, employee.age);
        }

        // Liberar el bloqueo de lectura
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLKW, &lock);

        sleep(2); // Esperar 2 segundos antes de leer nuevamente
    }

    pthread_exit(NULL);
}

int main() {
    char fileName[] = "employees.dat";
    pthread_t writerTid, readerTid;

    // Abrir el archivo para lectura y escritura
    fd = open(fileName, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        printf("Error al abrir el archivo.\n");
        return 1;
    }

    // Crear los hilos escritor y lector
    pthread_create(&writerTid, NULL, writerThread, NULL);
    pthread_create(&readerTid, NULL, readerThread, NULL);

    // Esperar a que los hilos terminen
    pthread_join(writerTid, NULL);
    pthread_join(readerTid, NULL);

    close(fd);
    return 0;
}
