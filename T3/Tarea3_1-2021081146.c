#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 4096

int buscar_archivo(char *directorio, char *nombre_archivo);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <nombre_archivo>\n", argv[0]);
        return 1;
    }

    char ruta_raiz[MAX_PATH_LENGTH] = "/"; // Ruta raíz del sistema de archivos

    int encontrado = buscar_archivo(ruta_raiz, argv[1]);
    if (!encontrado) {
        printf("El archivo '%s' no se encontró.\n", argv[1]);
    }

    return 0;
}

int buscar_archivo(char *directorio, char *nombre_archivo) {
    DIR *dir;
    struct dirent *entrada;
    char ruta[MAX_PATH_LENGTH];
    struct stat stat_info;

    // Verificar permisos antes de abrir el directorio
    if (access(directorio, R_OK) != 0) {
        return 0;
    }

    dir = opendir(directorio);
    if (dir == NULL) {
        perror("opendir");
        return 0;
    }

    while ((entrada = readdir(dir)) != NULL) {
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) {
            continue;
        }

        snprintf(ruta, MAX_PATH_LENGTH, "%s/%s", directorio, entrada->d_name);

        if (lstat(ruta, &stat_info) == -1) {
            perror("lstat");
            continue;
        }

        if (S_ISDIR(stat_info.st_mode)) {
            if (buscar_archivo(ruta, nombre_archivo)) {
                closedir(dir);
                return 1;
            }
        } else if (strcmp(entrada->d_name, nombre_archivo) == 0) {
            printf("Archivo encontrado: %s\n", ruta);
            closedir(dir);
            return 1;
        }
    }

    closedir(dir);
    return 0;
}
