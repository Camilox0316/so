#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMPLOYEES 10
#define MAX_NAME_LENGTH 50

struct Employee {
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
};

// Función para guardar la información de los empleados en un archivo
void saveEmployeeData(char *fileName) {
    FILE *file = fopen(fileName, "wb");
    if (file == NULL) {
        printf("Error al abrir el archivo.\n");
        return;
    }

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

    fwrite(employees, sizeof(struct Employee), MAX_EMPLOYEES, file);
    fclose(file);
    printf("Datos de empleados guardados en el archivo.\n");
}

// Función para imprimir los datos de los empleados en posiciones impares
void printOddEmployees(char *fileName) {
    FILE *file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Error al abrir el archivo.\n");
        return;
    }

    struct Employee employee;
    long offset = 0;

    printf("Empleados en posiciones impares:\n");
    for (int i = 1; i <= MAX_EMPLOYEES; i += 2) {
        offset = (i - 1) * sizeof(struct Employee);
        fseek(file, offset, SEEK_SET);
        fread(&employee, sizeof(struct Employee), 1, file);
        printf("ID: %d, Nombre: %s, Edad: %d\n", employee.id, employee.name, employee.age);
    }

    fclose(file);
}

int main() {
    char fileName[] = "employees.dat";

    saveEmployeeData(fileName);
    printOddEmployees(fileName);

    return 0;
}
